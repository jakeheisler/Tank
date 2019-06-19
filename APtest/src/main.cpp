#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

const char *ssid = "GyrpoScopeProject";
const char *pass = "GyrpoScope";

IPAddress ServerIP(192, 168, 4, 1);
IPAddress ClientIP(192, 168, 4, 2);

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

char packetBuffer[255]; //buffer to hold incoming packet
char ReplyBuffer[100];
unsigned int localPort = 2000; // local port to listen for UDP packets

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

int calx =0;
int caly =0;
int calz =0;
//=======================================================================
//                Setup
//=======================================================================
void setup()
{
        Serial.begin(9600);
        Serial.println();
        WiFi.softAP(ssid, pass); //Create Access point
        //Start UDP
        Serial.println("Starting UDP");
        udp.begin(localPort);
        Serial.print("Local port: ");
        Serial.println(udp.localPort());

        /* Initialise the sensor */
        if(!mag.begin())
        {
                /* There was a problem detecting the HMC5883 ... check your connections */
                Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
                while(1);
        }
}
//======================================================================
//                MAIN LOOP
//======================================================================
void loop()
{

        sensors_event_t event;
        delay(2);
        mag.getEvent(&event);
        delay(2);

        calx = event.magnetic.x;
        caly = event.magnetic.y;
        calz = event.magnetic.z;
        delay(2);

        while(1) {


                int packetSize = udp.parsePacket();
                if (packetSize) {
                        Serial.print("Received packet of size ");
                        Serial.println(packetSize);
                        //If serial data is recived send it to UDP
                        int len = udp.read(packetBuffer, 255);
                        if (len > 0) {
                                packetBuffer[len] = 0;
                        }
                        delay(10);
                        Serial.println("Contents:");
                        Serial.println(packetBuffer);
                }
                delay(10);

                sensors_event_t event;
                mag.getEvent(&event);
                /* Display the results (magnetic vector values are in micro-Tesla (uT)) */


                int dirx = event.magnetic.x-calx;
                int diry = event.magnetic.y-caly;
                int dirz = event.magnetic.z-calz;

                char dx[]="           ";
                char dy[]="           ";
                char dz[]="           ";
                char buf[]=" ";

                // itoa (dirx,dx,10);
                // itoa (diry,dy,10);
                // itoa (dirz,dz,10);
                //
                snprintf(dx, 10, "%d", dirx);
                snprintf(dy, 10, "%d", diry);
                snprintf(dz, 10, "%d", dirz);
                //
                strncat (dx, buf, 10);
                strncat (dx,dy, 10);
                strncat (dx, buf, 10);
                strncat (dx,dz, 10);

                strcpy(ReplyBuffer,dx);

                Serial.println(ReplyBuffer);
                //data sent to client

                udp.beginPacket(ClientIP, 2000);
                udp.write(ReplyBuffer);
                udp.endPacket();
                delay(500);

        }
}
