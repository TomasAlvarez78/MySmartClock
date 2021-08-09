#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Arduino_JSON.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <Ticker.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "icons.h"
#include "bitmapAnimations.h"
#include "buzzerNotes.h"

#define ScreenWidth 128
#define ScreenHeight 64

#define OLED_RESET -1
#define ScreenAddress 0x3C

Adafruit_SSD1306 display(ScreenWidth, ScreenHeight, &Wire, OLED_RESET);

const char *ssid = "LaGuaiFai2.4G";
const char *password = "Lululinda2021";

int xx = 40,yy = 16, tt = 0;

void RussiaAnthem();
void setCurrentSun();
void note(unsigned int frequency, unsigned long duration);
void RGB_color(int red_light_value, int green_light_value, int blue_light_value);
void showClock();
void showWeather();
void askWeather();
void ledTempShow();
void ShowWifi(int xx, int yy, int tt);
void ShowWalking(int xx, int yy, int tt);
void ShowDumbbell(int xx, int yy, int tt);
void ShowDeadLift(int xx, int yy, int tt);
void ShowOffice(int xx, int yy, int tt);

const uint8_t *askIcon(String main, String desc);

HTTPClient http;
WiFiClient cliente;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

String linkApi = "http://api.openweathermap.org/data/2.5/weather?lat=-32.186150&lon=-64.263620&appid=0f7b5874e089ae41c3278833c0622bc6&units=metric";
String jsonBuffer;
int httpCode;
String response;
JSONVar myObject;

String weekDays[7] = {"SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY"};
String months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
String formattedTime;

Ticker ticker1(askWeather, 30000, 0, MILLIS);

int sunrise[2];
int sunset[2];

#define BUZZER_PIN D5

#define ledRojo D6
#define ledVerde D7
#define ledAzul D8
  
const int equisde = 3; 

void setup(){
	Serial.begin(9600);
	
	pinMode(BUZZER_PIN,OUTPUT);
	pinMode(ledRojo,OUTPUT);
	pinMode(ledVerde,OUTPUT);
	pinMode(ledAzul,OUTPUT);

	if (!display.begin(SSD1306_SWITCHCAPVCC, ScreenAddress)){
		Serial.println(F("SSD1306 allocation failed"));
		for (;;); // Don't proceed, loop forever
	}
	
	display.setTextColor(WHITE);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED){
		ShowWifi(40,8,0);
		delay(500);
	}

	timeClient.begin();
	timeClient.setTimeOffset(-10800); // (-3 _ 0 default) * 3600
	askWeather();
	setCurrentSun();
	ticker1.start();
	Serial.println("Conectado a WiFi");
	Serial.println(WiFi.localIP());
}

void loop(){
	for (int i = 0; i < equisde; i++){
		ShowWalking(40,16,0);
	}
	for (int i = 0; i < equisde; i++){
		ShowDeadLift(40,16,0);
	}
	for (int i = 0; i < equisde; i++){
		ShowDumbbell(40,16,0);
	}
	for (int i = 0; i < equisde; i++){
		ShowOffice(40,16,10);
	}
	for (int i = 0; i < equisde; i++){	
		showWeather();
	}
	for (int i = 0; i < equisde; i++){
		showClock();
	}

	//RussiaAnthem();

	ticker1.update();
}

