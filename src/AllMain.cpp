// compile: g++ AllMain.cpp LEDController.o OFController.o -lws2811 -o test
#include <iostream>
#include <cstdint>
#include <vector>
using namespace std;

#include "../inc/LEDController.h"
#include "../inc/OFController.h"

int main() {	
    //Optical Fiber (OF) setup
    OFController OF;
    vector<int> status;
    OF.init();
    status.resize(5*Config::NUMPCA);

    // LED strip setup
    LEDController strip;
    vector<vector<int>> LEDstatus;
    vector<int> numLedsEachStrip;
    int num_strip = 8;
    numLedsEachStrip.clear();
    numLedsEachStrip.resize(num_strip);
    for (int i = 0; i < num_strip; i++) numLedsEachStrip[i] = 3;
    strip.init(numLedsEachStrip); 
    LEDstatus.resize(num_strip);
    for (int i = 0; i < num_strip; i++) LEDstatus[i].resize(numLedsEachStrip[i]);
	
    // breathe
    while (true) {
        // brightness increment from 0 to 255
        for (int a = 0; a <= 255; a++) {
            for (int i = 0; i < 5 * Config::NUMPCA; i++) {
                if (i%2 == 0) status[i] = 0xFFFFFF00 + a; 
                else status[i] = 0xFFFFFF00 ;
            }
            OF.sendAll(status);
            
            for (int i = 0; i < num_strip; i++) {
                for (int j = 0; j < numLedsEachStrip[i]; j++) {
                    LEDstatus[i][j] = 0xFFFFFF00 + a;
                }
            }
            strip.sendAll(LEDstatus);
            sleep(1/256); //sleep(<total time (sec)> / <range of a>)
        }
        sleep(1); //delay time when light up
        //brightness decrement from 255 to 0
        for (int a = 255; a >= 0; a--) {
            for (int i = 0; i < 5 * Config::NUMPCA; i++) {
                if (i%2 == 0) status[i] = 0xFFFFFF00 + a; 
                else status[i] = 0xFFFFFF00 ;
            }
            OF.sendAll(status);
            
            for (int i = 0; i < num_strip; i++) {
                for (int j = 0; j < numLedsEachStrip[i]; j++)
                    LEDstatus[i][j] = 0xFFFFFF00 + a;
            }
            strip.sendAll(LEDstatus);
            sleep(1/256); //sleep(<total time (sec)> / <range of a>)
        }
        sleep(1); //delay time when light off
    }
    strip.finish();
    return 0;
}
