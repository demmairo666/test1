#pragma once
#include <tiny3d.h>

struct Weapon {
    int ammo = 30;
    int maxAmmo = 30;
    bool flash = false;
    int flashTimer = 0;
    bool flashlightOn = true;
    void shoot() {
        if(ammo <= 0) return;
        ammo--;
        flash = true;
        flashTimer = 4;
    }
    void reload() { ammo = maxAmmo; }
    void update() {
        if(flashTimer > 0) { flashTimer--; if(flashTimer==0) flash=false; }
    }
    void draw2D() {
        // crosshair as small quad
        tiny3d_SetPolygon(TINY3D_QUADS);
        tiny3d_VertexPos(639, 359, 0); tiny3d_VertexColor(0xFF00FF00);
        tiny3d_VertexPos(641, 359, 0); tiny3d_VertexColor(0xFF00FF00);
        tiny3d_VertexPos(641, 361, 0); tiny3d_VertexColor(0xFF00FF00);
        tiny3d_VertexPos(639, 361, 0); tiny3d_VertexColor(0xFF00FF00);
        tiny3d_End();
        u32 col = flash ? 0xFFFFFF99 : 0xFF404040;
        tiny3d_SetPolygon(TINY3D_QUADS);
        tiny3d_VertexPos(700, 600, 0); tiny3d_VertexColor(col);
        tiny3d_VertexPos(900, 600, 0); tiny3d_VertexColor(col);
        tiny3d_VertexPos(900, 650, 0); tiny3d_VertexColor(col);
        tiny3d_VertexPos(700, 650, 0); tiny3d_VertexColor(col);
        tiny3d_End();
        if(flash) {
            tiny3d_SetPolygon(TINY3D_QUADS);
            tiny3d_VertexPos(880, 590, 0); tiny3d_VertexColor(0xFFFFFFAA);
            tiny3d_VertexPos(920, 590, 0); tiny3d_VertexColor(0xFFFFFFAA);
            tiny3d_VertexPos(920, 610, 0); tiny3d_VertexColor(0xFFFFFFAA);
            tiny3d_VertexPos(880, 610, 0); tiny3d_VertexColor(0xFFFFFFAA);
            tiny3d_End();
        }
    }
};
