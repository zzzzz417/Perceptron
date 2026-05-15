
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "train.h"
#include "tramath.h"
#include <float.h>

#define PPM_SIZE 5
#define SHAKE_SIZE 0.80F
void xy_shake(float* x, float* y)
{
    *x += ((float)(rand()) / RAND_MAX - 0.50F) * 2.00F * SHAKE_SIZE;
    *y += ((float)(rand()) / RAND_MAX - 0.50F) * 2.00F * SHAKE_SIZE;
}

int rand_range(int low, int high)
{
    assert(low <= high);
    if(low == high)return low;
    high += 1;
    return low + rand() % (high - low);
}

void layer_fill_std_rect(Layer layer, int y, int x, int h, int w, float value)
{
    assert(w>0);
    assert(h>0);

    int x0 = clampi(x, 0, WIDTH - 1);
    int y0 = clampi(y, 0, HEIGHT - 1);
    int x1 = clampi(x0 - 1 + w ,x0 ,WIDTH - 1);    
    int y1 = clampi(y0 - 1 + h ,y0 ,HEIGHT - 1);    
    for(int y = y0;y <= y1; y++){
        for(int x = x0;x <= x1; x++){
            layer[y][x] = value;
        }
    }
}

void layer_fill_std_cricle(Layer layer, int cx, int cy,int r,float value)
{
    assert(r>0);
    int x0 = clampi(cx - r, 0, WIDTH - 1);
    int y0 = clampi(cy - r, 0, HEIGHT - 1);
    int x1 = clampi(cx + r, 0, WIDTH - 1);
    int y1 = clampi(cy + r, 0, HEIGHT - 1);
    for(int y = y0;y <= y1; y++){
        for(int x = x0;x <= x1; x++){
            float dx = x - cx;
            float dy = y - cy;
            if(dy*dy + dx*dx <= r*r){
                layer[y][x] = value;
            }
        }
    }  
}

static void rand_circle_params(int *cx, int *cy, int *r)
{
    *cx = rand_range(0, WIDTH-1);
    *cy = rand_range(0, HEIGHT-1);
    int r_max = INT_MAX;
    if(r_max > *cx) r_max = *cx;
    if(r_max > *cy) r_max = *cy;
    if(r_max > HEIGHT - *cy) r_max = HEIGHT - *cy;
    if(r_max > WIDTH - *cx) r_max = WIDTH - *cx;
    if(r_max < 1) r_max = 1;
    *r = rand_range(1, r_max);
}

void layer_fill_ellipse(Layer layer)
{
    layer_fill_std_rect(layer, 0, 0, HEIGHT, WIDTH, 0.00f);
    int cx, cy, r;
    rand_circle_params(&cx, &cy, &r);

    float ratio = 0.6f + 0.8f * rand() / RAND_MAX;
    float rx = r;
    float ry = r * ratio;
    float rot_angle = (float)rand() / RAND_MAX * 2 * PI;

    float cos_rot = cos(rot_angle);
    float sin_rot = sin(rot_angle);

    float a = rx, b = ry;
    float perimeter = PI * (3.0f * (a + b) - sqrtf((3.0f * a + b) * (a + 3.0f * b)));
    const float DENSITY = 1.2f;
    float STEP = DENSITY / perimeter;

    for (float angle = 0; angle < 2 * PI; angle += STEP) {
        float ex = rx * cos(angle);
        float ey = ry * sin(angle);

        float rotated_x = ex * cos_rot - ey * sin_rot;
        float rotated_y = ex * sin_rot + ey * cos_rot;

        float x = cx + rotated_x;
        float y = cy + rotated_y;

        xy_shake(&x, &y);

        int px = (int)roundf(x);
        int py = (int)roundf(y);

        if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
            layer[py][px] = 1.0f;
        }
    }
}

