/*
 * Step I: Install PubSub library in arduino (https://github.com/knolleary/pubsubclient/)
 * Install mosquitto:  (i) sudo apt-get install mosquitto -y
 *                    (ii) sudo apt-get install mosquitto mosquitto-clients -y
 * Test mosquitto :  (i) mosquitto_sub -t "test_sensor_topic"
 *                  (ii) mosquitto_pub -t "test_sensor_topic" -m "HELLO WORLD!"
 * Upload the code on nodemcu
*/

#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

const char *ssid = "Patil";
const char *password = "Ganesh@123";

const int SWITCH_PIN = 34;
int LED_BUILTIN = 2;
const char *ID = "esp32";
const char *sensor_topic = "led";
const char *LED_state_topic = "led/state";

IPAddress broker(192,168,0, 103); // IP address of your MQTT broker eg. 192.168.1.50
WiFiClient wclient;
StaticJsonDocument<256> doc;
PubSubClient client(wclient); // Setup MQTT client
bool state=0;

void LEDCallback(char* state_topic, byte* payload, unsigned int length) {
  String response;
  for(int i = 0; i < length; i++) { 
    response += (char)payload[i];
  }

  Serial.println("Message arrived: [" + String(state_topic) + "] " + response);
  deserializeJson(doc, (const byte*)payload, length);

  if(doc["data"] == "on")
    digitalWrite(LED_BUILTIN, HIGH);
  else
    digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  
}

// Connect to WiFi network
void setup_wifi() {
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // Connect to network

  while (WiFi.status() != WL_CONNECTED) { // Wait for connection
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Reconnect to client
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ID)) {
      client.subscribe(LED_state_topic);
      Serial.println("connected");
      Serial.print("Publishing to: ");
      Serial.println(sensor_topic);
      Serial.println("Subscribing to: ");
      Serial.println(LED_state_topic);
      Serial.println('\n');

    } else {
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200); // Start serial communication at 115200 baud
  pinMode(SWITCH_PIN,INPUT);  // Configure SWITCH_Pin as an input
  pinMode(LED_BUILTIN, OUTPUT);
  delay(1000);
  setup_wifi(); // Connect to network
  client.setServer(broker, 1883);
  client.setCallback(LEDCallback);
}

void loop() {
  if (!client.connected()) { // Reconnect if connection is lost
    reconnect();
  }
  client.loop();

  Serial.println(digitalRead(SWITCH_PIN));
  
  // if the switch is being pressed
  if(digitalRead(SWITCH_PIN) == 0)  {
      client.publish(sensor_topic, "{\"data\": \"off\"}");
      Serial.println((String)sensor_topic + " => off");
  }
  else  {
    client.publish(sensor_topic, "{\"data\": \"on\"}");
    Serial.println((String)sensor_topic + " => on");
  }
  delay(500);
}
