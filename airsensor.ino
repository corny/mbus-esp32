#include "EspMQTTClient.h"
#include "settings.h"

#define baudrate 115200
#define MAX_DATA_SIZE 128
#define LED_BUILTIN 2
#define RXD2 9
#define TXD2 10

// All timestamps/periods in milliseconds
unsigned long uptime = 0;
unsigned long last_send = 0;
const unsigned long sending_interval = 15 * 60 * 1000;

#define INIT_BAUD 2400
#define INIT_CHAR 0x55
#define INIT_SIZE 512 // Number of characters

EspMQTTClient client(
  wifi_ssid,
  wifi_key,
  mqtt_server,  // MQTT Broker server ip
  "",           // Can be omitted if not needed
  "",           // Can be omitted if not needed
  "WMZ"         // Client name that uniquely identify your device
);


struct {
  char data[MAX_DATA_SIZE];
  unsigned int length;
} response = { .length = 0 };


void debugf(const char *fmt, ... ) {
  char buf[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt );
  vsnprintf(buf, 128, fmt, args);
  va_end (args);
  Serial.print(buf);
}

void debugf_float(char *fmt, float val) {
  char buf[9];
  dtostrf(val, 6, 2, buf);
  debugf(fmt, buf);
}


bool readWmz()
{
  debugf("reading WMZ\n");
  digitalWrite(LED_BUILTIN, HIGH); // Turn the LED on
  Serial.flush();

  // wakeup sequence
  byte wakeup[8];
  memset(wakeup, INIT_CHAR, sizeof(wakeup));

  unsigned long started = millis(), finished = 0;

  Serial2.begin(INIT_BAUD, SERIAL_8N1);
  
  // write sequence
  int written;
  for (written = 0; written < INIT_SIZE; ) {
    written += Serial2.write(wakeup, sizeof(wakeup));
  }
  Serial2.flush();

  finished = millis();

  // sleep
  delay(120);

  Serial2.begin(INIT_BAUD, SERIAL_8E1);

  // Sende Anfrage Daten Klasse 2 an Adresse 0x00
  char cmd[] = { 0x10, 0x5B, 0x00, 0x5B, 0x16 };
  Serial2.write(cmd, sizeof(cmd));
  Serial2.flush();

  debugf("sent wakeup sequence: %d bytes at %d baud in %d ms\n", written, INIT_BAUD, finished-started);

  // Antwort empfangen
  char *data = response.data;
  response.length = Serial2.readBytes(data + response.length, MAX_DATA_SIZE);
  debugf("received %d bytes\n", response.length);

  // Turn the LED off
  digitalWrite(LED_BUILTIN, LOW);
  
  if (!response.length) {
    return false;
  }
  
  for (int i = 0; i < response.length; i++) {
    debugf("%02x", data[i]);
  }
  debugf("\n");

  if (data[0] != 0x68)
  {
    debugf("Wrong starting byte received");
    return false;
  }
  
  return true;
}

void sendData() {
  // Send data to MQTT
  client.publish(mqtt_topic, (const uint8_t*) response.data, response.length, true);
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED pin as an output
  Serial.begin(baudrate);

  uint32_t chipId = 0;
  for(int i=0; i<17; i=i+8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  Serial.printf("ESP32 Chip model = %s Rev %d\n", ESP.getChipModel(), ESP.getChipRevision());
  Serial.printf("This chip has %d cores\n", ESP.getChipCores());
  Serial.print("Chip ID: ");
  Serial.println(chipId);

  client.enableDebuggingMessages();
  client.enableOTA(ota_key);
}


void onConnectionEstablished() {
  Serial.println("connection established");
}


void loop() {
  client.loop();
  uptime = millis();

  // uptime restarted at zero? (overflow after 50 days)
  if (last_send > uptime)
    last_send = 0;

  // Sending now?
  if (client.isConnected() && (last_send == 0 || last_send + sending_interval < uptime)) {
    debugf("Uptime: %04d\n", uptime / 1000);

    if (readWmz()){
      last_send = uptime;
      sendData();
      debugf("finished successfully\n");
    }else{
      debugf("failed\n");
      delay(1000);
    }

    Serial.flush();
  }
}
