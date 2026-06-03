#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <math.h>
#include "ctf_pages.h"

// ================= CONFIG =================
#define SAMPLE_RATE   250
#define BAUD_RATE     115200
#define INPUT_PIN     D10
#define LED_PIN       D0
#define NUM_LEDS      24
#define LO_PLUS       D1
#define LO_MINUS      D2

// ================= WiFi / CTF CONFIG =================
// HINT TO PARTICIPANTS: SSID is the AP name. Password? Think about what powers this device.
const char* AP_SSID     = "CipherCore-MK2";
const char* AP_PASS     = "heartbeat";        // L0 flag — crackable via wordlist + SSID hint
const char* ADMIN_USER  = "admin";
const char* ADMIN_PASS  = "cipher123";        // L2 — discoverable via JS source comment
const char* FLAG_L1     = "FLAG{r3c0n_1s_k3y}";
const char* FLAG_L2     = "FLAG{4uth_byp4ss_ftw}";
const char* FLAG_L3     = "FLAG{y0ur_h34rt_1s_m1n3}";
const char* FLAG_L4     = "FLAG{bpm_sp00f3d_l0l}";
const char* FLAG_L5     = "FLAG{s74g0_1n_7h3_n0153}";   // hidden in stego comment in /secret

IPAddress AP_IP(192, 168, 4, 1);
IPAddress AP_SUBNET(255, 255, 255, 0);

WebServer server(80);
DNSServer dnsServer;

// ================= SESSION =================
// Simple single-token session (intentionally weak — no expiry, no CSRF)
String sessionToken = "";
bool adminLoggedIn  = false;

// ================= LED OVERRIDE =================
// When CTF participant takes control, override ECG animation
bool ledOverride       = false;
uint8_t overrideR      = 0;
uint8_t overrideG      = 0;
uint8_t overrideB      = 0;
String  overrideMode   = "solid";   // solid | pulse | chase
int     spoofedBPM     = -1;        // -1 = use real BPM

// ================= NeoPixel =================
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// ================= ECG FILTER =================
float ECGFilter(float input);

// ================= ADAPTIVE THRESHOLD =================
float peakEnvelope  = 0.0;
float noiseEnvelope = 0.0;
float threshold     = 0.0;

void updateThreshold(float rectified) {
    if (rectified > peakEnvelope)
        peakEnvelope = peakEnvelope * 0.90 + rectified * 0.10;
    else
        peakEnvelope = peakEnvelope * 0.999 + rectified * 0.001;

    if (rectified < noiseEnvelope || noiseEnvelope == 0.0)
        noiseEnvelope = noiseEnvelope * 0.90 + rectified * 0.10;
    else
        noiseEnvelope = noiseEnvelope * 0.999 + rectified * 0.001;

    threshold = noiseEnvelope + 0.55 * (peakEnvelope - noiseEnvelope);
}

// ================= DETECTION =================
unsigned long lastBeatTime  = 0;
unsigned long lastValidBeat = 0;
float realBPM = 75.0;

int getDisplayBPM() {
    return (spoofedBPM > 0) ? spoofedBPM : (int)realBPM;
}

// ================= ANIMATION =================
unsigned long pulseStart        = 0;
bool          pulseActive       = false;
bool          secondaryPulse    = false;
unsigned long secondaryPulseStart = 0;
unsigned long lastAnimFrame     = 0;
bool          leadsOff          = false;
unsigned long lastWarnBlink     = 0;
bool          warnBlinkState    = false;
int           chaseOffset       = 0;
unsigned long lastChaseStep     = 0;

void reactorPulse() {
    if (ledOverride) return;   // CTF participant has control
    pulseActive    = true;
    pulseStart     = millis();
    secondaryPulse = false;
}

void triggerSecondaryPulse() {
    secondaryPulse      = true;
    secondaryPulseStart = millis();
}

void drawPulse(float intensity, uint8_t blueScale, uint8_t greenScale) {
    int brightness = constrain((int)(255.0 * intensity), 0, 255);
    int g = brightness * greenScale / 255;
    int b = brightness * blueScale  / 255;
    for (int i = 0; i < NUM_LEDS; i++)
        strip.setPixelColor(i, strip.Color(0, g, b));
}

void showLeadsOffWarning() {
    unsigned long now = millis();
    if (now - lastWarnBlink > 400) {
        lastWarnBlink  = now;
        warnBlinkState = !warnBlinkState;
    }
    uint32_t color = warnBlinkState ? strip.Color(60, 0, 0) : 0;
    for (int i = 0; i < NUM_LEDS; i++) strip.setPixelColor(i, color);
    strip.show();
}

