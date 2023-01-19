#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include <main.h>

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
    sleep(1);
}

int getIntFromUartOutput() {
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

void sendIntToUart(int subCode, int value) {
    unsigned char header[7] = {0x01, 0x23, subCode, 0x09, 0x00, 0x08, 0x05};
    unsigned char message[13];

    memcpy(message, &header, 7);
    memcpy(&message[7], &value, 4);

    short crc = calcula_CRC(message, 11);

    memcpy(&message[11], &crc, 2);

    int check = write(uart0_filestream, &message[0], 13);

    if(check < 0){
        printf("Ocorreu um erro na comunicação com o UART\n");
    }
    sleep(1);
}

void sendFloatToUart(int subCode, float value) {
    unsigned char header[7] = {0x01, 0x23, subCode, 0x09, 0x00, 0x08, 0x05};
    unsigned char message[13];

    memcpy(message, &header, 7);
    memcpy(&message[7], &value, 4);

    short crc = calcula_CRC(message, 11);

    memcpy(&message[11], &crc, 2);

    int check = write(uart0_filestream, &message[0], 13);

    if(check < 0){
        printf("Ocorreu um erro na comunicação com o UART\n");
    }
    sleep(1);
}

void sendByteToUart(int subCode, char value) {
    unsigned char header[7] = {0x01, 0x23, subCode, 0x09, 0x00, 0x08, 0x05};
    unsigned char message[10];

    memcpy(message, &header, 7);
    memcpy(&message[7], &value, 1);

    short crc = calcula_CRC(message, 8);

    memcpy(&message[8], &crc, 2);

    int check = write(uart0_filestream, &message[0], 10);

    if(check < 0){
        printf("Ocorreu um erro na comunicação com o UART\n");
    }
    sleep(1);
}