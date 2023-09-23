#include <stdio.h>
#include <string>
#include <cstring>
#include <queue>
#include <memory>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/sync.h"

#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "hardware/irq.h"
#include "hardware/rtc.h"
#include "hardware/watchdog.h"
#include "include/GPS.h"
#include "include/Pressure.h"
#include "include/Thermistor.h"
#include "include/LoRa.h"
// #include "include/LimitedQueue.h"
#include "include/CommandHandler.h"

// Ports
#define I2C1SDA 2
#define I2C1SCL 3
#define LED_RED 14
#define LED_GREEN 15
#define LED_ONBOARD 25
#define LED_CUTDOWN 13
#define THERMISTOR_PORT

// Frequencies and Speeds
#define LORA_FREQ 433E6
#define I2C_FREQ 400 * 1000

// Timing
#define MSG_SEND_INTERVAL_SEC 5
#define POST_INIT_DELAY_MS 5000
#define WATCHDOG_TIMEOUT_MS 3000
#define CUTDOWN_TIME_HOURS 5

// Other
#define DEBUG 0
#define EVER \
	;        \
	;

// LED helpers
void setLED()
{
	gpio_init(LED_CUTDOWN);
	gpio_init(LED_ONBOARD);
	gpio_init(LED_RED);
	gpio_init(LED_GREEN);
	gpio_set_dir(LED_RED, GPIO_OUT);
	gpio_set_dir(LED_ONBOARD, GPIO_OUT);
	gpio_set_dir(LED_CUTDOWN, GPIO_OUT);
	gpio_set_dir(LED_GREEN, GPIO_OUT);
	gpio_put(LED_CUTDOWN, 0);
}
void blinkRed()
{
	gpio_put(LED_RED, 1);
	gpio_put(LED_GREEN, 0);
}

void blinkGreen()
{
	gpio_put(LED_RED, 0);
	gpio_put(LED_GREEN, 1);
}

// Shared Object Declararion
std::shared_ptr<LoRa_t> SX1278(new LoRa_t);
std::shared_ptr<PressureSensor_t> PressureSensor(new PressureSensor_t());
std::shared_ptr<GPS_t> GPS(new GPS_t);
std::shared_ptr<Thermistor_t> Thermistor(new Thermistor_t(THERMISTOR_PORT));
std::shared_ptr<CommandHandler_t> commandHandler(new CommandHandler_t());

datetime_t time = {
	.year = 2023,
	.month = 1,
	.day = 1,
	.dotw = 1,
	.hour = 0,
	.min = 0,
	.sec = 0

};

void activateCutdown()
{
	printf("Activate cutdown");
	gpio_put(LED_CUTDOWN, 1);
}

/**************************************************************/

// Core 2 Main loop - Deals with radio
void core1_entry()
{
	printf("Hello from Core 2!\n");
	while (1)
	{
		// Check if time to send
		if (!commandHandler->dispatchAvailable())
		{
			std::string msg = commandHandler->getDispatch();
			SX1278->send(msg);
		}

		// Parse

		// Check if packet is available
		if (SX1278->radio.parsePacket() != 0)
		{
			std::string response = "";
			printf("Received via poll: ");

			// Read until done
			gpio_put(LED_ONBOARD, 1);
			while (SX1278->radio.available())
			{
				response.push_back((char)SX1278->radio.read());
			}

			// Print out recieved message
			printf("%s", response.c_str());
			printf("RSSI: %d\n", SX1278->radio.packetRssi());

			// Add command to queue
			cmd_t sid = static_cast<cmd_t>(response[0] - 0x30);
			commandHandler->addDispatch(commandHandler->createDispatch(ACKSEND, sid));
			commandHandler->addCommand(sid);
			gpio_put(LED_ONBOARD, 0);
		}
	}
}

// Core 1 Main loop
int main()
{
	stdio_init_all();

	// I/O init
	i2c_inst_t *i2cOne = i2c1;
	adc_init();
	rtc_init();
	i2c_init(i2cOne, I2C_FREQ);
	gpio_set_function(I2C1SDA, GPIO_FUNC_I2C);
	gpio_set_function(I2C1SCL, GPIO_FUNC_I2C);
	setLED();
	blinkRed();
	sleep_ms(POST_INIT_DELAY_MS);

	int lastTransmitSec = time.sec;

	rtc_set_datetime(&time);

	// Start all sensors
	SX1278->begin(LORA_FREQ);
	GPS->begin(i2cOne);
	PressureSensor->begin(i2cOne);

	printf("Starting Health Check...\n");

	// Health check
	if (SX1278->connected && GPS->connected && PressureSensor->connected)
	{

		printf("Health check passed\n");
		printf("Watchdog enabled\n");
		watchdog_enable(WATCHDOG_TIMEOUT_MS, 1);
	}

	else
	{
		printf("Health check failed\n");
		for (EVER)
			;
	}

	blinkGreen();

	PressureSensor->printPROM();

	int counter = 0;

	// Launch Second Core
	printf("Launching Core 2...\n");
	multicore_launch_core1(core1_entry);
	printf("Entering main loop\n");

	for (EVER)
	{
		// Check time
		rtc_get_datetime(&time);
		int difference = abs(time.sec - lastTransmitSec);

		// Sensor Reads
		PressureSensor->readTempPressure();
		Thermistor->readTemp();
		GPS->readI2CGPS();

		// Check if enough time has elapsed for regular message pings
		if (difference >= MSG_SEND_INTERVAL_SEC)
		{
			commandHandler->addCommand(REGULAR);
			lastTransmitSec = time.sec;
		}

		// Activate cutdown sensor when duration achieved
		if (time.hour >= CUTDOWN_TIME_HOURS)
		{
			commandHandler->addCommand(CUTDOWN);
		}

		// Check if command to dispatch
		if (!commandHandler->commandAvailable())
		{
			cmd_t c = commandHandler->getCommand();
			commandHandler->addDispatch(commandHandler->createDispatch(c, 0));
		}
		watchdog_update();
		sleep_ms(1);
		counter++;

		// Printing
		if (DEBUG)
		{

			// Print out Data
			printf("\n");
			printf("******************************\n");
			GPS->printBuff();
			PressureSensor->printTemp();
			PressureSensor->printPressure();
			Thermistor->printTemp();
			printf("Messages sent: %d\n", SX1278->msgCount);
			printf("Hour: %d, Min: %d, Sec: %d\n", time.hour, time.min, time.sec);
			printf("Loop done %d\n", counter);
			printf("Watchdog reset\n");

			printf("******************************\n");
			printf("\n");
		}
	}
}