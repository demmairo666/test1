#include <ppu-types.h>
#include <sys/process.h>
#include <sys/spu.h>
#include <io/pad.h>
#include <tiny3d.h>
#include <libfont.h>
#include <math.h>
#include <sysutil/sysutil.h>
#include <sysutil/video.h>

#include "camera.h"
#include "level.h"
#include "weapon.h"

SYS_PROCESS_PARAM(1001, 0x10000)

static bool running = true;

static void sysCallback() { running = false; }

int main() {
    sysUtilRegisterCallback(0, sysCallback, NULL);
    tiny3d_Init(1024*1024);
    ioPadInit(7);

    // Инициализация видео 720p
    videoState state;
    videoGetState(0, 0, &state);
    VideoConfiguration vconfig;
    memset(&vconfig, 0, sizeof(VideoConfiguration));
    vconfig.resolution = VIDEO_RESOLUTION_1280x720;
    vconfig.format = VIDEO_BUFFER_FORMAT_XRGB;
    vconfig.pitch = 1280*4;
    videoConfigure(0, &vconfig, NULL, 0);
    videoSetState(0, VIDEO_STATE_ENABLED, NULL);

    SetFontAlign(FONT_ALIGN_LEFT);
    SetFontColor(0xffffffff, 0x00000000);
    SetCurrentFont(FONT_TTF);

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
                if(paddata.BTN_CROSS) {} // зарезервировано
                // Стики: левый - движение, правый - обзор
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

                // Кнопки
                if(paddata.BTN_TRIANGLE) weapon.flashlightOn = !weapon.flashlightOn;
                if(paddata.BTN_SQUARE) { weapon.shoot(); ioPadSetRumble(i, 1, 1); }
                if(paddata.BTN_CROSS) weapon.reload();
                if(paddata.BTN_START) {} // пауза
                if(paddata.BTN_PS) running = false;
            }
        }

        level.update();
        weapon.update();

        tiny3d_Clear(0xff0a0a12, TINY3D_CLEAR_ALL);
        // 3D сцена
        tiny3d_Project2D(); // сброс для 3D будет ниже через Matrix
        // Настраиваем камеру
        cam.applyTransform();

        // Туман: затемняем дальние стены через цвет вершин (упрощенно)
        level.draw();

        // Свет фонарика - оверлей
        if(weapon.flashlightOn) {
            // луч фонарика как полупрозрачный конус - упрощенно кругом в центре
            tiny3d_Project2D();
            tiny3d_SetPolygon(TINY3D_QUADS);
            // легкое осветление центра
            tiny3d_VertexPos(400, 200, 0); tiny3d_VertexColor(0x10222211);
            tiny3d_VertexPos(880, 200, 0); tiny3d_VertexColor(0x10222211);
            tiny3d_VertexPos(880, 520, 0); tiny3d_VertexColor(0x10222211);
            tiny3d_VertexPos(400, 520, 0); tiny3d_VertexColor(0x10222211);
            tiny3d_End();
            // вернуть 3D не нужно - HUD рисуем в 2D
        }

        // 2D HUD
        tiny3d_Project2D();
        weapon.draw2D();

        // Текст HUD
        SetFontSize(18,18);
        char hud[128];
        sprintf(hud, "STALKER PS3 DEMO | Ammo: %d/%d | Pos: %.1f %.1f | [TRIANGLE] flashlight %s", 
            weapon.ammo, weapon.maxAmmo, cam.pos.x, cam.pos.z, weapon.flashlightOn?"ON":"OFF");
        DrawString(20, 20, hud);
        SetFontSize(14,14);
        DrawString(20, 680, "Left stick move | Right stick look | SQUARE shoot | CROSS reload | PS exit");

        tiny3d_Flip();
    }

    ioPadEnd();
    tiny3d_Exit();
    sysProcessExit(0);
    return 0;
}
