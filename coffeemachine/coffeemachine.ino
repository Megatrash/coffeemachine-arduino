#include <ESP8266WiFi.h>

const char* ssid     = "zgWifiSpot";
const char* password = "***********";

const char* host = "coffeemachine.herokuapp.com";
const char* streamId   = "";
const char* privateKey = "";

boolean alive = false;

void setup() {
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  pinMode(2, OUTPUT);
}

void loop() {
  delay(5000);

  // Serial.print("connecting to ");
  // Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
  String url = "/boil";

  // Serial.print("Requesting URL: ");
  // Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");
  delay(10);

  alive = false;
  
  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\n');
    Serial.print('>' + line + '<');
    if (line == "sudo boil") {
      alive = true;
      Serial.print(">>>> ALIIIIIVE <<<<<\r\n");
      break;
    }
  }

  if(alive) {
    digitalWrite(2, LOW);
    delay(480000);
  } else {
    digitalWrite(2, HIGH);
  }

  Serial.println();
  Serial.println("closing connection");
}

