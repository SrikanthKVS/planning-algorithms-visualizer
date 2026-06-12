// ws_client.js
// ─────────────────────────────────────────────────────────────
// Manages the WebSocket connection to the C++ server.
// Exposes: wsConnect(), wsDisconnect(), wsSend(obj)
// All other JS just calls wsSend() and registers onMessage handlers.
// ─────────────────────────────────────────────────────────────

let ws          = null;
let sentCount   = 0;
let recvCount   = 0;
let lastSentAt  = 0;

// ── DOM helpers ───────────────────────────────────────────────
const statusDot  = document.getElementById('status-dot');
const statusText = document.getElementById('status-text');
const logEl      = document.getElementById('log');
const statSent   = document.getElementById('stat-sent');
const statRecv   = document.getElementById('stat-recv');
const statLat    = document.getElementById('stat-latency');

function setStatus(state, text) {
  statusDot.className = 'status-dot ' + state;
  statusText.textContent = text;
}

function log(msg, cls = 'log-system') {
  const line = document.createElement('div');
  line.className = cls;
  const time = new Date().toLocaleTimeString('en-US', { hour12: false });
  line.textContent = `[${time}] ${msg}`;
  logEl.appendChild(line);
  logEl.scrollTop = logEl.scrollHeight;
}

function clearLog() {
  logEl.innerHTML = '';
}

// ── Connect ───────────────────────────────────────────────────
function wsConnect() {
  if (ws && ws.readyState === WebSocket.OPEN) {
    log('Already connected', 'log-system');
    return;
  }

  log('Connecting to ws://localhost:8080/ws ...', 'log-system');
  setStatus('', 'connecting...');

  ws = new WebSocket('ws://localhost:8080/ws');

  ws.onopen = () => {
    setStatus('connected', 'connected');
    log('Connected to C++ server ✓', 'log-system');
  };

  ws.onclose = (e) => {
    setStatus('', 'disconnected');
    log(`Disconnected (code ${e.code})`, 'log-error');
    ws = null;
  };

  ws.onerror = () => {
    setStatus('', 'error');
    log('Connection error — is the server running?', 'log-error');
  };

  ws.onmessage = (event) => {
    recvCount++;
    statRecv.textContent = recvCount;

    // Measure round-trip latency
    if (lastSentAt > 0) {
      statLat.textContent = (Date.now() - lastSentAt) + ' ms';
      lastSentAt = 0;
    }

    // Parse and log
    let parsed;
    try {
      parsed = JSON.parse(event.data);
    } catch {
      log('← (raw) ' + event.data, 'log-received');
      return;
    }

    log('← ' + JSON.stringify(parsed), 'log-received');

    // Dispatch to registered handlers
    if (parsed.type && messageHandlers[parsed.type]) {
      messageHandlers[parsed.type](parsed);
    }
  };
}

// ── Disconnect ────────────────────────────────────────────────
function wsDisconnect() {
  if (ws) {
    ws.close();
    ws = null;
  }
}

// ── Send ──────────────────────────────────────────────────────
function wsSend(obj) {
  if (!ws || ws.readyState !== WebSocket.OPEN) {
    log('Not connected — click Connect first', 'log-error');
    return;
  }
  const str = JSON.stringify(obj);
  lastSentAt = Date.now();
  ws.send(str);
  sentCount++;
  statSent.textContent = sentCount;
  log('→ ' + str, 'log-sent');
}

// ── Test helpers (called by buttons in index.html) ────────────
function sendPing() {
  wsSend({ type: 'ping', timestamp: Date.now() });
}

function sendTestMessage() {
  wsSend({ type: 'hello', message: 'from browser' });
}

// ── Message handler registry ──────────────────────────────────
// Other JS files register handlers like:
//   messageHandlers['node_added'] = (msg) => { drawEdge(...) }
const messageHandlers = {
  connected: (msg) => {
    log('Server says: ' + msg.message, 'log-system');
  },
  echo: (msg) => {
    // Server echoed our message back — round trip confirmed
    log('Echo confirmed ✓', 'log-system');
  }
};
