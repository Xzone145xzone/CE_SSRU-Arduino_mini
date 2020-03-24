void Line_Notify1(String message1) ;
void Line_Notify12(String message2) ;

#include <ESP8266WiFi.h>
#include <DHT.h>

#define WIFI_SSID "ssid"
#define WIFI_PASSWORD "pass"

#define LINE_TOKEN_PIR "xxxxxxxxxxxxxxx" // LINE Notify token 1
#define LINE_TOKEN_TMP "xxxxxxxxxxxxxxx" // LINE Notify token 2

#define PirPin D6
#define DHTPIN D7
#define DHTTYPE           DHT11

DHT dht(DHTPIN, DHTTYPE);

String message1 = "%E0%B8%A1%E0%B8%B5%E0%B8%9C%E0%B8%B9%E0%B9%89%E0%B8%9A%E0%B8%B8%E0%B8%81%E0%B8%A3%E0%B8%B8%E0%B8%81";
String message2 = "%E0%B8%AD%E0%B8%B8%E0%B8%93%E0%B8%AB%E0%B8%A0%E0%B8%B9%E0%B8%A1%E0%B8%B4";

bool beep_state = false;
bool send_state = false;
uint32_t ts, ts1, ts2;

void setup() {

  Serial.begin(115200);
  Serial.println();

  pinMode(PirPin, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  dht.begin();

  Serial.println("connecting");
  WiFi.mode(WIFI_STA);
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
    Line_Notify1(message1);
    beep_state = true;
  }
  delay(10);

}

void Line_Notify1(String message) {
  WiFiClientSecure client;

  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("connection failed");
    delay(2000);
    return;
  }

  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(LINE_TOKEN_PIR) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ESP8266\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(String("message=" + message1).length()) + "\r\n";
  req += "\r\n";
  req += "message=" + message1;
  // Serial.println(req);
  client.print(req);

  delay(20);

  // Serial.println("-------------");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
    //Serial.println(line);
  }
  // Serial.println("-------------");
}

////////////////////////////////////////////////////////////////

void read_sensor() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.println(" *C ");
  Line_Notify2(message2 + " " + t + "%C2%B0C");
}

void Line_Notify2(String message2) {
  WiFiClientSecure client;
  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("connection failed");
    return;
  }
  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(LINE_TOKEN_TMP) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ESP8266\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";
  req += "Content-Length: " + String(String("message=" + message2).length()) + "\r\n";
  req += "\r\n";
  req += "message=" + message2;
  client.print(req);
  delay(30);
  ts2 = millis();
  // Serial.println("-----");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
    //Serial.println(line);
  }
  // Serial.println("-----");
}