void updateReactor() {
    unsigned long now = millis();
    if (now - lastAnimFrame < 20) return;
    lastAnimFrame = now;

    if (leadsOff && !ledOverride) {
        showLeadsOffWarning();
        return;
    }

    // CTF LED override modes
    if (ledOverride) {
        if (overrideMode == "solid") {
            for (int i = 0; i < NUM_LEDS; i++)
                strip.setPixelColor(i, strip.Color(overrideR, overrideG, overrideB));
        } else if (overrideMode == "pulse") {
            float t   = (now % 1000) / 1000.0;
            float env = (t < 0.5) ? (t * 2.0) : (2.0 - t * 2.0);
            for (int i = 0; i < NUM_LEDS; i++)
                strip.setPixelColor(i, strip.Color(
                    (int)(overrideR * env),
                    (int)(overrideG * env),
                    (int)(overrideB * env)));
        } else if (overrideMode == "chase") {
            if (now - lastChaseStep > 40) {
                lastChaseStep = now;
                chaseOffset   = (chaseOffset + 1) % NUM_LEDS;
            }
            for (int i = 0; i < NUM_LEDS; i++) {
                int dist = (i - chaseOffset + NUM_LEDS) % NUM_LEDS;
                float bright = (dist < 6) ? (1.0 - dist / 6.0) : 0.0;
                strip.setPixelColor(i, strip.Color(
                    (int)(overrideR * bright),
                    (int)(overrideG * bright),
                    (int)(overrideB * bright)));
            }
        }
        strip.show();
        return;
    }

    // Normal ECG animation
    if (!pulseActive && !secondaryPulse) {
        for (int i = 0; i < NUM_LEDS; i++)
            strip.setPixelColor(i, strip.Color(0, 8, 18));
    }

    if (pulseActive) {
        unsigned long elapsed = now - pulseStart;
        if (elapsed <= 450) {
            float phase    = (float)elapsed / 450.0;
            float envelope = (phase < 0.12) ? (phase / 0.12) : exp(-4.5 * (phase - 0.12));
            drawPulse(envelope, 255, 180);
            if (elapsed > 120 && !secondaryPulse) triggerSecondaryPulse();
        } else {
            pulseActive = false;
        }
    }

    if (secondaryPulse) {
        unsigned long elapsed = now - secondaryPulseStart;
        if (elapsed <= 220) {
            float phase    = (float)elapsed / 220.0;
            float envelope = exp(-5.0 * phase) * 0.45;
            drawPulse(envelope, 180, 120);
        } else {
            secondaryPulse = false;
        }
    }

    strip.show();
}

// ================= SESSION HELPERS =================
String generateToken() {
    // Weak token — just millis() as hex. Intentionally guessable for CTF.
    return String(millis(), HEX);
}

bool isAuthenticated() {
    if (!server.hasHeader("Cookie")) return false;
    String cookie = server.header("Cookie");
    return (cookie.indexOf("session=" + sessionToken) >= 0 && sessionToken != "");
}

// ================= ROUTE HANDLERS =================

// GET / — Landing page
void handleRoot() {
    server.send(200, "text/html", PAGE_ROOT);
}

// GET /robots.txt — Classic hint
void handleRobots() {
    server.send(200, "text/plain",
        "User-agent: *\n"
        "Disallow: /admin\n"
        "Disallow: /api\n"
        "Disallow: /debug\n"
        "Disallow: /supersecret\n"
    );
}

// GET /admin — Login page (L2 challenge)
void handleAdminGet() {
    if (isAuthenticated()) {
        server.sendHeader("Location", "/panel");
        server.send(302, "text/plain", "");
        return;
    }
    server.send(200, "text/html", PAGE_ADMIN_LOGIN);
}

// POST /admin — Login handler
// INTENTIONAL VULN: password check is done client-side in JS (visible in source)
// Also: credentials hardcoded in a JS comment in PAGE_ADMIN_LOGIN
void handleAdminPost() {
    String user = server.arg("username");
    String pass = server.arg("password");

    // INTENTIONAL VULN: also accepts magic bypass token in URL
    // e.g. POST /admin?debug=1 skips auth entirely
    if (server.hasArg("debug") && server.arg("debug") == "1") {
        sessionToken = generateToken();
        adminLoggedIn = true;
        server.sendHeader("Set-Cookie", "session=" + sessionToken + "; Path=/");
        server.sendHeader("Location", "/panel");
        server.send(302);
        Serial.println("[CTF] Auth bypass used via ?debug=1");
        return;
    }

    if (user == ADMIN_USER && pass == ADMIN_PASS) {
        sessionToken  = generateToken();
        adminLoggedIn = true;
        server.sendHeader("Set-Cookie", "session=" + sessionToken + "; Path=/");
        server.sendHeader("Location", "/panel");
        server.send(302);
        Serial.println("[CTF] Admin login: " + user);
    } else {
        server.send(200, "text/html", PAGE_ADMIN_LOGIN_FAIL);
    }
}

