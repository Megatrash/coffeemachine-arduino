#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <ESP8266WiFi.h>
const char* ssid     = "zgWifiSpot";
const char* password = "";

const char* host = "coffeemachine.herokuapp.com";
const char* streamId   = "";
const char* privateKey = "";

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

void setup() {

  Serial.begin(115200);
  Serial.println("Adafruit MLX90614 test");
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

  // Log
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Temp sensor init
  mlx.begin();

  // Tx LED
  pinMode(LED_BUILTIN, OUTPUT);
}

void blink(int count, bool longblink=false) {
  for (int i=0; i < count; i++) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    if(longblink)
      delay(500);
  }
}

void post_temp(float ambiant, float object)
{

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 5000;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  // We now create a URI for the request
  String url = "/temp";

  // We prepare the post data
  String postdata = "";
  postdata+="ambiantcelcius=" + String(ambiant);
  postdata+="&objectcelcius=" + String(object);

  // This will send the request to the server
  client.print(String("POST ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: Dobby/1.0\r\n" +
               "Connection: close\r\n"
               "Content-Type: application/x-www-form-urlencoded\r\n"
               "Content-Length: " + postdata.length() + "\r\n\r\n" +
               postdata);
}

void check_temp(float amb, float obj) {

  // No coffee, turn LED off
  if (abs(amb - obj) < 10)
    digitalWrite(LED_BUILTIN, HIGH);
  
  // Coffee and temperature lower than 50 degrees celcius
  else if((int)obj<50)
    blink(5, true);

  // The more you're close to 50 degrees celcius, the fastest the light blinks
  else if((int)obj >= 50 and ((int)obj <= 55)) {
    int blinks = 1;
    if((int)obj < 55)
      blinks = 55-((int)obj);
    blink(blinks);
    digitalWrite(LED_BUILTIN, HIGH);
  }

  // When the beverage temperature is greater than 55 degrees, stay lighted
  else
    digitalWrite(LED_BUILTIN, LOW);
}

void loop() {

  // Read temperatures
  float amb = mlx.readAmbientTempC();
  float obj = mlx.readObjectTempC();

  // Debug
  Serial.print("Ambient = "); Serial.print(amb);
  Serial.print("*C\tObject = "); Serial.print(obj); Serial.println("*C");

  // React according to the beverage temperature
  check_temp(amb, obj);
  
  // Post the temperature to the server
  post_temp(amb, obj);

  // Loop every second and half
  delay(1500);
}
