#ifndef HTML_H
#define HTML_H

const char* html_content = R"HTML(
<!DOCTYPE html>
<html lang='en'>
<head>
    <meta charset='utf-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1'>
    <title>Shape Lab</title>
    <link rel='stylesheet' href='/style.css'>
</head>
<body>

<section class="app">
    <header class="head">
        <h1>Shape Lab</h1>
        <p>Draw a shape. Let the perceptron decide.</p>
    </header>

    <div class="canvas-frame">
        <canvas id="drawCanvas" width="340" height="340"></canvas>
    </div>
    <canvas id="dataCanvas" width="100" height="100" hidden></canvas>

    <div class="controls">
        <div class="brush-sizes" id="brushGroup">
            <button class="bs active" data-size="1"><i></i></button>
            <button class="bs" data-size="2"><i></i></button>
            <button class="bs" data-size="3"><i></i></button>
        </div>
        <span class="sep"></span>
        <button class="act" id="eraserBtn">eraser</button>
        <button class="act" id="clearBtn">clear</button>
    </div>

    <button class="primary" id="detectBtn">Identify</button>

    <div class="result" id="resultsPanel">
        <div class="gauge">
            <div class="gauge-track">
                <div class="gauge-fill" id="resultFill"></div>
                <div class="gauge-pin" id="resultMarker"></div>
            </div>
            <div class="gauge-labels">
                <span>Circle</span><span>Rectangle</span>
            </div>
        </div>
        <div class="reading" id="resultValue"></div>
        <div class="hint">draw something to begin</div>
    </div>
</section>

<script src='/script.js'></script>
</body>
</html>

)HTML";

const char* css_content = R"CSS(
/* ===== Shape Lab — Minimal ===== */

:root {
    --bg:      #fafaf8;
    --surface: #ffffff;
    --border:  #e8e5e0;
    --text:    #1a1a1a;
    --muted:   #8c8c88;
    --accent:  #2c3e6e;
    --accent-hover: #1a274a;
    --green:   #4a8c6a;
    --red:     #b85c4a;
    --radius:  8px;
    --font:    -apple-system, 'Segoe UI', 'Noto Sans SC', 'PingFang SC', sans-serif;
    --mono:    'SF Mono', 'Cascadia Code', 'Consolas', monospace;
}

*,*::before,*::after { box-sizing:border-box; margin:0; padding:0; }

html { font-size:16px; -webkit-font-smoothing:antialiased; }

body {
    font-family: var(--font);
    color: var(--text);
    background: var(--bg);
    min-height: 900px;
    display: flex;
    justify-content: center;
    align-items: center;
    user-select: none;
    -webkit-user-select: none;
}

/* ---- App Container ---- */
.app {
    width: 510px;
    padding: 40px 32px 48px;
    display: flex;
    flex-direction: column;
    align-items: center;
}

/* ---- Header ---- */
.head {
    text-align: center;
    margin-bottom: 32px;
}
.head h1 {
    font-size: 2rem;
    font-weight: 600;
    letter-spacing: -0.02em;
    color: var(--text);
    margin-bottom: 6px;
}
.head p {
    font-size: 0.85rem;
    color: var(--muted);
    letter-spacing: 0.01em;
    font-weight: 400;
}

/* ---- Canvas ---- */
.canvas-frame {
    width: 340px;
    height: 340px;
    background: var(--surface);
    border-radius: var(--radius);
    box-shadow: 0 0 0 1px var(--border), 0 2px 20px rgba(0,0,0,0.04);
    overflow: hidden;
    margin-bottom: 20px;
}

#drawCanvas {
    display: block;
    width: 340px;
    height: 340px;
    cursor: crosshair;
    image-rendering: auto;
}

/* ---- Controls ---- */
.controls {
    display: flex;
    align-items: center;
    gap: 4px;
    margin-bottom: 28px;
}

.brush-sizes {
    display: flex;
    gap: 4px;
}

.bs {
    width: 34px;
    height: 34px;
    border: none;
    border-radius: 50%;
    background: transparent;
    cursor: pointer;
    display: flex;
    align-items: center;
    justify-content: center;
    transition: background 0.15s;
}
.bs:hover { background: var(--border); }
.bs i {
    display: block;
    border-radius: 50%;
    background: var(--muted);
    transition: background 0.15s, transform 0.15s;
}
.bs:nth-child(1) i { width:4px;  height:4px; }
.bs:nth-child(2) i { width:7px;  height:7px; }
.bs:nth-child(3) i { width:10px; height:10px; }
.bs.active { background: var(--surface); box-shadow: 0 0 0 1px var(--border); }
.bs.active i { background: var(--text); }

