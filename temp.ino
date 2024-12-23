#include <WiFiNINA.h>

WiFiServer server(80);

const int sensor = A0;

const float v = 5.0;
const int resolution = 1024;
const float offset = 0.6;
const float scale = 0.01;

int sensorValue = 0;
float voltage = 0;
float temp = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  char ssid[50];
  char password[50];

  while (true) {
    Serial.println("SSID:");
    readInput(ssid, sizeof(ssid));

    Serial.println("password:");
    readInput(password, sizeof(password));

    WiFi.begin(ssid, password);

    Serial.println("connecting...");
    unsigned long startTime = millis();
    bool connected = false;

    while (millis() - startTime < 10000) {
      if (WiFi.status() == WL_CONNECTED) {
        connected = true;
        break;
      }
      delay(100);
    }

    if (connected) {
      Serial.println("success");
      Serial.print("IP addr: ");
      Serial.println(WiFi.localIP());
      break;
    } else {
      Serial.println("WiFi接続失敗。もう一度入力してください。");
      Serial.print("error: ");
      Serial.println(WiFi.status());
    }
  }

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');

    sensorValue = analogRead(sensor);
    voltage = sensorValue * (v / resolution);
    temp = (voltage - offset) / scale;

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<head>");
    client.println("<meta charset=\"UTF-8\">");
    client.println("<meta http-equiv='refresh' content='1'>");
    client.println("<title>Temprature</title>");
    client.println("</head>");
    client.println("<body>");
    client.println("<h1>現在の温度</h1>");
    client.print("<p>");
    client.print(temp);
    client.println(" °C</p>");
    client.println("</body>");
    client.println("</html>");

    client.stop();
  }
}

void readInput(char* buffer, size_t length) {
  size_t index = 0;
  while (index < length - 1) {
    if (Serial.available() > 0) {
      char c = Serial.read();
      if (c == '\n') break;
      buffer[index++] = c;
    }
  }
  buffer[index] = '\0';
}
