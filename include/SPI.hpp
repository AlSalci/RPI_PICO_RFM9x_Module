#include "pico/stdio.h"
#include "hardware/spi.h"
#include "pico/binary_info.h"
#include <iostream>
#include <vector>
#define buffer_length 0x64


class SPI
{
private:




public:

/*
@ SPI instance that is used by the board
*/
static spi_inst_t* spi_instance; 

static int baudrate;


uint8_t input_buffer[buffer_length];
uint8_t output_buffer[buffer_length];


SPI(int Rx, int Tx, int SCK, int CS, bool isSlave);

/*
@ Default data transfer function. Sends the data in the output buffer at the default buffer length
@ Params: None
*/



int Rx,Tx,SCK,CS;


/*
@ Reads the data from slave device, stores data in the master input buffer
@ Params: 
@ buffer- Data to send to the slave device over MOSI line
@ data_length- Expected length of the returned data, sent over MISO line
*/
void SPI_Send_Data(uint8_t *buffer, int data_length);


/*
@ Reads the data from slave device, stores data in the master input buffer
@ Params: 
@ reg- register to read data from, sent to slave device over MOSI line
@ data_length- Expected length of the returned data, sent over MISO line
*/
void SPI_Read_Data(uint8_t *reg,int data_length);


void print_buffer(uint8_t buffer[],size_t length);

void reset_buffers();
void make_Slave();
void make_Master();





};