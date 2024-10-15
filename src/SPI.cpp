#include "../include/SPI.hpp"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/time.h"
#include <cstdlib>



SPI::SPI(int Rx, int Tx, int SCK, int CS, bool isSlave)
{
spi_init(spi_instance,baudrate);
if(isSlave == true)
{
spi_set_slave(spi_instance,true);
}
this->Rx =Rx;
this->Tx =Tx;
this->SCK =SCK;
this->CS = CS;

gpio_set_function(Rx, GPIO_FUNC_SPI);
gpio_set_function(Tx, GPIO_FUNC_SPI);
gpio_set_function(SCK, GPIO_FUNC_SPI);
//gpio_set_function(CS, GPIO_FUNC_SPI);


gpio_init(CS);
gpio_set_dir(CS, GPIO_OUT);
gpio_put(CS,1); // Idles high for spi interface

//bi_decl(bi_4pins_with_func(Rx,Tx,SCK,CS,GPIO_FUNC_SPI));
reset_buffers();

}

void SPI::SPI_Send_Data(uint8_t *buffer, int data_length)
{
        gpio_put(CS,0);
        spi_write_blocking(spi_instance,buffer,data_length);
        gpio_put(CS,1);

}


void SPI::SPI_Read_Data(uint8_t *reg,int data_length)
{
    gpio_put(CS,0);
    spi_write_blocking(spi_instance,reg,1); //Assumes register is always an 8-bit value sent once
    spi_read_blocking(spi_instance, 0, input_buffer, data_length);
    gpio_put(CS,1);


}

void SPI::print_buffer(uint8_t buffer[],size_t length)
{

    for(size_t i = 0;i < length;i++)
    {
        printf("%02x ",buffer[i]);
    }
    printf("\n");


}




void SPI::reset_buffers()
{
    for(int i = 0; i < buffer_length; i++)
    {
        input_buffer[i] = 0;
        output_buffer[i] = 0;
    }
}

void SPI::make_Slave()
{
spi_set_slave(spi_instance,true);
}