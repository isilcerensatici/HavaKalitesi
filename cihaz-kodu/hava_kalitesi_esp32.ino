#include <Wire.h>               // I2C iletişimi için gerekli
#include <Adafruit_GFX.h>       // Ekran grafikleri için
#include <Adafruit_SSD1306.h>   // OLED Ekran sürücüsü
#include <WiFi.h>               // WiFi bağlantısı için
#include "ThingSpeak.h"         // <-- YENİ KÜTÜPHANE

// --- WiFi Bilgileri ---
const char* ssid = "iPhonex"; 
const char* password = "yusuf1121";
// ----------------------------------------

// --- THINGSPEAK AYARLARI  ---
unsigned long myChannelNumber = 3132649; // Thingspeak Kanal ID
const char * myWriteAPIKey = "TUS1ZDTH77JVQ27N"; // Thingspeak "Write API Key"
// ------------------------------------

// Ekran Ayarları
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Sensör Pini
const int sensorPin = 36; 

// Yüzde Ayarları
const int SENSOR_MIN = 850;   
const int SENSOR_MAX = 3000;  

// WiFi ve Thingspeak istemcisi
WiFiClient client;

// Thingspeak zamanlayıcısı
unsigned long lastConnectionTime = 0; 
const unsigned long postingInterval = 20000; // Thingspeak'e gönderme aralığı (20 saniye)

void setup() {
  Serial.begin(115200); 

  // Ekranı başlat
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 baglantisi basarisiz"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println(F("Baglaniyor..."));
  display.display();

  // WiFi'a Bağlan
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.print(".");
    display.display();
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Bağlantı başarılı ekranı
  display.clearDisplay();
  display.setCursor(0,0);
  display.println(F("Baglandi!"));
  display.setTextSize(1); 
  display.println(WiFi.localIP());
  display.display();

  // Thingspeak servisini başlat 
  ThingSpeak.begin(client);  

  delay(2000); 
}

void loop() {
  // --- Her 2 Saniyede Bir Ekranı Güncelle ---
  
  int sensorValue = analogRead(sensorPin);
  long percentage_long = map(sensorValue, SENSOR_MIN, SENSOR_MAX, 0, 100);
  int kirlilikYuzdesi = constrain(percentage_long, 0, 100);

  String durumMetni = "";
  if (kirlilikYuzdesi <= 20) {
    durumMetni = "Hava Temiz";
  } else if (kirlilikYuzdesi <= 60) {
    durumMetni = "Yeterince Temiz Degil";
  } else {
    durumMetni = "Hava Cok Kirli";
  }
  
  display.clearDisplay();
  
  display.setTextSize(1); 
  display.setCursor(0, 10);
  display.println(F("Kirlilik Yuzdesi"));

  display.setTextSize(2); 
  display.setCursor(20, 25);
  display.print(kirlilikYuzdesi); 
  display.print(F("%"));

  display.setTextSize(1);
  display.setCursor(0, 56);
  display.print(durumMetni); 

  display.display(); 

  // --- Her 20 Saniyede Bir Thingspeak'e Gönder  ---
  if (millis() - lastConnectionTime > postingInterval) {
    lastConnectionTime = millis(); // Zamanlayıcıyı sıfırla

    // Thingspeak'e veriyi gönder
    int httpCode = ThingSpeak.writeField(myChannelNumber, 1, kirlilikYuzdesi, myWriteAPIKey);

    if(httpCode == 200){
      Serial.println("Thingspeak gonderimi basarili.");
    }
    else{
      Serial.println("Thingspeak gonderimi basarisiz! Hata Kodu: " + String(httpCode));
    }
  }
  
  delay(2000); // Ekranın 2 saniyede bir güncellenmesi için
}
