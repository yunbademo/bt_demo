#include <LBT.h>
#include <LBTServer.h>
#define SPP_SVR "ARD_SPP"

const int LED_PIN = 13;
const char *led_on = "on";
const char *led_off = "off";
const char *led_get = "get";

void flash(int LedPin) {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LedPin, HIGH);
    delay(100);
    digitalWrite(LedPin, LOW);
    delay(100);
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("start BTS\n");

  bool success = LBTServer.begin((uint8_t*)SPP_SVR);
  if ( !success ) {
    Serial.println("Cannot begin Bluetooth Server successfully\n");
    // don't do anything else
    delay(0xffffffff);
  } else {
    LBTDeviceInfo info;
    if (LBTServer.getHostDeviceInfo(&info)) {
      Serial.printf("LBTServer.getHostDeviceInfo [%02x:%02x:%02x:%02x:%02x:%02x]",
                    info.address.nap[1], info.address.nap[0], info.address.uap, info.address.lap[2], info.address.lap[1], info.address.lap[0]);
    } else {
      Serial.println("LBTServer.getHostDeviceInfo failed\n");
    }
    Serial.println("Bluetooth Server begin successfully\n");
  }
}

void loop() {
  if (LBTServer.connected()) {
    //  Serial.println("connected");
    while (true) {
      if (LBTServer.available() && LBTServer.connected()) {
        char buffer[32] = {0};
        memset(buffer, 0, sizeof(buffer));
        int read_size = LBTServer.readBytes((uint8_t*)buffer, 32);
        if (!read_size)
          break;

        if (strncmp(buffer, led_on, strlen(led_on)) == 0) {
          digitalWrite(LED_PIN, HIGH);
        } else if (strncmp(buffer, led_off, strlen(led_off)) == 0) {
          digitalWrite(LED_PIN, LOW);
        }
        char* buffer_w;
        int val = digitalRead(LED_PIN);
        switch (val) {
          case HIGH:
            buffer_w = "on";
            break;
          case LOW:
            buffer_w = "off";
            break;
          default:
            buffer_w = "unknown";
            break;
        }

        Serial.printf("read size: %d, buf: %s\r\n", read_size, buffer);
        int write_size = LBTServer.write((uint8_t*)buffer_w, strlen(buffer_w));
        Serial.printf("write_size [%d]\r\n", write_size);
      } else {
        //    Serial.println("not available");
        break;
      }
      delay(100);
    }
  } else {
    Serial.println("not connected");
    bool connected = LBTServer.accept(10);
    if ( !connected ) {
      Serial.println("No connection request yet\n");
      delay(1000);
    } else {
      Serial.println("Connected\n");
    }
  }
  delay(100);
}
