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

unsigned long currentMillis, readMillis = 0, intervaloDHT = 10000;
#define DHTPIN 2          // Define o pino digital que o DHT11 está conectado (D4)
#define DHTTYPE DHT11     // DHT 11
DHT dht(DHTPIN, DHTTYPE); //Inicializando o objeto dht do tipo DHT passando como parâmetro o pino (DHTPIN) e o tipo do sensor (DHTTYPE)

int umidade = 0; float temp = 0;

void setup() {
  Serial.begin(115200);

  delay(250); // wait for the OLED to power up
  display.begin(i2c_Address, true); // Address 0x3C default
  display.cp437(true); // Use correct CP437 character codes
  display.clearDisplay();
  
  Serial.println("Setup ...");
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(18,0);
  display.println("Setup ...");
  display.display();

  WiFiManager wm;
  // Define um tempo limite para tentar conexão como AP
  wm.setConfigPortalTimeout(60);
  //autoConnect retorna false se falhar
  bool res = wm.autoConnect("ESP_D1","Bruno156"); // Cria AP caso não encontre rede WiFi

  if(!res) {
      Serial.println("WiFi não conectado - Modo Offline");
      display.clearDisplay();
      display.drawBitmap(2, 0, wifi_off, 16, 16, 1);
      display.setTextSize(2);
      display.setCursor(0,18);
      display.println("ESP_D1");
      display.println(WiFi.softAPIP());
      display.display();
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("connected...yeey :)");
      display.clearDisplay();
      display.drawBitmap(2, 0, wifi_on, 16, 16, 1);
      display.setTextSize(2);
      display.display();
  }

  Blynk.config(auth); //Configura o Token do do dispositivo no Blynk
  if(Blynk.connect(5000)){
    Blynk.syncVirtual(V5);
    mostraTempoAtualizacao();
  }
  dht.begin(); //Inicializa o sensor
}

void loop() {
  if (millis() - readMillis >= intervaloDHT){
    sendSensorDHT();
    verifyConnectionBlynkSV();
    mostraTempoAtualizacao();
  }
  Blynk.run();

  
}

void sendSensorDHT(){
  umidade = dht.readHumidity(); //Realiza a leitura da umidadedade e armazena na variável umidade
  temp = dht.readTemperature(); //Realiza a leitura da temperatura e armazena na variável temp

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
}

void verifyConnectionBlynkSV(){
  if (WiFi.status() != WL_CONNECTED){
    Serial.println("Conectado ao servidor Blynk!");
    display.drawBitmap(2, 0, wifi_off, 16, 16, 1);
    display.display();
  }else if (Blynk.connected()){
    Serial.println("Conectado ao servidor Blynk!");
    display.drawBitmap(2, 0, wifi_on, 16, 16, 1);
    display.display();
  }else{
    Serial.println("Sem conexão com servidor Blynk!");
    display.drawBitmap(2, 0, internet_off, 16, 16, 1);
    display.display();
  }
}

BLYNK_WRITE(V5){
  int tempoSegundos = param.asInt(); //lê o valor do servidor
  if (tempoSegundos >= 1 && tempoSegundos <=60){
    intervaloDHT = tempoSegundos*1000;
  }
}

void mostraTempoAtualizacao(){
  display.setTextSize(1);
  display.setCursor(100,0);
  display.print(String(intervaloDHT/1000)+"s");
  display.display();
}