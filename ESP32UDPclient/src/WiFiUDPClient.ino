/*
   ESP32 program
 */
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// WiFi network name and password:
const char *networkName = "GyrpoScopeProject";
const char *networkPswd = "GyrpoScope";

// IP address to send UDP data to:
// either use the ip address of the server or
// a network broadcast address
const char *udpAddress = "192.168.4.2";
const int udpPort = 2000;

// Are we currently connected?
boolean connected = false;
boolean failsafe = false;
char packetBuffer[55];
int mX = 23;
int dX = 27;
// The udp library class
WiFiUDP udp;

void setup() {
        // Initilize hardware serial:
        Serial.begin(9600);

        // Connect to the WiFi network
        connectToWiFi(networkName, networkPswd);
        pinMode(mX, OUTPUT);
        pinMode(dX, OUTPUT);
}

void loop() {
        // only send data when connected
        if (connected) {
                int packetSize = udp.parsePacket();
                if (packetSize) {
                        // Serial.print("Received packet of size ");
                        // Serial.println();
                        // If serial data is recived send it to UDP
                        int len = udp.read(packetBuffer, packetSize);
                        if (len > 0) {
                                packetBuffer[len] = 0;
                        }
                        delay(2);
                        // Serial.print("Contents:");
                        // Serial.println(packetBuffer);

                        ConvertData(packetBuffer);

                        // Serial.println(xyz);
                }
                delay(2);
                // Send a packet
                udp.beginPacket(udpAddress, udpPort);
                udp.printf(" ");
        }
        digitalWrite(dX, LOW);
        digitalWrite(mX, LOW);
        delayMicroseconds(800);
        digitalWrite(mX, HIGH);
        delayMicroseconds(800);
}

void connectToWiFi(const char *ssid, const char *pwd) {
        Serial.println("Connecting to WiFi network: " + String(ssid));

        // delete old config
        WiFi.disconnect(true);
        // register event handler
        WiFi.onEvent(WiFiEvent);

        // Initiate connection
        WiFi.begin(ssid, pwd);

        Serial.println("Waiting for WIFI connection...");
}
// wifi event handler
void WiFiEvent(WiFiEvent_t event) {
        switch (event) {
        case SYSTEM_EVENT_STA_GOT_IP:
                // When connected set
                Serial.print("WiFi connected! IP address: ");
                Serial.println(WiFi.localIP());
                // initializes the UDP state
                // This initializes the transfer buffer
                udp.begin(WiFi.localIP(), udpPort);
                connected = true;
                break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
                Serial.println("WiFi lost connection");
                connected = false;
                break;
        }
}
// function to convert from char to int
char ConvertData(char datain[3]) {
        int i = 0;
        char x[4];
        char y[4];
        char z[4];
        char delim[] = " ";
        char *databuffer = strtok(datain, delim);
        char dataout[4];

        while (databuffer != NULL) {
                i++;
                if (i == 1) {
                        strcpy(x, databuffer);
                        //Serial.println(x);
                        databuffer = strtok(NULL, " ");
                        ;
                } else if (i == 2) {
                        strcpy(y, databuffer);
                        // Serial.println(y);
                        databuffer = strtok(NULL, " ");
                        ;
                } else if (i == 3) {
                        strcpy(z, databuffer);
                        // Serial.println(z);
                        databuffer = strtok(NULL, " ");
                        ;
                } else {
                        i = 0;
                }
        }
        MotorControl(y, mX, dX);
        // rate(y);
        // rate(z);
        // return (dataout[]);
}
// function that takes int from ConvertData and outputs motor controls
void MotorControl(char incoming[], int m, int dirpin) {

        int speed = atoi(incoming);
        int time =0;

        if (speed < 0) {
                digitalWrite(dirpin,LOW);
        } else {
                digitalWrite(dirpin,HIGH);
        }
        if (abs(speed) <= 5) {
                time=1000;
        }

        delay(2);
        Serial.println(abs(speed));
        Serial.println(time);

        digitalWrite(m, LOW);
        delayMicroseconds(time);
        digitalWrite(m, HIGH);
        delayMicroseconds(time);
}
