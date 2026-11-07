#include "ui.hpp"

void RenderUI(struct nk_context *ctx, struct nk_glfw glfw)
{
    if (nk_begin(ctx, "noob3D",
        nk_rect(50, 50, 300, 200),
        NK_WINDOW_BORDER |
        NK_WINDOW_MOVABLE |
        NK_WINDOW_SCALABLE |
        NK_WINDOW_TITLE))
    {
        nk_layout_row_dynamic(ctx, 30, 1);

        nk_label(ctx, "Hello Nuklear!",
                    NK_TEXT_LEFT);

        if (nk_button_label(ctx, "Click Me"))
        {
            
        }

        nk_layout_row_dynamic(ctx, 30, 1);

        static int value = 50;

        nk_property_int(ctx, "Value:",
            0, &value, 100, 1, 1);
    }

    nk_end(ctx);
}