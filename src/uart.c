#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <main.h>

int LISTENING_COMMANDS = 0;

int rx_length = 0;

char getCharFromUartOutput();

int configureUart(){
    int uart_filestream = -1;
    char uart_path[] = "/dev/serial0";
    uart_filestream = open(uart_path, O_RDWR | O_NOCTTY | O_NDELAY);
    if(uart_filestream == -1){
        printf("Não foi possível iniciar a Uart.\n");
    }
    else {
        printf("UART inicializado.\n");
    }
    struct termios options;
    tcgetattr(uart_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart_filestream, TCIFLUSH);
    tcsetattr(uart_filestream, TCSANOW, &options);
    return uart_filestream;
}

void commandListener() {
    int uartCommand = 0;

    while (LISTENING_COMMANDS) {
            sendUartRequest(0xC3);
            sleep(1);
            uartCommand = getIntFromUartOutput();
            printf("%d\n", uartCommand);
            delay(500);
        }
}

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

void sendUartRequest(char subCode) {
    unsigned char header[7] = {0x01, 0x23, subCode, 0x09, 0x00, 0x08, 0x05};
    unsigned char tx_buffer[20];

    short crc = calcula_CRC(header, 7);

    memcpy(tx_buffer, &header, 7);
    memcpy(&tx_buffer[7], &crc, 2);

    if (uart0_filestream != -1)
    {
        int check = write(uart0_filestream, &tx_buffer[0], 9);
        if (check < 0)
        {
            printf("UART TX error\n");
        }
    }
}

int getIntFromUartOutput()
{
    unsigned char buffer[20];
    int response = -1;

    int data_size = read(uart0_filestream, buffer, 20);
    if(!data_size){
        printf("Nenhum dado recebido\n");
    }
    else if(data_size < 0){
        printf("Erro na leitura dos dados\n");
    }
    else {
        buffer[data_size] = '\0';
        memcpy(&response, &buffer[3], sizeof(int));
        return response;
    }
    return response; 
}

int getCommandOption(){
    int commandOption;
    printf("Digite o numero da opcao que deseja:\n");
    printf("====================================\n");
    printf("1. Controlar forno pelo dashboard web\n");
    printf("2. Controlar temperatura pelo terminal\n");
    scanf("%d", &commandOption);
    return commandOption;
}