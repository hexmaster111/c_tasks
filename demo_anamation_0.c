#include <raylib.h>
#include "task.h"

void main()
{
    InitWindow(400, 400, "Anamation");
    SetTargetFPS(60);
    // struct TaskRunner anamator = {0};

    int state = 0;

    const int starting_blinks = 4;
    const float blink_time = 0.25f;
    int blink_state = 0;
    int times_blinked = 0;
    float blink_time_now = 0.0f;

    const Rectangle rect_top_left = {10,10,10,10};

    while (!WindowShouldClose())
    {
        // TaskRunner_RunTasks(&anamator);

        BeginDrawing();
        ClearBackground(WHITE);

        switch (state)
        {
        case 2:
        /* line extention continues from out streched to closed */
        case 1:
        /* Lines Extending out from top left to attached corners */
        case 0:
        /* blinking currsor */
        if(blink_state && blink_time > blink_time_now ){
            DrawRectangleRec(rect_top_left, BLACK);
            blink_time_now += GetFrameTime();
        } else if(blink_state && blink_time_now > blink_time){
            blink_state = 0;
            blink_time_now = 0.0f;
        }



        }

        DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
}