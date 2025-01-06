#include <WiFi.h>

// Wi-Fi設定を直接記述
const char* ssid = "iPhoneXS";
const char* password = "Bananaisgood";

WiFiServer server(80);

// ピン定義
const int sensorPin = A0;
const int redLedPin = 2;
const int greenLedPin = 3;
const int blueLedPin = 4;
const int ledPin = 5;

// 定数
const float v = 5.0;
const int resolution = 1024;
const float offset = 0.6;
const float scale = 0.01;

// 変数
int sensorValue = 0;
float voltage = 0;
float temp = 0;

// LED状態定義
struct LedState {
  float minTemp;
  float maxTemp;
  int redState;
  int greenState;
  int blueState;
};

LedState ledStates[] = {
  {25.0, 100.0, HIGH, LOW, LOW},    // 25℃以上: 赤
  {21.0, 24.9, HIGH, HIGH, LOW},   // 21℃以上25℃未満: 赤+緑
  {0.5, 20.9, LOW, HIGH, LOW},     // 0.5℃以上21℃未満: 緑
  {0.0, 0.4, LOW, LOW, LOW}      // 0.5℃未満: 全て消灯
};

void setup() {
  Serial.begin(115200);
  delay(1000);
  while (!Serial);

  // ピンモード設定
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // Wi-Fi接続
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected. IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop() {
  // 温度測定
  sensorValue = analogRead(sensorPin);
  voltage = sensorValue * (v / resolution);
  temp = (voltage - offset) / scale;

  // LED制御
  for (const auto& state : ledStates) {
    if (temp >= state.minTemp && temp <= state.maxTemp) {
      digitalWrite(redLedPin, state.redState);
      digitalWrite(greenLedPin, state.greenState);
      digitalWrite(blueLedPin, state.blueState);
      break;
    }
  }

  if (voltage <= 0.5) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }

  // WiFi通信
  WiFiClient client = server.available();
  if (client) {
    String request = client.readStringUntil('\r');
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println("Refresh: 1");
    client.println();
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<head>");
    client.println("<meta charset=\"UTF-8\">");
    client.println("<title>Temperature Monitor</title>");
    client.println("<style>");
    client.println("body { font-family: Arial, sans-serif; background-color: #f0f8ff; color: #333333; text-align: center; margin: 0; padding: 0; }");
    client.println(".container { display: flex; flex-direction: column; justify-content: center; align-items: center; height: 100vh; }");
    client.println("h1 { font-size: 2.5em; margin-bottom: 0.5em; }");
    client.println(".temperature { font-size: 2em; color: #ff4500; }");
    client.println("footer { margin-top: 2em; font-size: 0.9em; color: #777777; }");
    client.println("</style>");
    client.println("</head>");
    client.println("<body>");
    client.println("<div class=\"container\">");
    client.println("<h1>現在の温度</h1>");
    client.println("<div class=\"temperature\">");
    client.print(temp);
    client.println(" °C</div>");
    client.println("<footer>Powered by Arduino Uno R4 WiFi</footer>");
    client.println("</div>");
    client.println("</body>");
    client.println("</html>");
    client.stop();
  } else {
    Serial.print("Temperature: ");
    Serial.print(temp);
    Serial.println(" °C");
    delay(1000);
  }
}
