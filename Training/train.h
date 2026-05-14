#ifndef TRAIN_H
#define TRAIN_H
//---------------config
#define HEIGHT 100
#define WIDTH 100
//---------------config


#ifdef __cplusplus
extern "C" {
#endif
//------------------extern "C" beg


#if defined(WIDTH) && defined(HEIGHT)

#if WIDTH <= 0
#error "WIDTH can not <= 0\n"
#endif
#if HEIGHT <= 0
#error "HEIGHT can not <= 0\n"
#endif

typedef float Layer[HEIGHT][WIDTH];


#else
#error "could not find defined (WDITH) or (HEIGHT)\n"
#endif


//------------------extern "C" end
#ifdef __cplusplus
}
#endif

#endif //TRAIN_H