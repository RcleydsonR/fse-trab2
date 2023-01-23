#include <global_state.h>

int LISTENING_COMMANDS = 0, COUNTDOWN_WORKING = 0, SYSTEM_ON = 0, OVEN_WORK = 0;

int uart0_filestream = -1, controlByCurve = 0, curveState = 0, pid = 0;

int deltaCurveTime[10] = {30, 60, 120, 20, 40, 60, 60, 60, 120, 10};
float deltaCurveTemperature[10] = {25.0, 38.0, 46.0, 54.0, 57.0, 61.0, 63.0, 54.0, 33.0, 25.0};

float refTemp = 0.0;
float internTemp = 0.0;
float externTemp = 0.0;