# 训练数据生成 API

## 数据类型

| 类型 | 定义 | 说明 |
|------|------|------|
| `Layer` | `typedef float Layer[HEIGHT][WIDTH]` (100×100) | 绘图画布, `1.0`=前景 `0.0`=背景 |
| `FillMode` | `enum { FILL_FILLED=0, FILL_OUTLINE=1 }` | 实心 / 轮廓 |
| `CircleSubtype` | `enum { CIRC_ELLIPSE, CIRC_OVAL, CIRC_DEFORMED, CIRC_MULTISTROKE, CIRC_SUBTYPE_COUNT }` | 圆类变形策略 |

## 生成器

### `layer_gen_circle`

```c
void layer_gen_circle(Layer layer, FillMode fill, CircleSubtype subtype);
```

生成一个圆类形状。先清空画布，随机位置和半径(10~40px)，保证完整在画布内。所有边缘像素经过 `xy_shake()` 抖动。

| subtype | 效果 |
|---------|------|
| `CIRC_ELLIPSE` | 椭圆, ry/rx=0.55~0.95, 随机旋转 |
| `CIRC_OVAL` | 卵形, 上半缩放0.45~0.85, 下半缩放1.10~1.60 |
| `CIRC_DEFORMED` | 变形圆, 正弦调制半径振幅0.08~0.25 |
| `CIRC_MULTISTROKE` | 2~4笔偏移弧线叠加, 中心偏移±3px |

`FILL_FILLED` 时在边缘外额外交叉积填充内部(卵形以平均缩放近似)。

---

### `layer_gen_rectangle`

```c
void layer_gen_rectangle(Layer layer, FillMode fill, int multistroke);
```

生成一个矩形。随机中心、半宽(8~35px)、半高(8~35px)、旋转角(0~2π)。通过旋转包围盒约束位置，保证完整在画布内。

| multistroke | 效果 |
|-------------|------|
| `0` (clean) | 1笔画, 4条边完整 |
| `1` (handdrawn) | 2~3笔画叠加, 中心/尺寸/角度独立偏移, 可能缺1边 |

`FILL_FILLED` 时以叉积法逐像素填充主顶点围成的凸多边形。

---

## 输出

### `layer_save_as_ppm`

```c
void layer_save_as_ppm(Layer layer, char* file_path);
```

将 Layer 输出为 PPM P6 二值图像。自动缩放到 500×500 (`PPM_SIZE=5`)。值域 min→max 映射为颜色 (低值=红黄色, 高值=蓝)。

---

## 抖动

### `xy_shake`

```c
void xy_shake(float* x, float* y);
```

对坐标添加 ±0.80px 均匀随机抖动。所有边缘像素经过此函数，模拟手绘/鼠标抖动。

---

## 配置 (宏)

| 宏 | 值 | 说明 |
|----|----|------|
| `HEIGHT` `WIDTH` | 100 | 画布尺寸 (train.h) |
| `PPM_SIZE` | 5 | PPM 输出缩放倍率 |
| `SHAKE_SIZE` | 0.80F | 抖动幅度 (px) |