const uint8_t *askIcon(String main, String desc){
	if (main == "Clear")
	{
		Serial.println(formattedTime.substring(0, 2).toInt());
		if (formattedTime.substring(0, 2).toInt() > sunrise[1] && formattedTime.substring(0, 2).toInt() < 20)
		{
			
			Serial.println("Es dia soleado");
			return clear_day_bits;
		}
		else
		{
			Serial.println("Es noche clara");
			return clear_night_bits;
		}
	}
	else if (main == "Clouds")
	{
		if (formattedTime.substring(0, 2).toInt() > 7 && formattedTime.substring(0, 2).toInt() < 20)
		{
			if (desc == "few clouds" || desc == "scattered clouds" || desc == "broken clouds")
			{
				Serial.println("Esta nublado de dia");
				return partly_cloudy_day_bits;
			}
			return partly_cloudy_day_bits;
		}
		if (desc == "few clouds" || desc == "scattered clouds" || desc == "broken clouds")
		{
			Serial.println("Esta nublado de noche");
			return partly_cloudy_night_bits;	
		}
		Serial.println("Esta simplemente nublado");
		return partly_cloudy_night_bits;
		//return cloudy_bits;
	}
	else if (main == "Thunderstorm" || main == "Rain" || main == "Drizzle")
	{
		Serial.println("Esta lloviendo");
		return rain_bits;
	}
	else if (main == "Fog" || main == "Haze")
	{
		Serial.println("Esta nebloso");
		return fog_bits;
	}
	else if (main == "Snow")
	{
		Serial.println("Esta nevando");
		return snow_bits;
	}
	return clear_day_bits;
}

void showClock(){

	timeClient.update();
	formattedTime = timeClient.getFormattedTime();

	unsigned long epochTime = timeClient.getEpochTime();

	struct tm *ptm = gmtime((time_t *)&epochTime);

	int currentDay = ptm->tm_mday;

	int currentMonth = ptm->tm_mon + 1;

	int currentYear = ptm->tm_year + 1900;

	String currentArgDate =  String(currentDay) + "/" +String(currentMonth) + "/" + String(currentYear);

	if(formattedTime.substring(0,2).toInt() > 6 && formattedTime.substring(0,2).toInt() < 7){
		setCurrentSun();
	}

	display.setCursor(0, 10);

	// Serial.print("Current date: ");
	// Serial.println(currentArgDate);
	// Serial.print("Current day: ");
	// Serial.println(weekDays[timeClient.getDay()]);

	display.clearDisplay();
	display.setTextSize(1);
	display.setCursor(40, 0);
	display.println(weekDays[timeClient.getDay()]);
	display.setTextSize(2);
	display.setCursor(16, 14);
	display.println(currentArgDate);
	display.setCursor(36, 35);
	display.println(formattedTime.substring(0, 5));
	display.display();
	delay(1000);
}

void showWeather(){
	//Printea datos de API

	String temp = String((int)myObject["main"]["temp"]);
	temp.remove(temp.indexOf("."));
	temp = temp + " "+  (char)247 +"C";
	String hum = String((int)myObject["main"]["humidity"]);
	hum = hum + "%";
	JSONVar valor = myObject["weather"][0];
	String estado =	valor.stringify(valor["main"]);
	String estado2 = valor.stringify(valor["description"]);
	// Serial.println(temp);
	// Serial.println(estado + "  " + estado2);

	// Display static text

	display.clearDisplay();

	display.setTextSize(2);

	display.setCursor(55, 10);
	estado.replace("\"","");
	display.println(estado);

	display.setCursor(55, 30);
	display.println(temp);
	display.println();

	display.setCursor(82, 50);
	display.println(hum);
	display.println();
	

	display.drawXBitmap(0, 5, askIcon(estado, estado2), 50, 50, WHITE);
	display.display();
	display.setTextSize(1);
	delay(2000);
}

void askWeather(){
	http.begin(cliente, linkApi);

	do{
		httpCode = http.GET();
		Serial.println(httpCode);
		if (httpCode == HTTP_CODE_OK)
		{
			Serial.print("HTTP response code ");
			Serial.println(httpCode);
			response = http.getString();
			Serial.println(response);
		}
	} while (httpCode != 200);

	http.end();

	myObject = JSON.parse(response);

	// JSON.typeof(jsonVar) can be used to get the type of the var
	if (JSON.typeof(myObject) == "undefined")
	{
		Serial.println("Parsing input failed!");
		return;
	}

	String temp = String((int)myObject["main"]["temp"]);
	temp.remove(temp.indexOf("."));
	if(temp.toInt() < 15){
		RGB_color(0,0,255);
	}else if(temp.toInt() > 28){
		RGB_color(255,0,0);
	}else{
		RGB_color(0,255,0);
	}

	// String hum = String((int)myObject["main"]["humidity"]);
	// if(hum.toInt() > 40){

	// }else{

	// }

}

