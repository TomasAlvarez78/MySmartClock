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
#include "animations.h"

#define ScreenWidth 128
#define ScreenHeight 64

#define OLED_RESET -1
#define ScreenAddress 0x3C

Adafruit_SSD1306 display(ScreenWidth, ScreenHeight, &Wire, OLED_RESET);

void ShowWalking();
void ShowWifi();
void ShowDeadLift();
void ShowDumbbell();

int xx = 40;
int yy = 8;
int tt = 0;

const char *ssid = "LaGuaiFai2.4G";
const char *password = "Lululinda2021";

void showClock();
void showWeather();
void askWeather();
const uint8_t *askIcon(String main, String desc);

HTTPClient http;
WiFiClient cliente;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

String zelda = "http://api.openweathermap.org/data/2.5/weather?lat=-32.186150&lon=-64.263620&appid=0f7b5874e089ae41c3278833c0622bc6&units=metric";
String jsonBuffer;
int httpCode;
String response;

String weekDays[7] = {"SUNDAY", "MONDAY", "TUESDAY", "WEDNESDAY", "THURSDAY", "FRIDAY", "SATURDAY"};
String months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
String formattedTime;

int repeticion = 0;
JSONVar myObject;

Ticker ticker1(askWeather, 30000, 0, MILLIS);

void setup()
{
	// put your setup code here, to run once:
	Serial.begin(9600);
	pinMode(D3,OUTPUT);
	if (!display.begin(SSD1306_SWITCHCAPVCC, ScreenAddress)){
		Serial.println(F("SSD1306 allocation failed"));
		for (;;)
			; // Don't proceed, loop forever
	}
	
	display.setTextColor(WHITE);
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED)
	{
		ShowWifi();
		delay(500);
	}

	askWeather();
	timeClient.begin();
	timeClient.setTimeOffset(-10800); // -3 _ 0 default
	ticker1.start();
	Serial.println(F("Conectado a WiFi"));
	Serial.println(WiFi.localIP());
}

void loop()
{
	for (int i = 0; i < 3; i++){
		ShowWalking();
	}
	for (int i = 0; i < 3; i++){
		ShowDeadLift();
	}
	for (int i = 0; i < 3; i++){
		ShowDumbbell();
	}
	for (int i = 0; i < 3; i++){
		showWeather();
	}
	for (int i = 0; i < 3; i++){
		showClock();
	}
	
	//digitalWrite(D3,HIGH);
	//delay(1000);
	//digitalWrite(D3,LOW);

	ticker1.update();
}

const uint8_t *askIcon(String main, String desc){
	if (main == "Clear")
	{
		Serial.println(formattedTime.substring(0, 2).toInt());
		if (formattedTime.substring(0, 2).toInt() > 7 && formattedTime.substring(0, 2).toInt() < 20)
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

	display.setCursor(0, 10);

	int monthDay = ptm->tm_mday;

	int currentMonth = ptm->tm_mon + 1;

	int currentYear = ptm->tm_year + 1900;

	//Print complete date:
	//String currentDate = String(currentYear) + "-" + String(currentMonth) + "-" + String(monthDay);
	String currentArgDate = String(currentMonth) + "/" + String(monthDay) + "/" + String(currentYear);
	Serial.print("Current date: ");
	Serial.println(currentArgDate);
	Serial.print("Current day: ");
	Serial.println(weekDays[timeClient.getDay()]);

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

	//after parsing data
	//String temp = String((int)myObject["main"]["feels_like"]);
	String temp = String((int)myObject["main"]["temp"]);
	temp.remove(temp.indexOf("."));
	temp = temp + " "+  (char)247 +"C";
	JSONVar valor = myObject["weather"][0];
	String estado =	valor.stringify(valor["main"]);
	String estado2 = valor.stringify(valor["description"]);
	Serial.println(temp);
	Serial.println(estado + "  " + estado2);
	//askIcon((const char*)valor["main"]);

	// Display static text

	display.clearDisplay();

	display.setTextSize(2);

	display.setCursor(55, 10);
	estado.replace("\"","");
	display.println(estado);

	display.setCursor(55, 30);
	display.println(temp);
	display.println();
	

	display.drawXBitmap(0, 5, askIcon(estado, estado2), 50, 50, WHITE);
	display.display();
	display.setTextSize(1);
	delay(2000);
}

void ShowWifi(){

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

void askWeather(){
	http.begin(cliente, zelda);

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
	Serial.println("Pregunte el clima :)");
}

void ShowWalking(){
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

void ShowDeadLift(){
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

void ShowDumbbell(){

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell0,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell1,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell2,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell3,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell4,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell5,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell6,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell7,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell8,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell9,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell10,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell11,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell12,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell13,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell14,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell15,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell16,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell17,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell18,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell19,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell20,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell21,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell22,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell23,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell24,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell25,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell26,48,48, 1);
	display.display();
	delay(tt);

	display.clearDisplay();
	display.drawBitmap(xx, yy,dumbbell27,48,48, 1);
	display.display();
	delay(tt);

}