// GET /panel — Control panel (requires auth)
void handlePanel() {
    if (!isAuthenticated()) {
        server.sendHeader("Location", "/admin");
        server.send(302);
        return;
    }
    server.send(200, "text/html", PAGE_PANEL);
}

// GET /api/status — Live BPM (L1 flag hidden in response)
void handleApiStatus() {
    // FLAG_L1 hidden as a field — discoverable via API enumeration
    String json = "{";
    json += "\"bpm\":" + String(getDisplayBPM()) + ",";
    json += "\"real_bpm\":" + String((int)realBPM) + ",";
    json += "\"threshold\":" + String(threshold, 1) + ",";
    json += "\"leads_off\":" + String(leadsOff ? "true" : "false") + ",";
    json += "\"override\":" + String(ledOverride ? "true" : "false") + ",";
    json += "\"flag\":\"" + String(FLAG_L1) + "\"";   // L1 — found by hitting /api/status
    json += "}";
    server.send(200, "application/json", json);
}

// POST /api/led — LED control (requires auth, L3 flag on success)
void handleApiLed() {
    if (!isAuthenticated()) {
        server.send(401, "application/json", "{\"error\":\"unauthorized\"}");
        return;
    }

    if (server.hasArg("r") && server.hasArg("g") && server.hasArg("b")) {
        overrideR    = constrain(server.arg("r").toInt(), 0, 255);
        overrideG    = constrain(server.arg("g").toInt(), 0, 255);
        overrideB    = constrain(server.arg("b").toInt(), 0, 255);
        ledOverride  = true;

        if (server.hasArg("mode"))
            overrideMode = server.arg("mode");
        else
            overrideMode = "solid";

        String json = "{\"status\":\"ok\",\"flag\":\"" + String(FLAG_L3) + "\"}";
        server.send(200, "application/json", json);
        Serial.println("[CTF] LED override by participant! RGB: "
            + String(overrideR) + "," + String(overrideG) + "," + String(overrideB));
    } else {
        server.send(400, "application/json", "{\"error\":\"missing r,g,b params\"}");
    }
}

// POST /api/bpm — BPM spoof (requires auth, L4 flag)
void handleApiBpm() {
    if (!isAuthenticated()) {
        server.send(401, "application/json", "{\"error\":\"unauthorized\"}");
        return;
    }
    if (server.hasArg("bpm")) {
        int val = server.arg("bpm").toInt();
        if (val >= 1 && val <= 300) {
            spoofedBPM = val;
            String json = "{\"status\":\"ok\",\"spoofed_bpm\":" + String(spoofedBPM)
                        + ",\"flag\":\"" + String(FLAG_L4) + "\"}";
            server.send(200, "application/json", json);
            Serial.println("[CTF] BPM spoofed to: " + String(spoofedBPM));
        } else {
            server.send(400, "application/json", "{\"error\":\"bpm out of range\"}");
        }
    } else {
        server.send(400, "application/json", "{\"error\":\"missing bpm param\"}");
    }
}

// POST /api/reset — Reset LED and BPM overrides (for Rahul to use between rounds)
void handleApiReset() {
    // Protected by a separate reset key (not admin password)
    if (server.hasArg("key") && server.arg("key") == "ciphercore_reset") {
        ledOverride  = false;
        spoofedBPM   = -1;
        overrideMode = "solid";
        sessionToken = "";
        adminLoggedIn = false;
        server.send(200, "application/json", "{\"status\":\"reset complete\"}");
        Serial.println("[CTF] RESET by operator");
    } else {
        server.send(403, "application/json", "{\"error\":\"forbidden\"}");
    }
}

// GET /debug — Fake debug page, red herring + hint
void handleDebug() {
    String page = PAGE_DEBUG;
    server.send(200, "text/html", page);
}

// GET /supersecret — L5 stego flag hidden in HTML comment
void handleSecret() {
    server.send(200, "text/html", PAGE_SECRET);
}

