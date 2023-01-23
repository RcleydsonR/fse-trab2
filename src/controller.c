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

pthread_t countDownThread, commandListenerThread;

void controlOvenByDashboard();
void controlOvenByCurve();
void updateCurveState();
void commandListener();

void handleCLI() {
    SYSTEM_ON = 1;
    LISTENING_COMMANDS = 1;
    sendByteToUart(0xD4, controlByCurve);

    pthread_create(&commandListenerThread, NULL, (void *)commandListener, NULL);

    while (SYSTEM_ON) {
        int command = getCommandOption();
        while (command < 1 || command > 3)
            printf("Opcao invalida! Insira uma opcao entre 1 e 3.\n");

        if (command == 3)
            definePIDConst();

        controlByCurve = command == 2;
        sendByteToUart(0xD4, controlByCurve);
    }
}

int getCommandOption(){
    int commandOption;
    printf("\n\n\n\n\n");
    printf("Digite o numero referente a opcao que deseja:\n");
    printf("==================================================\n");
    printf("1. Controlar temperatura pelo dashboard%s\n", controlByCurve ? "." : " - Selecionado.");
    printf("2. Controlar temperatura pela curva de referencia%s\n", controlByCurve ? " - Selecionado." : ".");
    printf("3. Controlar as constantes do PID.\n");
    scanf(" %d", &commandOption);
    return commandOption;
}

void definePIDConst() {
    double Kp, Ki, Kd;

    printf("\nDigite o valor de Kp:\n");
    scanf("%lf", &Kp);
    printf("Digite o valor de Ki:\n");
    scanf("%lf", &Ki);
    printf("Digite o valor de Kd:\n");
    scanf("%lf", &Kd);

    pid_configura_constantes(Kp, Ki, Kd);
    return;
}

void commandListener() {
    int uartCommand = 0;

    while (LISTENING_COMMANDS) {
        sendUartRequest(0xC3);
        uartCommand = getIntFromUartOutput();
        externTemp = getTempFromBme();
        handleCommand(uartCommand);
        sleep(1);
    }
}

void handleCommand(int command) {
    if (command == 0xA1){
        sendByteToUart(0xD3, 1);
    }
    else if (command == 0xA2){
        sendByteToUart(0xD3, 0);
        OVEN_WORK = 0;
        SYSTEM_ON = 0;
        LISTENING_COMMANDS = 0;
        COUNTDOWN_WORKING = 0;
    }
    else if (command == 0xA3 && SYSTEM_ON && !OVEN_WORK){
        sendByteToUart(0xD5, 1);
        controlOven();
    }
    else if (command == 0xA4 && OVEN_WORK){
        sendByteToUart(0xD5, 0);
        OVEN_WORK = 0;
        COUNTDOWN_WORKING = 0;
    }
    else if (command == 0xA5 && SYSTEM_ON){
        controlByCurve = controlByCurve == 0 ? 1 : 0;
        if(controlByCurve == 0)
            COUNTDOWN_WORKING = 0;
        sendByteToUart(0xD4, controlByCurve);
        sleep(1);
        controlOven();
    }
}

void controlOven() {
    OVEN_WORK = 0;
    if (controlByCurve)
        controlOvenByCurve();
    else
        controlOvenByDashboard();
}

void controlOvenByCurve() {
    OVEN_WORK = 1;
    COUNTDOWN_WORKING = 1;
    curveState = 0;
    pthread_create(&countDownThread, NULL, (void *)updateCurveState, NULL);

    while (OVEN_WORK) {
        if(curveState == 10){
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

        sendUartRequest(0xC3);
        int uartCommand = getIntFromUartOutput();
        handleCommand(uartCommand);
    }
}

void controlOvenByDashboard() {
    OVEN_WORK = 1;
    while (OVEN_WORK) {
        sendUartRequest(0xC1);
        internTemp = getFloatFromUart();
        sendUartRequest(0xC2);
        refTemp = getFloatFromUart();

        pid_atualiza_referencia(refTemp);
        pid = pid_controle(internTemp);
        controlGpioBasedOnPid(pid);
        sendIntToUart(0xD1, pid);
        
        sendUartRequest(0xC3);
        int uartCommand = getIntFromUartOutput();
        handleCommand(uartCommand);
    }
}

void updateCurveState()
{
    while(COUNTDOWN_WORKING && curveState < 10){
        sleep(deltaCurveTime[curveState]);
        curveState++;
    }
    
    COUNTDOWN_WORKING = 0;
    return;
}