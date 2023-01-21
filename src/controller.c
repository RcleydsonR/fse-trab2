#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <main.h>
#include <uart.h>

int LISTENING_COMMANDS = 0;
int SYSTEM_ON = 0;
int OVEN_WORK = 0;

void handleCLI() {
    while (1)
    {
        int command = getCommandOption();
        if (command == 1){
            LISTENING_COMMANDS = 1;
            commandListener();
        }
        else if (command == 2)
            continue;
        else
            printf("Opcao invalida!\n");
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
    printf("Digite o numero da opcao que deseja:\n");
    printf("====================================\n");
    printf("1. Controlar forno pelo dashboard\n");
    printf("2. Controlar temperatura pelo terminal\n");
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
    else if (command == 0xA3){
        sendByteToUart(0xD5, 1);
        OVEN_WORK = 1;
        controlOven();
    }
    else if (command == 0xA4 && SYSTEM_ON){
        sendByteToUart(0xD5, 0);
        SYSTEM_ON = 0;
        OVEN_WORK = 0;
    }
    else if (command == 0xA5 && SYSTEM_ON){
        printf("Alternar entre o modo de temperatura de referencia e curva de temperatura\n");
    }
}

void controlOven() {
    while (OVEN_WORK) {
        sendUartRequest(0xC1);
        float internTemp = getFloatFromUart();
        sendUartRequest(0xC2);
        float refTemp = getFloatFromUart();
        printf("Temperatura interna: %f\n", internTemp);
        printf("Temperatura referencia: %f\n", refTemp);
        sendUartRequest(0xC3);
        int uartCommand = getIntFromUartOutput();
        handleCommand(uartCommand);
    }
}