#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>

#include <main.h>
#include <uart.h>
#include <pid.h>
#include <temperature.h>
#include <gpio.h>

int LISTENING_COMMANDS = 0;
int SYSTEM_ON = 0;
int OVEN_WORK = 0;

int controlByCurve = 0;

void controlOvenByDashboard();
void controlOvenByCurve();

void handleCLI() {
    while (1)
    {
        int command = getCommandOption();
        while (command < 1 || command > 2)
            printf("Opcao invalida!\n");

        controlByCurve = command % 1;
        sendByteToUart(0xD4, controlByCurve);
        LISTENING_COMMANDS = 1;

        commandListener();
    }
}

void commandListener() {
    int uartCommand = 0;

    while (LISTENING_COMMANDS) {
            sendUartRequest(0xC3);
            uartCommand = getIntFromUartOutput();
            handleCommand(uartCommand);
            delay(500);
        }
}

int getCommandOption(){
    int commandOption;
    printf("Digite o numero da opcao que deseja para controle da temperatura de referência:\n");
    printf("====================================\n");
    printf("1. Controlar temperatura pelo dashboard\n");
    printf("2. Controlar temperatura pela curva de referência\n");
    scanf("%d", &commandOption);
    return commandOption;
}

void handleCommand(int command) {
    if (command == 0xA1){
        sendByteToUart(0xD3, 1);
        SYSTEM_ON = 1;
    }
    else if (command == 0xA2){
        sendByteToUart(0xD3, 0);
        SYSTEM_ON = 0;
        OVEN_WORK = 0;
    }
    else if (command == 0xA3 && SYSTEM_ON && !OVEN_WORK){
        sendByteToUart(0xD5, 1);
        OVEN_WORK = 1;
        controlOven();
    }
    else if (command == 0xA4 && OVEN_WORK){
        sendByteToUart(0xD5, 0);
        SYSTEM_ON = 0;
        OVEN_WORK = 0;
    }
    else if (command == 0xA5 && SYSTEM_ON){
        controlByCurve = controlByCurve == 0 ? 1 : 0;
        sendByteToUart(0xD4, controlByCurve);
        printf("Alternar entre o modo de temperatura de referencia e curva de temperatura\n");
    }
}

void controlOven() {
    if (controlByCurve)
        controlOvenByCurve();
    else
        controlOvenByDashboard();
}

void controlOvenByCurve() {
    int deltaCurveTime[10] = {0, 60, 120, 240, 260, 300, 360, 420, 480, 600};
    float deltaCurveTemperature[10] = {25.0, 38.0, 46.0, 54.0, 57.0, 61.0, 63.0, 54.0, 33.0, 25.0};
    int i = 0;

    while (OVEN_WORK) {
        printf("Temperatura referencia: %f\n", deltaCurveTemperature[i]);

        sendUartRequest(0xC1);
        float internTemp = getFloatFromUart();
        int pid = pid_controle(internTemp);

        printf("Error pid: %d\n", pid);
        printf("Temperatura interna: %f\n", internTemp);

        // timeAlreadyTaken = ((double)(clock() - startTime))/CLOCKS_PER_SEC;
        // printf("Tempo executando em segundos: %f \n", timeAlreadyTaken);

        // float tempAmbiente = getSensorTemperature(bme_conn);
        // printf("Temperatura ambiente: %f\n", tempAmbiente);
        
        sendUartRequest(0xC3);
        int uartCommand = getIntFromUartOutput();
        handleCommand(uartCommand);
    }
}

void controlOvenByDashboard() {
    while (OVEN_WORK) {
        sendUartRequest(0xC1);
        float internTemp = getFloatFromUart();
        sendUartRequest(0xC2);
        float refTemp = getFloatFromUart();

        pid_atualiza_referencia(refTemp);
        int pid = pid_controle(internTemp);
        controlGpioBasedOnPid(pid);

        printf("Temperatura referencia: %f\n", refTemp);
        printf("Temperatura interna: %f\n", internTemp);
        printf("pid: %d\n", pid);
        
        sendUartRequest(0xC3);
        int uartCommand = getIntFromUartOutput();
        handleCommand(uartCommand);
    }
}

// void updateReferenceTemperatureWithValue(float refTemp) {
//     printf("Temperatura referencia: %f\n", refTemp);
//     if (refTemp > 0){
//         sendFloatToUart(0xD2, refTemp);
//         float _ = getFloatFromUart();
//         pid_atualiza_referencia(refTemp);
//     }
// }

void updateReferenceTemperature() {
    sendUartRequest(0xC2);
    float refTemp = getFloatFromUart();
    if (refTemp > 0){
        sendFloatToUart(0xD2, refTemp);
        pid_atualiza_referencia(refTemp);
    }
}