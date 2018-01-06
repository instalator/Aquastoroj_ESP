#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "...";
const char* password = "...";
const char* mqtt_server = "192.168.1.190"; //Сервер MQTT

IPAddress ip(192, 168, 1, 62); //IP модуля
IPAddress gateway(192, 168, 1, 1); // шлюз
IPAddress subnet(255, 255, 255, 0); // маска

WiFiClient espClient;
PubSubClient client(espClient);

#define ID_CONNECT "aquastoroj"
#define LED  2
int bat   =  0;
int otkl  =  0;
int zaliv =  0;
int good  =  0;
int chk   =  0;
int chk1  =  0;
int chk2  =  0;
int chk3  =  0;
int chk4  =  0;
int chk5  =  0;
int opn   =  0;
int cld   =  0;
int prev_bat   =  2;
int prev_otkl  =  2;
int prev_zaliv =  2;
int prev_good  =  2;
int prev_chk   =  2;
int prev_chk1  =  2;
int prev_chk2  =  2;
int prev_chk3  =  2;
int prev_chk4  =  2;
int prev_chk5  =  2;
int prev_opn   =  2;
int prev_cld   =  2;
int reconect_count = 0;
long previousMillis = 0;
String inputString = "";
bool stringComplete = false;

char buf [50];

void setup_wifi() {
  delay(10);
  int cn = 0;
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);
  while (WiFi.status() != WL_CONNECTED) {
    cn++;
    delay(100);
    digitalWrite(LED, !digitalRead(LED));
    if (cn > 200) {
      ESP.restart();
    }
  }
  digitalWrite(LED, LOW);
}

void reconnect() {
  digitalWrite(LED, !digitalRead(LED));
  while (!client.connected()) {
    reconect_count++;
    digitalWrite(LED, !digitalRead(LED));
    if (client.connect(ID_CONNECT)) {
      client.publish("myhome/Aquastoroj/connection", "true");
      client.publish("myhome/Aquastoroj/ButtonOpen", "false");
      client.publish("myhome/Aquastoroj/ButtonClose", "false");
      client.publish("myhome/Aquastoroj/ButtonDisable60", "false");
      client.subscribe("myhome/Aquastoroj/#");
      digitalWrite(LED, HIGH);
      reconect_count = 0;
    } else {
      delay(5000);
    }
    if (reconect_count > 10) {
      ESP.restart();
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String strTopic = String(topic);
  String strPayload = String((char*)payload);
  ///////////
  if (strTopic == "myhome/Aquastoroj/ButtonOpen") {
    if (strPayload == "true") {
      Serial.println("B1");
      client.publish("myhome/Aquastoroj/ButtonOpen", "false");
    }
  }
  //////////
  if (strTopic == "myhome/Aquastoroj/ButtonClose") {
    if (strPayload == "true") {
      Serial.println("B2");
      client.publish("myhome/Aquastoroj/ButtonClose", "false");
    }
  }
  //////////
  if (strTopic == "myhome/Aquastoroj/ButtonDisable60") {
    if (strPayload == "true") {
      Serial.println("B3");
      client.publish("myhome/Aquastoroj/ButtonDisable60", "false");
    }
  }
}

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  ArduinoOTA.setHostname("Aquastoroj");
  ArduinoOTA.onStart([]() {});
  ArduinoOTA.onEnd([]() {});
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {});
  ArduinoOTA.onError([](ota_error_t error) {});
  ArduinoOTA.begin();
}

void loop() {
  ArduinoOTA.handle();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (Serial.available() > 0) {
    char inChar = (char)Serial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
      ParseStr();
    }
  }
}

char* StrToChar (String str) {
  int length = str.length();
  str.toCharArray(buf, length + 1);
  return buf;
}
void ParseStr() {
  String str = inputString;
  if (stringComplete == true) {
    bat = str.substring(0, 1).toInt();
    otkl = str.substring(2, 3).toInt();
    zaliv =  str.substring(4, 5).toInt();
    good = str.substring(6, 7).toInt();
    chk =  str.substring(8, 9).toInt();
    chk1 =  str.substring(10, 11).toInt();
    chk2 =  str.substring(12, 13).toInt();
    chk3 =  str.substring(14, 15).toInt();
    chk4 =  str.substring(16, 17).toInt();
    chk5 =  str.substring(18, 19).toInt();
    opn =  str.substring(20, 21).toInt();
    cld =  str.substring(22, 23).toInt();

    if (millis() - 2000 > previousMillis) {
      //client.publish("myhome/Aquastoroj/test", StrToChar(str.substring(2, 3)));
      previousMillis = millis();
      prev_chk1  =  chk1;
      prev_chk2  =  chk2 ;
      prev_chk3  =  chk3;
      prev_chk4  =  chk4 ;
      prev_chk5  =  chk5;
      if (bat != prev_bat) {
        prev_bat   =  bat;
        client.publish("myhome/Aquastoroj/battery", IntToBool(bat));
      }
      if (otkl != prev_otkl) {
        prev_otkl   =  otkl;
        client.publish("myhome/Aquastoroj/disable60", IntToBool(otkl));
      }
      if (zaliv != prev_zaliv) {
        prev_zaliv   =  zaliv;
        client.publish("myhome/Aquastoroj/flood", IntToBool(zaliv));
      }
      if (good != prev_good) {
        prev_good   =  good;
        client.publish("myhome/Aquastoroj/good", IntToBool(good));
      }
      if (chk != prev_chk) {
        prev_chk   =  chk;
        client.publish("myhome/Aquastoroj/check", IntToBool(chk));
      }
      /*client.publish("myhome/Aquastoroj/check1", IntToBool(chk1));
        client.publish("myhome/Aquastoroj/check2", IntToBool(chk2));
        client.publish("myhome/Aquastoroj/check3", IntToBool(chk3));
        client.publish("myhome/Aquastoroj/check4", IntToBool(chk4));
        client.publish("myhome/Aquastoroj/check5", IntToBool(chk5));*/
      if (opn != prev_opn) {
        prev_opn   =  opn;
        client.publish("myhome/Aquastoroj/open", IntToBool(opn));
      }
      if (cld != prev_cld) {
        prev_cld   =  cld;
        client.publish("myhome/Aquastoroj/closed", IntToBool(cld));
      }
    }
    inputString = "";
    stringComplete = false;
  }
}

const char* IntToBool (int r) {
  if (r > 0) {
    return "true";
  } else {
    return "false";
  }
}
