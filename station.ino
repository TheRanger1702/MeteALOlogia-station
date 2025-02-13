//Internet
#include <WiFi.h>
#include <HTTPClient.h>

//NTP
#include <NTPClient.h>
#include <time.h>

//DHT
#include <DHT.h>
#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

String station_id = "alpha"; //station id
String auth_key = "sample1"; //authorization key
String main_url = "<type your server's IP address here>" + station_id + "/sensors/"; //main url (url excluding sensors)

const char* ntpServer = "pool.ntp.org"; //NTP time server
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 0;

unsigned long lastTime = 0;
unsigned long timerDelay = 10000;

//MAIN CODE
void setup() 
{
  Serial.begin(115200);

  connectToWiFi("<SSID here>", "<WiFi password here>");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  dht.begin();
}

void loop() 
{
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED)
    {
      //tutaj należy wstawić kod odczytów z czujników
      postResults("alpha-temp", "temperature", dht.readTemperature(), getTimestamp());
      postResults("alpha-humid", "humidity", dht.readHumidity(), getTimestamp());
    } else 
    {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

//FUNCTIONS
void connectToWiFi(const char* ssid, const char* password)
{
  WiFi.begin(ssid, password);
  Serial.println(ssid);
  Serial.println(password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

String getTimestamp() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return "TIME_ERROR"; // Error while trying to get current time
    }

    // ISO 8601 time formatting
    char formattedTime[30];
    strftime(formattedTime, sizeof(formattedTime), "%Y-%m-%dT%H:%M:%S", &timeinfo);
    return String(formattedTime);
}

void postResults(String sensor_id, String measurement_type, int measurement, String timestamp) 
{
  WiFiClient client;
  HTTPClient http;

  http.begin(client, main_url + sensor_id + "/reports");

  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", auth_key);

  String json_payload = "{\"timestamp\": \"" + timestamp + "\", " + "\"data\": {\"" + measurement_type + "\": " + measurement + "}}";

  Serial.println(json_payload);
  int httpResponseCode = http.POST(json_payload);

  
  Serial.println(httpResponseCode);

  http.end();
}