#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>

#include "train.h"
#include "tramath.h"

#define PPM_SIZE 5
#define SHAKE_SIZE 0.80F
#define HIDDEN_SIZE 32
#define INPUT_SIZE (HEIGHT * WIDTH)
#define OUTPUT_SIZE 1

#define TOTAL_CIRCLE_SAMPLES    20
#define TOTAL_RECTANGLE_SAMPLES 20

// ========== Utility types and functions ==========

typedef enum {
    FILL_FILLED  = 0,
    FILL_OUTLINE = 1,
} FillMode;

void xy_shake(float* x, float* y)
{
    *x += ((float)(rand()) / RAND_MAX - 0.50F) * 2.00F * SHAKE_SIZE;
    *y += ((float)(rand()) / RAND_MAX - 0.50F) * 2.00F * SHAKE_SIZE;
}

int rand_range(int low, int high)
{
    if (low >= high) return low;
    return low + rand() % (high - low + 1);
}

static float rand_float(void)
{
    return (float)rand() / RAND_MAX;
}

static float rand_float_range(float low, float high)
{
    return low + (high - low) * rand_float();
}

static void layer_clear(Layer layer)
{
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            layer[y][x] = 0.0f;
}

static int point_in_convex_polygon(float px, float py, float* vx, float* vy, int n)
{
    int sign = 0;
    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
        float edge_x = vx[j] - vx[i];
        float edge_y = vy[j] - vy[i];
        float cross = edge_x * (py - vy[i]) - edge_y * (px - vx[i]);
        if (cross > 0.001f) {
            if (sign == 0) sign = 1;
            else if (sign == -1) return 0;
        } else if (cross < -0.001f) {
            if (sign == 0) sign = -1;
            else if (sign == 1) return 0;
        }
    }
    return 1;
}

// ========== Circle generator ==========

typedef enum {
    CIRC_ELLIPSE,
    CIRC_OVAL,
    CIRC_DEFORMED,
    CIRC_MULTISTROKE,
    CIRC_SUBTYPE_COUNT
} CircleSubtype;

static void draw_circle_perimeter(Layer layer, int cx, int cy,
                                   float rx, float ry, float rot_angle,
                                   float density)
{
    float cos_rot = cosf(rot_angle);
    float sin_rot = sinf(rot_angle);

    float a = rx, b = ry;
    float perimeter = PI * (3.0f * (a + b) - sqrtf((3.0f * a + b) * (a + 3.0f * b)));
    float step = density / perimeter;

    for (float angle = 0; angle < 2 * PI; angle += step) {
        float ex = rx * cosf(angle);
        float ey = ry * sinf(angle);

        float rxx = ex * cos_rot - ey * sin_rot;
        float ryy = ex * sin_rot + ey * cos_rot;

        float x = cx + rxx;
        float y = cy + ryy;

        xy_shake(&x, &y);

        int px = (int)roundf(x);
        int py = (int)roundf(y);
        if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
            layer[py][px] = 1.0f;
        }
    }
}

static void fill_ellipse_interior(Layer layer, int cx, int cy,
                                   float rx, float ry, float rot_angle)
{
    float cos_rot = cosf(rot_angle);
    float sin_rot = sinf(rot_angle);

    int r_max = (int)ceilf(fmaxf(rx, ry)) + 1;
    int x0 = cx - r_max; if (x0 < 0) x0 = 0;
    int y0 = cy - r_max; if (y0 < 0) y0 = 0;
    int x1 = cx + r_max; if (x1 >= WIDTH) x1 = WIDTH - 1;
    int y1 = cy + r_max; if (y1 >= HEIGHT) y1 = HEIGHT - 1;

    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            float dx = x - cx;
            float dy = y - cy;
            float lx = dx * cos_rot + dy * sin_rot;
            float ly = -dx * sin_rot + dy * cos_rot;
            if ((lx * lx) / (rx * rx) + (ly * ly) / (ry * ry) <= 1.0f) {
                layer[y][x] = 1.0f;
            }
        }
    }
}

