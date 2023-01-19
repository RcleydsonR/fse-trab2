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
            sleep(1);
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
        SYSTEM_ON = 1;
        sendByteToUart(0xD3, 1);
    }
    else if (command == 0xA2){
        SYSTEM_ON = 0;
        sendByteToUart(0xD3, 0);
    }
    else if (command == 0xA3){
        sendByteToUart(0xD5, 1);
    }
    else if (command == 0xA4 && SYSTEM_ON){
        sendByteToUart(0xD5, 0);
        LISTENING_COMMANDS = 0;
    }
    else if (command == 0xA5 && SYSTEM_ON){
        printf("Alternar entre o modo de temperatura de referencia e curva de temperatura\n");
    }
}

char getSubCodeFromOption(int option){
    switch (option){
        case 1:
            return 0xC1;
            break;
        case 2: 
            return 0xC2;
            break;
        case 3: 
            return 0xC3;
            break;
        case 4:
            return 0xD1;
            break;
        case 5: 
            return 0xD2;
            break;
        case 6: 
            return 0xD3;
            break;
        case 7:
            return 0xD4;
            break;
        case 8: 
            return 0xD5;
            break;
        default:
            return 0xD6;
    }
}