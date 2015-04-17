#include "all.h"



void display(uint8 x,uint8 y,int32 num)
 {
     if(num<0)
      {
          LCD_6x8char(x-1,y,'-');
          num=num*(-1);
      }
     else LCD_6x8char(x-1,y,' ');
     LCD_6x8char(x,y,num/10000+48);
     LCD_6x8char(x+1,y,num%10000/1000+48);
     LCD_6x8char(x+2,y,num%1000/100+48);
     LCD_6x8char(x+3,y,'.');
     LCD_6x8char(x+4,y,num%100/10+48);
     LCD_6x8char(x+5,y,num%10+48);
 }
void led_init()
 {
     GPIO_INIT(PORTA,7,OUTPUT);
     GPIO_INIT(PORTA,11,OUTPUT);
     GPIO_INIT(PORTA,12,OUTPUT);
     GPIO_INIT(PORTA,17,OUTPUT);
     GPIO_SET(PORTA,7,1);
     GPIO_SET(PORTA,11,1);
     GPIO_SET(PORTA,12,1);
     GPIO_SET(PORTA,17,1);
 }
void LED_ON(uint8 led_num,uint8 date)
 {
     GPIO_SET(PORTA,led_num,date);
 }
void motor_PWM_init()
 {
     FTM_PWM_Init(EPWM_MODE, FTM3_CH4_PTC8, 50, 0);
     FTM_PWM_Init(EPWM_MODE, FTM3_CH5_PTC9, 50, 0);
     FTM_PWM_Init(EPWM_MODE, FTM3_CH6_PTC10, 50, 0);
     FTM_PWM_Init(EPWM_MODE, FTM3_CH7_PTC11, 50, 0);
 }
void motor_dutyset(float pwm1_duty,float pwm2_duty,float pwm3_duty,float pwm4_duty)
 {
      FTM_PWM_Set_Duty(FTM3,FTM_CH4,pwm1_duty/100);
      FTM_PWM_Set_Duty(FTM3,FTM_CH5,pwm2_duty/100);
      FTM_PWM_Set_Duty(FTM3,FTM_CH6,pwm3_duty/100);
      FTM_PWM_Set_Duty(FTM3,FTM_CH7,pwm4_duty/100);
 }
//×îµÍ6%£¬×î¸ß8.5%
void servo_driver_init()
 {
     FTM_PWM_Init(EPWM_MODE, FTM2_CH0_PTA10, 50, 0.072935);
 }
void servo_set(float duty)
 {
     duty=(duty-500)*(8.5-6)/1000+7.2935;
     if(duty<6) duty=6;
     if(duty>8.5) duty=8.5;
     FTM_PWM_Set_Duty(FTM2, FTM_CH0,(duty)/100);
 }
void translate_uart(short acc)
 {
     if(acc<0)
      {
          UART_S1(UART0,'-');
          acc=acc*(-1);
      }
     UART_S1(UART0,(uint8)(acc/10000)+48);
     UART_S1(UART0,(uint8)(acc%10000/1000)+48);
     UART_S1(UART0,(uint8)(acc%1000/100)+48);
     UART_S1(UART0,'.');
     UART_S1(UART0,(uint8)(acc%100/10)+48);
     UART_S1(UART0,(uint8)(acc%10)+48);
     UART_S1(UART0,' ');
 }
union 
 {
     uint8 a[4];
     float val;
 }xg;
union
 {
     uint8 a[4];
     uint32 val;
 }xi;
union 
 {
     uint8 a[4];
     int32 val;
 }xh;

void send_flot(uint32 UART_NUM,float val)
 {
     xg.val=val;
     UART_SS(UART_NUM,"flot");
     UART_S1(UART_NUM,xg.a[3]);
     UART_S1(UART_NUM,xg.a[2]);
     UART_S1(UART_NUM,xg.a[1]);
     UART_S1(UART_NUM,xg.a[0]);
 }
void send_int32(uint32 UART_NUM,int val)
 {
     xh.val=val;
     UART_SS(UART_NUM,"in32");
     UART_S1(UART_NUM,xh.a[3]);
     UART_S1(UART_NUM,xh.a[2]);
     UART_S1(UART_NUM,xh.a[1]);
     UART_S1(UART_NUM,xh.a[0]);
 }
void send_line(uint32 UART_NUM,char line_num,float val)
 {
     xg.val=val;
     UART_SS(UART_NUM,"lin");
     UART_S1(UART_NUM,line_num+48);
     UART_S1(UART_NUM,xg.a[3]);
     UART_S1(UART_NUM,xg.a[2]);
     UART_S1(UART_NUM,xg.a[1]);
     UART_S1(UART_NUM,xg.a[0]);
 }
void read_pose(uint32 UART_NUM,char* data)
 {
     float driver;
     if((data[0]==8)&&(data[1]==0)&&(data[2]==2)&&(data[3]==3))
     {
          xg.a[0]=data[7];
          xg.a[1]=data[6];
          xg.a[2]=data[5];
          xg.a[3]=data[4];
          driver=(float)xg.val;
          servo_set(driver);
      }
     if((data[0]==8)&&(data[1]==1)&&(data[2]==2)&&(data[3]==3))
      {
          xg.a[0]=data[7];
          xg.a[1]=data[6];
          xg.a[2]=data[5];
          xg.a[3]=data[4];
          driver=(float)xg.val;
          motor_dutyset(0,driver,0,driver);
      }
 }
void change_par(char* data,float *date)
 {
      if((data[0]==8)&&(data[1]==2)&&(data[2]==2)&&(data[3]==3))
      {
          xg.a[0]=data[7];
          xg.a[1]=data[6];
          xg.a[2]=data[5];
          xg.a[3]=data[4];
          *date=(float)xg.val;
      }
     if((data[0]==8)&&(data[1]==3)&&(data[2]==2)&&(data[3]==3))
      {
          xg.a[0]=data[7];
          xg.a[1]=data[6];
          xg.a[2]=data[5];
          xg.a[3]=data[4];
          *date=(float)xg.val;
      }
     if((data[0]==8)&&(data[1]==4)&&(data[2]==2)&&(data[3]==3))
      {
          xg.a[0]=data[7];
          xg.a[1]=data[6];
          xg.a[2]=data[5];
          xg.a[3]=data[4];
          *date=(float)xg.val;
      }
 }


//********************************************************************************************