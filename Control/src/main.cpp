#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "config.h"


// --- Constantes de Configuración ---
unsigned long lastMeasurementTime = 0;
unsigned long lastWiFiCheck = 0;
// Pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C
// WiFi
const unsigned long WIFI_CHECK_INTERVAL = 300000; // Intervalo chequeo WiFi (5 min)
const unsigned long WIFI_CONNECT_TIMEOUT = 30000; // Timeout conexión WiFi (30s)

// Configuración OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

bool setupOLED()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS))
  {
    Serial.println("Error OLED");
    return false;
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.display();
  return true;
}

void updateDisplay(float tempC, float tempA, int caldera, int bomba)
{
  display.clearDisplay(); 
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.print(F("Tc: "));
  display.print(tempC, 1); // Mostrar 1 decimal
  display.cp437(true); // Habilitar página de códigos 437 para símbolo de grado
  display.write(167);  // Código para el símbolo de grado '°'
  display.print(F("C"));

  display.setCursor(0, 18);
  display.print(F("Ta: "));
  display.print(tempA, 1); 
  display.cp437(true); 
  display.write(167);  
  display.print(F("C"));

  // Indicadores Caldera/Bomba
  display.setTextSize(2); 
  display.setCursor(15, 45);
  display.print(F("C:"));
  display.drawCircle(45, 51, 7, SSD1306_WHITE); // Círculo exterior
  if (caldera != 0)
  {
    display.fillCircle(45, 51, 4, SSD1306_WHITE); // Relleno si está ON
  }
  else
  {
    display.drawCircle(45, 51, 4, SSD1306_WHITE); // Contorno si está OFF
  }

  display.setCursor(65, 45);
  display.print(F("B:"));
  display.drawCircle(95, 51, 7, SSD1306_WHITE);
  if (bomba != 0)
  {
    display.fillCircle(95, 51, 4, SSD1306_WHITE);
  }
  else
  {
    display.drawCircle(95, 51, 4, SSD1306_WHITE);
  }

  display.display();
}

// --- Funciones de Conexión ---

void connectWiFi()
{
  Serial.print(F("Configurando IP estática... "));
  if (!WiFi.config(local_IP, gateway, subnet))
  {
    Serial.println(F("Fallo. Usando DHCP..."));
    // No es necesario hacer nada más, WiFi.begin() usará DHCP si config falla
  }
  else
  {
    Serial.println(F("OK."));
  }

  Serial.print(F("Conectando a WiFi: "));
  Serial.print(ssid);
  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_CONNECT_TIMEOUT)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println(F("WiFi conectado!"));
    Serial.print(F("IP: "));
    Serial.println(WiFi.localIP());

    display.clearDisplay();
    display.println(F("WiFi conectado!"));
    display.print(F("IP: "));
    display.println(WiFi.localIP());
    display.display();
  }
  else
  {
    Serial.println(F("No se pudo conectar a Wi-Fi. Reiniciando..."));
    delay(1000);
    ESP.restart();
  }
}

void handleWiFi()
{
  if (millis() - lastWiFiCheck >= WIFI_CHECK_INTERVAL)
  {
    lastWiFiCheck = millis();
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.println(F("WiFi desconectado. Intentando reconectar..."));
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println(F("Reconectando WiFi.."));
      display.display();
      WiFi.disconnect();
      delay(100);
      WiFi.reconnect();
      lastWiFiCheck = millis();
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println(F("\n\nInicializando ESP32..."));

  if (!setupOLED())
    ESP.restart();

  connectWiFi();
  Serial.println(F("Setup completado."));
  lastMeasurementTime = millis(); // Iniciar timers
  lastWiFiCheck = millis();
}

void loop()
{
  handleWiFi();
  updateDisplay(23.5, 32.6, 1, 0);
  delay(1000);
}
