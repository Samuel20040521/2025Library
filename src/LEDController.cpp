// compile: g++ -o LEDController.o -I./include -c LEDController.cpp
#include "../inc/LEDController.h"
#include <fstream>
#include <iostream>
#include <unistd.h> // 用於 usleep

const std::string GPIO_BASE_PATH = "/sys/class/gpio/";

// LEDColor
LEDColor::LEDColor() : r(0), g(0), b(0), rgb(0) {}
void LEDColor::setColor(const int &colorCode) {
	const int R = (colorCode >> 24) & 0xff;
	const int G = (colorCode >> 16) & 0xff;
	const int B = (colorCode >> 8) & 0xff;
	const int A = (colorCode >> 0) & 0xff;

	float r_cal = 0.0, g_cal = 0.0, b_cal = 0.0;
	if ((R + G + B) > 0) {
		float a = A / 255.0;
		r_cal = (1.0) * (R / 255.0) * a;
		g_cal = (1.0) * (G / 255.0) * a;
		b_cal = (1.0) * (B / 255.0) * a;

		r_cal = pow(r_cal, Config::GAMMA_LED_R);
		g_cal = pow(g_cal, Config::GAMMA_LED_G);
		b_cal = pow(b_cal, Config::GAMMA_LED_B);

		r_cal *= Config::LED_MAX_BRIGHTNESS;
		g_cal *= Config::LED_MAX_BRIGHTNESS;
		b_cal *= Config::LED_MAX_BRIGHTNESS;

		r = int(r_cal);
		g = int(g_cal);
		b = int(b_cal);

		rgb = (r << 16) + (g << 8) + b;
	} else {
		r = g = b = 0;
		rgb = 0;
  	}
}
uint32_t LEDColor::getRGB() { return rgb; }

