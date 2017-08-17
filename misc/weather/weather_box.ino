/*
Name:    weather_box.ino
Created: 7/27/2016 1:44:31 PM
Author:  muhammed
*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiType.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#define LED_NUMBER 16
#define MAX_COMMAND_LENGTH 1000
#define BUFFER_SIZE 2000
#define ID 10001
//#define ESP_ID "weather_box"          //Access Point info
//#define ESP_PSW "1234"
//const char *ssid = "pultar";                // WiFi information
//const char *password = "mustafapultar";
const char *host = "hava.parkyeri.com";
const int http_port = 80;
const char *ip_host = "ip-api.com";

const int LED_PIN = 2;                    //the pin leds are connected to
const int PIR_PIN = 13;                   // the pin sensor is connected to
const int DEMO_PIN = 12;        // the pin demo switch is connected to

WiFiServer server(80);
int k = 0;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_NUMBER, LED_PIN, NEO_RGB + NEO_KHZ800);
char command[LED_NUMBER][MAX_COMMAND_LENGTH];
char copy[MAX_COMMAND_LENGTH];
bool run_;
int interval = 30;
int t_n;
void connect_WiFi();
void get_data(int);
int find_value(int x, int x0, int x1, int y0, int y1);
void request_again(int, int, int);
void test_file(int);
void light_leds();
void delete_file();
void flash_leds(WiFiManager *myWiFiManager);
void get_location();

long timer1;
long timer2;
long time_elapsed;
long overall_tick=0;
int var;
int demo_state = 0;
const char * city;
const char * country_code;
bool closing=false;

void setup() {
//  Serial.begin(9600);

  pinMode(LED_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  var = digitalRead(PIR_PIN);
  if (var != HIGH)
  {
    ESP.deepSleep(5000000);
  }
  else
  {
    for (int i = 0; i < LED_NUMBER; i++)
    {
      if (i == 0 || i == 1 || i == 2 || i == 5 || i == 6 || i == 7 || i == 8 || i == 15 )
        strip.setPixelColor(i,0,102,102);

    
    else
      strip.setPixelColor(i,0,0,0);
    }
    strip.show();
    
    connect_WiFi();

  }


}


void loop() {




  timer1 = millis();
  timer2 = millis();
  //connect_WiFi();
  server.begin();
  strip.begin();
  var = digitalRead(PIR_PIN);
  // Serial.println(var);
  demo_state = digitalRead(DEMO_PIN);

  get_location();
  Serial.println(city);
  Serial.println(country_code);
  if (var == HIGH) {


    get_data(demo_state);

    //ESP.wdtDisable();


    //Serial.println("started");

    for (int i = 0; i < LED_NUMBER; i++) {
      t_n = 0;
      test_file(i);

    }

    while (var == HIGH && time_elapsed < 15 * 60 * 1000  )        //?????????????????????????????
    {


      light_leds();
      if (demo_state == 1)
      {
        demo_state = 2;
        get_data(demo_state);
        continue;
      }


      if (demo_state == 2)
      {
        demo_state = 1;
        get_data(demo_state);
        continue;
      }


      if ((time_elapsed) >(15 * 60 * 1000))  //refreshes data if still running after 15 mins
        get_data(demo_state);
      timer1 = millis();
      time_elapsed = timer1 - timer2;
      var = digitalRead(PIR_PIN);


    }
    closing = true;
    light_leds();


  }



  for (int i = 0; i < LED_NUMBER; i++)
    strip.setPixelColor(i, 0, 0, 0);
  strip.show();
  delete_file();

  ESP.deepSleep(1000000);
}



void connect_WiFi()
{
  WiFiManager wifiManager;

  //wifiManager.resetSettings();
  wifiManager.setAPCallback(flash_leds);
  wifiManager.autoConnect("weather_box");
  /*
  //Serial.println("starting...");
  long timer1 = millis();                     //timer
  long timer2 = millis();                     //timer reference
  //Serial.println("connecting to wifi...");            //tries to connect last connected wifi
  while (timer1 - timer2 < 5000) {
  delay(500);
  timer1 = millis();


  }
  if (WiFi.status() == WL_CONNECTED) {
  //Serial.println("previous connection...");
  return;
  }
  else {
  WiFi.disconnect();                      //connecting new wifi
  delay(2000);
  WiFi.mode(WIFI_STA);
  //Serial.println("new connection...");
  WiFi.begin(ssid, password);
  //Serial.println("connected!");
  }
  */
}

