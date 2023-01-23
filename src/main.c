#include <stdio.h>
#include <signal.h>
#include <unistd.h>   
#include <fcntl.h>         
#include <termios.h>       
#include <string.h>     
#include <pthread.h>

#include "global_state.h"
#include "controller.h"
#include "temperature.h"
#include "pid.h"
#include "crc.h"
#include "gpio.h"
#include "log.h"

void exitProccess();

int main(int argc, const char * argv[]) {
    // Prepare to receive sig and finish program
    signal(SIGINT, exitProccess);

    // Initialize
    initGpio();
    uart0_filestream = configureUart();

    pthread_t cliThread, logThread;
    pthread_create(&logThread, NULL, (void *)runLogger, (void *)NULL);
    pthread_create(&cliThread, NULL, (void *)handleCLI, (void *)NULL);
    pthread_join(cliThread, NULL);

    return 0;
}

void exitProccess() {
    printf("Desligando programa...\n");
    disableFanAndResistor();
    disableFanAndResistor();
    closeLogger();
    close(uart0_filestream);
    exit(1);
}