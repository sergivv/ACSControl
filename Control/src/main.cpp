#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "config.h"

// Constantes de Configuración
const unsigned long MEASUREMENT_INTERVAL = 5000;        // Intervalo entre mediciones (5s)
const unsigned long DISPLAY_UPDATE_INTERVAL = 1000;     // Actualización pantalla (1s)
const unsigned long WIFI_CHECK_INTERVAL = 300000;       // Chequeo WiFi (5 min)
const unsigned long WIFI_CONNECT_TIMEOUT = 30000;       // Timeout conexión WiFi (30s)

// Configuración LCD OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C

// Variables Globales
unsigned long lastMeasurementTime = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastWiFiCheck = 0;
bool wifiConnected = false;

// Objeto de configuración OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

// --- Prototipos de Funciones ---
bool setupOLED();
void connectWiFi();
void handleWiFi();
void updateDisplay(float tempC, float tempA, int caldera, int bomba);

// --- Implementación OLED ---
bool setupOLED() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    Serial.println("❌ Error inicializando OLED");
    return false;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();
  Serial.println("✅ OLED inicializado correctamente");
  return true;
}

void updateDisplay(float tempC, float tempA, int caldera, int bomba) {
  display.clearDisplay(); 
  
  // Encabezado con estado WiFi
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("WiFi:");
  display.println(wifiConnected ? "ON" : "OFF");
  display.setCursor(50, 0);
  display.print(WiFi.localIP());
  display.drawLine(0, 10, 128, 10, SSD1306_WHITE); // Línea separadora

  // Temperaturas
  display.setTextSize(2);
  display.setCursor(0, 15);
  display.print(F("Tc:"));
  display.print(tempC, 1);
  display.cp437(true);
  display.write(167);
  display.print(F("C"));

  display.setCursor(0, 35);
  display.print(F("Ta:"));
  display.print(tempA, 1);
  display.cp437(true);
  display.write(167);
  display.print(F("C"));

  // Indicadores Caldera/Bomba
  display.setTextSize(1);
  display.setCursor(15, 55);
  display.print(F("C:"));
  display.setCursor(70, 55);
  display.print(F("B:"));

  // Círculos indicadores
  display.setTextSize(1);
  
  // Caldera
  display.setCursor(25, 55);
  display.print(caldera ? "ON " : "OFF");
  display.drawCircle(50, 57, 6, SSD1306_WHITE);
  if (caldera) {
    display.fillCircle(50, 57, 4, SSD1306_WHITE);
  }

  // Bomba
  display.setCursor(80, 55);
  display.print(bomba ? "ON" : "OFF");
  display.drawCircle(105, 57, 6, SSD1306_WHITE);
  if (bomba) {
    display.fillCircle(105, 57, 4, SSD1306_WHITE);
  }

  display.display();
}

// --- Funciones de Conexión ---
void connectWiFi() {
  Serial.println("📡 Iniciando conexión WiFi...");
  
  // Configurar IP estática si está definida
    Serial.print("🔧 Configurando IP estática... ");
    if (WiFi.config(local_IP, gateway, subnet)) {
      Serial.println("✅ OK");
    } else {
      Serial.println("❌ Fallo, usando DHCP");
    }

  Serial.print("🔌 Conectando a: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();

  // Conexión con timeout
  while (WiFi.status() != WL_CONNECTED && 
         millis() - startAttemptTime < WIFI_CONNECT_TIMEOUT) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("\n✅ WiFi conectado!");
    Serial.print("📡 IP: ");
    Serial.println(WiFi.localIP());

    // Mostrar en OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("WiFi Conectado");
    display.print("IP: ");
    display.println(WiFi.localIP());
    display.display();
    delay(2000); // Mostrar mensaje 2 segundos
  } else {
    wifiConnected = false;
    Serial.println("\n❌ Fallo conexión WiFi");
    
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("WiFi Fallo");
    display.println("Modo offline");
    display.display();
    delay(2000);
  }
}

void handleWiFi() {
  if (millis() - lastWiFiCheck >= WIFI_CHECK_INTERVAL) {
    lastWiFiCheck = millis();
    
    if (WiFi.status() != WL_CONNECTED) {
      wifiConnected = false;
      Serial.println("🔌 WiFi desconectado. Reconectando...");
      
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Reconectando WiFi...");
      display.display();
      
      WiFi.disconnect();
      delay(1000);
      connectWiFi();
    } else {
      wifiConnected = true;
    }
  }
}

// --- Setup y Loop Principal ---
void setup() {
  Serial.begin(115200);
  delay(1000); // Esperar estabilización
  Serial.println(F("\n🔥 Sistema Control Calefacción 🔥"));
  Serial.println(F("Inicializando..."));

  // Inicializar OLED
  if (!setupOLED()) {
    Serial.println("❌ Fallo crítico OLED. Reiniciando...");
    delay(2000);
    ESP.restart();
  }

  // Mostrar mensaje de bienvenida
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Control Calefaccion");
  display.println("Inicializando...");
  display.display();
  delay(2000);

  // Conectar WiFi
  connectWiFi();

  // Inicializar timers
  lastMeasurementTime = millis();
  lastDisplayUpdate = millis();
  lastWiFiCheck = millis();

  Serial.println(F("✅ Sistema inicializado correctamente"));
}

void loop()
{
  handleWiFi();
  updateDisplay(23.5, 32.6, 0, 1);
  delay(1000);
}
