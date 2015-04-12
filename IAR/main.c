#include "all.h"
//定义行列数

uint8 buff[512] = {0x5A};
uint8 buffout[512] = {0};
union {
	uint8 date1[X_MAX][Y_MAX];
	uint8 img[50][512];
}DAT;
uint8 sensor[8];
uint8  i=0,j=0,k=0,t=0;
uint8 flag_photo=0;

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
     while(!SCCB_Catch());
     
     //舵机初始化
     servo_driver_init();
     
     //电机初始化
     motor_PWM_init();
     //SD初始化
    GPIO_INIT(RST_PORT,RST_PIN,1);
    GPIO_INIT(DC_PORT,DC_PIN,1);
    GPIO_INIT(CS_PORT,CS_PIN,1);
    GPIO_DSE(CS_PORT,CS_PIN);
//	SPILCD_CS_SET;
    GPIOType_SPI(SPI2,SPI2_PCS0_PTB20,SPI2_SCK_PTB21,SPI2_SOUT_PTB22,SPI2_SIN_PTB23);
    SPI_init(SPI2,MASTER);
    //PIT
     PIT_CLOCK_Enable
    //PIT_INIT(PIT0,150000000,DEBUG_STOP);
    PIT_INIT(PIT0,150000000,DEBUG_STOP);
    PIT_Enable(PIT0);
    uint32 time1=0,time2,time;
     //优先级设置
     SET_IRQ_PRIOR(INT_PORTC,0);
     SET_IRQ_PRIOR(61,5);
     //LED初始化
     led_init();
     EnableInterrupts
     while(1)
      {
          if(flag_photo)
           {
               flag_photo=0;
               CAM_get();//图像处理以及中线提取，外部变量
               input=(center[4][1]+center[6][1]+center[8][1]+center[10][1]+center[12][1])/5;
               input_error=input-190/2;
               output=p*input_error;
               servo_set(500+output);
               send_flot(UART0,input);
               /*for(i=0;i<dot_num;i++)
						 {
                        date[center[i][0]][center[i][0]]=0;
						}
                 UART_SS(UART0,"image");
                   for(i=0;i<X_MAX;i++)
                       for(j=0;j<Y_MAX;j++)
                        {if(date[i][j]==0)
                            {
                                date[i][j]=1;
                            }
                           UART_S1(UART0,date[i][j]);
                        }
          /*     SD_Init();
          for (i=0;i<512;i++)
              buff[i]=0x5A;
          for (i=0;i<X_MAX;i++)
              for (j=0;j<Y_MAX;j++)
                  DAT.date1[i][j]=date[i][j];
          SD_WriteMultiSectors(buff,800+n*100,1);
          for (i=0;i<50;i++)
          SD_WriteMultiSectors(DAT.img[0],801+n*100,50);
          n++;
          if(n==100) 
           {
               n=1;
           }*/
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
          if((i%2==0)&&(i>5))
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
           }
#endif
#if 1  //逐行扫描
          if(i>5)
          DMA_Enable_new(DMAMUX0,DMA_CH0);
#endif
          if(i>=X_MAX+5)
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