#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLSS83weTC"
#define BLYNK_TEMPLATE_NAME "Home Automation"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_GFX.h>

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

#include "images.h"

char auth[] = "Ul_9V-w_d2I31lwh826Rmes2zDaqRy5n";

#define i2c_Address 0x3c
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long currentMillis, readMillis = 0;
#define DHTPIN 2          // Define o pino digital que o DHT11 está conectado (D4)
#define DHTTYPE DHT11     // DHT 11
DHT dht(DHTPIN, DHTTYPE); //Inicializando o objeto dht do tipo DHT passando como parâmetro o pino (DHTPIN) e o tipo do sensor (DHTTYPE)




void setup() {
  Serial.begin(115200);

  Serial.println("Setup ...");

  delay(250); // wait for the OLED to power up
  display.begin(i2c_Address, true); // Address 0x3C default
  display.cp437(true); // Use correct CP437 character codes
  display.clearDisplay();

  WiFiManager wm;

  wm.setConfigPortalTimeout(60);
  bool res;
  res = wm.autoConnect("ESP_D1","Bruno156"); // password protected ap

  if(!res) {
      Serial.println("Failed to connect");
      display.clearDisplay();
      display.drawBitmap(2, 0, wifi_off, 16, 16, 1);
      display.setTextSize(2);
      display.setCursor(0,18);
      display.println("ESP_D1");
      display.println(WiFi.softAPIP());
      display.display();
      ESP.restart();
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("connected...yeey :)");
      display.clearDisplay();
      display.drawBitmap(2, 0, wifi_on, 16, 16, 1);
      display.setTextSize(2);
      display.setCursor(20,17);
      display.println("CONECTADO!");
      display.display();
  }
  Blynk.config(auth);
  dht.begin(); //Inicializa o sensor

  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(20,17);
  display.println("Setup ...");
  display.display();

}

void loop() {
  if (millis() - readMillis >= 2000) {
    sendSensorDHT();
    verifyConnectionBlynkSV();
    Blynk.run();
  }
  
}

void sendSensorDHT(){
  int umidade = dht.readHumidity(); //Realiza a leitura da umidadedade e armazena na variável umidade
  float temp = dht.readTemperature(); //Realiza a leitura da temperatura e armazena na variável temp

  if (isnan(umidade) || isnan(temp)) { //Verifica se houve falha na leitura do sensor
    Serial.println("Falha na leitura do sensor DHT!");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(0,18);
    display.println("Falha na leitura do sensor DHT!");
    display.display();
    //return;
  }else{
    Blynk.virtualWrite(V6, temp); //Escreve no pino virtual v6 o valor de temperatura
    Blynk.virtualWrite(V7, umidade); //Escreve no pino virtual V5 o valor de umidadedade
    Serial.println("Temperatura:"+String(temp)+"º");
    Serial.println("umidadedade:"+String(umidade)+"%");
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(20,17);
    display.print(String(temp)); display.write(0xF8); // Print the degrees symbol
    display.setCursor(20,36);
    display.println(String(umidade)+"%");
    display.display();
  }
  readMillis = millis();
  //WiFi.status();
  //wm.report
}

void verifyConnectionBlynkSV(){
  if(Blynk.connected()){
    Serial.println("Conectado ao servidor Blynk!");
    display.drawBitmap(2, 0, wifi_on, 16, 16, 1);
    display.display();
  }else{
    Serial.println("Sem conexão com servidor Blynk!");
    display.drawBitmap(2, 0, internet_off, 16, 16, 1);
    display.display();
  }
}
