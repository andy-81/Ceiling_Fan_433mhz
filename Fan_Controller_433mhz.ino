#include <RCSwitch.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <map>

const char* mqttTopic = "bedroom-fan";
std::map<String, unsigned long> messageCodes = {
  {"light-on", 2888128608},
  {"light-off", 2888128625},
  {"fan-off", 2888128145},
  {"fan-speed-1", 2888126688},
  {"fan-speed-2", 2888127219},
  {"fan-speed-3", 2888129691},
  {"fan-speed-4", 2888129062},
  {"fan-speed-5", 2888130354},
  {"fan-speed-6", 2888129606},
  {"fan-wave", 2888127988},
  {"fan-spin-1", 2888130252},
  {"fan-spin-2", 2888130524},
  {"fan-stop", 2888128145},
  {"all-off", 2888128008},
  {"timer-1h", 288812700},
  {"timer-2h", 2888128805},
  {"timer-4h", 2888128805}
};

WiFiClient espClient;
PubSubClient client(espClient);
RCSwitch mySwitch = RCSwitch();

void setup() {
  Serial.begin(115200);

  setupRcSwitch(15, 1, 295);
  setupWifi("ssid", "password"); //replace ssid and password with your login details

  client.setServer("homeassistant.local", 1883); //MQTT server address and port
  client.setCallback(callback);
}

void sendRadioCommand(const String& message) {
  if (messageCodes.find(message) != messageCodes.end()) {
    mySwitch.send(messageCodes[message], 32);
  } else {
    Serial.println("Received an unrecognized message");
  }
}

void setupRcSwitch(int pin, int protocol, int pulseLength) {
  mySwitch.enableTransmit(digitalPinToInterrupt(pin)); 
  mySwitch.setProtocol(protocol);
  mySwitch.setPulseLength(pulseLength);
}

void setupWifi(const char* ssid, const char* password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", "username", "password")) { //I have added username and password down here but trying to work out if I can add it higher up
      Serial.println("connected");
      client.subscribe(mqttTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  sendRadioCommand(message);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(2); // allow the CPU to switch to other tasks
}
