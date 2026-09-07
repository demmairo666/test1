#include <raylib.h>
#include <math.h>

int main(void){
    InitWindow(1280,720,"STALKER PS3 raylib");
    SetTargetFPS(60);
    float px=0, pz=0;
    int ammo=30;
    bool flash=false; int ft=0;
    bool light=true;
    while(!WindowShouldClose()){
        // simple input via raylib gamepad
        if(IsGamepadAvailable(0)){
            float lx = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
            float ly = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
            float rx = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X);
            if(fabs(lx)<0.15f) lx=0;
            if(fabs(ly)<0.15f) ly=0;
            if(fabs(rx)<0.15f) rx=0;
            px += lx*2;
            if(IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT)) light=!light;
            if(IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) ammo=30;
            if(IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT)) { if(ammo>0){ ammo--; flash=true; ft=6; } }
            if(IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT)) break;
        }
        if(ft>0){ ft--; if(ft==0) flash=false; }

        BeginDrawing();
            ClearBackground((Color){10,10,18,255});
            // floor
            DrawRectangle(0,360,1280,360, (Color){42,42,42,255});
            // ceiling
            DrawRectangle(0,0,1280,360, (Color){26,26,26,255});
            // walls
            DrawRectangle(0,0,200,720, (Color){58,58,50,255});
            DrawRectangle(1080,0,200,720, (Color){58,58,50,255});
            // lamp
            DrawRectangle(630,20,20,20, (Color){200,255,120,255});
            if(light){
                DrawRectangle(400,200,480,320, (Color){34,34,17,30});
            }
            DrawRectangle(639,359,2,2, GREEN);
            if(flash) DrawRectangle(880,590,40,20, (Color){255,255,170,255});
            DrawRectangle(700,600,200,50, flash? (Color){255,255,153,255} : (Color){64,64,64,255});
            DrawRectangle(20,20, (int)(200*ammo/30.0f),10, GREEN);
            DrawText("STALKER raylib PS3", 20,40,20, LIGHTGRAY);
            DrawText("Left stick move | Square shoot | Triangle flashlight | Cross reload", 20,680,14, GRAY);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