void layer_gen_circle(Layer layer, FillMode fill_mode, CircleSubtype subtype)
{
    layer_clear(layer);

    int r_min = 10;
    int r_max_radius = 40;
    int r = rand_range(r_min, r_max_radius);
    float margin = r_max_radius + SHAKE_SIZE * 2.0f + 4.0f;
    int cx = rand_range((int)margin, WIDTH - 1 - (int)margin);
    int cy = rand_range((int)margin, HEIGHT - 1 - (int)margin);

    float rot_angle = rand_float() * 2.0f * PI;

    float rx = r, ry = r;

    switch (subtype) {
        case CIRC_ELLIPSE: {
            float ratio = rand_float_range(0.55f, 0.95f);
            ry = r * ratio;
            draw_circle_perimeter(layer, cx, cy, rx, ry, rot_angle, 1.2f);
            break;
        }
        case CIRC_OVAL: {
            float ratio = rand_float_range(0.55f, 0.95f);
            ry = r * ratio;
            float top_scale    = rand_float_range(0.45f, 0.85f);
            float bot_scale    = rand_float_range(1.10f, 1.60f);

            float a = rx, b = ry;
            float perimeter = PI * (3.0f * (a + b) - sqrtf((3.0f * a + b) * (a + 3.0f * b)));
            float step = 1.2f / perimeter;
            float cos_rot = cosf(rot_angle);
            float sin_rot = sinf(rot_angle);

            for (float angle = 0; angle < 2 * PI; angle += step) {
                float ex = rx * cosf(angle);
                float scale = (angle < PI) ? top_scale : bot_scale;
                float ey = ry * scale * sinf(angle);

                float rxx = ex * cos_rot - ey * sin_rot;
                float ryy = ex * sin_rot + ey * cos_rot;

                float x = cx + rxx;
                float y = cy + ryy;

                xy_shake(&x, &y);

                int px = (int)roundf(x);
                int py = (int)roundf(y);
                if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                    layer[py][px] = 1.0f;
                }
            }

            // For fill, use average scale for approximation
            if (fill_mode == FILL_FILLED) {
                float avg_scale = (top_scale + bot_scale) / 2.0f;
                fill_ellipse_interior(layer, cx, cy, rx, ry * avg_scale, rot_angle);
            }
            return;  // early return — already handled fill above
        }
        case CIRC_DEFORMED: {
            float deform = rand_float_range(0.08f, 0.25f);
            float freq1  = rand_float_range(2.0f, 8.0f);
            float freq2  = rand_float_range(1.0f, 5.0f);
            float phase  = rand_float() * 2 * PI;

            float perimeter = 2 * PI * r;
            float step = 1.2f / perimeter;
            float cos_rot = cosf(rot_angle);
            float sin_rot = sinf(rot_angle);

            for (float angle = 0; angle < 2 * PI; angle += step) {
                float rr = r * (1.0f + deform * (sinf(freq1 * angle + phase)
                                        + 0.5f * cosf(freq2 * angle)));
                float ex = rr * cosf(angle);
                float ey = rr * sinf(angle);

                float rxx = ex * cos_rot - ey * sin_rot;
                float ryy = ex * sin_rot + ey * cos_rot;

                float x = cx + rxx;
                float y = cy + ryy;

                xy_shake(&x, &y);

                int px = (int)roundf(x);
                int py = (int)roundf(y);
                if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                    layer[py][px] = 1.0f;
                }
            }
            break;
        }
        case CIRC_MULTISTROKE: {
            int strokes = 2 + rand() % 3;
            ry = r * rand_float_range(0.65f, 0.95f);

            float cos_rot = cosf(rot_angle);
            float sin_rot = sinf(rot_angle);

            for (int s = 0; s < strokes; s++) {
                float scx = cx + rand_float_range(-3.0f, 3.0f);
                float scy = cy + rand_float_range(-3.0f, 3.0f);
                float sr  = r * rand_float_range(0.88f, 1.08f);
                float arc_start = rand_float() * 2 * PI;
                float arc_end   = arc_start + rand_float_range(PI * 1.2f, 2 * PI);

                float sp = 2 * PI * sr;
                float step = 1.5f / sp;

                for (float angle = arc_start; angle <= arc_end; angle += step) {
                    float ex = sr * cosf(angle);
                    float ey = sr * sinf(angle);

                    float rxx = ex * cos_rot - ey * sin_rot;
                    float ryy = ex * sin_rot + ey * cos_rot;

                    float x = scx + rxx;
                    float y = scy + ryy;

                    xy_shake(&x, &y);

                    int px = (int)roundf(x);
                    int py = (int)roundf(y);
                    if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                        layer[py][px] = 1.0f;
                    }
                }
            }
            break;
        }
    }

    if (fill_mode == FILL_FILLED) {
        fill_ellipse_interior(layer, cx, cy, rx, ry, rot_angle);
    }
}

