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
#include "include/CommandHandler.h"
#include "include/SDCard.h"

// Ports
#define I2C1SDA 2
#define I2C1SCL 3
#define LED_RED 14
#define LED_GREEN 15
#define LED_ONBOARD 25
#define LED_CUTDOWN 6
#define THERMISTOR_PORT

// Frequencies and Speeds
#define LORA_FREQ 433E6
#define I2C_FREQ 400 * 1000

// Timing
#define MSG_SEND_INTERVAL_SEC 5
#define DEBUG_PRINT_INTERVAL 5
#define POST_INIT_DELAY_MS 5000
#define WATCHDOG_TIMEOUT_MS 10000
#define CUTDOWN_TIME_HOURS 5

// Other
#define DEBUG 1
#define BEACON 1
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
std::shared_ptr<SD_Card_t> SD(new SD_Card_t());

datetime_t curTime = {
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
	bool watchdogReset = watchdog_caused_reboot();
	if (watchdogReset)
	{
		printf("**********\n");
		printf("Watchdog reboot happened\n");
		printf("**********\n");
	}

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
			printf("&&&&&&&&&&\n");
			printf("Received via poll: ");

			// Read until done
			gpio_put(LED_ONBOARD, 1);
			while (SX1278->radio.available())
			{
				char c = SX1278->radio.read();
				response.push_back(c);
			}

			// Print out recieved message
			printf("%s", response.c_str());
			printf(" RSSI: %d\n", SX1278->radio.packetRssi());
			printf("&&&&&&&&&&\n");

			// Add command to queue
			uint32_t cmd;
			if (commandHandler->validateCommand(response, &cmd))
			{
				commandHandler->addCommand(cmd);
			}
			else
			{
				printf("Inavlid command received\n");
			}
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

	int lastTransmitSec = curTime.sec;
	int lastPrintSec = curTime.sec;
	rtc_set_datetime(&curTime);

	// Start all sensors
	SX1278->begin(LORA_FREQ);
	GPS->begin(i2cOne);
	PressureSensor->begin(i2cOne);
	SD->begin();

	printf("Starting Health Check...\n");

	// Health check
	if (SX1278->connected && GPS->connected)
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

	// PressureSensor->printPROM();

	int counter = 0;
	int testTimer = 0;

	// Launch Second Core
	printf("Launching Core 2...\n");
	multicore_launch_core1(core1_entry);
	printf("Entering main loop\n");

	for (EVER)
	{

		// Check time
		rtc_get_datetime(&curTime);
		int dispatchDifference = abs(curTime.sec - lastTransmitSec);
		int printDifference = abs(curTime.sec - lastPrintSec);

		// Sensor Reads

		PressureSensor->setPressure(PressureSensor->readPressure());
		PressureSensor->setTemp(PressureSensor->readTemp());
		PressureSensor->setAlt(PressureSensor->readAlt());
		Thermistor->readTemp();

		uint16_t availableBytes = GPS->getAvailableReadBytes();
		uint8_t GPSbuf[availableBytes];
		availableBytes = GPS->readGPS(GPSbuf, availableBytes);
		GPS->setData(GPSbuf, availableBytes);

		commandHandler->updateDispatchData(PressureSensor->temp, Thermistor->tempK, PressureSensor->pressure, GPS->lat, GPS->lon, GPS->time, GPS->alt);

		// Check if enough time has elapsed for regular message pings
		if (dispatchDifference >= MSG_SEND_INTERVAL_SEC && BEACON)
		{
			commandHandler->addCommand(REGULAR_TRANSMISSION);
			lastTransmitSec = curTime.sec;
		}

		// Activate cutdown sensor when duration achieved
		if (curTime.hour >= CUTDOWN_TIME_HOURS)
		{
			commandHandler->addCommand(CUTDOWN);
		}

		// Check if command to dispatch
		if (!commandHandler->commandAvailable())
		{
			uint32_t cmd = commandHandler->getCommand();
			commandHandler->executeCommand(cmd);
		}
		watchdog_update();
		sleep_ms(1);
		counter++;

		// Printing
		if (DEBUG && printDifference >= DEBUG_PRINT_INTERVAL)
		{

			// Print out Data
			printf("\n");
			testTimer++;
			printf("******************************\n");
			GPS->printLat();
			GPS->printLon();
			GPS->printAlt();
			GPS->printTime();
			PressureSensor->printTemp();
			PressureSensor->printPressure();
			PressureSensor->printAlt();
			Thermistor->printTemp();
			printf("Messages sent: %d\n", SX1278->msgCount);
			printf("Hour: %d, Min: %d, Sec: %d\n", curTime.hour, curTime.min, curTime.sec);
			printf("Loop done %d\n", counter);
			printf("Watchdog reset\n");
			printf("Manual commands sent %d, Acks Received %d\n", commandHandler->cmdSent, commandHandler->ackRec);

			printf("******************************\n");
			lastPrintSec = curTime.sec;

			SD->write(commandHandler->createSDCardPayload().data());
			printf("\n");
		}
	}
}