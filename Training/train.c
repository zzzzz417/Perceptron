
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "train.h"
#include "tramath.h"
#include "float.h"

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

void layer_fill_ellipse(Layer layer)
{
    layer_fill_std_rect(layer, 0, 0, HEIGHT, WIDTH, 0.00f);
    int cx = rand_range(0, WIDTH-1);
    int cy = rand_range(0, HEIGHT-1);
    int r_max = INT_MAX;
    if(r_max > cx) r_max = cx;
    if(r_max > cy) r_max = cy;
    if(r_max > HEIGHT-cy) r_max = HEIGHT-cy;
    if(r_max > WIDTH-cx) r_max = WIDTH-cx;
    if(r_max < 1) r_max = 1;
    int r = rand_range(1, r_max);

    // const float STEP = 0.02f;

    float ratio = 0.6f + 1.0f * rand() / RAND_MAX * (0.8f);
    float rx = r;
    float ry = r * ratio;
    float rot_angle = (float)rand() / RAND_MAX * 2 * PI;

    // 预计算旋转矩阵的 sin/cos
    float cos_rot = cos(rot_angle);
    float sin_rot = sin(rot_angle);

    float a = rx, b = ry;
    float perimeter = PI * (3.0f * (a + b) - sqrtf((3.0f * a + b) * (a + 3.0f * b)));
    
    // 目标：圆周上每隔1个像素画一个点（可调整密度）
    const float DENSITY = 1.2f; // 越小越密，1.0=像素级连续，>1留空隙
    float STEP = DENSITY / perimeter; // 角度步长 = 弧长/周长 = 像素间距/周长

    for (float angle = 0; angle < 2 * PI; angle += STEP) {
        // 1. 先在原点生成椭圆
        float ex = rx * cos(angle);
        float ey = ry * sin(angle);
        
        // 2. 旋转椭圆
        float rotated_x = ex * cos_rot - ey * sin_rot;
        float rotated_y = ex * sin_rot + ey * cos_rot;
        
        // 3. 平移到中心
        float x = cx + rotated_x;
        float y = cy + rotated_y;
        
        // 4. 抖动（可选）
        xy_shake(&x, &y);

        // 5. 取整 + 边界裁剪
        int px = (int)roundf(x);
        int py = (int)roundf(y);

        // 边界检查保证全在layer内
        if (px >= 0 && px < WIDTH && py >= 0 && py < HEIGHT) {
            layer[py][px] = 1.0f;
        }
    }
}

void layer_fill_oval()
{

}

void layer_random_rect()
{

}

void layer_random_cricle(Layer layer)
{
    layer_fill_std_rect(layer, 0, 0, HEIGHT, WIDTH, 0.00f);
    int cx = rand_range(0, WIDTH-1);
    int cy = rand_range(0, HEIGHT-1);
    int r_max = INT_MAX;
    if(r_max > cx) r_max = cx;
    if(r_max > cy) r_max = cy;
    if(r_max > HEIGHT-cy) r_max = HEIGHT-cy;
    if(r_max > WIDTH-cx) r_max = WIDTH-cx;
    if(r_max < 1) r_max = 1;
    int r = rand_range(1, r_max);
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
    for(int i = 0; i < 20; i++){
        snprintf(file_path, sizeof(file_path), "data/text%02d.ppm", i);
        layer_fill_ellipse(inputs);
        layer_save_as_ppm(inputs, file_path);
        printf("[INFO] preduced i%02d\n",i); 
    }

    printf("--------------------->Done!\n");
    return 0;
}

