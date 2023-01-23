#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include <global_state.h>
#include <uart.h>
#include <pid.h>
#include <temperature.h>
#include <gpio.h>

pthread_t countDownThread;

void controlOvenByDashboard();
void controlOvenByCurve();
void *updateCurveState(void *vargp);

void handleCLI() {
    while (1)
    {
        int command = getCommandOption();
        while (command < 1 || command > 2)
            printf("Opcao invalida!\n");

        controlByCurve = command == 2;
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
            sleep(1);
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
    COUNTDOWN_WORKING = 1;
    curveState = 0;
    pthread_create(&countDownThread, NULL, updateCurveState, NULL);

    while (OVEN_WORK) {
        if(curveState == 10){
            printf("Aquecimento vide curva de referência finalizado.");
            OVEN_WORK = 0;
            break;
        }

        refTemp = deltaCurveTemperature[curveState];
        sendUartRequest(0xC1);
        internTemp = getFloatFromUart();
        sendFloatToUart(0xD2, refTemp);

        pid_atualiza_referencia(refTemp);
        pid = pid_controle(internTemp);
        controlGpioBasedOnPid(pid);
        sendIntToUart(0xD1, pid);

        printf("Temperatura interna: %f\n", internTemp);
        printf("Temperatura referencia: %f\n", refTemp);
        printf("pid: %d\n", pid);
        
        sendUartRequest(0xC3);
        int uartCommand = getIntFromUartOutput();
        handleCommand(uartCommand);
    }
}

void controlOvenByDashboard() {
    while (OVEN_WORK) {
        sendUartRequest(0xC1);
        internTemp = getFloatFromUart();
        sendUartRequest(0xC2);
        refTemp = getFloatFromUart();

        pid_atualiza_referencia(refTemp);
        pid = pid_controle(internTemp);
        controlGpioBasedOnPid(pid);
        sendIntToUart(0xD1, pid);

        printf("Temperatura referencia: %f\n", refTemp);
        printf("Temperatura interna: %f\n", internTemp);
        printf("pid: %d\n", pid);
        
        sendUartRequest(0xC3);
        int uartCommand = getIntFromUartOutput();
        handleCommand(uartCommand);
    }
}

void *updateCurveState(void *vargp)
{
    printf("Entrei na thread\n");

    while(COUNTDOWN_WORKING && curveState < 10){
        sleep(deltaCurveTime[curveState]);
        curveState++;
        printf("Novo estado da curva: %d\n", curveState);
    }
    
    COUNTDOWN_WORKING = 0;
    return NULL;
}