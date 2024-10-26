#include <iostream>
#include <string>
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#include "../include/RFM9x.hpp"
//#include "../include/LCD.hpp"
#include "hardware/spi.h"
#include "pico/stdio.h"
//#include "../include/Distance_Sensor.hpp"

//Commands cmake .. -G "MinGW Makefiles"
//C:/Users/salci/Desktop/ece520programs/Applications/MSYS2/mingw64/bin/mingw32-make


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


std::vector<int> LEDS = {18,19,20,21};
void Initialize_LED_GPIOs(std::vector<int> LEDs,bool isOut);

void Mask_LEDs(uint8_t *(input_buffer));

void read_buttons(uint8_t *(output_buffer));


int main()
{
    stdio_init_all();

    
    
    SPI master(12,15,14,13,false);
    RFM9x radio(&master,11,10,9);
    set_gpio_to_callback(radio.G0, GPIO_IRQ_EDGE_RISE);
     
    spi_set_format(master.spi_instance, 8, SPI_CPOL_0, SPI_CPHA_0,SPI_MSB_FIRST);
        
    
    bool reciever = false;

    if(reciever == false )
    {
    Initialize_LED_GPIOs(LEDS, 0);
    } else{
         Initialize_LED_GPIOs(LEDS, 1);
    }
 
    while(true)
    {
     
    if(reciever == false)
    {
        
        read_buttons(master.output_buffer);
        printf("%x\n", master.output_buffer[0]);
        //master.output_buffer[0] = 'C';
        radio.transmit_data = true;
        radio.StateMachine_TX(); 
    } else  
    {  
        radio.recieve_data = true;
       radio.StateMachine_RX();  
       Mask_LEDs(master.input_buffer);
      
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


void Initialize_LED_GPIOs(std::vector<int> LEDs,bool isOut)
{

    for(int i = 0;i < LEDs.size(); i++)
    {
        gpio_init(LEDs[i]);
        gpio_set_dir(LEDs[i], isOut);
        gpio_put(LEDs[i], 0);
    }

}

void Mask_LEDs(uint8_t *(input_buffer))
{

int temp = 0x01; 
//printf("%x\n", input_buffer[0]);
for(int i = 0; i < LEDS.size();i++)
{
    gpio_put(LEDS[i],temp & input_buffer[0]);
    temp *= 2;
}




}

void read_buttons(uint8_t *(output_buffer))
{
    output_buffer[0] = 0x00; //Clears output buffer
    
    int temp = 0x01;
    for(int i = 0; i < LEDS.size();i++)
    {
       
        if(gpio_get(LEDS[i]) == 1)
        {
            output_buffer[0] = output_buffer[0] | temp;

        }
        

        temp *= 2;


    }
    

}