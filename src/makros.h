#ifndef MY_MAKROS_USEFULL_2013_11_12
#define MY_MAKROS_USEFULL_2013_11_12

#define INVBIT(port, bit) port = port ^ (1<<bit);
#define UPBIT(port, bit) port = port | (1<<bit);
#define DOWNBIT(port, bit) port = port & (~(1<<bit));

#define uchar unsigned char
#define uint unsigned int

#define TRUE 0xFF
#define FALSE 0x00


#endif
