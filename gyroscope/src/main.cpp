#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
int P1=14;
int P2=12;
int P3=13;

int calx =0;
int caly =0;
int calz =0;

void setup(void)
{
        Serial.begin(9600);
        pinMode(P1, OUTPUT);
        pinMode(P2, OUTPUT);
        pinMode(P3, OUTPUT);


        /* Initialise the sensor */
        if(!mag.begin())
        {
                /* There was a problem detecting the HMC5883 ... check your connections */
                Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
                while(1);
        }
}

void loop(void)
{
        /* Get a new sensor event */
        sensors_event_t event;
        delay(2);
        mag.getEvent(&event);
        delay(2);

        calx = event.magnetic.x;
        caly = event.magnetic.y;
        calz = event.magnetic.z;
        delay(2);

        while(1) {
                sensors_event_t event;
                mag.getEvent(&event);
                /* Display the results (magnetic vector values are in micro-Tesla (uT)) */
                Serial.print(" X: ");
                Serial.print(event.magnetic.x-calx);
                Serial.print(" Y: ");
                Serial.print(event.magnetic.y-caly);
                Serial.print(" Z: ");
                Serial.print(event.magnetic.z-calz);
                Serial.println(" ");

                if (event.magnetic.x<=15 && event.magnetic.x>=-15) {
                        digitalWrite(P1,HIGH);
                } else {
                        digitalWrite(P1,LOW);
                }
                if (event.magnetic.y<=30 && event.magnetic.y>=-30) {
                        digitalWrite(P2,HIGH);
                } else {
                        digitalWrite(P2,LOW);
                }
                if (event.magnetic.z<=30 && event.magnetic.z>=-30) {
                        digitalWrite(P3,HIGH);
                } else {
                        digitalWrite(P3,LOW);
                }

                delay(50);
        }
}