void get_data(int demo)
{ //gets color values for LEDs
  WiFiClient client;
  int t = 0;
  bool state = false;
  int p = 0;
  char c;
  
  for (int i = 0; i < LED_NUMBER; i++)
  {
    if (i == 0 || i == 6 || i == 10)
      strip.setPixelColor(i, 20, 0, 50);
    else if (i == 4 || i == 11 || i == 12 || i == 13 || i == 14)
      strip.setPixelColor(i, 20, 0, 50);
    else
      strip.setPixelColor(i, 0, 0, 0);
  }
  strip.show();

  if (client.connect(host, http_port))
  {
    
    client.print(String(
      "GET /") + "request.php?ID=" + ID + "&led_no=-1" +
      "&demo=" + demo +
      "&city=" + city + "%2C" + country_code +
      " HTTP/1.1\r\n" +
      "Host: " + host + "\r\n" +
      "Connection :close" + "\r\n\r\n"
    );
    delay(100);

    for (p = 0; p < LED_NUMBER; p++)
    {

      if (client.connect(host, http_port))
      {
        client.print(String(
          "GET /") + "request.php?ID=" + ID + "&led_no=" + p + "&demo=" + demo + " HTTP/1.1\r\n" +
          "Host: " + host + "\r\n" +
          "Connection :close" + "\r\n\r\n"
        );
        delay(100);
      }
      if (client.connect(host, http_port)) {

        client.print(String(
          "GET /") + ID + " HTTP/1.1\r\n" +
          "Host: " + host + "\r\n" +
          "Connection: close\r\n\r\n");
      }
      int counter = 0;
      t = 0;
      Serial.println("getting data");
      while (client.connected() && !client.available()) delay(1); //waits for data
      while (client.connected() || client.available()) { //connected or data available
        c = client.read();
        if (c == '?') {
          state = true;
          counter++;
        }
        if (counter == 2)
          state = false;
        if (state == true && c != '?') {
          command[p][t] = c;
          Serial.print(c);
          if (command[p][t] != c) {
            //Serial.print("not equal!!  ");

          }
          t++;
        }

      }



      state = false;
      delay(5);
      client.stop();
    }
  }
  
}

int find_value(int x, int x0, int x1, int y0, int y1) {       //x0: previous step tick  x1: next step tikc    x:current tick    y0: previous step color    y1: next step color   y :color we want to find

  int y;
  y = y0 + ((y1 - y0) * (x - x0) / (x1 - x0));
  return y;
}

void request_again(int p_no, int led_no, int demo)
{ //request data again if data received is not working
  //Serial.println(led_no);                               //for some corrupted bytes
  WiFiClient newclient;
  //Serial.println("requesting...");
  int t = 0;
  bool state = false;
  char c;
  int counter = 0;
  if (newclient.connect(host, http_port))
  {
    newclient.print(String(
      "GET /") + "request.php?ID=" + ID + "&led_no=" + led_no + "&demo=" + demo + " HTTP/1.1\r\n" +
      "Host: " + host + "\r\n" +
      "Connection :close" + "\r\n\r\n"
    );
    delay(100);
  }
  if (newclient.connect(host, http_port)) {

    newclient.print(String(
      "GET /") + ID + " HTTP/1.1\r\n" +
      "Host: " + host + "\r\n" +
      "Connection: close\r\n\r\n");
  }
  while (newclient.connected() && !newclient.available()) delay(1); //waits for data
  while (newclient.connected() || newclient.available()) { //connected or data available

    c = newclient.read();
    if (c == '?') {
      counter++;
      t = 0;
    }
    if (counter == 1)
      state = true;
    if (counter == 2)
      state = false;
    if (state == true && c != '?') {
      command[led_no][t] = c;
      //Serial.print(c);
      t++;
    }
  }
  state = false;
  newclient.stop();

  test_file(led_no);

}

