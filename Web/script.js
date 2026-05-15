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
