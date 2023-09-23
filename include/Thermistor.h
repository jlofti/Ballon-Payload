#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include <math.h>

static const float conversion_factor = 3.3f / (1 << 12);
static const double A = 0.001129148;
static const double B = 0.000234125;
static const double C = 0.0000000876741;
static const double Ro = 10000;

class Thermistor_t
{
public:
    float tempC;
    float tempK;
    float tempF;

    Thermistor_t();
    Thermistor_t(uint8_t pin_);

    /*
     * Sets ADC pins to begin reading Thermistor
     *
     * @param pin_ pin that will ADC read thermistor
     */
    void begin(uint8_t pin_);

    /*
     * ADC reads the pin that the thermistor is wired to and converts to readable temperature
     */
    void readTemp();

    /*
     * Prints temperature
     */
    void printTemp();
};