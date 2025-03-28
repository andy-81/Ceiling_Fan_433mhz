#include <RCSwitch.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <map>

//defining the variables to use in code. Update here only
const char* netSsid = "ssid"; //Your Network SSID
const char* netPassword = "password"; //Your Network Password
const char* mqttServer= "homeassistant.local"; //Your MQTT Server address
int mqttPort = 1883; //The MQTT port
const char* mqttUser = "user"; //MQTT Username
const char* mqttPass = "password"; //MQTT Password
int transmitPort = 15; //GPIO port the Transmitter is plugged into
int switchVersion = 1; //Version as determined by received data
int switchPulse = 281; //Pulse length as determined by the received data
int receivePort = 13; //Reciever GPIO Port

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

  setupRcSwitch(transmitPort, switchVersion, switchPulse);
  setupWifi(netSsid, netPassword);

  client.setServer(mqttServer, mqttPort); //MQTT server address and port
  client.setCallback(callback);

  mySwitch.enableReceive(digitalPinToInterrupt(receivePort));
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
    if (client.connect("ESP32Client", mqttUser, mqttPass)) { //I have added username and password down here but trying to work out if I can add it higher up
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

  if (mySwitch.available()) {
    unsigned long receivedCode = mySwitch.getReceivedValue();
    if (receivedCode == 0) {
      Serial.println("Unknown encoding");
    } else {
      Serial.print("Received ");
      Serial.println(receivedCode);
      client.publish("remotereceived/receivedSignal", String(receivedCode).c_str());
      client.publish("remotereceived/receivedbit", String(mySwitch.getReceivedBitlength()).c_str());
      client.publish("remotereceived/receivedProto", String(mySwitch.getReceivedProtocol()).c_str());
      client.publish("remotereceived/receivedLength", String(mySwitch.getReceivedDelay()).c_str());
    }
    mySwitch.resetAvailable();
  }

static unsigned long lastCheck = millis();
if (millis() - lastCheck > 10000) {
  Serial.println("Receiver still active...");
  lastCheck = millis();

  yield();
  //delay(10); // Small delay helps multitasking
}


}
