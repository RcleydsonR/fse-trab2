#include <stdio.h>
#include <wiringPi.h>
#include <softPwm.h>

#define GPIO_RESISTOR 4
#define GPIO_FAN 5
#define MAX_INTENSITY 100
#define MIN_INTENSITY 0

void initGpio() {
    if (wiringPiSetup () == -1)
        exit (1);
    pinMode(GPIO_RESISTOR, OUTPUT);
    pinMode(GPIO_FAN, OUTPUT);
    softPwmCreate(GPIO_FAN, MIN_INTENSITY, MAX_INTENSITY);
    softPwmCreate(GPIO_RESISTOR, MIN_INTENSITY, MAX_INTENSITY);
}    

void setFan(int pidIntensity) {
    softPwmWrite(GPIO_FAN, pidIntensity);
}

void setResistor(int pidIntensity) {
    softPwmWrite(GPIO_RESISTOR, pidIntensity < 40 ? 40 : pidIntensity);
}

void disableFanAndResistor() {
    softPwmWrite(GPIO_FAN, MIN_INTENSITY);
    softPwmWrite(GPIO_RESISTOR, MIN_INTENSITY);

}

void controlGpioBasedOnPid(int pidIntensity) {
    if (pidIntensity < 0) {
        setResistor(MIN_INTENSITY);
        setFan(pidIntensity * (-1));
    } else if (pidIntensity > 0) {
        setResistor(pidIntensity);
        setFan(MIN_INTENSITY);
    } else if (pidIntensity > -40 && pidIntensity < 0) {
        setFan(40);
        setResistor(MIN_INTENSITY);
    }
    else {
        setFan(MIN_INTENSITY);
        setResistor(MIN_INTENSITY);
    }
} 