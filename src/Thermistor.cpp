#include "../include/Thermistor.h"

Thermistor_t::Thermistor_t()
{
    // Default start thermistor pin 26
    begin(26);
}

Thermistor_t::Thermistor_t(uint8_t pin_)
{
    begin(pin_);
}

void Thermistor_t::begin(uint8_t pin_)
{
    // Initialize ADC pins
    adc_gpio_init(pin_);
    adc_select_input(0);
}

void Thermistor_t::readTemp()
{
    uint16_t result = adc_read();

    // Math
    float vout = result * conversion_factor;
    float Rt = (vout * Ro) / (3.3f - vout);
    tempK = 1 / (A + (B * log(Rt)) + C * pow(log(Rt), 3));
    tempC = tempK - 273.15;
    tempF = (tempC * (1.8f)) + 32.0f;
}

void Thermistor_t::printTemp()
{

    // Print the temperature in F and C
    printf("Thermistor temperature %f F, Celsius: %f C\n", tempF, tempC);
}