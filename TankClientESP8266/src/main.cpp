
 #include <Arduino.h>
 #include <ESP8266WiFi.h>
 #include <WiFiUdp.h>

// WiFi network name and password:
const char *ssid = "GyrpoScopeProject";
const char *pass = "GyrpoScope";

// IP address to send UDP data to:
// either use the ip address of the server or
// a network broadcast address
IPAddress ServerIP(192, 168, 4, 1);
IPAddress ClientIP(192, 168, 4, 2);
unsigned int localPort = 2000; // local port to listen for UDP packets


// Are we currently connected?
boolean connected = false;
boolean failsafe = false;
char packetBuffer[255];
int mX = 14;
int vcc = 12;
int vdd = 13;
// The udp library class
WiFiUDP udp;


//


void setup() {
        // Initilize hardware serial:
        Serial.begin(9600);

        // Connect to the WiFi network
        WiFi.begin(ssid, pass);   //Connect to access point
        while (WiFi.status() != WL_CONNECTED) {
                delay(500);
                Serial.print(".");
        }

        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(ssid);
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        //Start UDP
        Serial.println("Starting UDP");
        udp.begin(localPort);
        Serial.print("Local port: ");
        Serial.println(udp.localPort());


        pinMode(mX, OUTPUT);
        pinMode(vdd, OUTPUT);
        pinMode(vcc, OUTPUT);
}


// function that takes int from ConvertData and outputs motor controls
void MotorControl(char incoming[], int m) {

        int speed = atoi(incoming);
        int pmw_speed =0;

        if (speed>0) {

                pmw_speed=map(speed, 5, 50, 300,1000);
                digitalWrite(vdd,LOW);
                digitalWrite(vcc,HIGH);
                analogWrite(mX,pmw_speed);
        }
        else if (speed<0) {

                pmw_speed=map(speed, -5, -50, 300,1000);
                digitalWrite(vdd,HIGH);
                digitalWrite(vcc,LOW);
                analogWrite(mX,pmw_speed);
        } else {
                pmw_speed=0;
                digitalWrite(vdd,LOW);
                digitalWrite(vcc,LOW);
                analogWrite(mX,pmw_speed);
        }
}

// function to convert from char to int
void ConvertData(char datain[3]) {
        int i = 0;
        char x[4];
        char y[4];
        char z[4];
        char delim[] = " ";
        char *databuffer = strtok(datain, delim);

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

        MotorControl(y, mX);
}

void loop() {

        int packetSize = udp.parsePacket();
        if (packetSize) {
                //Serial.print("Received packet of size ");
                //Serial.println(packetSize);
                //If serial data is recived send it to UDP
                int len = udp.read(packetBuffer, 255);
                if (len > 0) {
                        packetBuffer[len] = 0;
                }

                delay(2);
                ConvertData(packetBuffer);
                // Serial.println(xyz);
        }

        delay(2);
        // Send a packet
        udp.beginPacket(ServerIP, 2000);
        udp.printf(" ");

}
