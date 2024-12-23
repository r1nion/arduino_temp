#include <WiFiNINA.h>

WiFiServer server(80);

const int sensor = A0;

const float v = 3.3;
const int resolution = 1024;
const float offset = 1.2;
const float scale = 0.02;

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
      Serial.println("success.");
      Serial.print("IP addr: ");
      Serial.println(WiFi.localIP());
      break;
    } else {
      Serial.println("Failed. Try again.");
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
    client.println("<title>Temperature Monitor</title>");
    client.println("<style>");
    client.println("body { font-family: Arial, sans-serif; background-color: #f0f8ff; color: #333; text-align: center; }");
    client.println(".temperature { color: #ff4500; font-size: 2em; }");
    client.println("</style>");
    client.println("</head>");
    client.println("<body>");
    client.println("<h1>Current Temperature</h1>");
    client.print("<div class=\"temperature\">");
    client.print(temp);
    client.println(" &deg;C</div>");
    client.println("<footer>Powered by Arduino Nano 33 IoT</footer>");
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
