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
      .forward {
        grid-column: 2;
      }
      .left {
        grid-column: 1;
      }
      .backward {
        grid-column: 2;
      }
      .right {
        grid-column: 3;
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
        <small>ESP32 · EINGEBETTETE SYSTEME</small>
        <h1>Tank Lab</h1>
      </div>
      <span id="connection" class="badge" role="status">Verbinde …</span>
    </div>
    <p>Dein Browser steuert den Tank direkt über sein WLAN.</p>
    <div class="grid">
      <section>
        <h2>Fahren</h2>
        <button id="arm">Fahren freigeben</button>
        <div class="pad" aria-label="Fahrtrichtung">
          <button class="move forward" data-direction="forward" aria-label="Vorwärts" disabled>
            ↑
          </button>
          <button class="move left" data-direction="left" aria-label="Links drehen" disabled>
            ←
          </button>
          <button class="move backward" data-direction="backward" aria-label="Rückwärts" disabled>
            ↓
          </button>
          <button class="move right" data-direction="right" aria-label="Rechts drehen" disabled>
            →
          </button>
        </div>
        <label for="speed">PWM: <strong id="speedValue">40</strong> %</label
        ><input id="speed" type="range" min="10" max="100" step="5" value="40" />
        <small
          >Fahrtaste gedrückt halten. Loslassen stoppt. Die Prozentzahl ist der PWM-Sollwert.</small
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
    </div>
    <footer>Kein Internet nötig · Befehls-Timeout: 500 ms · Servo deaktiviert</footer>
    <script>
      // Ein Browser besitzt die Fahrfreigabe. Sequenznummern verhindern alte Fahrbefehle nach STOPP.
      const client = Array.from(crypto.getRandomValues(new Uint8Array(12)), (b) =>
        b.toString(16).padStart(2, '0'),
      ).join('');
      let sequence = 0,
        armed = false,
        moving = null,
        timer = null,
        drivePending = false;
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
        armed = s.armed;
        $('arm').textContent = armed ? 'Freigegeben · zum Sperren klicken' : 'Fahren freigeben';
        document.querySelectorAll('.move').forEach((b) => (b.disabled = !armed));
        $('left').textContent = s.left + ' %';
        $('right').textContent = s.right + ' %';
        $('button').textContent = s.buttonPressed ? 'Gedrückt' : 'Losgelassen';
        $('count').textContent = s.buttonCount;
        $('color').value =
          '#' + [s.red, s.green, s.blue].map((v) => v.toString(16).padStart(2, '0')).join('');
        $('connection').textContent = 'Verbunden';
        if (!armed) clearMotion();
      }
      function clearMotion() {
        clearInterval(timer);
        timer = null;
        moving = null;
        document.querySelectorAll('.move').forEach((b) => b.classList.remove('pressed'));
      }
      async function lock() {
        clearMotion();
        armed = false;
        document.querySelectorAll('.move').forEach((b) => (b.disabled = true));
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
          await post('/api/drive', { client, seq: ++sequence, direction, speed: $('speed').value });
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
      document.querySelectorAll('.move').forEach((button) => {
        button.onpointerdown = (e) => {
          if (!armed || moving || !e.isPrimary) return;
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
          document.querySelectorAll('.move').forEach((b) => (b.disabled = true));
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
