#include <stdio.h>
#include <unistd.h>        
#include <fcntl.h>         
#include <termios.h>       
#include <string.h>      

int getCommandOption(){
    int commandOption;
    printf("Digite a opção desejada:\n");
    printf("1. Solicitar Temperatura Interna\n");
    printf("2. Solicita Temperatura de Referencia\n");
    printf("3. Lê comandos do usuario\n");
    printf("4. Envia sinal de controle (Int)\n");
    printf("5. Envia sinal de Referencia (Float)\n");
    printf("6. Envia Estado do Sistema (Ligado = 1 / Desligado = 0)\n");
    printf("7. Modo de Controle da Temperatura de referencia (Dashboard = 0 / Curva/Terminal = 1)\n");
    printf("8. Envia Estado de Funcionamento (Funcionando = 1 / Parado = 0)\n");
    printf("9. Envia Temperatura Ambiente (Float)\n");
    scanf("%d", &commandOption);
    return commandOption;
}

char getOptionCode(int option){
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

int main(int argc, const char * argv[]) {

    int uart0_filestream = -1;
    int matricula[4] = {9, 0, 8, 5};

    int option = getCommandOption();

    char optionValue = getOptionCode(option);

    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (uart0_filestream == -1)
    {
        printf("Erro - Não foi possível iniciar a UART.\n");
    }
    else
    {
        printf("UART inicializada!\n");
    }    
    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    unsigned char tx_buffer[20];
    unsigned char *p_tx_buffer;
    
    p_tx_buffer = &tx_buffer[0];
    *p_tx_buffer++ = optionValue;
    memcpy(*p_tx_buffer, matricula, 4);

    printf("Buffers de memória criados!\n");
    
    if (uart0_filestream != -1)
    {
        printf("Escrevendo caracteres na UART ...");
        int count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
        if (count < 0)
        {
            printf("UART TX error\n");
        }
        else
        {
            printf("Lido.\n");
        }
    }

    sleep(1);

    if (uart0_filestream != -1)
    {
    
        unsigned char rx_buffer[256];
        int data;
        int rx_length = read(uart0_filestream, (void*)rx_buffer, 255);     
        if (rx_length < 0)
        {
            printf("Erro na leitura.\n");
        }
        else if (rx_length == 0)
        {
            printf("Nenhum dado disponível.\n");
        }
        else
        {
            printf("Lido");
        }
    }

    close(uart0_filestream);
   return 0;
}