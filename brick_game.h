#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <furi_hal.h>
#include <stdbool.h>

typedef struct {
    int x, y;
    int width;
} Paddle;

typedef struct {
    int x, y;
    int vx, vy;
} Ball;

typedef struct {
    int x, y;
    bool destroyed;
} Brick;

#define MAX_BRICKS 10

typedef struct {
    Paddle paddle;
    Ball ball;
    Brick bricks[MAX_BRICKS];
    int score;
    bool game_over;
    bool victory;
} AppState;