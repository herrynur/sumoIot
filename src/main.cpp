#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define led 2

// wifi
const char *ssid = "Asus_X01BDA";
const char *password = "heri1234567";
// mqtt
const char *mqtt_server = "broker.hivemq.com";
#define MQTT_USERNAME ""
#define MQTT_KEY ""
// robotAA adalah id robot yang akan dikirimkan ke mqtt broker
#define topicMaju "sumoGR/robotAA/arahM"
#define topicBelok "sumoGR/robotAA/arahR"

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;

void setup_wifi();
void reconnect();
void pkiri(int speed_);
void pkanan(int speed_);
void callback(char *topic, byte *payload, unsigned int length);
// motor
void mundur(int speed_);
void maju(int speed_);

// speed motor dan setting motor
int speedMaju = 0;
int speedBelok = 0;
#define motorKn1 27
#define motorKn2 26
#define motorKr1 25
#define motorKr2 33

void setup()
{
  // motor
  pinMode(motorKn1, OUTPUT);
  pinMode(motorKn2, OUTPUT);
  pinMode(motorKr1, OUTPUT);
  pinMode(motorKr2, OUTPUT);
  digitalWrite(motorKn1, LOW);
  digitalWrite(motorKn1, LOW);
  digitalWrite(motorKn1, LOW);
  digitalWrite(motorKn1, LOW);
  // serial dan wifi
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop()
{
  // blink a led
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  // maju
  if (speedMaju >= 4 && speedMaju <= 6 && speedBelok == 0)
  {
    Serial.print("Maju dengan speed : ");
    Serial.println(speedMaju);
    maju((speedMaju - 3) * 40);
  }
  // Mundur
  else if (speedMaju >= 1 && speedMaju <= 3 && speedBelok == 0)
  {
    Serial.print("Maju dengan speed : ");
    Serial.println(speedMaju);
    mundur(speedMaju * 40);
  }
  // Belok Kiri
  else if (speedBelok >= 1 && speedMaju == 0 && speedBelok <= 3)
  {
    Serial.print("Belok Kiri dengan speed : ");
    Serial.println(speedBelok);
    pkiri(speedBelok * 40);
  }
  // Belok Kanan
  else if (speedBelok >= 4 && speedMaju == 0 && speedBelok <= 6)
  {
    Serial.print("Belok Kanan dengan speed : ");
    Serial.println(speedBelok);
    pkanan((speedBelok - 3) * 40);
  }
  // kombinasi belok kanan
  else if (speedMaju >= 4 && speedMaju <= 6 && speedBelok >= 4 && speedBelok <= 6)
  {
    Serial.print("Maju dan Belok Kanan dengan speed : ");
    Serial.println(speedMaju);
  }
  // kombinasi belok kiri
  else if (speedMaju >= 4 && speedMaju <= 6 && speedBelok >= 1 && speedBelok <= 3)
  {
    Serial.print("Maju dan Belok Kiri dengan speed : ");
    Serial.println(speedMaju);
  }
  else
  {
    maju(0);
    mundur(0);
  }
}

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // client.publish(topicMaju, "10");
      client.subscribe(topicMaju, 1);
      client.subscribe(topicBelok, 1);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // LED
  if (strcmp(topic, topicMaju) == 0)
  {
    speedMaju = ((char)payload[0] - 48);
    Serial.print("Speed Maju : ");
    Serial.println(speedMaju);
  }
  else if (strcmp(topic, topicBelok) == 0)
  {
    speedBelok = ((char)payload[0] - 48);
    Serial.print("Speed Belok : ");
    Serial.println(speedBelok);
  }
}

// motor
void mundur(int speed_)
{
  analogWrite(motorKn1, speed_);
  analogWrite(motorKn2, 0);
  analogWrite(motorKr1, speed_);
  analogWrite(motorKr2, 0);
}

void maju(int speed_)
{
  analogWrite(motorKn1, 0);
  analogWrite(motorKn2, speed_);
  analogWrite(motorKr1, 0);
  analogWrite(motorKr2, speed_);
}
void mtrknMj(int speed_)
{
  analogWrite(motorKn1, 0);
  analogWrite(motorKn2, speed_);
}

void mtrkrMj(int speed_)
{
  analogWrite(motorKr1, 0);
  analogWrite(motorKr2, speed_);
}

void mtrknMn(int speed_)
{
  analogWrite(motorKn1, speed_);
  analogWrite(motorKn2, 0);
}

void mtrkrMn(int speed_)
{
  analogWrite(motorKr1, speed_);
  analogWrite(motorKr2, 0);
}

void belokiri(int speed_)
{
  mtrknMj(speed_);
  mtrkrMj(speed_ / 2);
}

void belokanan(int speed_)
{
  mtrknMj(speed_ / 2);
  mtrkrMj(speed_);
}

void pkiri(int speed_)
{
  mtrknMj(speed_);
  mtrkrMn(speed_);
}

void pkanan(int speed_)
{
  mtrknMn(speed_);
  mtrkrMj(speed_);
}