#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "global_state.h"

static FILE *logFile;
int logRunning = 0;

void runLogger() {
    logFile = fopen("oven_log.csv", "a");
    logRunning = 1;
    char dateStamp[60];
    write_log_header();

    while (logRunning) {
        time_t dt;
        time(&dt);
        strftime(dateStamp, 60, "%FT%TZ", localtime(&dt));

        int resistorPercent = pid > 0 ? pid : 0;
        int fanPercent = pid < 0 ? (pid * -1) : 0;

        fprintf(logFile, "%s,%.2f,%.2f,%.2f,%d,%d\n", dateStamp, internTemp, externTemp, refTemp, resistorPercent, fanPercent);
        fflush(logFile);
        sleep(1);
    }
}

void write_log_header(){
    fprintf(logFile, "%s,%s,%s,%s,%s,%s\n", "Data", "Temperatura interna", "Temperatura externa", "Temperatura de referencia", "Resistor", "Ventoinha");
    fflush(logFile);
}

void closeLogger() {
    logRunning = 0;
    fflush(logFile);
    fclose(logFile);
}