#include <stdio.h>
#include <signal.h>
#include <unistd.h>   
#include <fcntl.h>         
#include <termios.h>       
#include <string.h>     
#include <pthread.h>

#include "controller.h"
#include "temperature.h"
#include "bme280.h"
#include "pid.h"
#include "crc.h"

int uart0_filestream;
struct bme280_dev bme_conn;

void exitProccess();

int main(int argc, const char * argv[]) {
    // Prepare to receive sig and finish program
    signal(SIGINT, exitProccess);

    // Initialize
    bme_conn = initBmeConn();
    uart0_filestream = configureUart();

    pthread_t tid;
    pthread_create(&tid, NULL, (void *)handleCLI, (void *)NULL);
    pthread_join(tid, NULL);

    return 0;
}

void exitProccess() {
    close(uart0_filestream);
    exit(0);
}