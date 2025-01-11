#include <iostream>
#include <cstdint>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include "../inc/LEDController.h"
#include "../inc/OFController.h"

using namespace std;
using namespace std::chrono;

class LightController {
private:
    const vector<unsigned int> RAINBOW_COLORS = {
        0xFF000000, 0xFF7F0000, 0xFFFF0000, 
        0x00FF0000, 0x0000FF00, 0x4B008200, 
        0xEE82EE00
    };
    
    OFController of;
    LEDController strip;
    vector<int> ofStatus;
    vector<vector<int>> ledStatus;
    vector<int> numLedsEachStrip;
    atomic<int> currentMode{0};
    atomic<bool> running{true};
    mutex modeMutex;
    
    const int NUM_STRIPS = 8;
    const int LEDS_PER_STRIP = 3;

public:
    LightController() {
        // Initialize OF Controller
        of.init();
        ofStatus.resize(5 * Config::NUMPCA);

        // Initialize LED strips
        numLedsEachStrip.resize(NUM_STRIPS, LEDS_PER_STRIP);
        strip.init(numLedsEachStrip);
        
        ledStatus.resize(NUM_STRIPS);
        for (int i = 0; i < NUM_STRIPS; i++) {
            ledStatus[i].resize(numLedsEachStrip[i]);
        }
    }

    void setMode(int mode) {
        lock_guard<mutex> lock(modeMutex);
        currentMode = mode;
    }

    void modeOff() {
        fill(ofStatus.begin(), ofStatus.end(), 0x00000000);
        of.sendAll(ofStatus);

        for (auto& strip : ledStatus) {
            fill(strip.begin(), strip.end(), 0x00000000);
        }
        strip.sendAll(ledStatus);
    }

    void modeWhite() {
        while (running && currentMode == 2) {
            fill(ofStatus.begin(), ofStatus.end(), 0xFFFFFFA0);
            of.sendAll(ofStatus);

            for (auto& strip : ledStatus) {
                fill(strip.begin(), strip.end(), 0xFFFFFF00);
            }
            strip.sendAll(ledStatus);
            
            this_thread::sleep_for(milliseconds(100));
        }
    }

    void modeRainbow() {
        int colorIndex = 0;
        
        while (running && currentMode == 1) {
            // Brightness fade-in
            for (int brightness = 0; brightness <= 255 && running && currentMode == 1; brightness++) {
                const int currentColor = RAINBOW_COLORS[colorIndex] + brightness;
                
                fill(ofStatus.begin(), ofStatus.end(), currentColor);
                of.sendAll(ofStatus);

                for (auto& strip : ledStatus) {
                    fill(strip.begin(), strip.end(), currentColor);
                }
                strip.sendAll(ledStatus);
                
                this_thread::sleep_for(microseconds(3906)); // 1 second / 256 steps
            }

            colorIndex = (colorIndex + 1) % RAINBOW_COLORS.size();
            this_thread::sleep_for(seconds(1));
        }
    }

    void run() {
        thread modeThread;
        int lastMode = -1;

        while (running) {
            if (currentMode != lastMode) {
                if (modeThread.joinable()) {
                    running = false;
                    modeThread.join();
                    running = true;
                }

                lastMode = currentMode;
                switch (currentMode) {
                    case 0:
                        modeOff();
                        break;
                    case 1:
                        modeThread = thread(&LightController::modeRainbow, this);
                        break;
                    case 2:
                        modeThread = thread(&LightController::modeWhite, this);
                        break;
                }
            }
            this_thread::sleep_for(milliseconds(100));
        }

        if (modeThread.joinable()) {
            modeThread.join();
        }
    }

    void stop() {
        running = false;
        strip.finish();
    }
};

int main() {
    LightController controller;
    thread controlThread(&LightController::run, &controller);

    while (true) {
        cout << "MODE? (0:Off; 1:Rainbow; 2:Half bright white)" << endl;
        int mode;
        cin >> mode;
        
        if (mode >= 0 && mode <= 2) {
            controller.setMode(mode);
        } else {
            cout << "Invalid mode selected" << endl;
        }
    }

    controller.stop();
    controlThread.join();
    return 0;
}