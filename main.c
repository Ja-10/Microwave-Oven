#include <xc.h>
#include "clcd.h"
#include "main.h"
#include "matrix_keypad.h"
#include "timers.h"
#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT disabled)
int operation_mode, reset_flag,min, sec, ret, flag = 0, heat_flag =0, disp_flag;
char sec_array[3], min_array[3];
//min_array[2], sec_array[2];

static void init_config(void) {
    init_clcd();
    
    init_matrix_keypad();
    
    init_timer2();
    
    BUZZER_DDR = 0;
    BUZZER = OFF;
    //TRISC2 = 0;
    FAN_DDR = 0;
    FAN =OFF;
        
    PEIE = 1;
    GIE = 1;
}

void main(void) {
    unsigned char key;
    init_config();
    power_on_screen();
    clear_screen();
    
    cooking_header();
    clear_screen();
    
    operation_mode = COOKING_MODE_DISPLAY;
    
    while(1) {
        key = read_matrix_keypad(STATE);
        if(operation_mode == MICRO_MODE || operation_mode == GRILL_MODE || operation_mode == CONVECTION_MODE)
        {
            ;
        }
        else if(key == 1)
        {
          operation_mode = MICRO_MODE;
          reset_flag = MICRO_MODE_RESET;
          clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
          clear_screen();
          clcd_print(" Power = 900W", LINE2(1));
         __delay_ms(3000);
          clear_screen();
        }
       
        else if(key == 2)
        {
            operation_mode = GRILL_MODE;
            reset_flag = GRILL_MODE_RESET;
            clear_screen();
        }
        
        else if(key == 3)
        {
            operation_mode = CONVECTION_MODE;  
            reset_flag = RESET_TEMP;  
            clear_screen();
        }
        
        else if(key == 4)
        {
            if(operation_mode == COOKING_MODE_DISPLAY)
            {
                clear_screen();
                min = 0;
                sec  = 30;
                TMR2ON = ON;
                FAN = ON;
                operation_mode = TIME_DISPLAY; 
                
            }
            else if(operation_mode == TIME_DISPLAY)
            {
                sec = sec + 30;
                if(sec >= 60)
                {
                    min++;
                    sec = sec - 60;
                }
            }
            else if(operation_mode == PAUSE_MODE)
            {
                TMR2ON = ON;
                FAN = ON;
                operation_mode = START_MODE;
            }
        }
        else if(key == 5)
        {
            operation_mode = PAUSE_MODE;
            FAN = OFF;
           
        }
        else if(key == 6)
        {
            operation_mode = STOP_MODE;
            clear_screen();
            
        }
        opearation_call(key);
        
    }
}

void opearation_call(unsigned char key)
{
    switch(operation_mode)
    {
        case COOKING_MODE_DISPLAY:
            cooking_modes();
            break;
        case START_MODE:
        case TIME_DISPLAY:
            time_display();
            break;
       
        case MICRO_MODE:
            set_time(key);
            break;
            
        case GRILL_MODE:
            set_time(key);
            break;
        case CONVECTION_MODE:
        
        if(heat_flag == 0)
        {
            ret = set_temp(key);
            if(ret == FAILURE)
            {
                flag = 1;
                reset_flag =  RESET_TEMP;

            }
            else if(ret == SUCCESS)
            {
                TMR2ON = OFF;
                flag = 1;
                heat_flag = 1;
                clear_screen();
                reset_flag = RESET_TIME;
                set_time(key);
            }
            else 
            {
                flag = 0;
            }
        }
        else
        {
            set_time(key);
        }
            break;
        case STOP_MODE:
            TMR2ON = OFF;
            FAN = OFF;
            operation_mode = COOKING_MODE_DISPLAY;
            break;
      /*  case PAUSE:
            TMR2ON = OFF;
             FAN = OFF;
            break; */
            
    }
    if(flag == 0)
    {
          reset_flag = RESET_NOTHING;
    }
  
}



// To Print Power on Message
void power_on_screen(void) {
    for(int i = 0; i < 16; i++)
    {
       clcd_putch(BLOCKS,LINE1(i)); // 0 to 15 for whole line 1
       clcd_putch(BLOCKS,LINE2(i));
    }
    clcd_print("  Powering ON ", LINE2(2));
    clcd_print(" Microwave Oven ", LINE3(1));

    // Delay
    __delay_ms(1000); // 3sec
}

void cooking_header(void)
{
    clcd_print("COOKING MODES",LINE2(1));
    __delay_ms(1000);
}

void cooking_modes(void){
    clcd_print("1.Micro", LINE1(0));
    clcd_print("2.Grill", LINE2(0));
    clcd_print("3.Convection", LINE3(0));
    clcd_print("4.Start", LINE4(0));
}

void clear_screen(){
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
}

