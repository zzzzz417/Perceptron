# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

```
gcc -o Training/train.exe Training/train.c -lm
```

Run the executable from the `Training/` directory (it writes output to `data/` relative to CWD):

```
cd Training && ./train.exe
```

## Architecture

This is a synthetic training-data generator for what will become a perceptron project. There is no neural network code yet — only shape rendering to PPM images.

- **`Training/train.h`** — Configuration constants (`HEIGHT=100`, `WIDTH=100`) and the `Layer` typedef (`float[HEIGHT][WIDTH]`), which is the core 2D grid type used throughout.
- **`Training/tramath.h`** — Header-only math helpers: `linear_remap` (value rescaling), `clampi` (int clamp), and `PI`.
- **`Training/train.c`** — Shape generation functions and `main`. The `main` function generates 5 PPM images of each shape type (25 total) into `data/`. Shapes are rendered by iterating over their perimeter and setting pixels to `1.0f` on a `0.0f` background. `xy_shake()` adds random jitter for a more realistic look.
- **`Training/data/`** — Output directory for generated `.ppm` binary (P6) images, scaled up 5× via `PPM_SIZE`.

## PPM output format

Binary P6 format. Each pixel is 3 bytes (R, G, B). The value `1.0f` maps to blue `(0, 0, 255)` and `0.0f` maps to yellow `(255, 255, 0)` via `linear_remap` across the min/max of the entire layer.
