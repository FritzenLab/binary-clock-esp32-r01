#define bit0 5
#define bit1 6
#define bit2 7
#define bit3 8
#define enables 9


// Inclusão da(s) biblioteca(s)
#include <WiFi.h>       // Biblioteca nativa do ESP32
#include <HTTPClient.h> // Biblioteca nativa do ESP32
#include <ArduinoJson.h>

// Configurações da rede WiFi à se conectar
const char* ssid = "Clovis 2.4G";
const char* password = "99143304";

String payload;
HTTPClient http; // o objeto da classe HTTPClient

int hour= 0;
int minute= 0;
long oldtime= 0;
int previoushour= 0;
int previousminute= 0;

String rawtime;
String hourstring;
String minutestring;

void setup() {
  // put your setup code here, to run once:
  pinMode(bit0, OUTPUT);
  pinMode(bit1, OUTPUT);
  pinMode(bit2, OUTPUT);
  pinMode(bit3, OUTPUT);
  pinMode(enables, OUTPUT);
  
  Serial.begin(115200);

  WiFi.disconnect(); // Desconecta do WiFI se já houver alguma conexão
  WiFi.mode(WIFI_STA); // Configura o ESP32 para o modo de conexão WiFi Estação
  Serial.println("[SETUP] Tentando conexão com o WiFi...");
  WiFi.begin(ssid, password); // Conecta-se à rede
  if(WiFi.waitForConnectResult() == WL_CONNECTED) // aguarda até que o módulo se
    //                                                  conecte ao ponto de acesso
  {
    Serial.println("[SETUP] WiFi iniciado com sucesso!");
  }else
  {
    Serial.println("[SETUP] Houve falha na inicialização do WiFi. Reiniciando ESP.");
    ESP.restart();
  }

  http.begin("https://api.timezonedb.com/v2.1/get-time-zone?key=FT1D0SUXOE5Z&format=json&by=zone&zone=America/Recife"); // configura o URL para fazer requisição no servidor
  digitalWrite(enables, HIGH);
  delay(1000);
  digitalWrite(enables, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(millis() - oldtime > 10000){

    

    oldtime = millis();

    /*//Serial.println("[HTTP] GET...");
    int httpCode = http.GET(); // inicia uma conexão e envia um cabeçalho HTTP para o
    //                              URL do servidor configurado
    //Serial.print("[HTTP] GET... código: ");
    //Serial.println(httpCode);
    if (httpCode == HTTP_CODE_OK) // se o cabeçalho HTTP foi enviado e o cabeçalho de
      //                               resposta do servidor foi tratado, ...
    {
      //Serial.println("[HTTP] GET... OK! Resposta: ");

      payload = http.getString(); // armazena a resposta da requisição
      //Serial.println(payload); // imprime a resposta da requisição
    } else // se não, ...
    {
      //Serial.print("HTTP GET... Erro. Mensagem de Erro: ");
      //Serial.println(http.errorToString(httpCode).c_str()); // Imprime a mensagem de erro da requisição
    }

    http.end();// Fecha a requisição HTTP*/
    JsonDocument remotedata;
    updateTimeFromAPI();
  
    //payload.replace("[         {             ", "");
    //payload.replace("         }     ]", "");
  
    //JsonDocument remotedata;

    DeserializationError error = deserializeJson(remotedata, (char*) payload.c_str());

    // For this I got help from the official assistant: https://arduinojson.org/v7/assistant/#/step1

    //JsonObject zones_0 = remotedata["zones"][0];
    //const char* zones_0_countryCode = zones_0["countryCode"]; // "BR"
    //const char* zones_0_countryName = zones_0["countryName"]; // "Brazil"
    //const char* zones_0_zoneName = zones_0["zoneName"]; // "America/Sao_Paulo"
    //int zones_0_gmtOffset = zones_0["gmtOffset"]; // -10800
    //long zones_0_timestamp = zones_0["timestamp"]; // 1730054668
    const char* zones_0_zoneFormatted = remotedata["formatted"]; // "America/Sao_Paulo"

    //long conversion= remotedata["timestamp"];
    //Serial.println(conversion);

    rawtime= String(zones_0_zoneFormatted);

    hour= rawtime.substring(11,13).toInt();
    minute= rawtime.substring(14,16).toInt();
    //Serial.println(hour2);
    //Serial.println(zones_0_zoneFormatted);

    Serial.print(hour);
    Serial.print(" ");
    Serial.println(minute);
         
  }
  refreshLeds();
  /*if(hour != previoushour){
    previoushour= hour;
    //digitalWrite(enables, HIGH);
    for(int i=0; i < 5; i++){
   
      if(bitRead((hour+32), i) == 1){
      
        turnledon(i+6);
        //Serial.print("hour ");      
        //Serial.println(i+6);
      }
      
    }
    //digitalWrite(enables, LOW);
    status= "hour";
  }
  
  if(minute != previousminute){
    previousminute= minute;
    //digitalWrite(enables, HIGH);
    for(int j=0; j < 6; j++){
   
      if(bitRead(minute, j) == 1){
      
        turnledon(j);
        //Serial.print("minute ");      
        //Serial.println(j);
      }
    
    }
    //digitalWrite(enables, LOW);
    status= "minute";
  }*/
  
}


int turnledon(int selection){
  
  digitalWrite(bit0, bitRead(selection, 0));
  digitalWrite(bit1, bitRead(selection, 1));
  digitalWrite(bit2, bitRead(selection, 2));
  digitalWrite(bit3, bitRead(selection, 3));
  
  //digitalWrite(bit4, bitRead(selection, 4));
  
  return 0;
}
void refreshLeds() {
  // Refresh hour (5 bits)
  for(int i=0; i < 5; i++){
    if(bitRead(hour, i)){
      turnledon(i+6);
      digitalWrite(enables, LOW);
      delayMicroseconds(800); 
      digitalWrite(enables, HIGH);
    }
  }

  // Refresh minute (6 bits)
  for(int j=0; j < 6; j++){
    if(bitRead(minute, j)){
      turnledon(j);
      digitalWrite(enables, LOW);
      delayMicroseconds(800);
      digitalWrite(enables, HIGH);
    }
  }
}
void updateTimeFromAPI() {
  if (http.GET() == HTTP_CODE_OK) {
    payload = http.getString();
    JsonDocument remotedata;
    deserializeJson(remotedata, payload);

    String timeStr = remotedata["formatted"];
    hour = timeStr.substring(11, 13).toInt();
    minute = timeStr.substring(14, 16).toInt();
  }
  http.end();
}