.sep {
    display: inline-block;
    width: 1px;
    height: 18px;
    background: var(--border);
    margin: 0 8px;
}

.act {
    height: 34px;
    padding: 0 14px;
    border: none;
    border-radius: 17px;
    background: transparent;
    color: var(--muted);
    font-family: var(--font);
    font-size: 0.8rem;
    font-weight: 500;
    letter-spacing: 0.02em;
    cursor: pointer;
    transition: all 0.15s;
}
.act:hover { color: var(--text); background: var(--border); }
.act.active { color: var(--surface); background: var(--red); }

/* ---- Primary Button ---- */
.primary {
    width: 100%;
    max-width: 260px;
    height: 48px;
    border: none;
    border-radius: var(--radius);
    background: var(--accent);
    color: #fff;
    font-family: var(--font);
    font-size: 0.95rem;
    font-weight: 600;
    letter-spacing: 0.04em;
    cursor: pointer;
    transition: background 0.2s, transform 0.1s, box-shadow 0.2s;
    margin-bottom: 32px;
}
.primary:hover {
    background: var(--accent-hover);
    box-shadow: 0 4px 16px rgba(44,62,110,0.25);
}
.primary:active { transform: scale(0.975); }
.primary.sending { opacity:0.6; pointer-events:none; }

/* ---- Result ---- */
.result {
    width: 100%;
    max-width: 310px;
    text-align: center;
    opacity: 0.4;
    transition: opacity 0.4s;
}
.result.has-prediction { opacity: 1; }

.gauge { margin-bottom: 8px; }

.gauge-track {
    position: relative;
    height: 4px;
    background: var(--border);
    border-radius: 2px;
    margin-bottom: 6px;
}

.gauge-fill {
    position: absolute;
    top: 0; left: 0; bottom: 0;
    width: 0%;
    border-radius: 2px;
    background: var(--accent);
    transition: width 0.6s cubic-bezier(0.22, 1, 0.36, 1);
}

.gauge-pin {
    position: absolute;
    top: -3px;
    width: 6px;
    height: 10px;
    background: var(--text);
    border-radius: 2px;
    margin-left: -3px;
    left: 0%;
    transition: left 0.6s cubic-bezier(0.22, 1, 0.36, 1);
}

.gauge-labels {
    display: flex;
    justify-content: space-between;
    font-size: 0.65rem;
    font-weight: 500;
    letter-spacing: 0.06em;
    text-transform: uppercase;
    color: var(--muted);
}

.reading {
    font-family: var(--mono);
    font-size: 1.15rem;
    font-weight: 500;
    color: var(--text);
    letter-spacing: 0.03em;
    margin-bottom: 2px;
    min-height: 1.4em;
}

.hint {
    font-size: 0.7rem;
    color: var(--muted);
}

)CSS";

