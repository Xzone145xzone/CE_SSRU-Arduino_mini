#include <ESP8266WiFi.h>
#include <DHT.h>
#include <WiFiClientSecureAxTLS.h> //กรณีข้ึน Error ใหเ้อาบรรทดัน้ีออก
// Config connect WiFi
#define WIFI_SSID "iPhone"
#define WIFI_PASSWORD "123456789"
// Line config
#define LINE_TOKEN "egd1IYYUoNeDaFMdpnCC5QCzJs5hh6bMmZWSJuF5IFz"
#define PirPin D6
#define DHTPIN D7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
uint32_t ts, ts1, ts2;
bool beep_state = false;
bool send_state = false;
String message1 = "คุณได้รับจดหมาย"; 
void setup() {
Serial.begin(9600);
WiFi.mode(WIFI_STA);
// connect to wifi.
WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
Serial.print("connecting");
while (WiFi.status() != WL_CONNECTED) {
Serial.print(".");
delay(500);
}
Serial.println();
Serial.print("connected: ");
Serial.println(WiFi.localIP());
delay(10000);
Serial.println("Pir Ready!!");
read_sensor();
ts = ts1 = ts2 = millis();
}
void loop() {
ts = millis();

if (WiFi.status() == WL_CONNECTED) {
digitalWrite(LED_BUILTIN, LOW);
} else {
digitalWrite(LED_BUILTIN, HIGH);
}
if ((ts - ts2 >= 60000) && (WiFi.status() == WL_CONNECTED)) {
read_sensor();
}

if ((ts - ts1 >= 5000) && (beep_state == true)) {
beep_state = false;
}

if ((digitalRead(PirPin) == HIGH) && (beep_state == false) && (WiFi.status() == WL_CONNECTED)) {
while (digitalRead(PirPin) == HIGH) delay(100);
Serial.println("Detect !");
Line_Notify(message1);
beep_state = true;
}
delay(10);
}
void Line_Notify(String message) {
axTLS::WiFiClientSecure client; //กรณีข้ึน Error ให้ลบ axTLS:: ขา้งหนา้ทิ้ง
if (!client.connect("notify-api.line.me", 443)) {
Serial.println("connection failed");
return;
}
String req = "";
req += "POST /api/notify HTTP/1.1\r\n";
req += "Host: notify-api.line.me\r\n";
req += "Authorization: Bearer " + String(LINE_TOKEN) + "\r\n";
req += "Cache-Control: no-cache\r\n";
req += "User-Agent: ESP8266\r\n";
req += "Connection: close\r\n";
req += "Content-Type: application/x-www-form-urlencoded\r\n";
req += "Content-Length: " + String(String("message=" + message).length()) + "\r\n";
req += "\r\n";
req += "message=" + message;
// Serial.println(req);
client.print(req);
delay(20);
// Serial.println("-------------");
while(client.connected()) {
String line = client.readStringUntil('\n');
if (line == "\r") {
break;
}
}
}
void read_sensor() {
float h = dht.readHumidity();
}
