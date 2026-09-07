#include <ppu-types.h>
#include <sys/process.h>
#include <sysutil/sysutil.h>
#include <io/pad.h>
#include <rsx/rsx.h>
#include <gcm_sys.h>
#include <sysutil/video.h>
#include <malloc.h>
#include <string.h>
#include <math.h>

SYS_PROCESS_PARAM(1001, 0x100000)

static volatile bool running=true;
static void sysCb(u64 s,u64 p,void*d){ (void)p;(void)d; if(s==SYSUTIL_EXIT_GAME) running=false; }

#define HOST_SIZE (4*1024*1024)
static void *hostAddr=NULL;
static gcmContextData *ctx=NULL;
static u32 *displayBuffer[2];
static s32 currBuf=0;
static videoResolution res;
static u32 sLabelVal=1;

static void waitFlip(){
    while(gcmGetLabelStatus(8) != sLabelVal) usleep(200);
    sLabelVal++;
}
static void flip(){
    if(currBuf==0) gcmSetFlip(ctx,0);
    else gcmSetFlip(ctx,1);
    rsxFlushBuffer(ctx);
    gcmSetWaitFlip(ctx);
    waitFlip();
    gcmSetDisplayBuffer(flip, currBuf);
    currBuf ^=1;
    gcmSetWaitFlip(ctx);
}

int main(){
    sysUtilRegisterCallback(SYSUTIL_EVENT_SLOT0, sysCb, NULL);
    hostAddr = memalign(1024*1024, HOST_SIZE);
    videoState vs;
    videoGetState(0,0,&vs);
    videoGetResolution(vs.displayMode.resolution, &res);
    // gcm init
    gcmInitBody(&ctx, HOST_SIZE, HOST_SIZE, hostAddr);
    videoConfiguration vconfig;
    memset(&vconfig,0,sizeof(vconfig));
    vconfig.resolution = vs.displayMode.resolution;
    vconfig.format = VIDEO_BUFFER_FORMAT_XRGB;
    vconfig.pitch = res.width*4;
    vconfig.aspect = VIDEO_ASPECT_AUTO;
    videoConfigure(0,&vconfig,NULL,0);
    videoSetState(0, VIDEO_STATE_ENABLED, NULL);
    // buffers
    for(int i=0;i<2;i++){
        displayBuffer[i]=(u32*)rsxMemalign(64, res.width*res.height*4);
        memset(displayBuffer[i],0,res.width*res.height*4);
        gcmAddressToOffset(displayBuffer[i], &displayBuffer[i][0]); // dummy to init offset - will use gcmSetDisplayBuffer
        // real offset
        u32 offset;
        gcmAddressToOffset(displayBuffer[i], &offset);
        gcmSetDisplayBuffer(i, offset, res.width*4, res.width, res.height);
    }
    gcmResetFlipStatus();
    ioPadInit(7);
    padInfo pi; padData pd;
    float px=0; int ammo=30; bool flash=false; int ft=0; bool light=true; float hue=0;
    // simple loop - own engine: just clear with rsx
    while(running){
        sysUtilCheckCallback();
        ioPadGetInfo(&pi);
        for(int i=0;i<MAX_PADS;i++) if(pi.status[i]){
            ioPadGetData(i,&pd);
            float lx=(pd.ANA_L_H-128)/128.0f;
            if(fabs(lx)<0.15f) lx=0;
            px+=lx*3;
            if(px<-200) px=-200; if(px>200) px=200;
            if(pd.BTN_TRIANGLE) light=!light;
            if(pd.BTN_SQUARE){ if(ammo>0){ammo--; flash=true; ft=8;} }
            if(pd.BTN_CROSS) ammo=30;
            if(pd.BTN_SELECT) running=false;
        }
        if(ft>0){ ft--; if(ft==0) flash=false; }
        hue+=0.02f;
        // rsx clear - use gcm to set clear color and flip
        // For from-scratch engine we just use rsxSetClearColor + rsxClearSurface via context
        // Minimal: use rsx to clear display buffer
        // We do simple: fill buffer on CPU then flip (software engine)
        u32 *fb = (u32*)displayBuffer[currBuf];
        // retrieve real address from offset - we stored offset incorrectly, get real ptr via rsx
        // Instead use direct pointer we allocated
        // We kept pointer, so use it
        // But displayBuffer was overwritten with offset - restore pointer
        // Simplify: keep separate pointers
        static u32 *realBuf[2]={0};
        if(!realBuf[0]){
            // first run, re-alloc correctly
            realBuf[0]=(u32*)rsxMemalign(64, res.width*res.height*4);
            realBuf[1]=(u32*)rsxMemalign(64, res.width*res.height*4);
            for(int i=0;i<2;i++){
                u32 off; gcmAddressToOffset(realBuf[i], &off);
                gcmSetDisplayBuffer(i, off, res.width*4, res.width, res.height);
            }
        }
        u32 *cur = realBuf[currBuf];
        // clear
        u8 r = 10 + (u8)(20*fabs(sin(hue)));
        u8 g = 10;
        u8 b = 18;
        u32 bg = (r<<16)|(g<<8)|b;
        for(u32 y=0;y<res.height;y++){
            for(u32 x=0;x<res.width;x++){
                cur[y*res.width+x] = bg | 0xFF000000;
            }
        }
        // floor
        for(u32 y=360;y<res.height;y++) for(u32 x=0;x<res.width;x++) cur[y*res.width+x]=0xFF2a2a2a;
        // walls
        for(u32 y=0;y<res.height;y++) for(u32 x=0;x<200;x++) cur[y*res.width+x]=0xFF3a3a32;
        for(u32 y=0;y<res.height;y++) for(u32 x=1080;x<res.width;x++) cur[y*res.width+x]=0xFF3a3a32;
        // lamp
        for(u32 y=20;y<40;y++) for(u32 x=630;x<650;x++) cur[y*res.width+x]= 0xFF00FF00 | ((int)(fabs(sin(hue))*80)<<16);
        // crosshair
        cur[359*res.width+639]=0xFF00FF00; cur[359*res.width+640]=0xFF00FF00;
        // weapon
        for(u32 y=600;y<650;y++) for(u32 x=700;x<900;x++) cur[y*res.width+x]= flash?0xFFFFFF99:0xFF404040;
        if(flash) for(u32 y=590;y<610;y++) for(u32 x=880;x<920;x++) cur[y*res.width+x]=0xFFFFFFAA;
        // ammo bar
        for(u32 y=20;y<30;y++) for(u32 x=20;x<20+(u32)(200*ammo/30.0f);x++) cur[y*res.width+x]=0xFF00FF00;

        flip();
    }
    ioPadEnd();
    sysProcessExit(0);
    return 0;
}