void light_leds() {

  //Serial.print("initial colors...");
  long t1 = millis();
  long t3 = millis();
  int next_step_tick = 0;
  int previous_step_tick = 0;
  int tick = 0;
  int r;
  int g;
  int b;
  double dimmer=1;
  while (t1 - t3 < 30000) {
    overall_tick++;
    tick++;
    int current_tick;
    for (int i = 0; i < LED_NUMBER; i++)
    {

      strncpy(copy, command[i], MAX_COMMAND_LENGTH);
      StaticJsonBuffer<BUFFER_SIZE> jsonBuffer2;
      JsonObject& root = jsonBuffer2.parseObject(copy);
      if (!root.success()) {
        //Serial.print(i);
        //Serial.println(".  led failed");
        strip.setPixelColor(i, 0, 0, 0);
        continue;
      }
      // //Serial.println("success!");
      int  max_tick = root["leds"][0]["max_tick"];
      // Serial.println(max_tick);
      if (overall_tick < 20)
      {
        dimmer = 0.1;
      }
      else if (20 <= overall_tick && overall_tick< 40)
      {
        dimmer = 0.3;
      }
      else if (40 <= overall_tick &&  overall_tick < 80)
      {
        dimmer = 0.5;
      }
      else if (80 <= overall_tick && overall_tick < 120)
      {
        dimmer = 0.8;
      }
      else if(closing == true )
      {
        
        if (20000 < (t1-t3) && (t1 - t3) <= 22000)
        {
          dimmer = 0.8;
        }
        else if (22000 < (t1 - t3) && (t1 - t3) <= 24000)
        {
          dimmer = 0.5;
        }
        else if (24000 < (t1 - t3) && (t1 - t3) <= 26000)
        {
          dimmer = 0.3;
        }
        else if (26000 < (t1 - t3) && (t1 - t3) <= 28000)
        {
          dimmer = 0.2;
        }
        if (28000 < (t1 - t3) )
        {
          dimmer = 0.1;
        }
        else
        {
          dimmer = 1;
        }
      }
      else
      {
        dimmer = 1;
      } 
      Serial.println(dimmer);
      Serial.println(overall_tick);
      current_tick = tick % max_tick;
      if (current_tick == 0) {
        r = root["leds"][0]["steps"][0]["red"];
        g = root["leds"][0]["steps"][0]["green"];
        b = root["leds"][0]["steps"][0]["blue"];
        strip.setPixelColor(i, (int)(g * dimmer) , (int)(r * dimmer) ,(int)( b * dimmer ));
      }
      else
      {
        int j = 1;

        while (current_tick > root["leds"][0]["steps"][j]["tick_no"]) {
          j++;
        }
        if (j == max_tick)
          next_step_tick = 0;
        else
          next_step_tick = root["leds"][0]["steps"][j]["tick_no"];


        if (next_step_tick != 0)
          previous_step_tick = root["leds"][0]["steps"][j - 1]["tick_no"];
        else
          previous_step_tick = root["leds"][0]["steps"][j]["tick_no"];
        if (root["leds"][0]["steps"][j]["change"] == 1)
        {
          r = find_value(current_tick, previous_step_tick, next_step_tick, root["leds"][0]["steps"][j - 1]["red"], root["leds"][0]["steps"][j]["red"]);
          g = find_value(current_tick, previous_step_tick, next_step_tick, root["leds"][0]["steps"][j - 1]["green"], root["leds"][0]["steps"][j]["green"]);
          b = find_value(current_tick, previous_step_tick, next_step_tick, root["leds"][0]["steps"][j - 1]["blue"], root["leds"][0]["steps"][j]["blue"]);
        }
        else {
          r = root["leds"][0]["steps"][j - 1]["red"];
          g = root["leds"][0]["steps"][j - 1]["green"];
          b = root["leds"][0]["steps"][j - 1]["blue"];
        }

        strip.setPixelColor(i, (int)(g * dimmer), (int)(r * dimmer), (int)(b * dimmer));
      }
    }
    strip.show();                             //lighting the LEDs
    delay(interval);


    t1 = millis();

  }

}

void test_file(int i) {
  t_n++;
  //checks if all data received are ok
  if (t_n > 5)
    return;

  strncpy(copy, command[i], MAX_COMMAND_LENGTH);
  //Serial.print(i);

  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer3;
  JsonObject& root = jsonBuffer3.parseObject(copy);
  if (!root.success()) {
    //Serial.println(command[i]);
    ////Serial.print(i);
    //Serial.println(".  led failed");
    int p_no = i + 1;
    request_again(p_no, i, demo_state);

  }
  //  else
  //Serial.println("success!");


}

void delete_file() {
  WiFiClient client2;

  if (client2.connect(host, http_port))
  {
    client2.print(String(
      "GET /") + "delete_file.php?ID=" + ID + " HTTP/1.1\r\n" +
      "Host: " + host + "\r\n"
      "Connection :close" + "\r\n\r\n"
    );
    delay(100);
  }
  client2.stop();
}

void flash_leds(WiFiManager *myWiFiManager)
{
  for (int i = 0; i<16; i++)
  {
    if (i == 1 || i == 2 || i == 4 || i == 7 || i == 8 || i == 11 || i == 13 || i == 14)
      strip.setPixelColor(i, 50, 50, 50);
  }
  strip.show();

}

void get_location()
{
  int t = 0;
  int k = 0;
  WiFiClient client_ip;
  if (client_ip.connect(ip_host, http_port)) {

    client_ip.print(String(
      "GET /json/") + " HTTP/1.1\r\n" +
      "Host: " + host + "\r\n" +
      "Connection: close\r\n\r\n");

    while (client_ip.connected() && !client_ip.available()) delay(1);
    while (client_ip.connected() || client_ip.available()) { 

      char c = client_ip.read();
      Serial.print(c);
      if (c == '\n')
        k++;
      if (k == 7) {
        command[0][t] = c;
        t++;
      }
    }
  }

  StaticJsonBuffer<1000> jsonBuffer_ip;
  JsonObject& root_ip = jsonBuffer_ip.parseObject(command[0]);

  city = root_ip["city"];
  Serial.println(city);
  country_code = root_ip["countryCode"];
  Serial.println(country_code);

  client_ip.stop();

}
