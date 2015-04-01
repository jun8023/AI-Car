#include "all.h"
uint16  i=0,j=0;
//定义行列数
#define  X_MAX     110
#define  Y_MAX    180
//定义中线起始和终止位置
#define start   0
#define end     10
//确定跟随扫描点个数
#define scale   15
uint8 root1=0,root2=0,sensor[8]={0};
float sensor_driver=0;
uint8   date[X_MAX+5][Y_MAX+15]={0};
uint8 flag_photo=0,read_date=0,temp_l=0,temp_r=0,time=0,ave_now=0;
uint8 center0,line_left=0,line_right=0;
int16 temp,temp_2;
uint16 left=0,right=0,center[X_MAX],left_flag=0,right_flag=0,left_last,right_last;
uint8 white=120,dark=100,dif=0;
float test=0;
void main()
 {
     SYS_CLOCK_SET(SYS_CLOCK_150M,1,2,3,6);
     DisableInterrupts
         //VSYNC中断
         GPIO_INIT(VSYNC_PORT,VSYNC_PIN,INPUT);
     GPIOPULL_SET(VSYNC_PORT,VSYNC_PIN,PULL_UP);
     Enable_IRQ(INT_PORTC);
     GPIOINT_Enable(VSYNC_PORT,VSYNC_PIN,LEVEL_FALLING);
     
     //HREF中断
     GPIO_INIT(HREF_PORT,HREF_PIN,INPUT);
     GPIOPULL_SET(HREF_PORT,HREF_PIN,PULL_DOWN);
     
     //PCLK中断
     GPIO_INIT(PCLK_PORT,PCKL_PIN,INPUT);
     GPIOPULL_SET(PCLK_PORT,PCKL_PIN,PULL_DOWN);
     GPIOINT_Enable(PCLK_PORT,PCKL_PIN,DMA_RISING);
     
     //DMA初始化
     DMA_Init_new(DMAMUX0, DMA_CH0,Src_PORTC,Y_MAX+15,1,DMA_Normal_Mode);
     DMA_Source_new(DMA_CH0, 0X400FF0D0,0, DMA_8BIT, 0);
     DMA_Destination_new(DMA_CH0, date,1, DMA_8BIT, 0);
     //DMA_Int_Enable(DMA_CH0,DMA_MAJOR);
     //设置输入端口
     GPIO_INIT(PORTD,0,INPUT);
     GPIO_INIT(PORTD,1,INPUT);
     GPIO_INIT(PORTD,2,INPUT);
     GPIO_INIT(PORTD,3,INPUT);
     GPIO_INIT(PORTD,4,INPUT);
     GPIO_INIT(PORTD,5,INPUT);  
     GPIO_INIT(PORTD,6,INPUT);
     GPIO_INIT(PORTD,7,INPUT);
     GPIOPULL_SET(PORTD,0,PULL_DOWN);
     GPIOPULL_SET(PORTD,1,PULL_DOWN);
     GPIOPULL_SET(PORTD,2,PULL_DOWN);
     GPIOPULL_SET(PORTD,3,PULL_DOWN);
     GPIOPULL_SET(PORTD,4,PULL_DOWN);
     GPIOPULL_SET(PORTD,5,PULL_DOWN);
     GPIOPULL_SET(PORTD,6,PULL_DOWN);
     GPIOPULL_SET(PORTD,7,PULL_DOWN);
     
     //UART
    GPIOType_UART(PORTB_16_17);
    UART_INIT(UART0,75000,115200);
    UART_FIFO_read_init(UART0);
    Enable_IRQ(61);
    UARTINT_Enable(UART0,RIE);
    
    //SCCB_INIT
     SCCB_init();
     //while(!SCCB_Catch());
         
     //舵机初始化
     servo_driver_init();
     
     //电机初始化
     motor_PWM_init();

     //优先级设置
     SET_IRQ_PRIOR(INT_PORTC,0);
     SET_IRQ_PRIOR(61,2);
     
     EnableInterrupts
         while(1)
          {
              for(test=0;test<1000;test++)
               {
                   send_line2(UART0,1,test);
                   send_line2(UART0,2,1000-test);
                   //send_line2(UART0,2,500-test);
               }
              if(flag_photo)
               {
                   flag_photo=0;
#if     0           //边缘显示算子
                   for(i=0;i<X_MAX+5;i++)
                       for(j=0;j<Y_MAX;j++)
                        {
                            if(date[i][j]>date[i][j+1])
                                date[i][j]=10*(date[i][j]-date[i][j+1]);
                            else date[i][j]=10*(date[i][j+1]-date[i][j]);
                        } 
#endif
                   
#if     1          //二值化
                   dif=white-dark;
                   for(i=0;i<X_MAX;i++)
                       for(j=0;j<Y_MAX;j++)
                        {
                            if(date[i][j]>=white-dif*2/3)
                                date[i][j]=254;
                            if(date[i][j]<=dark+dif*2/3)
                                date[i][j]=1;
                        }
#endif
                   
                   
                   
#if     1       //中线提取——二值化之前——全行扫描
                   for(i=0;i<X_MAX;i++)
                    {
                        left=3;right=Y_MAX-3;
                        for(j=3;j<Y_MAX-3;j++)
                         {
                             if((date[i][j+1]>date[i][j])&&(date[i][j+2]>date[i][j-1]))
                              {
                                  left=j;
                                  left_flag=1;//表示采集到左边缘
                                  break;
                              }
                             left_flag=0;
                         }
                        for(j=Y_MAX-3;j>3;j--)
                         {
                             if((date[i][j-1]>date[i][j])&&(date[i][j-2]>date[i][j+1]))
                              {
                                  right=j;
                                  right_flag=1;//采集到右边缘
                                  break;
                              }
                             right_flag=0;
                         }
                        if(left_flag&right_flag)
                         {
                             center[i]=(left+right)/2;left_last=left;right_last=right;
                         }
                        else if((left_flag==1)&&(right_flag==0))
                         {
                             center[i]=center[i-1]+left_last-left;left_last=left;
                         }
                        else if((left_flag==0)&&(right_flag==1))
                         {
                             center[i]=center[i-1]+right_last-right;right_last=right;
                         }
                        else center[i]=center[i-1];
                        date[i][center[i]]=0;
                    }
#endif
                   
#if     0         //中线提取—————跟随扫描
                   for(i=start;i<end;i++)
                    {
                        left=0;right=Y_MAX;
                        for(j=3;j<Y_MAX-3;j++)
                         {
                             if((date[i][j+1]>date[i][j])&&(date[i][j+2]>date[i][j-1]))
                              {
                                  left=j;
                                  left_flag=1;//表示采集到左边缘
                                  break;
                              }
                             left_flag=0;
                         }
                        for(j=Y_MAX-3;j>3;j--)
                         {
                             if((date[i][j-1]>date[i][j])&&(date[i][j-2]>date[i][j+1]))
                              {
                                  right=j;
                                  right_flag=1;//采集到右边缘
                                  break;
                              }
                             right_flag=0;
                         }
                        if(left_flag&right_flag)
                         {
                             center[i]=(left+right)/2;left_last=left;right_last=right;
                         }
                        else if((left_flag==1)&&(right_flag==0))
                         {
                             center[i]=center[i-1]+left_last-left;left_last=left;
                         }
                        else if((left_flag==0)&&(right_flag==1))
                         {
                             center[i]=center[i-1]+right_last-right;right_last=right;
                         }
                        else center[i]=center[i-1];
                        date[i][center[i]]=0;
                    }
                   root1=left;root2=right;
                   for(i=end;i<X_MAX;i++)
                    {
                        left=0;right=Y_MAX;
                        for(j=root1-scale;j<root1+scale;j++)
                         {
                             if((date[i][j+1]>date[i][j])&&(date[i][j+2]>date[i][j-1]))
                              {
                                  left=j;
                                  left_flag=1;//表示采集到左边缘
                                  break;
                              }
                             left_flag=0;
                         }
                        for(j=root2+scale;j<root2-scale;j--)
                         {
                             if((date[i][j-1]>date[i][j])&&(date[i][j-2]>date[i][j+1]))
                              {
                                  right=j;
                                  right_flag=1;//采集到右边缘
                                  break;
                              }
                             right_flag=0;
                         }
                        if(left_flag&right_flag)
                         {
                             center[i]=(left+right)/2;left_last=left;right_last=right;
                         }
                        else if((left_flag==1)&&(right_flag==0))
                         {
                             center[i]=center[i-1]+left_last-left;left_last=left;
                         }
                        else if((left_flag==0)&&(right_flag==1))
                         {
                             center[i]=center[i-1]+right_last-right;right_last=right;
                         }
                        else center[i]=center[i-1];
                        date[i][center[i]]=0;
                    }
#endif                 
                   
                   //UART_S1(UART0,0XFF);
                   UART_SS(UART0,"image");
                   for(i=X_MAX;i>0;i--)
                       for(j=Y_MAX;j>0;j--)
                           UART_S1(UART0,date[i][j]);
                   i=0;j=0;
                   EnableInterrupts
                       GPIOINT_Clear(PORTC);
                   Enable_IRQ(INT_PORTC);
                   GPIOINT_Enable(VSYNC_PORT,VSYNC_PIN,LEVEL_RISING);
               }
          }
 }

