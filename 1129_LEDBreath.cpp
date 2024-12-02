#include <stdio.h>

#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>

// library for PCA
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace std;

int main() {
    // 初始化變數
    const char *filename = "/dev/i2c-1";  // I2C 總線
    const int PCA_ADDR = 0x23;            // PCA9955B 的 I2C 位址
    int fd[1];
    unsigned char buffer[8];

    // 打開 I2C 總線
    if ((fd[0] = open(filename, O_RDWR)) < 0) {
        fprintf(stderr, "Failed to open the i2c bus.\n");
        return -1;
    }

    // 設置 I2C slave 位址
    // reference: Table 5. I2C-bus slave address (p.9)
    if (ioctl(fd[0], I2C_SLAVE, PCA_ADDR) < 0)  // 這個東西不用重複叫 會自己叫
    {
        fprintf(stderr, "Failed to acquire bus access or talk to slave.\n");
        close(fd[0]);
        return -1;
    }

    // 初始化 PCA9955B 設定
    // reference: 7.3 Register definitions (p.17) 45h
    unsigned char config[2] = {0x45, 0xFF};
    if (write(fd[0], config, 2) != 2) {
        fprintf(stderr, "Failed to write config to PCA9955B.\n");
        close(fd[0]);
        return -1;
    }

    // Mode 1 register:
    buffer[0] = 0x00;
    buffer[1] = 0x89;
    write(fd[0], buffer, 2);

    buffer[0] = 0x02;
    buffer[1] = 0xAA;
    write(fd[0], buffer, 2);

    /*
        buffer[0] = 0x88;
        buffer[1] = 0xAA;
        buffer[2] = 0xAA;
        buffer[3] = 0xAA;
        write(fd[0], buffer, 4);
    */

    for (int i = 0xFF; i > 0; i = i - 32) {
        // Auto-Increment
        // reference: Table 8. MODE1 (p.18) default value is 0x80
        buffer[0] = 0x88;
        buffer[1] = 0;
        buffer[2] = 0;
        buffer[3] = i;
        write(fd[0], buffer, 4);
        usleep(100000);
        cout << buffer[0] << endl;
    }

    for (int i = 0xFF; i > 0; i = i - 32) {
        buffer[0] = 0x88;
        buffer[1] = 0;
        buffer[2] = i;
        buffer[3] = 0;
        write(fd[0], buffer, 4);
        usleep(100000);
        cout << buffer[0] << endl;
    }

    for (int i = 0xFF; i > 0; i = i - 32) {
        buffer[0] = 0x88;
        buffer[1] = i;
        buffer[2] = 0;
        buffer[3] = 0;
        write(fd[0], buffer, 4);
        usleep(100000);
        cout << buffer[0] << endl;
    }

    buffer[0] = 0x88;
    buffer[1] = 0;
    buffer[2] = 0;
    buffer[3] = 0;
    write(fd[0], buffer, 4);

    /*
        buffer[0] = 0x08;
        buffer[1] = 0xFF;
        if(write(fd[0], buffer, 2)!=2)
            fprintf(stderr, "Wrong in stage 3");

        buffer[0] = 0x09;
        buffer[1] = 0xEE;
        write(fd[0], buffer, 2);

        unsigned char* dom[1];
        read(fd[0], dom, 1);
        cout << int(dom[0]) << endl;

        buffer[0] = 0x0A;
        buffer[1] = 0xFF;
        write(fd[0], buffer, 2);

        unsigned char* random[1];
        read(fd[0], random, 1);
        cout << int(random[0]) << endl;
    */
}
