#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <OneBitDisplay.h>

static uint8_t *pBuffer;
#define DISPLAY_WIDTH 72 // largura do display
#define DISPLAY_HEIGHT 40 // altura do display
#define SDA_PIN 5 // Pino do display
#define SCL_PIN 6 // Pino do display
#define NEOPIXEL_PIN 2  // Pino onde o Neopixel está conectado
#define NEOPIXEL_NUM 1  // Número de LEDs Neopixel

const char* ssid = "Suaconexão"; // nome da sua conexão
const char* password = "SuaSenha"; // Sua senha
const String klipper_ip = "SeuIP"; // Endereço IP da impressora com Klipper


// configuração do Led Neopixel no pino 2
Adafruit_NeoPixel neopixel = Adafruit_NeoPixel(NEOPIXEL_NUM, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// configuração do SSD1306 OLED
U8G2_SSD1306_72X40_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);   // EastRising 0.42" oled
// U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE); // OLED comum 128x64, mudar posição das coisas no display
ONE_BIT_DISPLAY obd; 

void u8g2_prepare(void) {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}


void setup() {
  Serial.begin(115200);
  delay(1000);

obd.setI2CPins(SDA_PIN, SCL_PIN);
obd.I2Cbegin(OLED_72x40);
obd.allocBuffer();
pBuffer = (uint8_t *)obd.getBuffer();
  u8g2.begin();
  u8g2.enableUTF8Print();	
  u8g2.clearBuffer();

  
  // Inicializa o Neopixel
  neopixel.begin();
  neopixel.show(); // Inicializa o LED

  // Conecta ao Wi-Fi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao Wi-Fi...");
  }

  Serial.println("Conectado ao Wi-Fi!");
}

void loop() {
  // Controla o LED Neopixel com base no status de conexão Wi-Fi
   if (WiFi.status() == WL_CONNECTED) {
    neopixel.setPixelColor(0, neopixel.Color(0, 5, 0)); // Verde quando conectado
  }
  neopixel.show();

 
           HTTPClient http; 
           
           http.begin("http://"+klipper_ip+"/api/printer"); //le a temperatura
          int httpCode = http.GET();                        //faz a requisicao
      
          if (httpCode > 0) { //checa o retorno
              String payload = http.getString();
              DynamicJsonDocument doc(payload.length()*2);
              deserializeJson(doc, payload);


    String temperature = doc["temperature"]["tool0"]["actual"].as<String>();
    String temperature2 = doc["temperature"]["bed"]["actual"].as<String>();


    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_6x10_tf);
      u8g2.setCursor(0, 8);
      u8g2.print("  Extrusor:");
      u8g2.setFont(u8g2_font_7x13B_tf);
      u8g2.setCursor(8, 19);
      u8g2.print(temperature);
      u8g2.setCursor(53, 19);
      u8g2.print("°C");
      u8g2.setFont(u8g2_font_6x10_tf);
      u8g2.setCursor(0, 29);
      u8g2.print("    Mesa:");
      u8g2.setFont(u8g2_font_7x13B_tf);
      u8g2.setCursor(8, 40);
      u8g2.print(temperature2);
      u8g2.setCursor(53, 40);
      u8g2.print("°C");
    } while (u8g2.nextPage());
  }

  http.end();

  delay(1000); // Aguarda 1 segundo antes de fazer a próxima requisição
}
