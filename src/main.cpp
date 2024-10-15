#include <iostream>
#include <string>
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#include "../include/RFM9x.hpp"
//#include "../include/LCD.hpp"
#include "hardware/spi.h"
#include "pico/stdio.h"
//#include "../include/Distance_Sensor.hpp"




spi_inst_t* SPI::spi_instance = spi1;
int SPI::baudrate = 1000*1000;

/*
@ General Callback function to be used to for system interrupts
@ Params: 
@ gpio- GPIO that caused the interrupt
@ event - Event that caused the interrupt
*/
void callback(uint gpio, uint32_t events);


/*
@ Sets a gpio to cause an interrupt, assumes it is already initalized and is an input
@ Params: 
@ gpio- GPIO to cause the interrupt
@ event - Event to cause the interrupt
*/
void set_gpio_to_callback(int gpio,uint32_t event);




int main()
{
    stdio_init_all();

    
    //sleep_us(400);
    SPI master(12,15,14,13,false);
    RFM9x radio(&master,18,19,20);
    set_gpio_to_callback(radio.G0, GPIO_IRQ_EDGE_RISE);
     //sleep_us(400);
    spi_set_format(master.spi_instance, 8, SPI_CPOL_0, SPI_CPHA_0,SPI_MSB_FIRST);
        
    gpio_init(1);
    gpio_set_dir(1, GPIO_IN);
    gpio_pull_up(1);
    
    bool reciever = false;
 
    while(true)
    {
      
    if(reciever == false)
    {
        if(gpio_get(1) == 1)
        {
        
       
       radio.StateMachine_TX();
       if(radio.transmit_data == false)
        {
            radio.transmit_data = true;
        }
        } else {
        
        radio.transmit_data = false;
        printf("Enter Charater:\n");
        std::cin>> master.output_buffer[0];
        }
        
    } else  
    {
         if(gpio_get(1) == 1)
        {
        
       
       radio.StateMachine_RX();
         if(radio.recieve_data == false)
            {
            radio.recieve_data = true;
            }
        } else{
            radio.recieve_data = false;
        }

        
        //printf("Character Recieved: %c\n",master.input_buffer[0]);
    }
    
    }

    return 0;
}


void callback(uint gpio, uint32_t events)
{
printf("Hello");
//sleep_ms(5000);


}

void set_gpio_to_callback(int gpio,uint32_t event)
{
gpio_set_irq_enabled_with_callback(gpio, event, true, &callback);

}