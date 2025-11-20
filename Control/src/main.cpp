#include <Adafruit_SSD1306.h>

// Configuración OLED
Adafruit_SSD1306 display(128, 64, &Wire);

bool setupOLED()
{
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
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

void displayMessage(const char* line1, const char* line2, 
  const char* line3, const char* line4, 
  const char* line5, const char* line6,
  const char* line7, const char* line8)
{
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(line1);
  display.println(line2);
  display.println(line3);
  display.println(line4);
  display.println(line5);
  display.println(line6);
  display.println(line7);
  display.println(line8);
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
    displayMessage("ACSControl", "Inicializando...", "1", "2", "3", "4", "5", "Listo!");
    delay(1000);
}
