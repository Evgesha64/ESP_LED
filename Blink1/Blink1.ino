#include <WiFi.h>
#include <PubSubClient.h>

#include <FastLED.h>
#define DEBUG 
// ��������� WiFi
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
    String topics = topic;
    //Serial.println(topics);
    String ckl_led;
   
    if (topics == "topic_ckl") {
     //Serial.println("Выполнена функция msgCallback");
   
        for (int i = 0; i < length; i++) {
            ckl_led += (char)payload[i]; 
        }

    for (int i = 0; i <= 59; ) {
        leds[i].setRGB(my_led[ckl_led.toInt()][i-1][0], my_led[ckl_led.toInt()][i - 1][1], my_led[ckl_led.toInt()][i - 1][2]);
        i++;  
             }
            FastLED.show();
        #ifdef DEBUG
            //Serial.println(ckl_led.toInt());
        #endif // DEBUG
    const char* adresCkl_led = ckl_led.c_str();
    client.publish(topicCallback, adresCkl_led);
    ckl_led = "";
   
  }
   if (topics == "topic_num_RGB") {
       
        String str_led;
        String num_led;
        String color_led1;
        String color_led2;
        String color_led3;

        int f1 = 0;
        for (unsigned int i = 0; i < length; i++) {

            if ((char)payload[i] == '#') {
                if (f1 = 1 && (char)payload[i] == '#') {
                    f1 = 2;
                    continue;
                }
                //Serial.println("(" + str_led + ")");
                str_led = "";
                f1 = 1;
                continue;
            }

            if ((char)payload[i] == ',') {
                if (f1 == 5 && (char)payload[i] == ',') {
                    my_led[str_led.toInt()][num_led.toInt()][0] = color_led1.toInt();
                    my_led[str_led.toInt()][num_led.toInt()][1] = color_led2.toInt();
                    my_led[str_led.toInt()][num_led.toInt()][2] = color_led3.toInt();
                    Serial.println(num_led + " " + color_led1 + " " + color_led2 + " " + color_led3);

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

            if (f1 == 1) {
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
}

// Функция для подключения к MQTT брокеру
void connectMQTT() {
    while (!client.connected()) {
        Serial.println("Подключение к MQTT брокеру...");
        if (client.connect(client_id)) {
            Serial.println("Подключено к MQTT брокеру");
            
            client.subscribe(topic_num_RGB);
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

#ifdef DEBUG
    Serial.begin(115200);
#endif // DEBUG

    connectWiFi();
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(msgCallback);
    //добавляем нашу ленту в библиотеку FastLED
    FastLED.addLeds <WS2812, PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(50);
   
}

    void loop() {
        if (!client.setBufferSize(60000)) {
            Serial.print("Не удалось выделить достаточно памяти для буфера!!! ");
        }
        if (!client.connected()) {
            connectMQTT();  // ������������ � MQTT �������, ���� ���������� ���������
        }
        
        client.loop();
    }