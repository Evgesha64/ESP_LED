#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <FastLED.h>
//#define DEBUG

//const char* ssid = "TP-Link_D30C";
//const char* password = "43954971";
const char* ssid = "PC64 8943";
const char* password = "12345678";
// ��������� MQTT
const char* mqtt_server = "192.168.0.103";
const int mqtt_port = 1883;  // ���� �� ��������� ��� MQTT
const char* mqtt_topic = "topic_ckl";
const char* topicCallback = "topicCallback";
const char* topic_num_RGB = "topic_num_RGB";
const char* client_id = "esp32";


#define PIN     15   // ���, � �������� ���������� �����
#define NUM_LEDS    60   // ���������� ����������� � �����

WiFiClient espClient;
PubSubClient client(espClient);
PubSubClient client1(espClient);
CRGB leds[NUM_LEDS];

int my_led[60][60][3]{};

// ������� ��� ����������� � WiFi
void connectWiFi() {
    WiFi.begin(ssid, password);
    // Serial.print("����������� � WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    //Serial.println("");
    //Serial.println("���������� � WiFi");
}

// �������, ���������� ��� ��������� ��������� �� MQTT
void msgCallback(char* topic, byte* payload, unsigned int length) {
    String ckl_led;
    
    
        for (unsigned int i = 0; i < length; i++) {
            ckl_led += (char)payload[i]; 
        }
    
    

    for (int i = 0; i <= 59; ) {

        leds[i].setRGB(my_led[ckl_led.toInt()][i - 1][0], my_led[ckl_led.toInt()][i - 1][1], my_led[ckl_led.toInt()][i - 1][2]);
        i++;

    }
    
    
    FastLED.show();
    
    
    

    const char* adresCkl_led = ckl_led.c_str();
    client.publish(topicCallback, adresCkl_led);
   #ifdef DEBUG
    Serial.println(ckl_led.toInt());
   #endif // DEBUG


}
void msgCallbackRGB(char* topic, byte* payload, unsigned int length) {
    String str_led;
    String num_led;
    String color_led1;
    String color_led2;
    String color_led3;
    #ifdef DEBUG
    Serial.println("��������� ������� msgCallbackRGB");
    #endif // DEBUG
        int f1 = 0;
        for (unsigned int i = 0; i < length; i++) {
            
            if ((char)payload[i] == '#') {
               if (f1 = 1 && (char)payload[i] == '#'){
                    f1 = 2;
                    continue;
               }
               str_led = "";
                    f1 = 1;
                     continue;
            }
             
            if ((char)payload[i] == ',') {
                if (f1 == 5 && (char)payload[i] == ',') {
                    my_led[str_led.toInt()][num_led.toInt()][0] = color_led1.toInt();
                    my_led[str_led.toInt()][num_led.toInt()][1] = color_led2.toInt();
                    my_led[str_led.toInt()][num_led.toInt()][2] = color_led3.toInt();

                    
                    num_led = "";
                    color_led1 = "";
                    color_led2 = "";
                    color_led3 = "";

                    f1 = 2;
                    continue;
                }
                f1++;
                continue;
            }

            if (f1 == 1 ) {
                str_led += (char)payload[i];
                // Serial.print((char)payload[i]);
            }
            if (f1 == 2) {
                num_led += (char)payload[i];
                // Serial.print((char)payload[i]);
            }
            if (f1 == 3) {
                color_led1 += (char)payload[i];
                // Serial.print((char)payload[i]);
            }
            if (f1 == 4) {
                color_led2 += (char)payload[i];
                //  Serial.print((char)payload[i]);
            }
            if (f1 == 5) {
                color_led3 += (char)payload[i];
                // Serial.print((char)payload[i]);
            }

        }

                
       

}
void reedRGB() {


 }
// ������� ��� ����������� � MQTT �������
void connectMQTT() {
    while (!client.connected()) {
        Serial.println("����������� � MQTT �������...");
        if (client.connect(client_id)) {
            Serial.println("���������� � MQTT �������");
            client.subscribe(mqtt_topic);
            client1.subscribe(topic_num_RGB);
        }
        else {
            Serial.print("�� ������� ������������ � MQTT �������, ������: ");
            Serial.println(client.state());
            delay(2000);
        }
    }

}

void setup() {
   
  #ifdef DEBUG
    Serial.begin(115200);
  #endif // DEBUG

    
    connectWiFi();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(msgCallback);
    client1.setServer(mqtt_server, mqtt_port);
    client1.setCallback(msgCallbackRGB);
    //��������� ���� ����� � ���������� FastLED
    FastLED.addLeds <WS2812, PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(50);
    /*
    for (int i = 0; i <= 58; i++) {
        int i2 = 0;
        for (int i1 = 0; i1 <= 1; i1++) {
            my_led[i][i][0] = 255;

        }


    }*/
}

void loop() {
    if (!client.connected() && !client1.connected()) {
        connectMQTT();  // ������������ � MQTT �������, ���� ���������� ���������
    }
    client.loop();
    client1.loop();
}
/*if (!client.connected()) {
  connectMQTT();  // ������������ � MQTT �������, ���� ���������� ����������

}
client.loop();
int my_led[][12][6] {
        {{255,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{255,0,0}},
        {{0,0,0},{255,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{255,0,0},{0,0,0}},
        {{0,0,0},{0,0,0},{255,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{255,0,0},{0,0,0},{0,0,0}},
        {{0,0,0},{0,0,0},{0,0,0},{255,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{255,0,0},{0,0,0},{0,0,0},{0,0,0}},
        {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{255,0,0},{0,0,0},{0,0,0},{255,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},
        {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,255,0},{0,255,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}}
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


 for(int i = 0; i <= 61 ; i++) { // ���������� �� ��� ���������
     leds[i] = CRGB::Black;

  }

  for(int i = 0; i <= 2 ; i++) { // ���������� �� ��� ���������
    Serial.print((int)doc["m1"][i]);
    Serial.print(" ");
  }
  */




  /*
  for(int i = 0; i < myrazdelg.size(); i++) { // ���������� �� ��� ���������
    int num = myrazdelg[i];
    Serial.println(num);
  }
  int myrazdelg[1] = {doc["myrazdel"]["Arr"]};
  Serial.println(myrazdelg[0]);



   for(int i = 0; i < myrazdel.size(); i++) { // ���������� �� ��� ���������
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
    // leds[numbe] = Color; // ������ ��� ������� ������� ����� ����
    // FastLED.show(); // ���������� ���������� �� �����
    Serial.println(payload[0]);
  }
}
leds[number] = colora; // ������ ��� ������� ������� ����� ����
     FastLED.show(); // ���������� ���������� �� �����
     Serial.println();
     */