void set_time(unsigned char key)
{
    static int blink, wait, blink_pos, key_count, disp_flag;
    if(reset_flag >= 0x11)
    {
        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
        key = ALL_RELEASED; 
        key_count = 0;
        sec = 0;
        min = 0;
        wait = 0;
        blink = 0;
        blink_pos = 0;
        clcd_print("SET TIME (MM:SS)", LINE1(0));
        clcd_print("TIME- 00:00", LINE2(0));
        clcd_print("*:CLEAR  #:ENTER", LINE4(0));
    }
    if(key != ALL_RELEASED && key != '*' && key != '#')
    {
        //if(key)
        key_count++;
        if(key_count <= 2)
        {
            sec = sec * 10 + key; 
            // blink_pos = 1;
         
        }
        else if (key_count > 2 && key_count < 5) 
        {
            min = min *10 + key;
            
        }
        if(key_count < 2)
        {
            //blink_pos = 0;
        }
        else if(key_count > 2 && key_count < 5)
        {
             //blink_pos = 1;
        }
    }
    
    else if(key == '*')
    {
        if(key_count <=2)
        {
            sec = 0;
            key_count = 0;
         //blink_pos = 0;
        }
        else if (key_count > 2 && key_count < 5)
        {
            min = 0;
            key_count = 2;
          //blink_pos = 1;
        }
    }
    else if (key == '#')
    {
       clear_screen();
       TMR2ON = 1;
       FAN = ON;
       disp_flag =1;
       operation_mode = TIME_DISPLAY;
    }
    
    if (disp_flag == 0 ) {
        sec_array[0] = sec / 10 + '0';
        sec_array[1] = sec % 10 + '0';
        sec_array[2] = '\0';
        
        min_array[0] = min / 10 + '0';
        min_array[1] = min % 10 + '0';
        min_array[2] = '\0';
        
        if(wait++ == 50)
        {
            wait = 0;
            blink = !blink;
            clcd_print(min_array,LINE2(6));
            clcd_print(sec_array,LINE2(9));
        }
        if(blink)
        {   
            switch(blink_pos)
            {
                case 0:
                    clcd_print("  ", LINE2(9));
                break;
                case 1:
                        clcd_print("  ", LINE2(6));
                break;
            }
        }
    }
}
char door_status_check(void)
{
    if(DOOR == OPEN)
    {
        
        FAN = OFF;
        TMR2ON = 0;
        clear_screen();
        clcd_print("Door Status:OPEN", LINE1(2));
        clcd_print("  Please Close", LINE4(0));
        BUZZER = ON;
        while(DOOR == OPEN)
        {
            ;
        }
        clear_screen();
        TMR2ON = 1;
        BUZZER = OFF;
        FAN = ON;
       
    }
    return CLOSED;
        
       
}
void time_display(void)
{
    door_status_check();
    clcd_print(" TIME = ", LINE1(0));
    min_array[0] = min / 10 + '0';
    min_array[1] = min % 10 + '0';
    min_array[2] = '\0';
    clcd_print(min_array, LINE1(9));
    clcd_putch(':', LINE1(11));
    
    
    clcd_putch(sec / 10 + '0',LINE1(12));
    clcd_putch(sec % 10 + '0',LINE1(13));
    
    clcd_print(" 4.START/RESUME ",LINE2(0));
    clcd_print(" 5.PAUSE        ",LINE3(0));
    clcd_print(" 6.STOP         ",LINE4(0));
    
    if((min == 0) && (sec == 0))
    {
        clear_screen();
        TMR2ON = 0; 
        FAN = OFF;
        clcd_print("Cooking Time UP", LINE2(0));
        BUZZER = ON;
        __delay_ms(3000);
        BUZZER = OFF;
        
        clear_screen();
        operation_mode = COOKING_MODE_DISPLAY;
      
        
    }
    
}

char set_temp(unsigned char key)
{
    static int blink, wait, key_count, temp; 
    if(reset_flag == RESET_TEMP)
    {
        //clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
        key = ALL_RELEASED; 
        key_count = 0;
        blink = 0;
        temp = 0;
        wait = 0;
        clcd_print("SET TEMP. (  ", LINE1(0));
        clcd_putch(DEGREE, LINE1(12));
        clcd_print("C )", LINE1(13));
        clcd_print(" TEMP : 000", LINE2(0));
        clcd_print("*:CLEAR  #:ENTER", LINE4(0));
    
    }
     if(key != ALL_RELEASED && key != '*' && key != '#')
    {
        //if(key)
        key_count++;
        if(key_count <= 3)
        {
            temp = temp * 10 + key; 
         
        }
      
    }
     else if(key == '*')
    {
         key_count = 0;
         temp = 0;
     }
    else if (key == '#')
    {
       clear_screen();
       if(temp > 250)
       {
           clcd_print(" Invalid Temp.", LINE2(0));
            BUZZER = ON;
           __delay_ms(2000);
            BUZZER = OFF;
            clear_screen();
            return FAILURE;
       }
       else
       {
           clcd_print("  Pre-Heating", LINE1(0));
           sec = 50;
           TMR2ON = 1;
           clcd_print("Time.Rem = sec",LINE3(0));
           
           while(sec)
           {
               clcd_putch((sec / 100 + '0'), LINE3(10));
               clcd_putch((sec/10) % 10 + '0', LINE3(11));
               clcd_putch(sec%10 + '0', LINE3(12));
           } 
       }
       return SUCCESS;
      
    }
    
     if(wait++ == 25)
    {
        wait = 0;
        blink = !blink;
        clcd_putch((temp / 100 + '0'), LINE2(8));
        clcd_putch((temp/10) % 10 + '0', LINE2(9));
        clcd_putch(temp%10 + '0', LINE2(10));
        

    }
    if(blink)
    { 
                clcd_print("   ", LINE2(8));
    }
    return 0x11;
}
