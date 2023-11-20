#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <FastLED.h>
// Настройки WiFi
//const char* ssid = "TP-Link_D30C";
//const char* password = "43954971";
const char* ssid = "PC64 8943";
const char* password = "12345678";
// Настройки MQTT
const char* mqtt_server = "192.168.0.103";
const int mqtt_port = 1883;  // Порт по умолчанию для MQTT
const char* mqtt_topic = "topik";
const char* topicCallback = "topicCallback";
const char* client_id = "esp32";


#define PIN     15   // пин, к которому подключена лента
#define NUM_LEDS    60   // количество светодиодов в ленте

WiFiClient espClient;
PubSubClient client(espClient);
CRGB leds[NUM_LEDS];

int my_led[60][60][3]{};

// Функция для подключения к WiFi
void connectWiFi() {
    WiFi.begin(ssid, password);
    // Serial.print("Подключение к WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    //Serial.println("");
    //Serial.println("Подключено к WiFi");
}

// Функция, вызываемая при получении сообщений по MQTT
void msgCallback(char* topic, byte* payload, unsigned int length) {
    String ckl_led;
   // String num_led;
   // String color_led1;
   // String color_led2;
   // String color_led3;


    for (unsigned int i = 0; i < length; i++) {
        int f1 = 0;
        if ((char)payload[i] == '#') {
            f1 = 1;
            continue;
        }

        if ((char)payload[i] == ',') {
            f1++;
            continue;
        }
        if (f1 == 0) {
            ckl_led += (char)payload[i];
        }

        /* if(f1 == 1){
         num_led += (char)payload[i];
      // Serial.print((char)payload[i]);
       }
         if(f1 == 2){
         color_led1 += (char)payload[i];
        // Serial.print((char)payload[i]);
       }
         if(f1 == 3){
          color_led2 += (char)payload[i];
        //  Serial.print((char)payload[i]);
       }
         if(f1 == 4){
          color_led3 += (char)payload[i];
         // Serial.print((char)payload[i]);
       } */
    }
   
    for (int i = 0; i <= 59; ) {

        leds[i].setRGB(my_led[ckl_led.toInt()][i - 1][0], my_led[ckl_led.toInt()][i - 1][1], my_led[ckl_led.toInt()][i - 1][2]);
        i++;

    }

    FastLED.show();
    

    

    const char* adresCkl_led = ckl_led.c_str();
    client.publish(topicCallback, adresCkl_led);
   // Serial.println(ckl_led.toInt());

}
// Функция для подключения к MQTT брокеру
void connectMQTT() {
    while (!client.connected()) {
        Serial.println("Подключение к MQTT брокеру...");
        if (client.connect(client_id)) {
            Serial.println("Подключено к MQTT брокеру");
            client.subscribe(mqtt_topic);
        }
        else {
            Serial.print("Не удалось подключиться к MQTT брокеру, ошибка: ");
            Serial.println(client.state());
            delay(2000);
        }
    }

}

void setup() {
   // Serial.begin(115200);

    connectWiFi();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(msgCallback);
    //добавляем нашу ленту в библиотеку FastLED
    FastLED.addLeds <WS2812, PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(50);
    for (int i = 0; i <= 58; i++) {
        for (int i1 = 0; i1 <= 1; i1++) {
            my_led[i][i][0] = 255;

        }
    }
}

void loop() {
    if (!client.connected()) {
        connectMQTT();  // Подключаемся к MQTT брокеру, если соединение прервалос
    }
    client.loop();
}
/*if (!client.connected()) {
  connectMQTT();  // Подключаемся к MQTT брокеру, если соединение прервалось

}
client.loop();
int my_led[][12][6] {
        {{255,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{255,0,0}},
        {{0,0,0},{255,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{255,0,0},{0,0,0}},
        {{0,0,0},{0,0,0},{255,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{255,0,0},{0,0,0},{0,0,0}},
        {{0,0,0},{0,0,0},{0,0,0},{255,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{255,0,0},{0,0,0},{0,0,0},{0,0,0}},
        {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{255,0,0},{0,0,0},{0,0,0},{255,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
        {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,255,0},{0,255,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}
       };

      for (int i = 0; i <= 11 ; i++) {

        leds[i].setRGB(my_led[in_data][i][0], my_led[in_data][i][1], my_led[in_data][i][2]);

      }
      FastLED.show();
      Serial.println(in_data);
      in_data++;
      if(in_data == 6){
        in_data = 0;
      }
       delay(200);
 /*
//Serial.println(ckl_led + "\t" + num_led + "\t" + color_led1 +"\t" + color_led2 +"\t" + color_led3) ;
  /*
   my_led[][num_led.toInt()][0] = color_led1.toInt();
   my_led[][num_led.toInt()][1] = color_led2.toInt();
   my_led[][num_led.toInt()][2] = color_led3.toInt();


  StaticJsonDocument<256> doc;
  deserializeJson(doc, payload, length);
  int nomer = doc["r1"]["s1"];
  int nomer2 = doc["r1"]["s2"];
  Serial.print(nomer);
   Serial.println(nomer2);


 for(int i = 0; i <= 61 ; i++) { // проходимся по его элементам
     leds[i] = CRGB::Black;

  }

  for(int i = 0; i <= 2 ; i++) { // проходимся по его элементам
    Serial.print((int)doc["m1"][i]);
    Serial.print(" ");
  }
  */




  /*
  for(int i = 0; i < myrazdelg.size(); i++) { // проходимся по его элементам
    int num = myrazdelg[i];
    Serial.println(num);
  }
  int myrazdelg[1] = {doc["myrazdel"]["Arr"]};
  Serial.println(myrazdelg[0]);



   for(int i = 0; i < myrazdel.size(); i++) { // проходимся по его элементам
    int num = myrazdel[i];
    Serial.println(num);
  }
   int number;
    for (unsigned int i = 0; i < length; i++) {


       Serial.println((char)payload[i]);
  }
   if ((char)payload[0] == '#'){

   String colorString = "";
   for (unsigned int i = 0; i < length; i++) {
    char c = (char)payload[i];
    colora = c;
       Serial.println(c);
  }

  }
  else {

    for (unsigned int i = 0; i < length; i++) {
     number = atoi((char*)payload);
     //Serial.print(number);
    // CRGB Color = parseColor(color);
    // leds[numbe] = Color; // задаем для первого пикселя синий цвет
    // FastLED.show(); // отправляем информацию на ленту
    Serial.println(payload[0]);
  }
}
leds[number] = colora; // задаем для первого пикселя синий цвет
     FastLED.show(); // отправляем информацию на ленту
     Serial.println();
     */



