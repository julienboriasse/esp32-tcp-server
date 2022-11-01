// Source : http://www.iotsharing.com/2017/05/tcp-udp-ip-with-esp32.html
// Source : https://www.dfrobot.com/blog-948.html

#include <Arduino.h>
#include "secrets.h"
#include "WiFi.h"

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

const int port = 10000;
WiFiServer server(port);

String translateEncryptionType(wifi_auth_mode_t encryptionType)
{

  switch (encryptionType)
  {
  case (WIFI_AUTH_OPEN):
    return "Open";
  case (WIFI_AUTH_WEP):
    return "WEP";
  case (WIFI_AUTH_WPA_PSK):
    return "WPA_PSK";
  case (WIFI_AUTH_WPA2_PSK):
    return "WPA2_PSK";
  case (WIFI_AUTH_WPA_WPA2_PSK):
    return "WPA_WPA2_PSK";
  case (WIFI_AUTH_WPA2_ENTERPRISE):
    return "WPA2_ENTERPRISE";
  }
}

void scanNetworks()
{

  int numberOfNetworks = WiFi.scanNetworks();

  Serial.print("Number of networks found: ");
  Serial.println(numberOfNetworks);

  for (int i = 0; i < numberOfNetworks; i++)
  {

    Serial.print("Network name: ");
    Serial.println(WiFi.SSID(i));

    Serial.print("Signal strength: ");
    Serial.println(WiFi.RSSI(i));

    Serial.print("MAC address: ");
    Serial.println(WiFi.BSSIDstr(i));

    Serial.print("Encryption type: ");
    String encryptionTypeDescription = translateEncryptionType(WiFi.encryptionType(i));
    Serial.println(encryptionTypeDescription);
    Serial.println("-----------------------");
  }
}

void connectToNetwork()
{
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }

  Serial.println("Connected to network");
}

void handleTCPClient() {
  WiFiClient client = server.available();
  uint8_t data[30];
  if (client)
  {
    Serial.println("New client");
    /* check client is connected */
    while (client.connected())
    {
      if (client.available())
      {
        int len = client.read(data, 30);
        if (len < 30)
        {
          data[len] = '\0';
        }
        else
        {
          data[30] = '\0';
        }
        Serial.print("client sent: ");
        Serial.println((char *)data);
      }
    }
  }
}

void setup()
{

  Serial.begin(115200);

  scanNetworks();
  connectToNetwork();

  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop()
{
  handleTCPClient();
}