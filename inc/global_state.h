#ifndef GLOBAL_STATE_H_
#define GLOBAL_STATE_H_

extern int LISTENING_COMMANDS, COUNTDOWN_WORKING, SYSTEM_ON, OVEN_WORK, uart0_filestream, controlByCurve, curveState, pid;
extern float refTemp, internTemp, externTemp;
extern int deltaCurveTime[10];
extern float deltaCurveTemperature[10];

#endif /* GLOBAL_STATE_H_ */