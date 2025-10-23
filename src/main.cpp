#include <Arduino.h>
#include "secrets.h"
#include "WiFi.h"

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

const int port = 10000;
WiFiServer server(port);
const int LED_PIN = LED_BUILTIN; 
const bool LED_ACTIVE_LOW = true; // true si la LED s'allume quand la broche = LOW
const int LED_ON  = (LED_ACTIVE_LOW ? LOW  : HIGH);
const int LED_OFF = (LED_ACTIVE_LOW ? HIGH : LOW);

void handleTCPClient() {
  WiFiClient client = server.available();
  uint8_t data[31]; // 30 octets + terminaison
  if (client)
  {
    Serial.println("New client from ip: " + client.remoteIP().toString() + ", port: " + String(client.remotePort()));
    /* check client is connected */
    while (client.connected())
    {
      if (client.available())
      {
        int len = client.read(data, 30);
        if (len > 0)
        {
          if (len < 30)
          {
            data[len] = '\0';
          }
          else
          {
            data[30] = '\0';
          }

          String cmd = String((char*)data);
          cmd.trim();            // retire CR/LF et espaces
          cmd.toUpperCase();     // accepte on/On/ON

          Serial.print("client sent: ");
          Serial.println(cmd);

          if (cmd == "ON") {
            digitalWrite(LED_PIN, LED_ON);
            Serial.println("LED -> ON");
            client.println("LED ON");
          } else if (cmd == "OFF") {
            digitalWrite(LED_PIN, LED_OFF);
            Serial.println("LED -> OFF");
            client.println("LED OFF");
          } else {
            client.println("UNKNOWN COMMAND");
          }
        }
      }
    }

    Serial.println("Client disconnected");
    client.stop();
  }
}

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
  default:  
    return "UNKNOWN";
  }
  return "UNKNOWN";
}

void scanNetworks()
{

  int numberOfNetworks = WiFi.scanNetworks();

  // Add delay so the terminal can catch up
  delay(3000);

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

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LED_OFF); // état initial

  // Test visuel rapide pour vérifier la broche et la polarité
  for (int i = 0; i < 3; ++i) {
    digitalWrite(LED_PIN, LED_ON);
    delay(200);
    digitalWrite(LED_PIN, LED_OFF);
    delay(200);
  }

  // Print MAC address
  Serial.println("MCU MAC address: " + WiFi.macAddress());

  scanNetworks();
  connectToNetwork();

  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  // Votre code principal ici
  handleTCPClient();
}