// 404 handler
void handleNotFound() {
    server.send(404, "text/html", PAGE_404);
}

// ================= SETUP =================

void setup() {
    Serial.begin(BAUD_RATE);

    pinMode(LO_PLUS,  INPUT);
    pinMode(LO_MINUS, INPUT);
    analogReadResolution(12);

    strip.begin();
    strip.setBrightness(200);
    strip.show();

    // Boot animation — sweep cyan
    for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, strip.Color(0, 60, 120));
        strip.show();
        delay(30);
    }
    for (int i = 0; i < NUM_LEDS; i++) strip.setPixelColor(i, 0);
    strip.show();

    // WiFi AP
    WiFi.softAPConfig(AP_IP, AP_IP, AP_SUBNET);
    WiFi.softAP(AP_SSID, AP_PASS);
    Serial.println("[WiFi] AP started: " + String(AP_SSID));
    Serial.println("[WiFi] IP: " + WiFi.softAPIP().toString());

    // DNS — captive portal redirect everything to 192.168.4.1
    dnsServer.start(53, "*", AP_IP);

    // Routes
    const char* headerKeys[] = {"Cookie"};
    server.collectHeaders(headerKeys, 1);
    server.on("/",              HTTP_GET,  handleRoot);
    server.on("/robots.txt",    HTTP_GET,  handleRobots);
    server.on("/admin",         HTTP_GET,  handleAdminGet);
    server.on("/admin",         HTTP_POST, handleAdminPost);
    server.on("/panel",         HTTP_GET,  handlePanel);
    server.on("/api/status",    HTTP_GET,  handleApiStatus);
    server.on("/api/led",       HTTP_POST, handleApiLed);
    server.on("/api/bpm",       HTTP_POST, handleApiBpm);
    server.on("/api/reset",     HTTP_POST, handleApiReset);
    server.on("/debug",         HTTP_GET,  handleDebug);
    server.on("/supersecret",   HTTP_GET,  handleSecret);
    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("[Web] Server started");

    Serial.println("================================");
    Serial.println("  CIPHERCORE MK2 — CTF EDITION");
    Serial.println("  BIO-LINK + NETLINK ACTIVE");
    Serial.println("================================");
}

// ================= LOOP =================

void loop() {
    dnsServer.processNextRequest();
    server.handleClient();

    static unsigned long lastSample = 0;
    static float prevSignal = 0.0;
    unsigned long nowMicros = micros();

    if (nowMicros - lastSample >= (1000000UL / SAMPLE_RATE)) {
        lastSample = nowMicros;

        leadsOff = digitalRead(LO_PLUS) || digitalRead(LO_MINUS);

        if (!leadsOff) {
            float raw    = (float)analogRead(INPUT_PIN);
            float signal = ECGFilter(raw);
            float rect   = fabsf(signal);

            updateThreshold(rect);

            unsigned long now = millis();
            bool risingEdge   = (rect > threshold && prevSignal <= threshold);

            if (risingEdge && (now - lastValidBeat > 500)) {
                if (lastBeatTime > 0) {
                    float instantBPM = 60000.0 / (float)(now - lastBeatTime);
                    if (instantBPM >= 40.0 && instantBPM <= 200.0) {
                        realBPM = realBPM * 0.8 + instantBPM * 0.2;
                        Serial.print("❤  BPM: ");
                        Serial.println((int)realBPM);
                    }
                }
                reactorPulse();
                lastBeatTime  = now;
                lastValidBeat = now;
            }
            prevSignal = rect;
        }
    }

    updateReactor();
}

// ================= ECG FILTER =================

float ECGFilter(float input) {
    float output = input;
    { static float z1, z2; float x = output - 0.70682283*z1 - 0.15621030*z2; output = 0.28064917*x + 0.56129834*z1 + 0.28064917*z2; z2=z1; z1=x; }
    { static float z1, z2; float x = output - 0.95028224*z1 - 0.54073140*z2; output = 1.00000000*x + 2.00000000*z1 + 1.00000000*z2; z2=z1; z1=x; }
    { static float z1, z2; float x = output - -1.95360385*z1 - 0.95423412*z2; output = 1.00000000*x + -2.00000000*z1 + 1.00000000*z2; z2=z1; z1=x; }
    { static float z1, z2; float x = output - -1.98048558*z1 - 0.98111344*z2; output = 1.00000000*x + -2.00000000*z1 + 1.00000000*z2; z2=z1; z1=x; }
    return output;
}
