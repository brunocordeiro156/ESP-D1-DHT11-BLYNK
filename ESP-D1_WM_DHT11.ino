#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPLSS83weTC"
#define BLYNK_TEMPLATE_NAME "Home Automation"

#include <BlynkSimpleEsp8266.h>
#include <SPI.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <DHT.h>
// For a connection via I2C using Wire include
#include <Wire.h>
#include "SH1106Wire.h"
#include "images.h"

char auth[] = "Ul_9V-w_d2I31lwh826Rmes2zDaqRy5n";

unsigned long currentMillis, readMillis = 0;
#define DHTPIN 2          // Define o pino digital que o DHT11 está conectado (D4)
#define DHTTYPE DHT11     // DHT 11
DHT dht(DHTPIN, DHTTYPE); //Inicializando o objeto dht do tipo DHT passando como parâmetro o pino (DHTPIN) e o tipo do sensor (DHTTYPE)

// D2 -> SDA
// D5 -> SCL
SH1106Wire display(0x3c, D2, D5);

void setup() {

    Serial.begin(115200);

    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);

    WiFiManager wm;

    // reset settings - wipe stored credentials for testing
    // these are stored by the esp library
    // wm.resetSettings();

    // Automatically connect using saved credentials,
    // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
    // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
    // then goes into a blocking loop awaiting configuration and will return success result
    wm.setConfigPortalTimeout(60);
    bool res;
    res = wm.autoConnect("ESP_D1","Bruno156"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
        ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
    }
    Blynk.config(auth);
    dht.begin(); //Inicializa o sensor
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
    display.clear();
    display.setFont(ArialMT_Plain_10);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawStringMaxWidth(0, 15, 128, "Falha na leitura do sensor DHT!");
    display.display();
    //return;
  }else{
    Blynk.virtualWrite(V6, temp); //Escreve no pino virtual v6 o valor de temperatura
    Blynk.virtualWrite(V7, umidade); //Escreve no pino virtual V5 o valor de umidadedade
    Serial.println("Temperatura:"+String(temp)+"º");
    Serial.println("umidadedade:"+String(umidade)+"%");
    display.clear();
    display.setFont(ArialMT_Plain_24);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 4, String(temp)+"º");
    display.drawString(64, 34, String(umidade)+"%");
    display.display();
  }
  readMillis = millis();
}

void verifyConnectionBlynkSV(){
  if(Blynk.connected()){
    display.drawXbm(3, 0, Icon_width, Icon_height, wifi_on);
    display.display();
    Serial.println("Conectado ao servidor Blynk!");
  }else{
    Serial.println("Sem conexão com servidor Blynk!");
    display.drawXbm(3, 0, Icon_width, Icon_height, wifi_off);
    display.display();
  }
}
