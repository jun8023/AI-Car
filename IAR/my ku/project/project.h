#ifndef __PROJECT_H__
#define __PROJECT_H__

#define led_on  0
#define led_off 1
#define led0    7
#define led1    11
#define led2    12
#define led3    17
void display(uint8 x,uint8 y,int32 num);
void led_init();
void LED_ON(uint8 led_num,uint8 date);
void motor_PWM_init();
void motor_dutyset(float pwm1_duty,float pwm2_duty,float pwm3_duty,float pwm4_duty);
void servo_driver_init();
void servo_set(float duty);
void translate_uart(short acc);
void send_flot(uint32 UART_NUM,float val);
void send_int32(uint32 UART_NUM,int val);
void send_line(uint32 UART_NUM,char line_num,float val);
void read_pose(uint32 UART_NUM,char* data);
void change_par(char* data,float *date);










#endif