void layer_fill_oval(Layer layer)
{
    layer_fill_std_rect(layer, 0, 0, HEIGHT, WIDTH, 0.00f);
    int cx, cy, r;
    rand_circle_params(&cx, &cy, &r);

    float rx = r;
    float ry = r * (0.55f + 0.55f * rand() / RAND_MAX);
    float rot_angle = (float)rand() / RAND_MAX * 2 * PI;
    float cos_rot = cos(rot_angle);
    float sin_rot = sin(rot_angle);

    float top_scale = 0.50f + 0.35f * rand() / RAND_MAX;
    float bot_scale = 1.10f + 0.50f * rand() / RAND_MAX;

    float a = rx, b = ry;
    float perimeter = PI * (3.0f * (a + b) - sqrtf((3.0f * a + b) * (a + 3.0f * b)));
    const float DENSITY = 1.2f;
    float STEP = DENSITY / perimeter;

    for (float angle = 0; angle < 2 * PI; angle += STEP) {
        float ex = rx * cos(angle);
        float scale = (angle < PI) ? top_scale : bot_scale;
        float ey = ry * scale * sin(angle);

        float rotated_x = ex * cos_rot - ey * sin_rot;
        float rotated_y = ex * sin_rot + ey * cos_rot;

        float x = cx + rotated_x;
        float y = cy + rotated_y;

        xy_shake(&x, &y);

        int px = (int)roundf(x);
        int py = (int)roundf(y);
        if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
            layer[py][px] = 1.0f;
        }
    }
}

void layer_random_rect(Layer layer)
{
    layer_fill_std_rect(layer, 0, 0, HEIGHT, WIDTH, 0.00f);
    int x = rand_range(0, WIDTH-2);
    int y = rand_range(0, HEIGHT-2);
    int w = rand_range(1, WIDTH - x);
    int h = rand_range(1, HEIGHT - y);
    layer_fill_std_rect(layer, y, x, h, w, 1.0f);
}

void layer_random_cricle(Layer layer)
{
    layer_fill_std_rect(layer, 0, 0, HEIGHT, WIDTH, 0.00f);
    int cx, cy, r;
    rand_circle_params(&cx, &cy, &r);

    int x0 = clampi(cx - r, 0, WIDTH - 1);
    int y0 = clampi(cy - r, 0, HEIGHT - 1);
    int x1 = clampi(cx + r, 0, WIDTH - 1);
    int y1 = clampi(cy + r, 0, HEIGHT - 1);
    for (int y = y0; y <= y1; y++) {
        for (int x = x0; x <= x1; x++) {
            float dx = x - cx;
            float dy = y - cy;
            if (dy*dy + dx*dx <= r*r) {
                layer[y][x] = 1.0f;
            }
        }
    }
}

void layer_fill_deformed_circle(Layer layer)
{
    layer_fill_std_rect(layer, 0, 0, HEIGHT, WIDTH, 0.00f);
    int cx, cy, r;
    rand_circle_params(&cx, &cy, &r);

    float deform = 0.08f + 0.22f * rand() / RAND_MAX;
    float freq1 = 2.0f + 6.0f * rand() / RAND_MAX;
    float freq2 = 1.0f + 4.0f * rand() / RAND_MAX;
    float phase = (float)rand() / RAND_MAX * 2 * PI;

    float perimeter = 2 * PI * r;
    const float DENSITY = 1.0f;
    float STEP = DENSITY / perimeter;

    for (float angle = 0; angle < 2 * PI; angle += STEP) {
        float rr = r * (1.0f + deform * (sinf(freq1 * angle + phase) + 0.5f * cosf(freq2 * angle)));
        float x = cx + rr * cosf(angle);
        float y = cy + rr * sinf(angle);

        xy_shake(&x, &y);

        int px = (int)roundf(x);
        int py = (int)roundf(y);
        if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
            layer[py][px] = 1.0f;
        }
    }
}

