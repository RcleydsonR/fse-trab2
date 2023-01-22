#ifndef GPIO_H_
#define GPIO_H_

void initGpio();
void setFan(int pidIntensity);
void setResistor(int pidIntensity);
void controlGpioBasedOnPid(int pidIntensity);

#endif /* GPIO_H_ */