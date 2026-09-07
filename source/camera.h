#pragma once
#include <tiny3d.h>
#include <matrix.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct Vec3 { float x,y,z; };
struct Camera {
    Vec3 pos = {0, 1.7f, 5.0f};
    float yaw = 0.0f;
    float pitch = 0.0f;
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
        if(pos.x < -1.8f) pos.x = -1.8f;
        if(pos.x > 1.8f) pos.x = 1.8f;
        if(pos.z < -9.0f) pos.z = -9.0f;
        if(pos.z > 9.0f) pos.z = 9.0f;
    }
    void applyTransform() {
        MATRIX proj = MatrixProjPerspective(fov * M_PI/180.0f, 1280.0f/720.0f, 0.1f, 100.0f);
        VECTOR eye = {pos.x, pos.y, pos.z};
        VECTOR center = { pos.x + sinf(yaw)*cosf(pitch), pos.y + sinf(pitch), pos.z - cosf(yaw)*cosf(pitch) };
        VECTOR up = {0,1,0};
        MATRIX view = MakeLookAtMatrix(eye, center, up);
        tiny3d_SetProjectionMatrix(&proj);
        tiny3d_SetMatrixModelView(&view);
    }
};
