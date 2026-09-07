#include <ppu-types.h>
#include <sys/process.h>
#include <io/pad.h>
#include <tiny3d.h>
#include <sysutil/sysutil.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "camera.h"
#include "level.h"
#include "weapon.h"

SYS_PROCESS_PARAM(1001, 0x10000)

static bool running = true;

static void sysCallback(u64 status, u64 param, void *usrdata) {
    (void)param; (void)usrdata;
    if(status == SYSUTIL_EXIT_GAME) running = false;
}

int main() {
    sysUtilRegisterCallback(SYSUTIL_EVENT_SLOT0, sysCallback, NULL);
    tiny3d_Init(1024*1024);
    ioPadInit(7);

    Camera cam;
    Level level;
    Weapon weapon;

    padInfo padinfo;
    padData paddata;

    float moveSpeed = 0.08f;
    float lookSpeed = 0.04f;

    while(running) {
        sysUtilCheckCallback();
        ioPadGetInfo(&padinfo);
        for(int i=0;i<MAX_PADS;i++){
            if(padinfo.status[i]){
                ioPadGetData(i, &paddata);
                float lx = (paddata.ANA_L_H - 128)/128.0f;
                float ly = (paddata.ANA_L_V - 128)/128.0f;
                float rx = (paddata.ANA_R_H - 128)/128.0f;
                float ry = (paddata.ANA_R_V - 128)/128.0f;
                if(fabs(lx) < 0.15f) lx=0;
                if(fabs(ly) < 0.15f) ly=0;
                if(fabs(rx) < 0.15f) rx=0;
                if(fabs(ry) < 0.15f) ry=0;
                cam.move(-ly * moveSpeed, lx * moveSpeed);
                cam.addYaw(rx * lookSpeed);
                cam.addPitch(-ry * lookSpeed);
                if(paddata.BTN_TRIANGLE) weapon.flashlightOn = !weapon.flashlightOn;
                if(paddata.BTN_SQUARE) weapon.shoot();
                if(paddata.BTN_CROSS) weapon.reload();
                if(paddata.BTN_SELECT) running = false;
                if(paddata.BTN_START) {} // pause
            }
        }

        level.update();
        weapon.update();

        tiny3d_Clear(0xff0a0a12, TINY3D_CLEAR_ALL);
        tiny3d_Project3D();
        cam.applyTransform();
        level.draw();

        tiny3d_Project2D();
        if(weapon.flashlightOn) {
            tiny3d_SetPolygon(TINY3D_QUADS);
            tiny3d_VertexPos(400, 200, 0); tiny3d_VertexColor(0x10222211);
            tiny3d_VertexPos(880, 200, 0); tiny3d_VertexColor(0x10222211);
            tiny3d_VertexPos(880, 520, 0); tiny3d_VertexColor(0x10222211);
            tiny3d_VertexPos(400, 520, 0); tiny3d_VertexColor(0x10222211);
            tiny3d_End();
        }
        weapon.draw2D();
        // simple HUD without libfont - colored bar for ammo
        tiny3d_SetPolygon(TINY3D_QUADS);
        float w = 200.0f * weapon.ammo / (float)weapon.maxAmmo;
        tiny3d_VertexPos(20, 20, 0); tiny3d_VertexColor(0xFF00FF00);
        tiny3d_VertexPos(20+w, 20, 0); tiny3d_VertexColor(0xFF00FF00);
        tiny3d_VertexPos(20+w, 30, 0); tiny3d_VertexColor(0xFF00FF00);
        tiny3d_VertexPos(20, 30, 0); tiny3d_VertexColor(0xFF00FF00);
        tiny3d_End();

        tiny3d_Flip();
    }

    ioPadEnd();
    tiny3d_Exit();
    sysProcessExit(0);
    return 0;
}
