#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

struct bme280_dev initBmeConn(void);
float getSensorTemperature(struct bme280_dev dev);

#endif /* TEMPERATURE_H_ */