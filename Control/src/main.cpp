#include <Adafruit_SSD1306.h>

// --- Constantes de Configuración ---
// Pantalla OLED
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C

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

void setup()
{
  Serial.begin(115200);

  if (!setupOLED())
    ESP.restart();
}

void loop()
{
  updateDisplay(23.5, 32.6, 1, 0);
  delay(1000);
}