void ShowWifi(int xx, int yy, int tt){

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi0, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi1, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi2, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi3, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi4, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi5, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi6, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi7, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi8, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi9, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi10, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi11, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi12, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi13, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi14, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi15, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi16, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi17, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi18, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi19, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi20, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi21, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi22, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi23, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi24, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi25, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi26, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, wifi27, 48, 48, 1);
	display.display();
	delay(tt);

	return;
}

void ShowWalking(int xx, int yy, int tt){
    display.clearDisplay();
	display.drawBitmap(xx, yy, walking0, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking1, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking2, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking3, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking4, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking5, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking6, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking7, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking8, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking9, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking10, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking11, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking12, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking13, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking14, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking15, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking16, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking17, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking18, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking19, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking20, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking21, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking22, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking23, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking24, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking25, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking26, 48, 48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy, walking27, 48, 48, 1);
	display.display();
	delay(tt);
	return;
}

void ShowDeadLift(int xx, int yy, int tt){
	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift0,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift1,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift2,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift3,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift4,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift5,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift6,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift7,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift8,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift9,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift10,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift11,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift12,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift13,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift14,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift15,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift16,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift17,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift18,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift19,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift20,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift21,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift22,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift23,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift24,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift25,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift26,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,deadlift27,48,48, 1);
	display.display();
	delay(tt);
}

void ShowDumbbell(int xx, int yy, int tt){

	display.setTextSize(2);
	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell0,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell1,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell2,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell3,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell4,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell5,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell6,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell7,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell8,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell9,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell10,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell11,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell12,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell13,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell14,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell15,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell16,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell17,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell18,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell19,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell20,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell21,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell22,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell23,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell24,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell25,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell26,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.setCursor(8,0);
	display.println("Calistenia");
	display.drawBitmap(xx, yy,dumbbell27,48,48, 1);
	display.display();
	delay(tt);
}

void ShowOffice(int xx, int yy, int tt){
	display.clearDisplay();
	display.drawBitmap(xx, yy,office0,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office1,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office2,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office3,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office4,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office5,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office6,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office7,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office8,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office9,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office10,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office11,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office12,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office13,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office14,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office15,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office16,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office17,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office18,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office19,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office20,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office21,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office22,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office23,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office24,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office25,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office26,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,office27,48,48, 1);
	display.display();
	delay(tt);

}


void RussiaAnthem(){
	// Repeating I
	for (int i = 0; i < NUMBER_OF_NOTES - 4; i++) {
		note(anthem[i][0], anthem[i][1]);
	}

	// Repeating II
	for (int i = 1; i < NUMBER_OF_NOTES; i++) {
		// skip notes from the end of the first repeat.
		if (i >= NUMBER_OF_NOTES - 9 && i < NUMBER_OF_NOTES - 4)
		continue;
		note(anthem[i][0], anthem[i][1]);
	}
}

void note(unsigned int frequency, unsigned long duration) {
  tone(BUZZER_PIN, frequency);
  delay(duration);
  noTone(BUZZER_PIN);
}

void RGB_color(int red_light_value, int green_light_value, int blue_light_value){
  analogWrite(ledRojo, red_light_value);
  analogWrite(ledVerde, green_light_value);
  analogWrite(ledAzul, blue_light_value);
}

void setCurrentSun(){
	//Sunrise
	unsigned long epochTimeSunrise = (int)myObject["sys"]["sunrise"];
	struct tm *ptm = gmtime((time_t *)&epochTimeSunrise);
	sunrise[0] = ptm->tm_hour-3;
	sunrise[1] = ptm->tm_min;

	//Sunset
	unsigned long epochTimeSunset = (int)myObject["sys"]["sunset"];
	struct tm *ptm2 = gmtime((time_t *)&epochTimeSunset);
	sunset[0] = ptm2->tm_hour-3;
	sunset[1] = ptm2->tm_min;
}