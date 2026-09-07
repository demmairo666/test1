#pragma once
#include <tiny3d.h>
#include <math.h>

struct Vec3 { float x,y,z; };
struct Camera {
    Vec3 pos = {0, 1.7f, 5.0f}; // рост сталкера 1.7м
    float yaw = 0.0f;   // по горизонтали
    float pitch = 0.0f; // по вертикали
    float fov = 75.0f;

    void addYaw(float d) { yaw += d; }
    void addPitch(float d) { 
        pitch += d; 
        if(pitch > 1.4f) pitch = 1.4f;
        if(pitch < -1.4f) pitch = -1.4f;
    }
    void move(float forward, float strafe) {
        float cosY = cosf(yaw);
        float sinY = sinf(yaw);
        pos.x += sinY * forward + cosY * strafe;
        pos.z += -cosY * forward + sinY * strafe;
        // границы корридора
        if(pos.x < -1.8f) pos.x = -1.8f;
        if(pos.x > 1.8f) pos.x = 1.8f;
        if(pos.z < -9.0f) pos.z = -9.0f;
        if(pos.z > 9.0f) pos.z = 9.0f;
    }
    void applyTransform() {
        MATRIX proj, view;
        MatrixPerspective(&proj, fov * M_PI/180.0f, 1280.0f/720.0f, 0.1f, 100.0f);
        Vec3 target = { pos.x + sinf(yaw)*cosf(pitch), pos.y + sinf(pitch), pos.z - cosf(yaw)*cosf(pitch) };
        Vec3 up = {0,1,0};
        MatrixLookAt(&view, (VECTOR*)&pos, (VECTOR*)&target, (VECTOR*)&up);
        tiny3d_SetProjectionMatrix(&proj);
        tiny3d_SetModelViewMatrix(&view);
    }
};
