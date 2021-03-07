#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <DHT.h>

#define DHTPIN 23
#define DHTTYPE DHT11
#define FIREBASE_HOST ""
#define FIREBASE_AUTH ""
#define SERVO_PIN1 5
#define buzzer 23

String TD32_Get_Firebase(String path);
int TD32_Set_Firebase(String path, String value, bool push = false);
int TD32_Push_Firebase(String path, String value);

// Replace with your network credentials
const char *ssid = "";
const char *password = "";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
WiFiServer server(80);
DHT dht(DHTPIN, DHTTYPE);

String formattedDate;
String dayStamp;
String timeStamp;
String timeWSec;
String Hour;
String Minute;
String Second;
String setHour[5];
String setMinute[5];
String setSecond[5];
String Set[5];
String Button;
float localHum = 0;
float localTemp = 0;
float localTempSum = 0;
int channel = 0;
int freq = 50;
int resolution = 8;
int dutyCycle = 10;
int cnt;
uint32_t timer;

void setup()
{
  Serial.begin(115200);
  wifi();
  timeClient.begin();
  timeClient.setTimeOffset(25200);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, HIGH);
  ledcSetup(channel, freq, resolution);
  ledcAttachPin(SERVO_PIN1, channel);
  ledcWrite(channel, dutyCycle);
}

void loop()
{
  while (!timeClient.update())
  {
    timeClient.forceUpdate();
  }

  dateTime();
  Button = TD32_Get_Firebase("Data/Button/b1"); // Read before check
  for (int j = 0; j < 5; j++)
  {
    if (timeWSec == Set[j] || Button == "1")
    {
      Button = "0";
      TD32_Set_Firebase("Data/Button/b1", "0");
      Serial.println("Feeding");
      ledcWrite(channel, 21.5);
      delay(15000);
      ledcWrite(channel, 10);
      delay(20000);
    }
  }
  getDHT();
  TD32_Set_Firebase("Sensor/Temp", String(localTemp));
  TD32_Set_Firebase("Sensor/Hum", String(localHum));
}

const char *FIREBASE_ROOT_CA = ""

int TD32_Set_Firebase(String path, String value, bool push){
  WiFiClientSecure ssl_client;
  String host = String(FIREBASE_HOST);
  host.replace("https://", "");

  if (host[host.length() - 1] == '/')
    host = host.substring(0, host.length() - 1);
  String resp = "";

  int httpCode = 404; 

  String firebase_method = (push) ? "POST " : "PUT ";
  ssl_client.setCACert(FIREBASE_ROOT_CA);
  if (ssl_client.connect(host.c_str(), 443))
  {
    String uri = ((path[0] != '/') ? String("/") : String("")) + path + String(".json?auth=") + String(FIREBASE_AUTH);
    String request = "";
    request += firebase_method + uri + " HTTP/1.1\r\n";
    request += "Host: " + host + "\r\n";
    request += "User-Agent: TD_ESP32\r\n";
    request += "Connection: close\r\n";
    request += "Accept-Encoding: identity;q=1,chunked;q=0.1,*;q=0\r\n";
    request += "Content-Length: " + String(value.length()) + "\r\n\r\n";
    request += value;

    ssl_client.print(request);
    while (ssl_client.connected() && !ssl_client.available())
      delay(10);
    if (ssl_client.connected() && ssl_client.available())
    {
      resp = ssl_client.readStringUntil('\n');
      httpCode = resp.substring(resp.indexOf(" ") + 1, resp.indexOf(" ", resp.indexOf(" ") + 1)).toInt();
    }
    ssl_client.stop();
  }
  else
  {
    Serial.println("[Firebase] can't connect to Firebase Host");
  }
  return httpCode;
}

int TD32_Push_Firebase(String path, String value){
  return TD32_Set_Firebase(path, value, true);
}

String TD32_Get_Firebase(String path){
  WiFiClientSecure ssl_client;
  String host = String(FIREBASE_HOST);
  host.replace("https://", "");
  if (host[host.length() - 1] == '/')
    host = host.substring(0, host.length() - 1);
  String resp = "";
  String value = "";
  ssl_client.setCACert(FIREBASE_ROOT_CA);
  if (ssl_client.connect(host.c_str(), 443))
  {
    String uri = ((path[0] != '/') ? String("/") : String("")) + path + String(".json?auth=") + String(FIREBASE_AUTH);
    String request = "";
    request += "GET " + uri + " HTTP/1.1\r\n";
    request += "Host: " + host + "\r\n";
    request += "User-Agent: TD_ESP32\r\n";
    request += "Connection: close\r\n";
    request += "Accept-Encoding: identity;q=1,chunked;q=0.1,*;q=0\r\n\r\n";

    ssl_client.print(request);
    while (ssl_client.connected() && !ssl_client.available())
      delay(10);
    if (ssl_client.connected() && ssl_client.available()){
      while (ssl_client.available())
        resp += (char)ssl_client.read();
        value = resp.substring(resp.lastIndexOf('\n') + 1, resp.length() - 1);
    }
    ssl_client.stop();
  }else{
    Serial.println("[Firebase] can't connect to Firebase Host");
  }
  return value;
}
