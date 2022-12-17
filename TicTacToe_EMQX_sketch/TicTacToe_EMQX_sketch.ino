#include <ArduinoMqttClient.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Keypad.h>

int latchPin = 2; //Pin connected to ST_CP of 74HC595 (Pin12)
int clockPin = 4; //Pin connected to SH_CP of 74HC595 (Pin11)
int dataPin = 15; //Pin connected to DS of 74HC595 (Pin14)

int ledBoard[] = {
  0x00, 0x40, 0x20, 0x0B, 0x60, 0x60, 0x60, 0x00
};

// WiFi
const char *ssid = "Av-hotspot"; // Enter your WiFi name (PHONE HOTSPOT)
const char *password = "mmml4444"; // Enter WiFi password (PHONE HOTSPOT)

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic  = "esp/fum";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

char keys[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'}
  {'*', '0', '#', 'D'}
};

byte rowPins[4] = {14, 27, 26};
byte colPins[4] = {13, 21, 22};

Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 4);
int keyPressed = myKeypad.getKey();

//set interval for sending messages (milliseconds)
const long interval = 8000;
unsigned long previousMillis = 0;

int count = 0;

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  //set pins to output

  while (!Serial) {
    ; 
  }

  Serial.print("\nAttempting to connect: ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    // retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(mqtt_broker);

  if (!mqttClient.connect(mqtt_broker, mqtt_port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }

  Serial.println("You're connected to the MQTT broker!");
  Serial.println();

  // set the message receive callback
  mqttClient.onMessage(onMqttMessage);
  Serial.print("Subscribing to topic: ");
  Serial.println(topic);
  Serial.println();
  // subscribe to a topic
  mqttClient.subscribe(topic);
}
  
void loop() {
  int cols;
  mqttClient.poll();

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    //save the last time a message was sent
    previousMillis = currentMillis;

    //record random value from A0, A1, and A2
    int gameStart = 1;

    Serial.print("Sending message to topic: ");
    Serial.println(topic);
    Serial.println(gameStart);
    delay(3000);

    Serial.print("Sending message to topic: ");
    Serial.println(topic);
    Serial.println(keyPressed);

    //send message, the Print interface can be used to set the message contents
    mqttClient.beginMessage(topic);
    mqttClient.print(gameStart);
    mqttClient.endMessage();

    mqttClient.beginMessage(topic);
    mqttClient.print(keyPressed);
    mqttClient.endMessage();

    Serial.println();
  }
}

void onMqttMessage(int msgSize) {
// we received a message, print out the topic and contents
Serial.println("Received a message with topic '");
Serial.print(mqttClient.messageTopic());
Serial.print("', length ");
Serial.print(msgSize);
Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    Serial.print((char)mqttClient.read());
  }
Serial.println();
Serial.println();
}