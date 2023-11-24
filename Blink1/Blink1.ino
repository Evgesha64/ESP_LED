#include <WiFi.h>
#include <PubSubClient.h>
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
    String topics = topic;
    //Serial.println(topics);


    if (topics == "topic_ckl") {
        String ckl_led;
        String num_led;
        String color_led1;
        String color_led2;
        String color_led3;


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

            //Serial.println("Выполнена функция msgCallback");

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
        Serial.begin(115200);
        Serial.begin(115200);



        connectWiFi();
        client.setServer(mqtt_server, mqtt_port);
        client.setCallback(msgCallback);

        client1.setCallback(msgCallbackRGB);
        //добавляем нашу ленту в библиотеку FastLED
        FastLED.addLeds <WS2812, PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
        FastLED.setBrightness(50);
        /*


        }

            }


        }*/
    }

    void loop() {
        if (!client.connected()) {
            connectMQTT();  // Подключаемся к MQTT брокеру, если соединение прервалос
        }

        client.loop();
    }