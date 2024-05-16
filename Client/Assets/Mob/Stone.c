#include <Client/Assets/Render.h>

#include <Client/Renderer/Renderer.h>

void rr_stone_draw(struct rr_renderer *renderer)
{       
            rr_renderer_scale(renderer, .8);
            rr_renderer_set_fill(renderer, 0xff606060);
            rr_renderer_begin_path(renderer);
            rr_renderer_move_to(renderer, -51.08, -54.77);
            rr_renderer_line_to(renderer, 13.74, -69.95);
            rr_renderer_line_to(renderer, 72.00, -25.23);
            rr_renderer_line_to(renderer, 66.26, 33.85);
            rr_renderer_line_to(renderer, 19.49, 69.95);
            rr_renderer_line_to(renderer, -42.26, 56.50);
            rr_renderer_line_to(renderer, -72.00, 6.36);
            rr_renderer_fill(renderer);
            rr_renderer_set_fill(renderer, 0xff777777);
            rr_renderer_begin_path(renderer);
            rr_renderer_move_to(renderer, -32.09, 42.41);
            rr_renderer_line_to(renderer, -55.97, 5.87);
            rr_renderer_line_to(renderer, -40.75, -41.54);
            rr_renderer_line_to(renderer, 11.02, -54.16);
            rr_renderer_line_to(renderer, 57.56, -18.49);
            rr_renderer_line_to(renderer, 51.47, 28.05);
            rr_renderer_line_to(renderer, 14.93, 54.15);
            rr_renderer_fill(renderer);
            rr_renderer_set_fill(renderer, 0xff84785c);
            rr_renderer_begin_path(renderer);
            rr_renderer_move_to(renderer, -42.48, -8.00);
            rr_renderer_bezier_curve_to(renderer, -42.48, -10.51, -40.44,
                                        -12.54, -37.93, -12.54);
            rr_renderer_bezier_curve_to(renderer, -36.73, -12.54, -35.57,
                                        -12.07, -34.72, -11.21);
            rr_renderer_bezier_curve_to(renderer, -33.87, -10.36, -33.39, -9.20,
                                        -33.39, -8.00);
            rr_renderer_bezier_curve_to(renderer, -33.39, -5.49, -35.42, -3.45,
                                        -37.93, -3.45);
            rr_renderer_bezier_curve_to(renderer, -40.44, -3.45, -42.48, -5.49,
                                        -42.48, -8.00);
            rr_renderer_fill(renderer);
}
