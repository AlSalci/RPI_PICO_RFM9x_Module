#include "hardware/spi.h"
#include "hardware/gpio.h"
#include "pico/stdio.h"
#include "pico/time.h"
#include "../include/SPI.hpp" // Should always include SPI file


#define Read 0x00 //MSB of address should be 0 to authroize read command
#define Write 0x80// MSB of address should be 1 to authroize write command

#define reg_FIFO 0x00
#define address_TX 0x80 // Start Address for TX in FIFO
#define address_RX 0x00 // Start Address for RX in FIFO
#define FIFO_PNTR 0x0D //FIFO address pointer, MUST BE SET before read and write operation to the FIFO

#define Reg_OpMode 0x01 
//OpMode Commands===========================
#define FSK_OOK_MODE 0x00
#define LoRa_MODE 0x80// Can only be changed in sleep mode

#define Modulation_Type_FSK 0x00
#define Modulation_Type_OOK 0x20

#define LowFrequency_Mode_On 0x08

//For lora settings
#define Mode_Sleep 0x00
#define Mode_Stdby 0x01
#define Mode_FSTX 0x02
#define Mode_TX 0x03
#define Mode_FSRX 0x04
#define Mode_RX_Recieve_Cont 0x05
#define Mode_RX_Recieve_Sing 0x06
#define Mode_CAD 0x07
//==========================================

#define reg_Modem_Config 0x1E
#define reg_Pa_Config 0x09


enum STATE_RX
{
    WAIT_RX,
    SLEEP_RX,
    STANDBY_RX,
    RECIEVE
};

enum STATE_TX
{
    STANDBY_TX,
    FIFO_WRITE,
    TRANSMIT,
    WAIT

};



class RFM9x 
{

private:
SPI *spi;


public:
/*
RST = Reset
G0= interrupt pin
EN = Enable Pin
All other pins are controlled by SPI object
*/
int RST,G0,EN;


RFM9x(SPI *spi,int RST,int G0,int EN);


/*
@ State Machine for the recieving module
@ Params:None
*/
void StateMachine_RX();
STATE_RX reciever_state;

/*
@ State Machine for the trasnmitting module, note that the write state uses the output buffer to write to the FIFO
@ Params:None
*/
void StateMachine_TX();
STATE_TX transmitter_state;
bool transmit_data,recieve_data;

/*
@ Reads data from radio module, Store all data in the input buffer of the SPI master device
@ Params: 
@ reg- Register to read from(Should only be a single 8-bit value)
@ spi - spi object that handles all SPI communication
*/
void RFM9x_Read_Register(uint8_t reg);

/*
@ Reads data from radio module
@ Params: 
@ reg- Register to read from(Should only be a single 8-bit value)
@ data- data to send (should be single 8-bit value)
@ spi - spi object that handles all SPI communication
*/
void RFM9x_Write_Register(uint8_t reg,uint8_t data);


/*
@ Sets the state of the Module to go into constant recieve mode
@ Params:NONE
*/
void RFM9x_Cont_Recieve();

/*
@ Sets the state of the Module to into stand by mode
@ Params:NONE
*/
void RFM9x_Standby();


void RFM9x_TX_mode();

void RFM9x_RX_mode();







};
