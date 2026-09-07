#include <ppu-types.h>
#include <sys/process.h>
#include <sysutil/sysutil.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>

SYS_PROCESS_PARAM(1001, 0x10000)

static volatile bool running = true;
static void sysCallback(u64 status, u64 param, void *usrdata){
    (void)param;(void)usrdata;
    if(status==SYSUTIL_EXIT_GAME) running=false;
}

int main(int argc, char *argv[]){
    (void)argc;(void)argv;
    sysUtilRegisterCallback(SYSUTIL_EVENT_SLOT0, sysCallback, NULL);

    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK)<0){
        sysProcessExit(1);
    }

    SDL_Window *win = SDL_CreateWindow("STALKER PS3 SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280,720, SDL_WINDOW_SHOWN);
    if(!win){ SDL_Quit(); sysProcessExit(1); }
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if(!ren){ SDL_DestroyWindow(win); SDL_Quit(); sysProcessExit(1); }

    SDL_Joystick *joy=NULL;
    if(SDL_NumJoysticks()>0) joy = SDL_JoystickOpen(0);

    // simple camera
    float px=0, pz=5;
    float yaw=0, pitch=0;
    int ammo=30;
    bool flash=false; int flashTimer=0;
    bool flashlight=true;
    float lamp=0;

    SDL_Event ev;
    while(running){
        sysUtilCheckCallback();
        while(SDL_PollEvent(&ev)){
            if(ev.type==SDL_QUIT) running=false;
            if(ev.type==SDL_JOYBUTTONDOWN){
                // PS3 mapping: 12=Triangle,15=Square,14=Cross,0=Select
                if(ev.jbutton.button==12) flashlight=!flashlight;
                if(ev.jbutton.button==15){ if(ammo>0){ ammo--; flash=true; flashTimer=6; } }
                if(ev.jbutton.button==14) ammo=30;
                if(ev.jbutton.button==0) running=false; // SELECT exit
            }
        }
        // axis
        float lx=0, ly=0, rx=0, ry=0;
        if(joy){
            lx = SDL_JoystickGetAxis(joy,0)/32767.0f;
            ly = SDL_JoystickGetAxis(joy,1)/32767.0f;
            rx = SDL_JoystickGetAxis(joy,2)/32767.0f;
            ry = SDL_JoystickGetAxis(joy,3)/32767.0f;
        }
        if(fabs(lx)<0.15f) lx=0;
        if(fabs(ly)<0.15f) ly=0;
        if(fabs(rx)<0.15f) rx=0;
        if(fabs(ry)<0.15f) ry=0;

        yaw += rx*0.04f;
        pitch += -ry*0.04f;
        if(pitch>1.4f) pitch=1.4f;
        if(pitch<-1.4f) pitch=-1.4f;
        float cosY=cosf(yaw), sinY=sinf(yaw);
        px += sinY*(-ly*0.08f) + cosY*(lx*0.08f);
        pz += -cosY*(-ly*0.08f) + sinY*(lx*0.08f);
        if(px<-1.8f) px=-1.8f; if(px>1.8f) px=1.8f;
        if(pz<-9) pz=-9; if(pz>9) pz=9;

        if(flashTimer>0){ flashTimer--; if(flashTimer==0) flash=false; }
        lamp = 0.7f + 0.3f*sinf(SDL_GetTicks()*0.005f);

        // render
        SDL_SetRenderDrawColor(ren, 10,10,18,255);
        SDL_RenderClear(ren);

        // corridor floor
        SDL_Rect floor = {0, 360, 1280, 360};
        SDL_SetRenderDrawColor(ren, 42,42,42,255);
        SDL_RenderFillRect(ren, &floor);
        // ceiling with flicker
        SDL_Rect ceil = {0,0,1280, 360};
        SDL_SetRenderDrawColor(ren, 26+(int)(lamp*20),26,26,255);
        SDL_RenderFillRect(ren, &ceil);
        // walls
        SDL_Rect left = {0,0,200,720};
        SDL_Rect right = {1080,0,200,720};
        SDL_SetRenderDrawColor(ren, 58,58,50,255);
        SDL_RenderFillRect(ren, &left);
        SDL_RenderFillRect(ren, &right);
        // lamp
        SDL_Rect l = {630, 20, 20, 20};
        SDL_SetRenderDrawColor(ren, (int)(lamp*200),(int)(lamp*255),(int)(lamp*120),255);
        SDL_RenderFillRect(ren, &l);
        // flashlight overlay
        if(flashlight){
            SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(ren, 34,34,17,30);
            SDL_Rect f = {400,200,480,320};
            SDL_RenderFillRect(ren, &f);
            SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_NONE);
        }
        // crosshair
        SDL_Rect ch1 = {639,359,2,2};
        SDL_SetRenderDrawColor(ren, 0,255,0,255);
        SDL_RenderFillRect(ren, &ch1);
        // weapon
        SDL_Rect weap = {700,600,200,50};
        if(flash) SDL_SetRenderDrawColor(ren, 255,255,153,255);
        else SDL_SetRenderDrawColor(ren, 64,64,64,255);
        SDL_RenderFillRect(ren, &weap);
        if(flash){
            SDL_Rect fl = {880,590,40,20};
            SDL_SetRenderDrawColor(ren,255,255,170,255);
            SDL_RenderFillRect(ren, &fl);
        }
        // ammo bar
        SDL_Rect ammoBg = {20,20,200,10};
        SDL_SetRenderDrawColor(ren, 40,40,40,255);
        SDL_RenderFillRect(ren, &ammoBg);
        SDL_Rect ammoFg = {20,20, (int)(200*ammo/30.0f),10};
        SDL_SetRenderDrawColor(ren, 0,255,0,255);
        SDL_RenderFillRect(ren, &ammoFg);

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    if(joy) SDL_JoystickClose(joy);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    sysProcessExit(0);
    return 0;
}
