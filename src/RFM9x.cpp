#include "../include/RFM9x.hpp"




RFM9x::RFM9x(SPI *spi,int RST,int G0,int EN)
{
  this->RST = RST;
  this->G0 = G0;
  this->EN = EN;
  this->spi = spi;

  gpio_init(RST);
  gpio_init(G0);
  gpio_init(EN);

  gpio_set_dir(RST,GPIO_OUT);
  gpio_set_dir(EN,GPIO_OUT);
  gpio_set_dir(G0,GPIO_IN);
  

    gpio_put(EN,1); //Turns on Radio

    //Resets Radio=========================================
    gpio_put(RST,0);
    sleep_us(50);
     gpio_put(RST,1); 
    
    //=====================================================
 
    RFM9x_Write_Register(Reg_OpMode, Mode_Sleep|LoRa_MODE);
    RFM9x_Write_Register(Reg_OpMode, Mode_Sleep|LoRa_MODE);
    RFM9x_Read_Register(Reg_OpMode);
    if(spi->input_buffer[0] != (Mode_Sleep|LoRa_MODE))
    {
        printf("CRITICAL ERROR: Initialization Failed");

    }

    RFM9x_Write_Register(Reg_OpMode, Mode_Stdby|LoRa_MODE);
    RFM9x_Read_Register(Reg_OpMode);
    if(spi->input_buffer[0] != (Mode_Stdby|LoRa_MODE))
    {
        printf("CRITICAL ERROR: Initialization Failed");

    }
    RFM9x_Write_Register(0x11,0x08); // Set interrupt masks for reciever and transimiter
    RFM9x_Read_Register(0x1D);
    RFM9x_Write_Register(0x1D, (spi->input_buffer[0] & ~0x07)|0x60 );

    RFM9x_Read_Register(reg_Pa_Config);
    RFM9x_Write_Register(reg_Pa_Config, (spi->input_buffer[0] & ~0x70)|0xC0 );

    RFM9x_Read_Register(reg_Modem_Config);
   // RFM9x_Write_Register(reg_Modem_Config, (spi->input_buffer[0] & ~0xF0)|0xC0 );

  //  RFM9x_Write_Register(0x1E,0x08); 
    RFM9x_Standby();

    reciever_state = STANDBY_RX;
    transmitter_state = STANDBY_TX;
    transmit_data = false;
    recieve_data = false;
    

}

void RFM9x::RFM9x_Read_Register(uint8_t reg)
{
    //Will ALWAYS store read data in the input buffer of the SPI object
     uint8_t temp[] = {reg};
    spi->SPI_Read_Data(temp, 1);
    sleep_us(50); // Gives time for data to be set in device if needed
}


void RFM9x::RFM9x_Write_Register(uint8_t reg,uint8_t data)
{
    uint8_t temp[] = {static_cast<uint8_t>(reg | Write),data};
    
    spi->SPI_Send_Data(temp, 2);
    sleep_us(50);

}

void RFM9x::RFM9x_Cont_Recieve()
{
RFM9x_Read_Register(Reg_OpMode); // Reads value in register before reseting values
RFM9x_Write_Register(Reg_OpMode,(spi->input_buffer[0] & ~0x07)|Mode_Stdby);
}

void RFM9x::RFM9x_Standby()
{
RFM9x_Read_Register(Reg_OpMode);
RFM9x_Write_Register(Reg_OpMode,(spi->input_buffer[0] & ~0x07)|Mode_Stdby);
}

void RFM9x::RFM9x_TX_mode()
{
RFM9x_Read_Register(Reg_OpMode);
RFM9x_Write_Register(Reg_OpMode,(spi->input_buffer[0] & ~0x07)|Mode_TX);
RFM9x_Read_Register(Reg_OpMode);
}

void RFM9x::RFM9x_RX_mode()
{
RFM9x_Read_Register(Reg_OpMode);
RFM9x_Write_Register(Reg_OpMode,(spi->input_buffer[0] & ~0x07)|Mode_RX_Recieve_Cont);

}


void RFM9x::StateMachine_RX()
{
if(recieve_data == false)
{
    RFM9x_Standby();
    reciever_state = STANDBY_RX;

}else if(reciever_state == STANDBY_RX) // Reciever Standby
{
    
    RFM9x_Standby();
    //sleep_ms(3000);
    reciever_state = RECIEVE;

} else if(reciever_state == RECIEVE) // request recive mode
{

    RFM9x_RX_mode();
    reciever_state = WAIT_RX;
} else if(reciever_state == WAIT_RX) // Wait for data
{
RFM9x_Read_Register(0x12);
//RFM9x_Read_Register(Reg_OpMode);
//RFM9x_Read_Register(reg_FIFO);
//spi->print_buffer(spi->input_buffer, 1);

if((spi->input_buffer[0] & 0x40) != 0)
{
    RFM9x_Write_Register(0x12, 0x00);
    RFM9x_Write_Register(FIFO_PNTR,address_RX);//FIFO start address
    RFM9x_Read_Register(reg_FIFO);
     printf("%x\n", spi->input_buffer[0]);
    //printf("Character Recieved: %c\n",spi->input_buffer[0]);
   // spi->print_buffer(spi->input_buffer, 1);
    //reciever_state = STANDBY_RX;
}

}
 




}



void RFM9x::StateMachine_TX()
{
if(transmit_data == false) // Default state when the transmitter is not sending data
{
    
    RFM9x_Standby();

    transmitter_state = STANDBY_TX;

} else  
{
    if(transmitter_state == STANDBY_TX) // Standby state
    {

        transmitter_state = FIFO_WRITE;

    } else if(transmitter_state == FIFO_WRITE) // Write to FIFO 
    {
        //RFM9x_Standby();
         RFM9x_Write_Register(FIFO_PNTR,address_TX);//Start location of transmiter memory in FIFO
        RFM9x_Write_Register(reg_FIFO, spi->output_buffer[0]);
        sleep_us(100);
        //RFM9x_Read_Register(reg_FIFO);
        //spi->print_buffer(spi->input_buffer, 1);
        
        transmitter_state = TRANSMIT;
    } else if (transmitter_state == TRANSMIT) //Trasmit mode
    {
        RFM9x_TX_mode();
        transmitter_state = WAIT;
    }
    
    else if (transmitter_state == WAIT) // Wait for tx to finish
    {
        //sleep_us(500);
        RFM9x_Read_Register(Reg_OpMode);
         ///RFM9x_Write_Register(0x0D,0x00);//FIFO start address
        //RFM9x_Read_Register(reg_FIFO);
        if((spi->input_buffer[0]& 0x07 )== Mode_Stdby)
        {
             RFM9x_Read_Register(0x12);
            transmit_data = false; 
            transmitter_state = STANDBY_TX;
        }

       
        
    
    }



}



}