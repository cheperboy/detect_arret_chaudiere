#include "Arduino.h"

#define THERMOMETER 1

const float B = 2700;
const float R0 = 50000;
const float T0 = 298.15;
const float expected_resistance = R0 * exp(B * (1/273.15 - 1/T0));

/* If you change the pulldown resistors change this. */
const float pulldown_resistance = 50000;
const bool using_pullups = false;
const int temp_samples = 16;
const int AD_MAX = 1023;


/* Given a sensor value and the resistance of the pulldown resistor
   compute the sensed resistance */
float compute_resistance(int value, float pulldown_resistance) {
  if (using_pullups)
    value = 1024 - value;  
  return -(pulldown_resistance * (float(value) - 1024.0) / float(value));    
}


/* Given a resistance and R0, T0 and B value describing the NTC thermistor 
 * return the temperature in kelvin.
 */ 
float compute_temperature(float resistance, float R0, float T0, float B) {
   return ((1/(1.0 / T0 + (1.0 / B) * log(resistance / R0))));
}

float compute_temperature(float resistance) {
    return compute_temperature(resistance, R0, T0, B);
}

int measure_adc_value() {
  int i;
  int sum = 0;
  for(i=0;i<temp_samples;i++)
    sum += analogRead(THERMOMETER);
  sum /= temp_samples;
  return sum;
}

float temp_as_c() {
  float adc_value = measure_adc_value();
  float resistance_value = compute_resistance(adc_value, pulldown_resistance);
  float temp_k = compute_temperature(resistance_value);
  float temp = temp_k - 273.15;
  return temp;
}
