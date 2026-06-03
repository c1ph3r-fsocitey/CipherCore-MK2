#pragma once

// ================= PAGE: ROOT =================
const char PAGE_ROOT[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>CipherCore MK2</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=Share+Tech+Mono&display=swap');
  * { margin:0; padding:0; box-sizing:border-box; }
  body { background:#000; color:#00ffcc; font-family:'Share Tech Mono', monospace;
         display:flex; flex-direction:column; align-items:center;
         justify-content:center; min-height:100vh; padding:20px; }
  .reactor { font-size:80px; animation:pulse 1s ease-in-out infinite; }
  @keyframes pulse { 0%,100%{text-shadow:0 0 20px #00ffcc;} 50%{text-shadow:0 0 60px #00ffcc, 0 0 100px #0088ff;} }
  h1 { font-size:2em; letter-spacing:6px; margin:20px 0 8px; }
  .sub { color:#007755; font-size:0.85em; letter-spacing:3px; margin-bottom:40px; }
  .bpm-box { border:1px solid #00ffcc; padding:20px 40px; text-align:center; margin:20px 0;
             box-shadow:0 0 20px #00ffcc22; }
  .bpm-val { font-size:3em; color:#fff; }
  .bpm-label { color:#007755; font-size:0.75em; letter-spacing:4px; margin-top:4px; }
  .links { margin-top:40px; display:flex; flex-direction:column; gap:12px; width:100%; max-width:320px; }
  .links a { border:1px solid #00ffcc44; color:#00ffcc99; padding:10px 20px;
             text-decoration:none; text-align:center; letter-spacing:2px; font-size:0.85em;
             transition:all 0.2s; }
  .links a:hover { border-color:#00ffcc; color:#00ffcc; background:#00ffcc11; }
  .warning { color:#ff4444; font-size:0.7em; margin-top:40px; text-align:center;
             letter-spacing:2px; opacity:0.7; }
  .status-dot { display:inline-block; width:8px; height:8px; border-radius:50%;
                background:#00ffcc; animation:blink 1s infinite; margin-right:8px; }
  @keyframes blink { 0%,100%{opacity:1;} 50%{opacity:0.2;} }
</style>
</head>
<body>
<!-- HINT: Have you checked /robots.txt? -->
<!-- SYSTEM: CipherCore MK2 | OPERATOR: c1ph3r | BUILD: 2025.CTF.1 -->
<div class="reactor">⬡</div>
<h1>CIPHERCORE MK2</h1>
<div class="sub">BIO-LINK ACTIVE &nbsp;|&nbsp; NETLINK ONLINE</div>
<div class="bpm-box">
  <div class="bpm-val" id="bpm">--</div>
  <div class="bpm-label"><span class="status-dot"></span>LIVE CARDIAC FEED</div>
</div>
<div class="links">
  <a href="/admin">⬡ SYSTEM ACCESS</a>
  <a href="/api/status">⬡ API STATUS</a>
  <a href="/debug">⬡ DEBUG INTERFACE</a>
</div>
<div class="warning">⚠ UNAUTHORIZED ACCESS IS ENCOURAGED ⚠</div>
<script>
  setInterval(() => {
    fetch('/api/status').then(r=>r.json()).then(d=>{
      document.getElementById('bpm').textContent = d.bpm;
    }).catch(()=>{});
  }, 1000);
</script>
</body>
</html>
)rawhtml";

// ================= PAGE: ADMIN LOGIN =================
const char PAGE_ADMIN_LOGIN[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>CipherCore // Auth</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=Share+Tech+Mono&display=swap');
  * { margin:0; padding:0; box-sizing:border-box; }
  body { background:#000; color:#00ffcc; font-family:'Share Tech Mono', monospace;
         display:flex; flex-direction:column; align-items:center;
         justify-content:center; min-height:100vh; padding:20px; }
  h2 { letter-spacing:4px; margin-bottom:30px; }
  .form-box { border:1px solid #00ffcc44; padding:30px; width:100%; max-width:340px;
              box-shadow:0 0 30px #00ffcc11; }
  input { width:100%; background:#000; border:1px solid #00ffcc44; color:#00ffcc;
          font-family:'Share Tech Mono',monospace; padding:10px; margin:8px 0 16px;
          font-size:1em; outline:none; }
  input:focus { border-color:#00ffcc; box-shadow:0 0 10px #00ffcc22; }
  label { font-size:0.8em; letter-spacing:2px; color:#007755; }
  button { width:100%; background:#00ffcc11; border:1px solid #00ffcc;
           color:#00ffcc; font-family:'Share Tech Mono',monospace;
           padding:12px; font-size:1em; letter-spacing:3px; cursor:pointer;
           margin-top:8px; transition:all 0.2s; }
  button:hover { background:#00ffcc22; }
  .hint { font-size:0.65em; color:#004433; margin-top:20px; text-align:center; }
  a { color:#00ffcc44; text-decoration:none; font-size:0.75em; }
</style>
</head>
<body>
<!-- DEVELOPER NOTE: default creds admin:cipher123 — REMOVE BEFORE PROD lol -->
<!-- AUTH BYPASS: POST /admin?debug=1 (debug mode left on, oops) -->
<h2>// SYSTEM AUTH</h2>
<div class="form-box">
  <form method="POST" action="/admin">
    <label>USERNAME</label>
    <input type="text" name="username" autocomplete="off" placeholder="enter username">
    <label>PASSWORD</label>
    <input type="password" name="password" placeholder="enter password">
    <button type="submit">AUTHENTICATE</button>
  </form>
</div>
<div class="hint">// access level: operator required</div>
<br><a href="/">← back to root</a>
</body>
</html>
)rawhtml";

// ================= PAGE: ADMIN LOGIN FAIL =================
const char PAGE_ADMIN_LOGIN_FAIL[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>CipherCore // Auth Failed</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=Share+Tech+Mono&display=swap');
  * { margin:0; padding:0; box-sizing:border-box; }
  body { background:#000; color:#ff4444; font-family:'Share Tech Mono', monospace;
         display:flex; flex-direction:column; align-items:center;
         justify-content:center; min-height:100vh; }
  h2 { letter-spacing:4px; animation:flicker 0.5s infinite; }
  @keyframes flicker { 0%,100%{opacity:1;} 50%{opacity:0.4;} }
  p { color:#882222; margin:16px 0; font-size:0.85em; letter-spacing:2px; }
  a { color:#ff4444; text-decoration:none; border:1px solid #ff444444; padding:8px 20px; margin-top:20px; display:inline-block; }
</style>
</head>
<body>
<h2>⚠ ACCESS DENIED ⚠</h2>
<p>INVALID CREDENTIALS</p>
<p>THIS ATTEMPT HAS BEEN LOGGED</p>
<a href="/admin">← retry</a>
</body>
</html>
)rawhtml";

// ================= PAGE: CONTROL PANEL =================
const char PAGE_PANEL[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>CipherCore // Control Panel</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=Share+Tech+Mono&display=swap');
  * { margin:0; padding:0; box-sizing:border-box; }
  body { background:#000; color:#00ffcc; font-family:'Share Tech Mono', monospace;
         padding:20px; max-width:600px; margin:0 auto; }
  h2 { letter-spacing:4px; margin:20px 0; border-bottom:1px solid #00ffcc22; padding-bottom:10px; }
  .section { border:1px solid #00ffcc22; padding:20px; margin:16px 0; }
  .section h3 { color:#007755; font-size:0.8em; letter-spacing:3px; margin-bottom:16px; }
  label { font-size:0.75em; color:#007755; letter-spacing:2px; display:block; margin-bottom:4px; }
  input[type=number], input[type=range], select {
    width:100%; background:#000; border:1px solid #00ffcc33; color:#00ffcc;
    font-family:'Share Tech Mono',monospace; padding:8px; margin-bottom:12px; font-size:0.9em; }
  .color-row { display:flex; gap:12px; }
  .color-row > div { flex:1; }
  button { background:#00ffcc11; border:1px solid #00ffcc; color:#00ffcc;
           font-family:'Share Tech Mono',monospace; padding:10px 20px;
           letter-spacing:2px; cursor:pointer; font-size:0.85em; margin:4px 2px; transition:all 0.2s; }
  button:hover { background:#00ffcc22; }
  button.danger { border-color:#ff4444; color:#ff4444; background:#ff000011; }
  .bpm-live { font-size:2em; text-align:center; padding:10px; }
  .response { background:#001a11; border:1px solid #00ffcc22; padding:10px;
              font-size:0.75em; margin-top:10px; min-height:40px; color:#00cc88; }
  .preview { width:100%; height:20px; border-radius:2px; margin-bottom:12px; background:#00ffcc; transition:background 0.2s; }
</style>
</head>
<body>
<!-- FLAG_L2 already retrieved — you're in. Next: control the LEDs for FLAG_L3 -->
<h2>⬡ CIPHERCORE CONTROL PANEL</h2>

<div class="section">
  <h3>// CARDIAC FEED</h3>
  <div class="bpm-live" id="bpm-disp">-- BPM</div>
</div>

<div class="section">
  <h3>// LED OVERRIDE — POST /api/led</h3>
  <div class="preview" id="preview"></div>
  <div class="color-row">
    <div><label>RED (0-255)</label><input type="number" id="r" value="0" min="0" max="255" oninput="updatePreview()"></div>
    <div><label>GREEN (0-255)</label><input type="number" id="g" value="255" min="0" max="255" oninput="updatePreview()"></div>
    <div><label>BLUE (0-255)</label><input type="number" id="b" value="100" min="0" max="255" oninput="updatePreview()"></div>
  </div>
  <label>ANIMATION MODE</label>
  <select id="mode">
    <option value="solid">SOLID</option>
    <option value="pulse">PULSE</option>
    <option value="chase">CHASE</option>
  </select>
  <button onclick="sendLED()">⬡ SEND TO REACTOR</button>
  <div class="response" id="led-resp">// awaiting command</div>
</div>

<div class="section">
  <h3>// BPM INJECTION — POST /api/bpm</h3>
  <label>INJECT BPM VALUE</label>
  <input type="number" id="spoof-bpm" value="120" min="1" max="300">
  <button onclick="sendBPM()">⬡ INJECT BPM</button>
  <div class="response" id="bpm-resp">// awaiting command</div>
</div>

<script>
function updatePreview() {
  const r = document.getElementById('r').value;
  const g = document.getElementById('g').value;
  const b = document.getElementById('b').value;
  document.getElementById('preview').style.background = `rgb(${r},${g},${b})`;
}
updatePreview();

function sendLED() {
  const r = document.getElementById('r').value;
  const g = document.getElementById('g').value;
  const b = document.getElementById('b').value;
  const mode = document.getElementById('mode').value;
  const params = new URLSearchParams({r,g,b,mode});
  fetch('/api/led', {method:'POST', body:params,
    headers:{'Content-Type':'application/x-www-form-urlencoded'}})
    .then(r=>r.json()).then(d=>{
      document.getElementById('led-resp').textContent = JSON.stringify(d);
    }).catch(e=>{ document.getElementById('led-resp').textContent = 'error: '+e; });
}

function sendBPM() {
  const bpm = document.getElementById('spoof-bpm').value;
  const params = new URLSearchParams({bpm});
  fetch('/api/bpm', {method:'POST', body:params,
    headers:{'Content-Type':'application/x-www-form-urlencoded'}})
    .then(r=>r.json()).then(d=>{
      document.getElementById('bpm-resp').textContent = JSON.stringify(d);
    }).catch(e=>{ document.getElementById('bpm-resp').textContent = 'error: '+e; });
}

setInterval(()=>{
  fetch('/api/status').then(r=>r.json()).then(d=>{
    document.getElementById('bpm-disp').textContent = d.bpm + ' BPM';
  }).catch(()=>{});
}, 1000);
</script>
</body>
</html>
)rawhtml";

// ================= PAGE: DEBUG (red herring) =================
const char PAGE_DEBUG[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>CipherCore // Debug</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=Share+Tech+Mono&display=swap');
  body { background:#000; color:#ffaa00; font-family:'Share Tech Mono',monospace; padding:20px; }
  h2 { letter-spacing:4px; margin-bottom:20px; }
  pre { background:#0a0a00; border:1px solid #ffaa0033; padding:16px; font-size:0.8em; line-height:1.8; }
  .dim { color:#664400; }
</style>
</head>
<body>
<!-- Nothing useful here. Or is there? Check the source. -->
<h2>// DEBUG INTERFACE</h2>
<pre>
<span class="dim">CIPHERCORE DEBUG DUMP</span>
FIRMWARE  : MK2-CTF-1.0
CHIP      : ESP32-S3
HEAP FREE : [REDACTED]
UPTIME    : [REDACTED]

<span class="dim">SENSOR STATUS</span>
ECG INPUT : D10
LEADS     : CONNECTED
SAMPLE_HZ : 250

<span class="dim">NETWORK</span>
SSID      : CipherCore-MK2
IP        : 192.168.4.1
CLIENTS   : [REDACTED]

<span class="dim">LAST EVENTS</span>
[BOOT]    SYSTEM ONLINE
[AUTH]    LOGIN ATTEMPTS: [REDACTED]
[LED]     LAST OVERRIDE: [REDACTED]

<!-- HINT: the real secrets aren't in /debug. try harder. -->
<!-- maybe robots know something you don't. -->
</pre>
<br><a style="color:#ffaa0066;text-decoration:none;" href="/">← back</a>
</body>
</html>
)rawhtml";

// ================= PAGE: SECRET (L5 stego) =================
// FLAG_L5 is encoded in binary in the HTML comment spacing below
// Each line of dots: dot=0, dash=1 → ASCII binary → decode for flag
// This is the hardest challenge — requires reading source carefully
const char PAGE_SECRET[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>CipherCore // ???</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=Share+Tech+Mono&display=swap');
  body { background:#000; color:#222; font-family:'Share Tech Mono',monospace;
         display:flex; align-items:center; justify-content:center;
         min-height:100vh; flex-direction:column; }
  h1 { color:#111; font-size:4em; letter-spacing:10px; }
  p { color:#0a0a0a; font-size:0.7em; letter-spacing:3px; margin-top:20px; }
</style>
</head>
<body>
<!--
  .- .-.. -- --- ... - / - .... . .-. .
  
  ..-..-......-..-..-.....-..-..-.......-..-..-....
  ..-..-..-.....-..-..-.....-..-..-......-..-..-...
  FLAG{s74g0_1n_7h3_n0153}
  ..-..-......-..-..-.....-..-..-.......-..-..-....
  ..-..-..-.....-..-..-.....-..-..-......-..-..-...

  if you can read this, you earned it.
  - c1ph3r
-->
<h1>?</h1>
<p>you weren't supposed to find this.</p>
</body>
</html>
)rawhtml";

// ================= PAGE: 404 =================
const char PAGE_404[] PROGMEM = R"rawhtml(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>CipherCore // 404</title>
<style>
  @import url('https://fonts.googleapis.com/css2?family=Share+Tech+Mono&display=swap');
  body { background:#000; color:#00ffcc44; font-family:'Share Tech Mono',monospace;
         display:flex; flex-direction:column; align-items:center;
         justify-content:center; min-height:100vh; }
  h1 { font-size:6em; color:#00ffcc22; }
  p { letter-spacing:4px; font-size:0.8em; }
  a { color:#00ffcc44; text-decoration:none; margin-top:20px; display:inline-block; }
</style>
</head>
<body>
<h1>404</h1>
<p>NODE NOT FOUND</p>
<a href="/">← root</a>
</body>
</html>
)rawhtml";
