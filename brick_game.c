#include <furi.h>
#include <gui/gui.h>
#include <gui/view_port.h>
#include <furi_hal.h>

static void draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);
    canvas_draw_str(canvas, 10, 30, "BrickGame is working!");
}

int32_t brick_game_app(void* p) {
    UNUSED(p);

    Gui* gui = furi_record_open("gui");
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, NULL);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    furi_delay_ms(2000);

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close("gui");

    return 0;
}