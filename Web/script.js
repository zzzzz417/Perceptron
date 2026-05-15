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

    const SCALE = 3.0;
    let brushSize = 2;
    let isEraser  = false;
    let isDrawing = false;
    let lastX = 0, lastY = 0;

    /* ---- Title bar: drag + buttons ---- */
    document.getElementById('tbar').addEventListener('mousedown', function (e) {
        if (e.target.closest('.tbar-btn')) return;  // skip buttons
        fetch('/app/drag');
    });
    document.getElementById('btnMin').addEventListener('click', function () {
        fetch('/app/minimize');
    });
    document.getElementById('btnClose').addEventListener('click', function () {
        fetch('/app/close');
    });

    /* ---- Canvas ---- */
    function resetDataCanvas() {
        dataCtx.fillStyle = '#000000';
        dataCtx.fillRect(0, 0, 100, 100);
    }
    resetDataCanvas();
    function drawPoint(ax, ay) {
        dataCtx.fillStyle = isEraser ? '#000000' : '#ffffff';
        var dr = brushSize / 2;
        dataCtx.beginPath();
        dataCtx.arc(ax, ay, dr, 0, Math.PI * 2);
        dataCtx.fill();

        visCtx.fillStyle = isEraser ? '#000' : '#1a1a1a';
        var vr = dr * SCALE;
        if (isEraser) {
            visCtx.clearRect(ax * SCALE - vr - 1, ay * SCALE - vr - 1, vr * 2 + 2, vr * 2 + 2);
        } else {
            visCtx.beginPath();
            visCtx.arc(ax * SCALE, ay * SCALE, vr, 0, Math.PI * 2);
            visCtx.fill();
        }
    }

    function drawLine(x0, y0, x1, y1) {
        var dx = x1 - x0, dy = y1 - y0;
        var dist = Math.max(Math.abs(dx), Math.abs(dy));
        if (dist === 0) { drawPoint(x0, y0); return; }
        for (var i = 0; i <= dist; i++) {
            var t = i / dist;
            drawPoint(x0 + dx * t, y0 + dy * t);
        }
    }

    function pos(e) {
        var r = visCanvas.getBoundingClientRect();
        return { x: Math.round((e.clientX - r.left) / SCALE), y: Math.round((e.clientY - r.top) / SCALE) };
    }
    function tpos(e) {
        var r = visCanvas.getBoundingClientRect();
        var t = e.touches[0];
        return { x: Math.round((t.clientX - r.left) / SCALE), y: Math.round((t.clientY - r.top) / SCALE) };
    }

    visCanvas.addEventListener('mousedown', function (e) { isDrawing = true; var p = pos(e); lastX = p.x; lastY = p.y; drawPoint(p.x, p.y); });
    visCanvas.addEventListener('mousemove', function (e) { if (!isDrawing) return; var p = pos(e); drawLine(lastX, lastY, p.x, p.y); lastX = p.x; lastY = p.y; });
    visCanvas.addEventListener('mouseup',   function () { isDrawing = false; });
    visCanvas.addEventListener('mouseleave',function () { isDrawing = false; });
    visCanvas.addEventListener('touchstart',function (e) { if (e.touches.length !== 1) return; e.preventDefault(); isDrawing = true; var p = tpos(e); lastX = p.x; lastY = p.y; drawPoint(p.x, p.y); }, { passive: false });
    visCanvas.addEventListener('touchmove', function (e) { if (!isDrawing || e.touches.length !== 1) return; e.preventDefault(); var p = tpos(e); drawLine(lastX, lastY, p.x, p.y); lastX = p.x; lastY = p.y; }, { passive: false });
    visCanvas.addEventListener('touchend',  function () { isDrawing = false; });

    /* ---- Tools ---- */
    brushGroup.addEventListener('click', function (e) {
        var btn = e.target.closest('.bs');
        if (!btn) return;
        brushSize = parseInt(btn.dataset.size, 10);
        brushGroup.querySelectorAll('.bs').forEach(function (b) { b.classList.remove('active'); });
        btn.classList.add('active');
        if (isEraser) { isEraser = false; eraserBtn.classList.remove('active'); }
    });

    eraserBtn.addEventListener('click', function () {
        isEraser = !isEraser;
        eraserBtn.classList.toggle('active', isEraser);
    });

    clearBtn.addEventListener('click', function () {
        resetDataCanvas();
        visCtx.clearRect(0, 0, 300, 300);
        resultValue.textContent = '';
        resultFill.style.width = '0%';
        resultMarker.style.left = '0%';
        resultsPanel.classList.remove('has-prediction');
    });

    /* ---- Predict ---- */
    detectBtn.addEventListener('click', function () {
        var img = dataCtx.getImageData(0, 0, 100, 100);
        var pixels = [];
        for (var i = 0; i < img.data.length; i += 4) pixels.push(img.data[i] / 255.0);

        detectBtn.classList.add('sending');
        detectBtn.textContent = '…';

        fetch('/api/predict', {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ pixels: pixels })
        })
        .then(function (res) { if (!res.ok) throw new Error('status ' + res.status); return res.json(); })
        .then(function (d) {
            var p = d.prediction;
            var pct = Math.round(p * 100);
            resultFill.style.width = pct + '%';
            resultMarker.style.left = pct + '%';
            resultValue.textContent = p.toFixed(4);
            resultValue.style.color = p < 0.5 ? 'var(--green)' : 'var(--red)';
            resultsPanel.classList.add('has-prediction');
        })
        .catch(function (err) {
            resultValue.textContent = '—';
            resultValue.style.color = 'var(--muted)';
            console.error(err);
        })
        .then(function () {
            detectBtn.classList.remove('sending');
            detectBtn.textContent = 'Identify';
        });
    });
})();


document.addEventListener('contextmenu', e => e.preventDefault());