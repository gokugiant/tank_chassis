#pragma once
#include <Arduino.h>
// Diese Seite laeuft im BROWSER; die Klassen daneben laufen auf dem ESP32.
const char WebPage[] PROGMEM = R"HTML(
<!doctype html>
<html lang="de">
  <head>
    <meta charset="utf-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1" />
    <title>Tank Lab</title>
    <style>
      :root {
        color-scheme: dark;
        font:
          16px system-ui,
          sans-serif;
        background: #101721;
        color: #edf3fa;
      }
      * {
        box-sizing: border-box;
      }
      body {
        max-width: 850px;
        margin: auto;
        padding: 24px;
      }
      h1 {
        font-size: 2.4rem;
        margin: 6px 0;
      }
      h2 {
        font-size: 1.15rem;
        margin: 0 0 18px;
      }
      p {
        color: #b9c9d9;
        line-height: 1.5;
      }
      .top {
        display: flex;
        justify-content: space-between;
        align-items: center;
        gap: 16px;
      }
      .badge {
        background: #27384a;
        padding: 8px 13px;
        border-radius: 30px;
        white-space: nowrap;
      }
      .grid {
        display: grid;
        grid-template-columns: 1fr 1fr;
        gap: 18px;
        margin-top: 22px;
      }
      section {
        background: #1a2533;
        border: 1px solid #344459;
        border-radius: 20px;
        padding: 23px;
      }
      button,
      input {
        font: inherit;
      }
      button {
        border: 1px solid #53677e;
        background: #2c4056;
        color: white;
        border-radius: 12px;
        padding: 13px;
        cursor: pointer;
        min-height: 48px;
      }
      button:disabled {
        opacity: 0.35;
        cursor: default;
      }
      button:focus-visible {
        outline: 3px solid #79d6fd;
        outline-offset: 3px;
      }
      button.pressed {
        background: #467b9b;
      }
      #arm {
        background: #136675;
        width: 100%;
      }
      #emergency {
        background: #9e3038;
        width: 100%;
        font-weight: 700;
        margin-top: 16px;
      }
      .pad {
        display: grid;
        grid-template-columns: repeat(3, 1fr);
        gap: 10px;
        max-width: 290px;
        margin: 22px auto;
      }
      .move {
        font-size: 1.7rem;
        touch-action: none;
        user-select: none;
        -webkit-user-select: none;
      }
      .forward-left {
        grid-area: 1 / 1;
      }
      .forward {
        grid-area: 1 / 2;
      }
      .forward-right {
        grid-area: 1 / 3;
      }
      .left {
        grid-area: 2 / 1;
      }
      .right {
        grid-area: 2 / 3;
      }
      .backward-left {
        grid-area: 3 / 1;
      }
      .backward {
        grid-area: 3 / 2;
      }
      .backward-right {
        grid-area: 3 / 3;
      }
      .pad-stop {
        grid-area: 2 / 2;
        background: #9e3038;
      }
      input[type='range'] {
        width: 100%;
        margin: 15px 0;
      }
      input[type='color'] {
        width: 100%;
        height: 48px;
        border: 0;
        background: none;
        padding: 0;
      }
      .swatches {
        display: flex;
        gap: 8px;
        margin: 16px 0;
      }
      .swatches {
        flex-wrap: wrap;
      }
      .swatches button {
        flex: 1;
        padding: 8px;
      }
      .swatches button:nth-child(1) {
        border-color: #ef7676;
      }
      .swatches button:nth-child(2) {
        border-color: #6ed89d;
      }
      .swatches button:nth-child(3) {
        border-color: #6cb7ff;
      }
      .readings {
        display: flex;
        gap: 10px;
        margin-top: 20px;
      }
      .reading {
        flex: 1;
        background: #111c29;
        padding: 12px;
        border-radius: 12px;
      }
      .reading strong {
        display: block;
        font-size: 1.5rem;
        margin-top: 4px;
      }
      #message {
        min-height: 1.5em;
        color: #8bd8ef;
      }
      small {
        color: #b9c9d9;
      }
      footer {
        margin-top: 20px;
        color: #a4b7ca;
        font-size: 0.85rem;
      }
      @media (max-width: 620px) {
        body {
          padding: 16px;
        }
        .grid {
          grid-template-columns: 1fr;
        }
        .top {
          align-items: start;
        }
        h1 {
          font-size: 2rem;
        }
        section {
          padding: 19px;
        }
      }
    </style>
  </head>
  <body>
    <div class="top">
      <div>
        <h1>Tank Lab</h1>
      </div>
      <span id="connection" class="badge" role="status">Verbinde …</span>
    </div>
    <div class="grid">
      <section>
        <h2>Fahren</h2>
        <button id="arm">Fahren freigeben</button>
        <div class="pad" aria-label="Fahrtrichtung">
          <button
            class="move forward-left"
            data-direction="forward-left"
            aria-label="Vorwärts Linkskurve"
            disabled
          >
            ↖
          </button>
          <button class="move forward" data-direction="forward" aria-label="Vorwärts" disabled>
            ↑
          </button>
          <button
            class="move forward-right"
            data-direction="forward-right"
            aria-label="Vorwärts Rechtskurve"
            disabled
          >
            ↗
          </button>
          <button
            class="move left"
            data-direction="left"
            aria-label="Auf der Stelle links drehen"
            disabled
          >
            ←
          </button>
          <button id="padStop" class="pad-stop" aria-label="Stoppen und sperren">■</button>
          <button
            class="move right"
            data-direction="right"
            aria-label="Auf der Stelle rechts drehen"
            disabled
          >
            →
          </button>
          <button
            class="move backward-left"
            data-direction="backward-left"
            aria-label="Rückwärts Linkskurve"
            disabled
          >
            ↙
          </button>
          <button class="move backward" data-direction="backward" aria-label="Rückwärts" disabled>
            ↓
          </button>
          <button
            class="move backward-right"
            data-direction="backward-right"
            aria-label="Rückwärts Rechtskurve"
            disabled
          >
            ↘
          </button>
        </div>
        <p>
          <small
            >Neue Vorderseite = bisheriges Heck. Diagonalen fahren eine Kurve mit fester PWM 100 %
            außen / 50 % innen, unabhängig vom PWM-Regler. Die Pfeile bezeichnen die Fahrkurve,
            keine gemessene 45°-Drehung. Rückwärts lenkt das Heck in die Pfeilrichtung.</small
          >
        </p>
        <label for="speed">PWM: <strong id="speedValue">50</strong> %</label
        ><input id="speed" type="range" min="50" max="100" step="5" value="50" />
        <small
          >Fahrtaste gedrückt halten. Loslassen stoppt. Die Prozentzahl ist der PWM-Sollwert.</small
        >
        <p>
          <label><input id="stabilize" type="checkbox" /> Richtung bei Vor/Zurück halten</label>
        </p>
        <small
          >Benötigt kalibrierten Winkel. Korrektur maximal 12 PWM-Punkte; manuelles Lenken und
          Loslassen beenden das Halten.</small
        >
        <h3>Gezielt drehen</h3>
        <div class="swatches">
          <button data-turn="90" data-turn-direction="left" disabled>↶ 90°</button>
          <button data-turn="90" data-turn-direction="right" disabled>90° ↷</button>
          <button data-turn="180" data-turn-direction="left" disabled>↶ 180°</button>
          <button data-turn="180" data-turn-direction="right" disabled>180° ↷</button>
          <button data-turn="360" data-turn-direction="left" disabled>↶ 360°</button>
          <button data-turn="360" data-turn-direction="right" disabled>360° ↷</button>
        </div>
        <p id="navigationState" role="status">Bereit</p>
        <small
          >Klick startet eine Drehung mit mindestens 50 % PWM. Der PWM-Regler begrenzt auch die
          Drehleistung. Seite geöffnet lassen. Nach Ausrollen und Feinkorrektur bleibt die Freigabe
          bestehen. Genauigkeit und PWM müssen am Fahrzeug noch abgestimmt werden. Zuerst mit freiem
          Platz testen.</small
        >
        <button id="emergency">STOPP &amp; SPERREN</button>
        <div class="readings">
          <div class="reading">Links<strong id="left">0 %</strong></div>
          <div class="reading">Rechts<strong id="right">0 %</strong></div>
        </div>
      </section>
      <section>
        <h2>Licht &amp; Taster</h2>
        <label for="color">RGB-Farbe</label><input id="color" type="color" value="#ff0000" />
        <div class="swatches">
          <button data-color="#ff0000">Rot</button><button data-color="#00ff00">Grün</button
          ><button data-color="#0000ff">Blau</button>
        </div>
        <button id="off" style="width: 100%">Licht aus</button>
        <div class="readings">
          <div class="reading">Taster<strong id="button">Losgelassen</strong></div>
        </div>
        <p>Gedrückt: <strong id="count">0</strong> Mal</p>
        <small
          >Der echte Taster stoppt und sperrt die Fahrt. Jeder Druck schaltet lokal zwischen Rot,
          Grün und Blau um.</small
        >
        <p id="message" role="status"></p>
      </section>
      <section>
        <h2>Turret</h2>
        <label for="turret">Sollwinkel: <strong id="turretValue">90</strong>°</label>
        <input id="turret" type="range" min="0" max="180" value="90" />
        <button id="turretCenter">Mitte (90°)</button>
        <p id="turretState">Signal aus</p>
      </section>
      <section>
        <h2>Bewegungssensor</h2>
        <p id="imuState" role="status">Warte auf Sensor …</p>
        <table style="width: 100%; text-align: left">
          <thead>
            <tr>
              <th>Achse</th>
              <th>Beschleunigung (g)</th>
              <th>Drehrate (°/s)</th>
            </tr>
          </thead>
          <tbody>
            <tr>
              <th>X</th>
              <td id="ax">—</td>
              <td id="gx">—</td>
            </tr>
            <tr>
              <th>Y</th>
              <td id="ay">—</td>
              <td id="gy">—</td>
            </tr>
            <tr>
              <th>Z</th>
              <td id="az">—</td>
              <td id="gz">—</td>
            </tr>
          </tbody>
        </table>
        <p id="imuAge"></p>
        <h3>Relativer Drehwinkel</h3>
        <p id="headingState" role="status">Bitte kalibrieren</p>
        <p>
          Winkel: <strong id="headingAngle">—</strong> ° · Drehrate:
          <strong id="headingRate">—</strong> °/s
        </p>
        <button id="calibrate">Stillstand kalibrieren (2 s)</button>
        <button id="zeroHeading" disabled>Winkel auf 0°</button>
        <p>
          <small
            >Vorher STOPP &amp; SPERREN. Tank flach, Sensor-Z nach oben und 2 s vollkommen still
            halten. Danach von Hand drehen: Wir prüfen zuerst Achse und Vorzeichen. Der Winkel ist
            relativ und kann driften.</small
          >
        </p>
        <small
          >Die Richtungskorrektur ist nur mit aktivierter Option und gültigem Winkel aktiv.
          Beschleunigung enthält die Schwerkraft (ruhig: insgesamt etwa 1 g). Die Tabelle zeigt rohe
          Drehraten; der relative Winkel verwendet den Nullpunktabgleich. X/Y/Z beziehen sich auf
          die Sensorplatine.</small
        >
      </section>
    </div>
    <script>
      // Ein Browser besitzt die Fahrfreigabe. Sequenznummern verhindern alte Fahrbefehle nach STOPP.
      const client = Array.from(crypto.getRandomValues(new Uint8Array(12)), (b) =>
        b.toString(16).padStart(2, '0'),
      ).join('');
      let sequence = 0,
        armed = false,
        moving = null,
        timer = null,
        drivePending = false,
        autoTurning = false,
        autoTimer = null,
        autoPending = false,
        motionEpoch = 0;
      const $ = (id) => document.getElementById(id);
      function message(text) {
        $('message').textContent = text;
      }
      async function post(path, data = {}) {
        const response = await fetch(path, {
          method: 'POST',
          headers: { 'X-Tank-Control': '1', 'Content-Type': 'application/x-www-form-urlencoded' },
          body: new URLSearchParams(data),
          signal: AbortSignal.timeout(1200),
        });
        const state = await response.json();
        if (!response.ok) throw Error(state.error || 'Anfrage fehlgeschlagen');
        return state;
      }
      function render(s) {
        $('speed').min = s.minSpeed;
        $('speed').max = s.maxSpeed;
        $('speed').value = Math.max(s.minSpeed, Math.min(s.maxSpeed, Number($('speed').value)));
        $('speedValue').textContent = $('speed').value;
        const h = s.heading;
        $('headingState').textContent = h?.status || 'Bitte kalibrieren';
        $('headingAngle').textContent = h?.ready ? h.angle.toFixed(1) : '—';
        $('headingRate').textContent = h?.ready ? h.rate.toFixed(2) : '—';
        $('calibrate').disabled = s.armed || s.turretActive || !!h?.calibrating;
        $('zeroHeading').disabled = s.armed || s.turretActive || !h?.ready;
        const imu = s.imu;
        const valid = !!imu?.valid;
        $('imuState').textContent = valid
          ? `${imu.model || 'IMU'} verbunden`
          : 'Sensor fehlt oder liefert keine aktuellen Daten';
        for (const key of ['ax', 'ay', 'az', 'gx', 'gy', 'gz'])
          $(key).textContent = valid && Number.isFinite(imu[key]) ? imu[key].toFixed(3) : '—';
        $('imuAge').textContent = valid ? `Messwertalter bei Abfrage: ${imu.ageMs} ms` : '';
        armed = s.armed;
        $('turret').min = s.turretMin;
        $('turret').max = s.turretMax;
        if (document.activeElement !== $('turret')) $('turret').value = s.turretAngle;
        $('turretValue').textContent = $('turret').value;
        $('turretState').textContent = s.turretActive ? 'Signal aktiv' : 'Signal aus';
        $('arm').textContent = armed ? 'Freigegeben · zum Sperren klicken' : 'Fahren freigeben';
        document.querySelectorAll('.move').forEach((b) => (b.disabled = !armed));
        $('left').textContent = s.left + ' %';
        $('right').textContent = s.right + ' %';
        $('button').textContent = s.buttonPressed ? 'Gedrückt' : 'Losgelassen';
        $('count').textContent = s.buttonCount;
        $('color').value =
          '#' + [s.red, s.green, s.blue].map((v) => v.toString(16).padStart(2, '0')).join('');
        const nav = s.navigation;
        $('navigationState').textContent = nav?.status || 'Bereit';
        if (nav?.mode === 'turn')
          $('navigationState').textContent +=
            ` · Ziel ${nav.target.toFixed(1)}° · Rest ${nav.error?.toFixed(1) ?? '—'}°`;
        if (autoTurning && nav?.mode !== 'turn') {
          clearInterval(autoTimer);
          autoTimer = null;
          autoTurning = false;
        }
        const busy = nav?.mode !== 'idle' || !!moving || autoTurning;
        document
          .querySelectorAll('[data-turn]')
          .forEach(
            (b) => (b.disabled = !armed || !h?.ready || busy || s.left !== 0 || s.right !== 0),
          );
        document
          .querySelectorAll('.move')
          .forEach((b) => (b.disabled = !armed || nav?.mode === 'turn'));
        $('connection').textContent = 'Verbunden';
        if (!armed) clearMotion();
      }
      for (const [id, path] of [
        ['calibrate', '/api/heading/calibrate'],
        ['zeroHeading', '/api/heading/zero'],
      ]) {
        $(id).onclick = async () => {
          try {
            render(await post(path));
          } catch (e) {
            message(e.message);
          }
        };
      }
      async function turnHeartbeat(epoch) {
        if (!autoTurning || autoPending || epoch !== motionEpoch) return;
        autoPending = true;
        try {
          const s = await post('/api/turn/heartbeat', { client, seq: ++sequence });
          if (epoch === motionEpoch) render(s);
        } catch (e) {
          if (epoch === motionEpoch) {
            await lock();
            message(e.message);
          }
        } finally {
          autoPending = false;
        }
      }
      document.querySelectorAll('[data-turn]').forEach(
        (b) =>
          (b.onclick = async () => {
            if (!armed || moving || autoTurning) return;
            clearMotion();
            const epoch = motionEpoch;
            autoTurning = true;
            document.querySelectorAll('[data-turn], .move').forEach((x) => (x.disabled = true));
            try {
              const s = await post('/api/turn', {
                client,
                seq: ++sequence,
                direction: b.dataset.turnDirection,
                degrees: b.dataset.turn,
                speed: $('speed').value,
              });
              if (epoch !== motionEpoch) {
                await lock();
                return;
              }
              render(s);
              if (s.navigation?.mode === 'turn' && s.armed) {
                autoTurning = true;
                autoTimer = setInterval(() => turnHeartbeat(epoch), 150);
              }
            } catch (e) {
              await lock();
              message(e.message);
            }
          }),
      );
      function clearMotion() {
        ++motionEpoch;
        clearInterval(autoTimer);
        autoTimer = null;
        autoTurning = false;
        clearInterval(timer);
        timer = null;
        moving = null;
        document.querySelectorAll('.move').forEach((b) => b.classList.remove('pressed'));
      }
      async function lock() {
        clearMotion();
        armed = false;
        document.querySelectorAll('.move, [data-turn]').forEach((b) => (b.disabled = true));
        try {
          render(await post('/api/arm', { enabled: 0 }));
        } catch (e) {
          message('Keine Verbindung. Gerät stoppt ohne neue Befehle automatisch.');
        }
      }
      async function release() {
        if (!moving) return;
        clearMotion();
        try {
          render(await post('/api/stop', { client, seq: ++sequence }));
        } catch (e) {
          await lock();
          message(e.message);
        }
      }
      async function drive() {
        if (!moving || drivePending) return;
        drivePending = true;
        const direction = moving;
        try {
          await post('/api/drive', {
            client,
            seq: ++sequence,
            direction,
            speed: $('speed').value,
            stabilize: $('stabilize').checked ? 1 : 0,
          });
        } catch (e) {
          clearMotion();
          await lock();
          message(e.message);
        } finally {
          drivePending = false;
        }
      }
      $('arm').onclick = async () => {
        if (armed) {
          await lock();
          return;
        }
        try {
          render(await post('/api/arm', { enabled: 1, client, seq: ++sequence }));
          message('Freigegeben. Fahrtaste gedrückt halten.');
        } catch (e) {
          message(e.message);
        }
      };
      $('emergency').onclick = lock;
      $('padStop').onclick = lock;
      document.querySelectorAll('.move').forEach((button) => {
        button.onpointerdown = (e) => {
          if (!armed || moving || autoTurning || !e.isPrimary) return;
          e.preventDefault();
          button.setPointerCapture(e.pointerId);
          moving = button.dataset.direction;
          button.classList.add('pressed');
          drive();
          timer = setInterval(drive, 150);
        };
        button.onpointerup = release;
        button.onpointercancel = release;
        button.onlostpointercapture = release;
      });
      window.addEventListener('pointerup', release);
      window.addEventListener('blur', lock);
      document.addEventListener('visibilitychange', () => {
        if (document.hidden) lock();
      });
      window.addEventListener('pagehide', () => {
        clearMotion();
        fetch('/api/arm', {
          method: 'POST',
          headers: { 'X-Tank-Control': '1', 'Content-Type': 'application/x-www-form-urlencoded' },
          body: 'enabled=0',
          keepalive: true,
        }).catch(() => {});
      });
      $('stabilize').onchange = release;
      $('speed').oninput = () => {
        $('speedValue').textContent = $('speed').value;
      };
      async function setColor(hex) {
        try {
          render(
            await post('/api/led', {
              red: parseInt(hex.slice(1, 3), 16),
              green: parseInt(hex.slice(3, 5), 16),
              blue: parseInt(hex.slice(5, 7), 16),
            }),
          );
          message('Licht geändert.');
        } catch (e) {
          message(e.message);
        }
      }
      $('color').onchange = (e) => setColor(e.target.value);
      document
        .querySelectorAll('[data-color]')
        .forEach((b) => (b.onclick = () => setColor(b.dataset.color)));
      $('off').onclick = () => setColor('#000000');
      async function setTurret(angle) {
        try {
          render(await post('/api/turret', { angle }));
          message('Turret-Sollwinkel gesetzt.');
        } catch (e) {
          message(e.message);
        }
      }
      $('turret').oninput = () => {
        $('turretValue').textContent = $('turret').value;
      };
      $('turret').onchange = () => setTurret($('turret').value);
      $('turretCenter').onclick = () => setTurret(90);
      async function poll() {
        try {
          const response = await fetch('/api/status', {
            cache: 'no-store',
            signal: AbortSignal.timeout(1200),
          });
          if (!response.ok) throw Error();
          render(await response.json());
        } catch (e) {
          clearMotion();
          armed = false;
          document.querySelectorAll('.move, [data-turn]').forEach((b) => (b.disabled = true));
          $('connection').textContent = 'Verbindung fehlt';
          message('Ohne neue Fahrbefehle stoppt der Tank automatisch.');
        } finally {
          setTimeout(poll, 350);
        }
      }
      poll();
    </script>
  </body>
</html>
)HTML";
