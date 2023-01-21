#ifndef UART_H_
#define UART_H_

int configureUart();
void sendUartRequest(char subCode);
int getIntFromUartOutput();
float getFloatFromUart();
void sendIntToUart(int subCode, int value);
void sendFloatToUart(int subCode, float value);
void sendByteToUart(int subCode, char value);

#endif /* UART_H_ */