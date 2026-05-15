#ifndef MODEL_DATA_H
#define MODEL_DATA_H

#define WIDTH 100
#define HEIGHT 100
#define INPUT_SIZE (WIDTH * HEIGHT)
// #define HIDDEN_SIZE 32
// #define OUTPUT_SIZE 1

// ===================== 模型权重全部写死在这里 =====================
 float model_w1[INPUT_SIZE][HIDDEN_SIZE] = {
    #include "w1_data.inl"
};

 float model_w2[HIDDEN_SIZE][OUTPUT_SIZE] = {
    #include "w2_data.inl"
};

 float model_b1[HIDDEN_SIZE] = {
    #include "b1_data.inl"
};

 float model_b2[OUTPUT_SIZE] = {
    #include "b2_data.inl"
};

#endif