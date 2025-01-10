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


    cout << "MODE?? (0:off; 1:rainbow; 2:white, half bright)"<<endl;
    int mode;
    cin>>mode;
    // breathe
    if(mode==2) {
        while(true) {
          for(int i = 0; i < 5 * Config::NUMPCA; i++){
            status[i] = 0xFFFFFFA0;
          }
          OF.sendAll(status);
          for(int i = 0; i < num_strip; i++) {
            for (int j = 0; j < numLedsEachStrip[i]; j++) {
              LEDstatus[i][j] = 0xFFFFFF00;
            }
          }
          strip.sendAll(LEDstatus);
        }
    }
    else if(mode==0){
        for(int i = 0; i < 5 * Config::NUMPCA; i++){
            status[i] = 0x00000000;
        }
        OF.sendAll(status);
        for(int i = 0; i < num_strip; i++){
            for(int j = 0; j < numLedsEachStrip[i]; j++){
                LEDstatus[i][j] = 0x00000000;
            }
        }
        strip.sendAll(LEDstatus);
    }
    else{
        unsigned int rainbowColor[7] = {0xFF000000, 0xFF7F0000, 0xFFFF0000, 0x00FF0000, 0x0000FF00, 0x4B008200, 0xEE82EE00};
        int color_no = 0;
        while (true) {
            // brightness increment from 0 to 255
            for (int a = 0; a <= 255; a++) {
                for (int i = 0; i < 5 * Config::NUMPCA; i++) {
                    status[i] = rainbowColor[color_no] + a; 
                }
                OF.sendAll(status);

                for (int i = 0; i < num_strip; i++) {
                    for (int j = 0; j < numLedsEachStrip[i]; j++) {
                        LEDstatus[i][j] = rainbowColor[color_no] + a;
                    }
                    //Pan try to change Q4
                }
                strip.sendAll(LEDstatus);
                sleep(1/256); //sleep(<total time (sec)> / <range of a>)
            }
            color_no++;
            if(color_no == 7) color_no = 0;
            sleep(1); //delay time when light up
        }
    }
    strip.finish();
    return 0;
}
