#include <raylib.h>
#include "task.h"
#include <stdlib.h>

typedef struct SquareResizingAnamation
{
    Rectangle *obj;

    int state;
    float trans, speed, distance;
} SquareResizeContext;

enum
{
    SRT_STATE_0,
    SRT_STATE_1,
    SRT_STATE_2,
    SRT_STATE_3,
    SRT_MAX
};

void SquareResizeTask(struct TaskRunner *run, struct TaskInfo *self, void *data)
{
    SquareResizeContext *ctx = data;

    switch (ctx->state)
    {
        // clang-format off
    case SRT_STATE_0: ctx->obj->x+=1; ctx->trans += ctx->speed; break;
    case SRT_STATE_1: ctx->obj->x-=1; ctx->trans += ctx->speed; break;
    case SRT_STATE_2: ctx->obj->y+=1; ctx->trans += ctx->speed; break;
    case SRT_STATE_3: ctx->obj->y-=1; ctx->trans += ctx->speed; break;
        // clang-format on
    }

    // TraceLog(LOG_INFO, "%d %.2f", ctx->state, ctx->trans);

    if (ctx->trans >= ctx->distance)
    {
        ctx->state += 1;
        ctx->state %= SRT_MAX;

        ctx->trans = 0;
    }
}

#define SQUARES (25)

#define map(VAL, fromLow, fromHigh, toLow, toHigh) \
    ((VAL - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow)

float GetRandomFloat(float min, float max)
{
    if (min > max)
    {
        float tmp = max;
        max = min;
        min = tmp;
    }

    int r = GetRandomValue(0, RAND_MAX);
    return r * (max - min) / RAND_MAX + min;
}

void main()
{
    InitWindow(800, 600, "Anamation");
    SetTargetFPS(60);
    struct TaskRunner anamator = {0};

    Rectangle rect[SQUARES];
    struct TaskInfo task[SQUARES];
    SquareResizeContext ctx[SQUARES];
    Color rcolor[SQUARES];

    for (int i = 0; i < SQUARES; i++)
    {
        ctx[i] = (SquareResizeContext){
            .obj = &rect[i],
            .distance = GetRandomFloat(0.5f, 1.5f), // normaly 0-1
            .speed = GetRandomFloat(0.001f, 0.01f),     // .01 - .001 ~
            .trans = GetRandomFloat(0, 1),          // spot in anamation
            .state = GetRandomValue(0, SRT_MAX - 1) // 0 - SRT_MAX-1
        };

        rect[i] = (Rectangle){
            .x = GetRandomValue(0, GetScreenWidth()),
            .y = GetRandomValue(0, GetScreenHeight()),
            .width = 100,
            .height = 100,
        };

        rcolor[i] = (Color){
            .a = GetRandomValue(128, 255),
            .r = GetRandomValue(0, 255),
            .g = GetRandomValue(0, 255),
            .b = GetRandomValue(0, 255),
        };

        task[i] = (struct TaskInfo){
            .method = SquareResizeTask,
            .task_state = &ctx[i],
            .task_resault = 0,
        };

        TaskRunner_AddTask(&anamator, &task[i]);
    }

    while (!WindowShouldClose())
    {
        TaskRunner_RunTasks(&anamator);

        BeginDrawing();
        ClearBackground(WHITE);
        // DrawRectangleRec(rect, BLACK);
        // DrawRectangleRec(rect2, BLACK);
        for (int i = 0; i < SQUARES; i++)
        {
            DrawRectangleRec(rect[i], rcolor[i]);
        }

        DrawFPS(10, 10);

        EndDrawing();
    }

    CloseWindow();
}