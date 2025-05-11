#include <furi.h>
#include <gui/gui.h>
#include <gui/view_port.h>

static void draw(Canvas* canvas, void* ctx) {
    UNUSED(ctx);
    canvas_clear(canvas);
    canvas_draw_str(canvas, 10, 30, "BrickGame OK");
}

int32_t brick_game_app(void* p) {
    UNUSED(p);
    Gui* gui = furi_record_open("gui");
    ViewPort* vp = view_port_alloc();
    view_port_draw_callback_set(vp, draw, NULL);
    gui_add_view_port(gui, vp, GuiLayerFullscreen);
    furi_delay_ms(2000);
    gui_remove_view_port(gui, vp);
    view_port_free(vp);
    furi_record_close("gui");
    return 0;
}