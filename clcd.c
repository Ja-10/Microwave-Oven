#include <xc.h>
#include "clcd.h"

void clcd_write(unsigned char byte, unsigned char mode)  
{
    CLCD_RS = mode;
    
    CLCD_DATA_PORT = byte & 0xF0;   
    CLCD_EN = HI;
    __delay_us(100);
    CLCD_EN = LOW;

    CLCD_DATA_PORT = (byte & 0x0F) << 4; 
    CLCD_EN = HI;
    __delay_us(100);
    CLCD_EN = LOW;
    
    __delay_us(4100); 
}

static void init_display_controller(void)
{
    __delay_ms(30);
    
    clcd_write(EIGHT_BIT_MODE, INST_MODE);
    __delay_us(4100);
    clcd_write(EIGHT_BIT_MODE, INST_MODE);
    __delay_us(100);
    clcd_write(EIGHT_BIT_MODE, INST_MODE);
    __delay_us(1);
    
    clcd_write(FOUR_BIT_MODE , INST_MODE);
    __delay_us(100);
    clcd_write(TWO_LINES_5x8_4_BIT_MODE, INST_MODE);
   
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(500);
    clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
    __delay_us(100);
}

void init_clcd(void)
{
    CLCD_DATA_PORT_DDR = 0x00;
    
    CLCD_RS_DDR = 0;
    CLCD_EN_DDR = 0;
    
    init_display_controller();
}

void clcd_putch(const char data, unsigned char addr)
{
    clcd_write(addr, INST_MODE);
    clcd_write(data, DATA_MODE);
}

void clcd_print(const char *str, unsigned char addr)
{
    clcd_write(addr, INST_MODE);
    
    while (*str != '\0')
    {
        clcd_write(*str, DATA_MODE); 
        str++;
    }
}
