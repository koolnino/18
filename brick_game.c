#include "brick_game.h"

static void draw(Canvas* canvas, void* ctx) {
    AppState* state = ctx;

    canvas_clear(canvas);

    if(state->game_over) {
        canvas_draw_str(canvas, 30, 30, "GAME OVER");
        char final_score[20];
        snprintf(final_score, sizeof(final_score), "Score: %d", state->score);
        canvas_draw_str(canvas, 30, 45, final_score);
        return;
    }

    if(state->victory) {
        canvas_draw_str(canvas, 30, 30, "YOU WIN!");
        char final_score[20];
        snprintf(final_score, sizeof(final_score), "Score: %d", state->score);
        canvas_draw_str(canvas, 30, 45, final_score);
        return;
    }

    canvas_draw_box(canvas, state->paddle.x, state->paddle.y, state->paddle.width, 3);
    canvas_draw_dot(canvas, state->ball.x, state->ball.y);

    for(int i = 0; i < MAX_BRICKS; ++i) {
        if(!state->bricks[i].destroyed) {
            canvas_draw_box(canvas, state->bricks[i].x, state->bricks[i].y, 10, 5);
        }
    }

    char score_str[20];
    snprintf(score_str, sizeof(score_str), "Score: %d", state->score);
    canvas_draw_str(canvas, 2, 10, score_str);
}

static void input_callback(InputEvent* event, void* ctx) {
    AppState* state = ctx;
    if(state->game_over || state->victory) return;

    if(event->type == InputTypePress || event->type == InputTypeRepeat) {
        if(event->key == InputKeyLeft && state->paddle.x > 0) {
            state->paddle.x -= 5;
        } else if(event->key == InputKeyRight && state->paddle.x + state->paddle.width < 128) {
            state->paddle.x += 5;
        }
    }
}

static void play_sound(bool destroy) {
    if(destroy) {
        furi_hal_pwm_start(2000, 50);
        furi_delay_ms(100);
        furi_hal_pwm_stop();
    } else {
        furi_hal_pwm_start(1000, 30);
        furi_delay_ms(30);
        furi_hal_pwm_stop();
    }
}

static bool all_bricks_destroyed(AppState* state) {
    for(int i = 0; i < MAX_BRICKS; ++i) {
        if(!state->bricks[i].destroyed) return false;
    }
    return true;
}

int32_t brick_game_app(void* p) {
    UNUSED(p);

    AppState state = {
        .paddle = {.x = 50, .y = 60, .width = 20},
        .ball = {.x = 60, .y = 30, .vx = 1, .vy = -1},
        .score = 0,
        .game_over = false,
        .victory = false
    };

    for(int i = 0; i < MAX_BRICKS; ++i) {
        state.bricks[i].x = 10 + i * 12;
        state.bricks[i].y = 20;
        state.bricks[i].destroyed = false;
    }

    Gui* gui = furi_record_open("gui");
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw, &state);
    view_port_input_callback_set(view_port, input_callback, &state);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    while(1) {
        if(state.game_over || state.victory) {
            view_port_update(view_port);
            furi_delay_ms(100);
            continue;
        }

        state.ball.x += state.ball.vx;
        state.ball.y += state.ball.vy;

        if(state.ball.x <= 0 || state.ball.x >= 127) {
            state.ball.vx *= -1;
            play_sound(false);
        }
        if(state.ball.y <= 0) {
            state.ball.vy *= -1;
            play_sound(false);
        }

        if(state.ball.y >= state.paddle.y - 2 &&
           state.ball.x >= state.paddle.x &&
           state.ball.x <= state.paddle.x + state.paddle.width) {
            state.ball.vy *= -1;
            play_sound(false);
        }

        for(int i = 0; i < MAX_BRICKS; ++i) {
            if(!state.bricks[i].destroyed &&
               state.ball.x >= state.bricks[i].x &&
               state.ball.x <= state.bricks[i].x + 10 &&
               state.ball.y >= state.bricks[i].y &&
               state.ball.y <= state.bricks[i].y + 5) {
                state.bricks[i].destroyed = true;
                state.ball.vy *= -1;
                state.score += 10;
                play_sound(true);
            }
        }

        if(all_bricks_destroyed(&state)) {
            state.victory = true;
        }

        if(state.ball.y > 64) {
            state.game_over = true;
        }

        view_port_update(view_port);
        furi_delay_ms(50);
    }

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close("gui");

    return 0;
}