// LEDController
LEDController::LEDController() { num_channel = 0; }
int LEDController::init(const std::vector<int> &numLedsEachStrip) {
	close_gpio();
	// initialize WS2812B
	ws2811_return_t return_WS2811;
	num_channel = numLedsEachStrip.size(); // num_channel: number of led strips
	// initialize variables for jgarff's ws281x library
	for (int i = 0; i < LATCH_LED_STRIP_NUM; i++) { 
		ledString[i].freq = TARGET_FREQ;
		ledString[i].dmanum = DMA;
		ledString[i].channel[0].gpionum = GPIO_PIN;
		ledString[i].channel[0].invert = 0;
		ledString[i].channel[0].count = Config::WS2812_NUM_LED_EACH_STRIP[i]; 
		// Didn't see the necessity initializing led count here
		ledString[i].channel[0].strip_type = STRIP_TYPE;
		ledString[i].channel[0].brightness = 255;
	}
	for (int i = 0; i < num_channel; i++) {
		ledString[i].channel[0].count = numLedsEachStrip[i]; // setup led number for each strip
		if ((return_WS2811 = ws2811_init(&ledString[i])) != WS2811_SUCCESS) {
			fprintf(stderr, "ws2811_init %d failed: %s\n", i, 
					ws2811_get_return_t_str(return_WS2811));
			return return_WS2811;
		}
	}

	// initialize GPIO_PIN
	gpioInit();
	for (int i = 0; i < num_channel; i++) {
		// let all LEDs off
		select_channel(i);
		for (int j = 0; j < ledString[i].channel[0].count; j++) {
			ledString[i].channel[0].leds[j] = 0;
		}
		// render
		if ((return_WS2811 = ws2811_render(&ledString[i])) != WS2811_SUCCESS) {
		fprintf(stderr, "ws2811_render %d failed: %s\n", i,
				ws2811_get_return_t_str(return_WS2811));
		return return_WS2811;
		}
		usleep(ledString[i].channel[0].count * 30);
	}
	return WS2811_SUCCESS;
}
int LEDController::sendAll(const std::vector<std::vector<int>> &statusLists) {
  // Check if data size is consistent with stored during initialization
  	for (int i = 0; i < num_channel; i++) {
    	if (int(statusLists[i].size()) > int(ledString[i].channel[0].count)) {
		//            fprintf(stderr, "Error: Strip %d is longer then init
		//            settings: %d\n", (int)statusLists[i].size(),
		//            ledString[i].channel[0].count);
		return -1;
    }
  }
  // Push data to LED strips
  play(statusLists);
  return 0;
}
int LEDController::play(const std::vector<std::vector<int>> &statusLists) {
  	ws2811_return_t return_WS2811;
  	LEDColor led;
  	for (int i = 0; i < num_channel; i++) {
    	select_channel(i);
		for (int j = 0; j < ledString[i].channel[0].count; j++) {
			led.setColor(statusLists[i][j]);
			ledString[i].channel[0].leds[j] = led.getRGB();
		}
		if ((return_WS2811 = ws2811_render(&ledString[i])) != WS2811_SUCCESS) {
			// fprintf(stderr, "ws2811_render %d failed: %s\n", i, ws2811_get_return_t_str(return_WS2811));
			return return_WS2811;
		}
		usleep(ledString[i].channel[0].count * 30);
  	}
  return 0;
}
void LEDController::gpioInit() {
  int fd_export = open("/sys/class/gpio/export", O_WRONLY);

  if (fd_export == -1) {
    perror("Unable to open /sys/class/gpio/export");
    exit(1);
  }

  if (write(fd_export, "23", 2) != 2) {
    perror("Error writing to /sys/class/gpio/export");
    exit(1);
  }

  if (write(fd_export, "24", 2) != 2) {
    perror("Error writing to /sys/class/gpio/export");
    exit(1);
  }

  if (write(fd_export, "25", 2) != 2) {
    perror("Error writing to /sys/class/gpio/export");
    exit(1);
  }

  close(fd_export);

  // Set the pin to be an output by writing "out" to
  // /sys/class/gpio/gpio24/direction

  int fd = open("/sys/class/gpio/gpio23/direction", O_WRONLY);
  if (fd == -1) {
    perror("Unable to open /sys/class/gpio/gpio23/direction");
    exit(1);
  }

  if (write(fd, "out", 3) != 3) {
    perror("Error writing to /sys/class/gpio/gpio23/direction");
    exit(1);
  }

  close(fd);

  // Set the pin to be an output by writing "out" to
  // /sys/class/gpio/gpio24/direction

  fd = open("/sys/class/gpio/gpio24/direction", O_WRONLY);
  if (fd == -1) {
    perror("Unable to open /sys/class/gpio/gpio24/direction");
    exit(1);
  }

  if (write(fd, "out", 3) != 3) {
    perror("Error writing to /sys/class/gpio/gpio24/direction");
    exit(1);
  }

  close(fd);

  // Set the pin to be an output by writing "out" to
  // /sys/class/gpio/gpio24/direction

  fd = open("/sys/class/gpio/gpio25/direction", O_WRONLY);
  if (fd == -1) {
    perror("Unable to open /sys/class/gpio/gpio25/direction");
    exit(1);
  }

  if (write(fd, "out", 3) != 3) {
    perror("Error writing to /sys/class/gpio/gpio25/direction");
    exit(1);
  }

  close(fd);

  A0 = open("/sys/class/gpio/gpio23/value", O_WRONLY);
  if (fd == -1) {
    perror("Unable to open /sys/class/gpio/gpio23/value");
    exit(1);
  }

  A1 = open("/sys/class/gpio/gpio24/value", O_WRONLY);
  if (fd == -1) {
    perror("Unable to open /sys/class/gpio/gpio24/value");
    exit(1);
  }

  A2 = open("/sys/class/gpio/gpio25/value", O_WRONLY);
  if (fd == -1) {
    perror("Unable to open /sys/class/gpio/gpio25/value");
    exit(1);
  }
}

void LEDController::select_channel(int address) {
	if (address > 7 || address < 0) {
		throw std::invalid_argument("Address out of range: must be in between 0 to 7");
	}
	setValue(A0, address & 0x01);
	setValue(A1, (address >> 1) & 0x01);
	setValue(A2, (address >> 2) & 0x01);
}

void LEDController::setValue(int pin, int value) {
  	std::ofstream valueFile(GPIO_BASE_PATH + "gpio" + std::to_string(pin) + "/value");
  	if (!valueFile.is_open()) {
    	throw std::runtime_error("Can't set GPIO pin" + std::to_string(pin) + "'s value");
  	}
	valueFile << value;
	valueFile.close();
}

void LEDController::finish() {
  for (int i = 0; i < num_channel; i++)
    ws2811_fini(&ledString[i]);
  	printf("LED GPIO finished.\n");
  	close(A0);
  	close(A1);
  	close(A2);
}
void LEDController::close_gpio() {
	int fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd == -1) {
		perror("Unable to open /sys/class/gpio/unexport");
		exit(1);
  	}

  	if (write(fd, "23", 2) != 2) {
    	fprintf(stderr, "Error writing to /sys/class/gpio/unexport: 23\n");
  	}

  if (write(fd, "24", 2) != 2) {
    fprintf(stderr, "Error writing to /sys/class/gpio/unexport: 24\n");
  }

  if (write(fd, "25", 2) != 2) {
    fprintf(stderr, "Error writing to /sys/class/gpio/unexport: 25\n");
  }
}