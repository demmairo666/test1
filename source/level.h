#pragma once
#include <tiny3d.h>
#include <math.h>

// Простой корридор: пол, потолок, стены + туман как в ЧН
// Вершины в формате tiny3D

struct Level {
    float fogDensity = 0.08f;
    float lampFlicker = 0.0f;
    int tick = 0;

    void update() {
        tick++;
        // мерцание лампы как в Сталкере
        lampFlicker = 0.7f + 0.3f * sinf(tick * 0.17f) + 0.1f * sinf(tick * 0.53f);
        if((tick % 120) == 0 && (rand()%5)==0) lampFlicker *= 0.2f; // случайное моргание
    }

    void draw() {
        // Пол - темный бетон
        tiny3d_SetPolygon(TINY3D_QUADS);
        tiny3d_VertexPos(-2, 0, -10); tiny3d_VertexColor(0xFF2a2a2a); tiny3d_VertexTexture(0,0);
        tiny3d_VertexPos( 2, 0, -10); tiny3d_VertexColor(0xFF2a2a2a); tiny3d_VertexTexture(1,0);
        tiny3d_VertexPos( 2, 0,  10); tiny3d_VertexColor(0xFF2a2a2a); tiny3d_VertexTexture(1,1);
        tiny3d_VertexPos(-2, 0,  10); tiny3d_VertexColor(0xFF2a2a2a); tiny3d_VertexTexture(0,1);
        tiny3d_End();

        // Потолок
        tiny3d_SetPolygon(TINY3D_QUADS);
        u32 ceilCol = 0xFF1a1a1a | ((u32)(lampFlicker*40) << 0);
        tiny3d_VertexPos(-2, 3, -10); tiny3d_VertexColor(ceilCol);
        tiny3d_VertexPos( 2, 3, -10); tiny3d_VertexColor(ceilCol);
        tiny3d_VertexPos( 2, 3,  10); tiny3d_VertexColor(ceilCol);
        tiny3d_VertexPos(-2, 3,  10); tiny3d_VertexColor(ceilCol);
        tiny3d_End();

        // Левая стена
        tiny3d_SetPolygon(TINY3D_QUADS);
        tiny3d_VertexPos(-2, 0, -10); tiny3d_VertexColor(0xFF3a3a32);
        tiny3d_VertexPos(-2, 3, -10); tiny3d_VertexColor(0xFF3a3a32);
        tiny3d_VertexPos(-2, 3,  10); tiny3d_VertexColor(0xFF3a3a32);
        tiny3d_VertexPos(-2, 0,  10); tiny3d_VertexColor(0xFF3a3a32);
        tiny3d_End();

        // Правая стена
        tiny3d_SetPolygon(TINY3D_QUADS);
        tiny3d_VertexPos(2, 0, -10); tiny3d_VertexColor(0xFF3a3a32);
        tiny3d_VertexPos(2, 3, -10); tiny3d_VertexColor(0xFF3a3a32);
        tiny3d_VertexPos(2, 3,  10); tiny3d_VertexColor(0xFF3a3a32);
        tiny3d_VertexPos(2, 0,  10); tiny3d_VertexColor(0xFF3a3a32);
        tiny3d_End();

        // Дальняя стена с знаком радиации
        tiny3d_SetPolygon(TINY3D_QUADS);
        tiny3d_VertexPos(-2, 0, -10); tiny3d_VertexColor(0xFF4a4a3a);
        tiny3d_VertexPos( 2, 0, -10); tiny3d_VertexColor(0xFF4a4a3a);
        tiny3d_VertexPos( 2, 3, -10); tiny3d_VertexColor(0xFF4a4a3a);
        tiny3d_VertexPos(-2, 3, -10); tiny3d_VertexColor(0xFF4a4a3a);
        tiny3d_End();

        // Лампа на потолке (эмиссивный квад)
        tiny3d_SetPolygon(TINY3D_QUADS);
        u32 lampCol = 0xFF000000 | ((u32)(lampFlicker*255) << 8) | ((u32)(lampFlicker*200) << 16) | ((u32)(lampFlicker*120));
        float lx = 0, ly = 2.9f, lz = 0;
        tiny3d_VertexPos(lx-0.2f, ly, lz-0.2f); tiny3d_VertexColor(lampCol);
        tiny3d_VertexPos(lx+0.2f, ly, lz-0.2f); tiny3d_VertexColor(lampCol);
        tiny3d_VertexPos(lx+0.2f, ly, lz+0.2f); tiny3d_VertexColor(lampCol);
        tiny3d_VertexPos(lx-0.2f, ly, lz+0.2f); tiny3d_VertexColor(lampCol);
        tiny3d_End();
    }
};
