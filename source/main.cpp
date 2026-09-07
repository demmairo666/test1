#include <ppu-types.h>
#include <sys/process.h>
#include <io/pad.h>
#include <tiny3d.h>
#include <sysutil/sysutil.h>
#include <math.h>
#include <string.h>

SYS_PROCESS_PARAM(1001, 0x10000)

static volatile bool running = true;

static void sysCallback(u64 status, u64 param, void *usrdata) {
    (void)param; (void)usrdata;
    if(status == SYSUTIL_EXIT_GAME) running = false;
}

int main() {
    sysUtilRegisterCallback(SYSUTIL_EVENT_SLOT0, sysCallback, NULL);
    // init tiny3d first, pad second - order matters for some firmwares
    s32 ret = tiny3d_Init(1024*1024);
    if(ret < 0) {
        // if tiny3d fails, just exit to XMB
        sysProcessExit(1);
    }
    ioPadInit(7);

    padInfo padinfo;
    padData paddata;
    float hue = 0;

    while(running) {
        sysUtilCheckCallback();
        ioPadGetInfo(&padinfo);
        for(int i=0;i<MAX_PADS;i++){
            if(padinfo.status[i]){
                ioPadGetData(i, &paddata);
                if(paddata.BTN_SELECT || paddata.BTN_START) {} // avoid immediate exit
                // SELECT = exit to XMB for test
                if(paddata.BTN_SELECT) running = false;
            }
        }

        // simple animated clear to prove we stay in game
        hue += 0.01f;
        u8 r = (u8)( 10 + 30 * fabs(sin(hue)) );
        u8 g = (u8)( 10 + 10 * fabs(sin(hue*1.3f)) );
        u8 b = (u8)( 18 + 20 * fabs(sin(hue*0.7f)) );
        u32 col = (0xff<<24) | (r<<16) | (g<<8) | b;

        tiny3d_Clear(col, TINY3D_CLEAR_ALL);

        // draw a simple 2D quad in center - if you see it, 2D works
        tiny3d_Project2D();
        tiny3d_SetPolygon(TINY3D_QUADS);
        tiny3d_VertexPos(500, 300, 0); tiny3d_VertexColor(0xFF00FF00);
        tiny3d_VertexPos(780, 300, 0); tiny3d_VertexColor(0xFF00FF00);
        tiny3d_VertexPos(780, 420, 0); tiny3d_VertexColor(0xFF00FF00);
        tiny3d_VertexPos(500, 420, 0); tiny3d_VertexColor(0xFF00FF00);
        tiny3d_End();

        // second quad - weapon placeholder
        tiny3d_SetPolygon(TINY3D_QUADS);
        tiny3d_VertexPos(700, 600, 0); tiny3d_VertexColor(0xFF404040);
        tiny3d_VertexPos(900, 600, 0); tiny3d_VertexColor(0xFF404040);
        tiny3d_VertexPos(900, 650, 0); tiny3d_VertexColor(0xFF404040);
        tiny3d_VertexPos(700, 650, 0); tiny3d_VertexColor(0xFF404040);
        tiny3d_End();

        tiny3d_Flip();
    }

    ioPadEnd();
    tiny3d_Exit();
    sysProcessExit(0);
    return 0;
}