// ========== Rectangle generator ==========

void layer_gen_rectangle(Layer layer, FillMode fill_mode, int use_multistroke)
{
    layer_clear(layer);

    int hw = rand_range(8, 35);
    int hh = rand_range(8, 35);
    float angle = rand_float() * 2.0f * PI;

    // Compute rotated bounding box to constrain center position
    float cos_a = cosf(angle);
    float sin_a = sinf(angle);
    float corners[4][2] = {
        { hw,  hh}, { hw, -hh}, {-hw, -hh}, {-hw,  hh}
    };
    float x_min = INFINITY, x_max = -INFINITY;
    float y_min = INFINITY, y_max = -INFINITY;
    for (int i = 0; i < 4; i++) {
        float rot_x = corners[i][0] * cos_a - corners[i][1] * sin_a;
        float rot_y = corners[i][0] * sin_a + corners[i][1] * cos_a;
        if (rot_x < x_min) x_min = rot_x;
        if (rot_x > x_max) x_max = rot_x;
        if (rot_y < y_min) y_min = rot_y;
        if (rot_y > y_max) y_max = rot_y;
    }
    float margin = 3.0f + SHAKE_SIZE * 2.0f;
    int cx_min = (int)ceilf(-x_min + margin);
    int cx_max = (int)floorf(WIDTH - 1 - x_max - margin);
    int cy_min = (int)ceilf(-y_min + margin);
    int cy_max = (int)floorf(HEIGHT - 1 - y_max - margin);

    if (cx_min > cx_max) cx_min = cx_max = (cx_min + cx_max) / 2;
    if (cy_min > cy_max) cy_min = cy_max = (cy_min + cy_max) / 2;

    int cx = rand_range(cx_min, cx_max);
    int cy = rand_range(cy_min, cy_max);

    // Vertex positions of the base (unshaken) rectangle, for fill test
    float vx[4], vy[4];
    for (int i = 0; i < 4; i++) {
        vx[i] = cx + corners[i][0] * cos_a - corners[i][1] * sin_a;
        vy[i] = cy + corners[i][0] * sin_a + corners[i][1] * cos_a;
    }

    int strokes = use_multistroke ? (2 + rand() % 2) : 1;

    for (int s = 0; s < strokes; s++) {
        float scx = cx + (use_multistroke ? rand_float_range(-2.5f, 2.5f) : 0);
        float scy = cy + (use_multistroke ? rand_float_range(-2.5f, 2.5f) : 0);
        float shw = hw * (use_multistroke ? rand_float_range(0.92f, 1.06f) : 1.0f);
        float shh = hh * (use_multistroke ? rand_float_range(0.92f, 1.06f) : 1.0f);
        float sa  = angle + (use_multistroke ? rand_float_range(-0.08f, 0.08f) : 0);

        float cs = cosf(sa), ss = sinf(sa);
        float svx[4], svy[4];
        for (int i = 0; i < 4; i++) {
            svx[i] = scx + corners[i][0] * cs - corners[i][1] * ss;
            svy[i] = scy + corners[i][0] * ss + corners[i][1] * cs;
        }

        int edges_to_draw = use_multistroke ? (3 + rand() % 2) : 4;

        for (int e = 0; e < edges_to_draw; e++) {
            int e2 = (e + 1) % 4;
            float x1 = svx[e], y1 = svy[e];
            float x2 = svx[e2], y2 = svy[e2];
            float len = hypotf(x2 - x1, y2 - y1);
            float density = 1.5f;
            int steps = (int)ceilf(len * density);
            if (steps < 2) steps = 2;

            for (int i = 0; i <= steps; i++) {
                float t = (float)i / (float)steps;
                float x = x1 + t * (x2 - x1);
                float y = y1 + t * (y2 - y1);

                xy_shake(&x, &y);

                int px = (int)roundf(x);
                int py = (int)roundf(y);
                if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                    layer[py][px] = 1.0f;
                }
            }
        }
    }

    if (fill_mode == FILL_FILLED) {
        float bx_min = INFINITY, bx_max = -INFINITY;
        float by_min = INFINITY, by_max = -INFINITY;
        for (int i = 0; i < 4; i++) {
            if (vx[i] < bx_min) bx_min = vx[i];
            if (vx[i] > bx_max) bx_max = vx[i];
            if (vy[i] < by_min) by_min = vy[i];
            if (vy[i] > by_max) by_max = vy[i];
        }
        int ix0 = (int)floorf(bx_min); if (ix0 < 0) ix0 = 0;
        int iy0 = (int)floorf(by_min); if (iy0 < 0) iy0 = 0;
        int ix1 = (int)ceilf(bx_max);  if (ix1 >= WIDTH) ix1 = WIDTH - 1;
        int iy1 = (int)ceilf(by_max);  if (iy1 >= HEIGHT) iy1 = HEIGHT - 1;

        for (int y = iy0; y <= iy1; y++) {
            for (int x = ix0; x <= ix1; x++) {
                if (point_in_convex_polygon((float)x, (float)y, vx, vy, 4)) {
                    layer[y][x] = 1.0f;
                }
            }
        }
    }
}