void PORTC_IRQHandler(void)
 {
     uint32 port_date=PORTC_ISFR;
     if(port_date&0X8000)
      {
          PORTC_ISFR|=0X8000;
#if 0//隔行扫描
          if(i%2==0)
           {
               DMA_Enable_new(DMAMUX0,DMA_CH0);
               if(i>=X_MAX*2)
                {
                    flag_photo=1;
                    Disable_IRQ(INT_PORTC);
                    DisableInterrupts;
                    GPIOINT_Disable(HREF_PORT,HREF_PIN);
                    DMA_Disable_new(DMAMUX0,DMA_CH0);
                    DMA_SetDestAddress(DMA_CH0, date);       
                }
           }le_new(DMAMUX0,DMA_CH0);
#endif
          if(i>=X_MAX)
           {
               flag_photo=1;
               Disable_IRQ(INT_PORTC);
               DisableInterrupts;
               GPIOINT_Disable(HREF_PORT,HREF_PIN);
               DMA_Disable_new(DMAMUX0,DMA_CH0);
               DMA_SetDestAddress(DMA_CH0, date);       
           }
          i++;
      }
              if(port_date&0X20000)
          {
              PORTC_ISFR|=0X20000;
              GPIOINT_Disable(VSYNC_PORT,VSYNC_PIN);
              GPIOINT_Enable(HREF_PORT,HREF_PIN,LEVEL_RISING);
          }
 }

void DMA0_DMA16_IRQHandler(void)
 {
     DMA_Clear_Int(DMA_CH0);
 }
void UART0_RX_TX_IRQHandler(void)
 {
     UART_FIFO_Read_value(UART0,sensor);
     read_pose(UART0,sensor);
 }