const char* js_content = R"JS(
/* ===== Shape Lab — Drawing + API ===== */
(function () {
    'use strict';

    const visCanvas  = document.getElementById('drawCanvas');
    const dataCanvas = document.getElementById('dataCanvas');
    const eraserBtn  = document.getElementById('eraserBtn');
    const clearBtn   = document.getElementById('clearBtn');
    const detectBtn  = document.getElementById('detectBtn');
    const brushGroup = document.getElementById('brushGroup');
    const resultFill   = document.getElementById('resultFill');
    const resultMarker = document.getElementById('resultMarker');
    const resultValue  = document.getElementById('resultValue');
    const resultsPanel = document.getElementById('resultsPanel');

    const visCtx  = visCanvas.getContext('2d');
    const dataCtx = dataCanvas.getContext('2d');

    const SCALE = 3.4;  // 340 / 100

    let brushSize = 2;
    let isEraser  = false;
    let isDrawing = false;
    let lastX = 0, lastY = 0;

    function resetDataCanvas() {
        dataCtx.fillStyle = '#000000';
        dataCtx.fillRect(0, 0, 100, 100);
    }
    resetDataCanvas();

    function drawPoint(ax, ay) {
        dataCtx.fillStyle = isEraser ? '#000000' : '#ffffff';
        const dr = brushSize / 2;
        dataCtx.beginPath();
        dataCtx.arc(ax, ay, dr, 0, Math.PI * 2);
        dataCtx.fill();

        visCtx.fillStyle = isEraser ? '#000' : '#1a1a1a';
        const vr = dr * SCALE;
        if (isEraser) {
            visCtx.clearRect(ax * SCALE - vr - 1, ay * SCALE - vr - 1, vr * 2 + 2, vr * 2 + 2);
        } else {
            visCtx.beginPath();
            visCtx.arc(ax * SCALE, ay * SCALE, vr, 0, Math.PI * 2);
            visCtx.fill();
        }
    }

    function drawLine(x0, y0, x1, y1) {
        const dx = x1 - x0, dy = y1 - y0;
        const dist = Math.max(Math.abs(dx), Math.abs(dy));
        if (dist === 0) { drawPoint(x0, y0); return; }
        for (let i = 0; i <= dist; i++) {
            const t = i / dist;
            drawPoint(x0 + dx * t, y0 + dy * t);
        }
    }

    function pos(e) {
        const r = visCanvas.getBoundingClientRect();
        return { x: Math.round((e.clientX - r.left) / SCALE), y: Math.round((e.clientY - r.top) / SCALE) };
    }
    function tpos(e) {
        const r = visCanvas.getBoundingClientRect();
        const t = e.touches[0];
        return { x: Math.round((t.clientX - r.left) / SCALE), y: Math.round((t.clientY - r.top) / SCALE) };
    }

    visCanvas.addEventListener('mousedown', e => { isDrawing = true; const p = pos(e); lastX = p.x; lastY = p.y; drawPoint(p.x, p.y); });
    visCanvas.addEventListener('mousemove', e => { if (!isDrawing) return; const p = pos(e); drawLine(lastX, lastY, p.x, p.y); lastX = p.x; lastY = p.y; });
    visCanvas.addEventListener('mouseup',   () => { isDrawing = false; });
    visCanvas.addEventListener('mouseleave',() => { isDrawing = false; });
    visCanvas.addEventListener('touchstart', e => { if (e.touches.length !== 1) return; e.preventDefault(); isDrawing = true; const p = tpos(e); lastX = p.x; lastY = p.y; drawPoint(p.x, p.y); }, { passive: false });
    visCanvas.addEventListener('touchmove',  e => { if (!isDrawing || e.touches.length !== 1) return; e.preventDefault(); const p = tpos(e); drawLine(lastX, lastY, p.x, p.y); lastX = p.x; lastY = p.y; }, { passive: false });
    visCanvas.addEventListener('touchend',   () => { isDrawing = false; });

    brushGroup.addEventListener('click', e => {
        const btn = e.target.closest('.bs');
        if (!btn) return;
        brushSize = parseInt(btn.dataset.size, 10);
        brushGroup.querySelectorAll('.bs').forEach(b => b.classList.remove('active'));
        btn.classList.add('active');
        if (isEraser) { isEraser = false; eraserBtn.classList.remove('active'); }
    });

    eraserBtn.addEventListener('click', () => {
        isEraser = !isEraser;
        eraserBtn.classList.toggle('active', isEraser);
    });

    clearBtn.addEventListener('click', () => {
        resetDataCanvas();
        visCtx.clearRect(0, 0, 340, 340);
        resultValue.textContent = '';
        resultFill.style.width = '0%';
        resultMarker.style.left = '0%';
        resultsPanel.classList.remove('has-prediction');
    });

    detectBtn.addEventListener('click', async () => {
        const img = dataCtx.getImageData(0, 0, 100, 100);
        const pixels = [];
        for (let i = 0; i < img.data.length; i += 4) pixels.push(img.data[i] / 255.0);

        detectBtn.classList.add('sending');
        detectBtn.textContent = '…';

        try {
            const res = await fetch('/api/predict', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ pixels })
            });
            if (!res.ok) throw new Error('status ' + res.status);
            const d = await res.json();
            const p = d.prediction;

            const pct = Math.round(p * 100);
            resultFill.style.width = pct + '%';
            resultMarker.style.left = pct + '%';
            resultValue.textContent = (p).toFixed(4);
            resultValue.style.color = p < 0.5 ? 'var(--green)' : 'var(--red)';
            resultsPanel.classList.add('has-prediction');
        } catch (err) {
            resultValue.textContent = '—';
            resultValue.style.color = 'var(--muted)';
            console.error(err);
        } finally {
            detectBtn.classList.remove('sending');
            detectBtn.textContent = 'Identify';
        }
    });
})();

)JS";

#endif