void layer_fill_handdrawn_circle(Layer layer)
{
    layer_fill_std_rect(layer, 0, 0, HEIGHT, WIDTH, 0.00f);
    int cx, cy, r;
    rand_circle_params(&cx, &cy, &r);

    int strokes = 2 + rand() % 4;

    float perimeter = 2 * PI * r;
    const float DENSITY = 1.5f;
    float STEP = DENSITY / perimeter;

    for (int s = 0; s < strokes; s++) {
        float scx = cx + ((float)rand() / RAND_MAX - 0.5f) * 2.0f * SHAKE_SIZE * 2.5f;
        float scy = cy + ((float)rand() / RAND_MAX - 0.5f) * 2.0f * SHAKE_SIZE * 2.5f;
        float sr = r * (0.90f + 0.20f * rand() / RAND_MAX);

        for (float angle = 0; angle < 2 * PI; angle += STEP) {
            float x = scx + sr * cosf(angle);
            float y = scy + sr * sinf(angle);

            xy_shake(&x, &y);

            int px = (int)roundf(x);
            int py = (int)roundf(y);
            if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
                layer[py][px] = 1.0f;
            }
        }
    }
}

void layer_save_as_ppm(Layer layer, char* file_path)
{
    FILE* f = fopen(file_path, "wb");
    if(f == NULL){
        fprintf(stderr, "[ERROR] could not open file %s :\n %m \n", file_path);
        exit(1);
    }

    float kmin = FLT_MAX;
    float kmax = FLT_MIN;
    for(int y = 0; y < HEIGHT; y++){
        for(int x = 0;x < WIDTH; x++){
            if(layer[y][x] > kmax)kmax = layer[y][x];
            if(layer[y][x] < kmin)kmin = layer[y][x];
        }
    }

    // printf("kmax == %.2f, kmin == %.2f\n", kmax, kmin);

    fprintf(f, "P6\n%d %d 255\n", WIDTH * PPM_SIZE, HEIGHT * PPM_SIZE);

    for(int y = 0; y < HEIGHT * PPM_SIZE; y++){
        for(int x = 0; x < WIDTH * PPM_SIZE; x++){
            float massg = linear_remap(
                layer[y / PPM_SIZE][x / PPM_SIZE],
                kmin,
                kmax,
                0.00f,
                255.00f
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

static Layer inputs;

int main(){

    // for(int y = 0; y < HEIGHT; y++){
    //     for(int x = 0; x < WIDTH; x++){
    //         inputs[y][x] = 0.1f;
    //     }
    // }

    // inputs[0][0] = 0.0f;
    char file_path[256];
    int idx = 0;

    for (int i = 0; i < 5; i++) {
        snprintf(file_path, sizeof(file_path), "data/ellipse%02d.ppm", idx);
        layer_fill_ellipse(inputs);
        layer_save_as_ppm(inputs, file_path);
        printf("[INFO] ellipse   %02d\n", idx++);
    }
    for (int i = 0; i < 5; i++) {
        snprintf(file_path, sizeof(file_path), "data/oval%02d.ppm", idx);
        layer_fill_oval(inputs);
        layer_save_as_ppm(inputs, file_path);
        printf("[INFO] oval      %02d\n", idx++);
    }
    for (int i = 0; i < 5; i++) {
        snprintf(file_path, sizeof(file_path), "data/circle%02d.ppm", idx);
        layer_random_cricle(inputs);
        layer_save_as_ppm(inputs, file_path);
        printf("[INFO] circle    %02d\n", idx++);
    }
    for (int i = 0; i < 5; i++) {
        snprintf(file_path, sizeof(file_path), "data/deformed%02d.ppm", idx);
        layer_fill_deformed_circle(inputs);
        layer_save_as_ppm(inputs, file_path);
        printf("[INFO] deformed  %02d\n", idx++);
    }
    for (int i = 0; i < 5; i++) {
        snprintf(file_path, sizeof(file_path), "data/handdrawn%02d.ppm", idx);
        layer_fill_handdrawn_circle(inputs);
        layer_save_as_ppm(inputs, file_path);
        printf("[INFO] handdrawn %02d\n", idx++);
    }

    printf("--------------------->Done!\n");
    return 0;
}

