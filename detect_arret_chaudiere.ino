#include "Wire.h"
#include <EEPROM.h>
#include <Deuligne.h>

#include "temp_sensor.h"
#include "utils.h"
#include "lcd.h"

#define ADDRESS 1
int value = 0;

int consigne = 0;
int consigne_init = 0;

Deuligne lcd;
void setup()
{
	Serial.begin(19200); 
	lcd.init();
	lcd.setCursor(0, 0);
	lcd.print("init");  

	delay(3000);
	
//	EEPROM.write(ADDRESS, 15);

	consigne = EEPROM.read(ADDRESS);

/*
	char buffer[5];
	sprintf(buffer, "%d", value);
	print_custom (buffer);
*/

//	while (1);
  }


void save_consigne (){
	int mem = EEPROM.read(ADDRESS);
	if(mem!=consigne){
		EEPROM.write(ADDRESS, consigne);
	}
}

void print_custom (char*){
	lcd.setCursor(0, 0);
	lcd.print("custom");  
	lcd.setCursor(0, 1);
	lcd.print(value);  
}
void print_consigne (int consigne){
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("consigne");  
	lcd.setCursor(0, 1);
	lcd.print(consigne);  
}

void print_temp (float temp){
	lcd.setCursor(0, 0);
	lcd.print("température");   
	lcd.setCursor(0, 1);
	lcd.print(Arrondi(temp));  
//	Serial.print("temp: ");
//	Serial.println(temp);
}

void print_and_update_consigne(){
	
}

void loop() {
	unsigned long time;
	delay(500);
	int button = -1;
	
	float temp = temp_as_c(); // get temp value
	print_temp (temp);
	
	button=lcd.get_key();
	delay(100); // for debounce
	if(button > -1) {
		time = millis();
		print_consigne(consigne);
		delay(1000);
		while(millis() < time + 3000) {		
			button=lcd.get_key();
			delay(100); // for debounce
			if(button == LEFT) {--consigne;time = millis();print_consigne(consigne);delay(300);}
			if(button == RIGHT) {consigne++;time = millis();print_consigne(consigne);delay(300);}
			if(button == DOWN) {consigne=consigne-5;time = millis();print_consigne(consigne);delay(300);}
			if(button == UP) {consigne=consigne-5;time = millis();print_consigne(consigne);delay(300);}
		}
		save_consigne ();
		lcd.clear();
	}
	
}
