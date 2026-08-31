// Visualizador 3D de orientacao (cubo para satelite, foguete para foguetes).
// O objeto exibido depende do ultimo evento recebido:
//   updateSat    -> cubo (satelite #213)
//   updateRocket -> foguete (#11 / #51)
// Orientacao: inclinacao (roll/pitch) a partir do acelerometro + giro (yaw)
// a partir da integracao do giroscopio no eixo Z.

(function () {
  var container = document.getElementById("cube3d");
  if (!container) return;

  var deviceLabel = document.getElementById("deviceLabel");
  var angleLabel = document.getElementById("angleLabel");

  // ── Cena, camera e renderer ────────────────────────────────────────────
  var scene = new THREE.Scene();
  scene.background = new THREE.Color(0x232333);

  var camera = new THREE.PerspectiveCamera(45, 1, 0.1, 100);
  camera.position.set(4, 3, 7);

  var renderer = new THREE.WebGLRenderer({ antialias: true });
  renderer.setPixelRatio(window.devicePixelRatio || 1);
  container.appendChild(renderer.domElement);

  function resize() {
    var w = container.clientWidth;
    var h = container.clientHeight;
    if (!w || !h) return;
    renderer.setSize(w, h);
    camera.aspect = w / h;
    camera.updateProjectionMatrix();
  }
  window.addEventListener("resize", resize);
  resize();

  // ── Iluminacao ─────────────────────────────────────────────────────────
  scene.add(new THREE.AmbientLight(0xffffff, 0.6));
  var dirLight = new THREE.DirectionalLight(0xffffff, 0.8);
  dirLight.position.set(5, 10, 7);
  scene.add(dirLight);

  // ── Controles de orbita (inspecao manual) ──────────────────────────────
  var controls = new THREE.OrbitControls(camera, renderer.domElement);
  controls.target.set(0, 0, 0);
  controls.enableDamping = true;
  controls.update();

  // ── Grid de referencia ─────────────────────────────────────────────────
  var grid = new THREE.GridHelper(10, 10, 0x4a4cc9, 0x3a3a5a);
  scene.add(grid);

  // ── Modelo: cubo (satelite) ────────────────────────────────────────────
  var cubeGroup = new THREE.Group();
  var cubeGeom = new THREE.BoxGeometry(2, 2, 2);
  var cubeMesh = new THREE.Mesh(
    cubeGeom,
    new THREE.MeshStandardMaterial({
      color: 0xd63031,
      transparent: true,
      opacity: 0.85,
    })
  );
  var cubeEdges = new THREE.LineSegments(
    new THREE.EdgesGeometry(cubeGeom),
    new THREE.LineBasicMaterial({ color: 0xffffff })
  );
  cubeGroup.add(cubeMesh);
  cubeGroup.add(cubeEdges);

  // ── Modelo: foguete (construido por primitivas) ────────────────────────
  var rocketGroup = new THREE.Group();
  var bodyMat = new THREE.MeshStandardMaterial({ color: 0x4a4cc9 });
  var noseMat = new THREE.MeshStandardMaterial({ color: 0xcbcbe2 });
  var finMat = new THREE.MeshStandardMaterial({ color: 0xd63031 });

  var body = new THREE.Mesh(new THREE.CylinderGeometry(0.6, 0.6, 2.6, 24), bodyMat);
  var nose = new THREE.Mesh(new THREE.ConeGeometry(0.6, 1.2, 24), noseMat);
  nose.position.y = 1.9;

  var finGeom = new THREE.BoxGeometry(0.08, 1.0, 0.9);
  for (var i = 0; i < 4; i++) {
    var fin = new THREE.Mesh(finGeom, finMat);
    var angle = (i / 4) * Math.PI * 2;
    fin.position.set(Math.sin(angle) * 0.6, -1.3, Math.cos(angle) * 0.6);
    fin.rotation.y = -angle;
    rocketGroup.add(fin);
  }
  rocketGroup.add(body);
  rocketGroup.add(nose);

  // Ambos os modelos usam a mesma ordem de Euler (yaw -> pitch -> roll).
  cubeGroup.rotation.order = "YXZ";
  rocketGroup.rotation.order = "YXZ";

  cubeGroup.visible = false;
  rocketGroup.visible = false;
  scene.add(cubeGroup);
  scene.add(rocketGroup);

  // ── Estado de orientacao ───────────────────────────────────────────────
  var yaw = 0; // giro acumulado (giroscopio gz)
  var roll = 0; // inclinacao lateral (acelerometro)
  var pitch = 0; // inclinacao frontal/traseira (acelerometro)
  var lastTime = null;

  function applyOrientation(model) {
    model.rotation.y = yaw;
    model.rotation.x = pitch;
    model.rotation.z = roll;
  }

  // Convencao do FakeCom: objeto nivelado quando az ~ -9.8 e ax/ay ~ 0.
  // (Pode exigir ajuste de sinal/eixos para o sensor real.)
  function updateTiltFromAccel(ax, ay, az) {
    roll = Math.atan2(ay, -az);
    pitch = Math.atan2(-ax, -az);
  }

  function deg(rad) {
    return ((rad * 180) / Math.PI).toFixed(1);
  }

  // ── Graficos 2D (Chart.js) ─────────────────────────────────────────────
  var MAX_POINTS = 300;
  var chartStart = null;
  var accelChart = null;
  var velChart = null;
  var altChart = null;

  function elapsedSecs() {
    if (chartStart === null) chartStart = performance.now();
    return (performance.now() - chartStart) / 1000;
  }

  function makeLineChart(canvasId, label, color, fill, yTitle) {
    var canvas = document.getElementById(canvasId);
    if (!canvas) return null;

    var axisColor = "#cbcbe2";
    var gridColor = "rgba(203, 203, 226, 0.12)";

    return new Chart(canvas, {
      type: "line",
      data: {
        datasets: [
          {
            label: label,
            data: [],
            borderColor: color,
            backgroundColor: fill,
            tension: 0.3,
            pointRadius: 0,
            borderWidth: 2,
            fill: !!fill,
          },
        ],
      },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        animation: false,
        interaction: { mode: "nearest", intersect: false },
        scales: {
          x: {
            type: "linear",
            title: { display: true, text: "Tempo (s)", color: axisColor },
            ticks: { color: axisColor },
            grid: { color: gridColor },
          },
          y: {
            title: { display: true, text: yTitle, color: color },
            ticks: { color: axisColor },
            grid: { color: gridColor },
          },
        },
        plugins: { legend: { labels: { color: axisColor } } },
      },
    });
  }

  function ensureCharts() {
    if (accelChart || typeof Chart === "undefined") return;
    accelChart = makeLineChart("accelChart", "Aceleração (m/s²)", "#e67e22", "rgba(230, 126, 34, 0.15)", "Aceleração (m/s²)");
    velChart = makeLineChart("velChart", "Velocidade (m/s)", "#2ecc71", "rgba(46, 204, 113, 0.15)", "Velocidade (m/s)");
    altChart = makeLineChart("altChart", "Altitude (m)", "#4a4cc9", "rgba(74, 76, 201, 0.25)", "Altitude (m)");
  }

  function pushChartPoint(t, accel, vel, alt) {
    ensureCharts();
    if (!accelChart || !velChart || !altChart) return;

    accelChart.data.datasets[0].data.push({ x: t, y: accel });
    velChart.data.datasets[0].data.push({ x: t, y: vel });
    altChart.data.datasets[0].data.push({ x: t, y: alt });

    if (accelChart.data.datasets[0].data.length > MAX_POINTS) {
      accelChart.data.datasets[0].data.shift();
      velChart.data.datasets[0].data.shift();
      altChart.data.datasets[0].data.shift();
    }

    accelChart.update("none");
    velChart.update("none");
    altChart.update("none");
  }

  function handlePacket(data, kind) {
    var gz = parseFloat(data.gz) || 0;
    var ax = parseFloat(data.ax) || 0;
    var ay = parseFloat(data.ay) || 0;
    var az = parseFloat(data.az) || 0;

    // Integra o giroscopio (yaw) com o intervalo entre pacotes.
    var now = performance.now();
    if (lastTime !== null) {
      var dt = (now - lastTime) / 1000;
      if (dt > 0 && dt < 2.0) {
        yaw += gz * dt;
      }
    }
    lastTime = now;

    updateTiltFromAccel(ax, ay, az);

    // Telemetria para os graficos 2D.
    var accel = Math.sqrt(ax * ax + ay * ay + az * az);
    var vel = parseFloat(data.vz) || 0;
    var alt = parseFloat(data.altura) || 0;
    pushChartPoint(elapsedSecs(), accel, vel, alt);

    var active;
    if (kind === "sat") {
      cubeGroup.visible = true;
      rocketGroup.visible = false;
      active = cubeGroup;
      deviceLabel.textContent = "Satélite #213 — Cubo";
    } else {
      cubeGroup.visible = false;
      rocketGroup.visible = true;
      active = rocketGroup;
      deviceLabel.textContent = "Foguete — Foguete";
    }

    applyOrientation(active);
    angleLabel.textContent =
      "roll: " + deg(roll) + "° · pitch: " + deg(pitch) + "° · yaw: " + deg(yaw) + "°";
  }

  // ── Socket.IO ──────────────────────────────────────────────────────────
  var socket = io.connect();
  socket.on("updateSat", function (msg) {
    handlePacket(msg, "sat");
  });
  socket.on("updateRocket", function (msg) {
    handlePacket(msg, "rocket");
  });

  ensureCharts();

  // ── Loop de render ─────────────────────────────────────────────────────
  function animate() {
    requestAnimationFrame(animate);
    controls.update();
    renderer.render(scene, camera);
  }
  animate();
})();
