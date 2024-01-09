#include <WiFi.h>
#include <PubSubClient.h>
#include <FastLED.h>
//#define DEBUG 
#define WiFi_select_2  // Выбор сети WiFi 1 - "TP-Link_D30C", 2 - "PC64 8943"

#ifdef WiFi_select_1
const char* ssid = "TP-Link_D30C";
const char* password = "43954971";
#endif 

#ifdef WiFi_select_2
const char* ssid = "PC64 8943";
const char* password = "12345678";
#endif 

// ��������� MQTT
const char* mqtt_server = "192.168.0.103";
const int mqtt_port = 1883;  // ���� �� ��������� ��� MQTT
const char* mqtt_topic = "topic_ckl";
const char* topicCallback = "topicCallback";
const char* topic_num_RGB = "topic_num_RGB";
const char* topictime = "topictime";
const char* client_id = "esp32";

#define PIN     15   // ���, � �������� ���������� �����
#define NUM_LEDS    60   // ���������� ����������� � �����

WiFiClient espClient;
PubSubClient client(espClient);
CRGB leds[NUM_LEDS];

//int my_led[2][60][3]{};
//int my_led_clear[2]{};

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
    long start = micros();
    String topics = topic;
    //Serial.println(topics);

    if (topics == "topic_ckl") {
        //Serial.println("Выполнена функция msgCallback");





        //Serial.println(ckl_led.toInt());
        FastLED.show();
        String topic_cklv = "1";
        const char* topic_cklv1 = topic_cklv.c_str();
        client.publish(topicCallback, topic_cklv1);



    }

    if (topics == "topic_num_RGB") {

        String str_led;
        String num_led;
        String color_led1;
        String color_led2;
        String color_led3;
        num_led = "";
        color_led1 = "";
        color_led2 = "";
        color_led3 = "";
        FastLED.clear();
        int f1 = 1;
        for (unsigned int i = 0; i < length; i++) {

            /*if ((char)payload[i] == '#') {
                if (f1 == 1 && (char)payload[i] == '#') {
                    f1 = 2;
                    continue;

                }

                str_led = "";
                f1 = 1;
                continue;
            }*/

            if ((char)payload[i] == ',') {

                if (f1 == 4 && (char)payload[i] == ',') {


                    leds[num_led.toInt()].setRGB(color_led1.toInt(), color_led2.toInt(), color_led3.toInt());

                    //my_led[my_led_clear[0]][my_led_clear[1]][0] = 0;
                    //my_led[my_led_clear[0]][my_led_clear[1]][1] = 0;
                    //my_led[my_led_clear[0]][my_led_clear[1]][2] = 0;
                    //Serial.println("(" + str_led + ")");
                    //my_led[str_led.toInt()][num_led.toInt()][0] = color_led1.toInt();
                    //my_led[str_led.toInt()][num_led.toInt()][1] = color_led2.toInt();
                    //my_led[str_led.toInt()][num_led.toInt()][2] = color_led3.toInt();
                    //my_led_clear[0] = str_led.toInt();
                    //my_led_clear[1] = num_led.toInt();
                    num_led = "";
                    color_led1 = "";
                    color_led2 = "";
                    color_led3 = "";
                    f1 = 1;

                    continue;
                }
                f1++;
                continue;
            }
            switch (f1) {
            case 1:
                num_led += (char)payload[i];
                break;
            case 2:
                color_led1 += (char)payload[i];
                break;
            case 3:
                color_led2 += (char)payload[i];
                break;
            case 4:
                color_led3 += (char)payload[i];
                break;

            }

        }

    }

    long end = micros();
    long seconds = (long)end - start;
    String seconds1 = String(seconds);
    const char* seconds2 = seconds1.c_str();
    client.publish(topictime, seconds2);
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


    // Serial.begin(115200);


    connectWiFi();

    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(msgCallback);
    client.setBufferSize(1000);
    //добавляем нашу ленту в библиотеку FastLED
    FastLED.addLeds <WS2812, PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(30);

}

void loop() {

    if (!client.connected()) {
        connectMQTT();  // ������������ � MQTT �������, ���� ���������� ���������
    }

    client.loop();
}