// ========== PPM output ==========

void layer_save_as_ppm(Layer layer, char* file_path)
{
    FILE* f = fopen(file_path, "wb");
    if (f == NULL) {
        fprintf(stderr, "[ERROR] could not open file %s\n", file_path);
        exit(1);
    }

    float kmin = FLT_MAX;
    float kmax = FLT_MIN;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (layer[y][x] > kmax) kmax = layer[y][x];
            if (layer[y][x] < kmin) kmin = layer[y][x];
        }
    }

    fprintf(f, "P6\n%d %d 255\n", WIDTH * PPM_SIZE, HEIGHT * PPM_SIZE);

    for (int y = 0; y < HEIGHT * PPM_SIZE; y++) {
        for (int x = 0; x < WIDTH * PPM_SIZE; x++) {
            float massg = linear_remap(
                layer[y / PPM_SIZE][x / PPM_SIZE],
                kmin, kmax,
                0.00f, 255.00f
            );
            char pixel[3] = {
                (char)(int)(255.00f - massg),
                0,
                (char)(int)(massg)
            };
            fwrite(pixel, sizeof(pixel), 1, f);
        }
    }

    fclose(f);
}

// ========== Main ==========

float w1[INPUT_SIZE][HIDDEN_SIZE];
float w2[HIDDEN_SIZE][OUTPUT_SIZE];
float b1[HIDDEN_SIZE];
float b2[OUTPUT_SIZE];
// #define w1 model_w1
// #define w2 model_w2
// #define b1 model_b1
// #define b2 model_b2

void forward(Layer input, float temp_cla[], float* out)
{
    float in[WIDTH * HEIGHT];
    for(int y = 0; y < HEIGHT; y++){
        for(int x = 0; x < WIDTH; x++){
            in[y * WIDTH + x] = input[y][x];
        }
    }

    for(int h = 0; h < INPUT_SIZE; h++){
        float s = b1[h];
        for(int i = 0; i < INPUT_SIZE; i++){
            s += in[i] * w1[i][h];
        }
        temp_cla[h] = signgam
    }

}



static Layer inputs;

int main(void)
{
    srand(42);  // fixed seed for reproducibility

    char file_path[256];

    printf("--------------------->Done.\n");
    return 0;
}
