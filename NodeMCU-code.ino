#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <string.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

const char* mqtt_server = "test.mosquitto.org";
#define trigger D1  //to change to another wifi
WiFiClient espClient;
PubSubClient client(espClient);



void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);  //to print the topic received from node red 
  Serial.print("] ");
  for (int i = 0; i < length; i++) {     
    Serial.print((char)payload[i]);       //to print the payload receieved from node red
  }
  Serial.println();
  
  // Switch on the LED if received ON to indicate switch on Motor
  if ((char)payload[0] == 'O' && (char)payload[1] == 'N' ) {
    char *buff=(char *)calloc(length-3,sizeof(char));   
    for (int j = 3; j < length; j++)
    {
      *(buff+j-3)=*(payload+j);
    }
    int deep_t =(String(buff)).toInt();   // calculte the received time in minutes
    Serial.print("deep sleep time is:");
    Serial.print(deep_t);
    Serial.println(" minutes");
    int value_micro_second=deep_t*60000000;   // convert time (minutes to seconds)
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on 
    delay(2000);
    
    ESP.deepSleep(value_micro_second); //deep sleep for time value_micro_second
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off 
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      
     
      // subscribe the topic AgroIoT/IN
      client.subscribe("AgroIoT/IN");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(115200);
  pinMode(trigger,INPUT); 
  //to connect wifi
  WiFiManager wifiManager;
    
    //wifiManager.resetSettings();
    
  wifiManager.autoConnect("AgroIoT");

    //if the connection is successful, you will get here.
  Serial.println("connected... :)");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

void loop() {
  // put your main code here, to run repeatedly:
  // to change the wifi
  if(digitalRead(trigger) == HIGH){
      WiFiManager wifiManager;
      
      wifiManager.resetSettings();
      
      wifiManager.autoConnect("AgroIoT");
  
      //if the connection is successful, you will get here.
      Serial.println("connected... :)");
  }  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

}
