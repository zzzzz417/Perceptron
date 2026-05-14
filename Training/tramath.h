#ifndef TRAMATH_H
#define TRAMATH_H
#ifdef __cplusplus
extern "C"{
#endif


#include<assert.h>
//-----------------------------
#define PI 3.1415926535f

static float inline linear_remap(float k, float low, float up, float new_low, float new_up){
    assert(k <= up && k >= low && new_low <= new_up); 
    return new_low + (k - low) * (new_up - new_low) / (up - low);
}

static float inline linear_remap_01(float k, float low, float up)
{   
    return linear_remap(k, low, up, 0.0f, 1.0f);
}

static inline int clampi(int x,int low,int high)
{
    if(x < low)x = low;
    if(x > high)x = high;
    return x;
}

//-----------------------------
#ifdef __cplusplus
}
#endif
#endif//TRAMATH_H