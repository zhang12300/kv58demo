/******************************************************************************/
/*******************************************************************************
  文件名：摄像头寄存器设置文件CameraSet.c
  功  能：摄像头特性控制
  日  期：2014.10.09
  作  者：HJZ
  备  注：
*******************************************************************************/
/******************************************************************************/
//头文件
#include "CameraSet.h"


//全局变量声明
volatile ImgStatusDef ImgStatus = ImgGetFinish;   //当前图像状态标志标志位
//uint8 ImgRaw[CameraHight][CameraWidth];           //原始数据储存数组
uint8 ImgStore1[CameraHight][CameraWidth];                    //图像数据储存数组1
uint8 ImgStore2[CameraHight][CameraWidth];                    //图像数据储存数组2

//uint8 ImgSend[CameraWidth * CameraHight];      //要传送到上位机的数组，由于鹰眼摄像头是一字节8个像素，因而需要解压为 1字节1个像素，方便处理

//OV7725寄存器专用初始值，全局变量
CameraRegs OV7725Regs[] =
{
    //寄存器地址，寄存器值
    {OV7725_COM4         , 0xC1},//01
    {OV7725_CLKRC        , 0x00},//02
    {OV7725_COM2         , 0x03},//03
    {OV7725_COM3         , 0xD0},//04
    {OV7725_COM7         , 0x40},//05
    {OV7725_HSTART       , 0x3F},//06
    {OV7725_HSIZE        , 0x50},//07
    {OV7725_VSTRT        , 0x03},//08
    {OV7725_VSIZE        , 0x78},//09
    {OV7725_HREF         , 0x00},//10
    {OV7725_SCAL0        , 0x0A},//11
    {OV7725_AWB_Ctrl0    , 0xE0},//12
    {OV7725_DSPAuto      , 0xff},//13
    {OV7725_DSP_Ctrl2    , 0x0C},//14
    {OV7725_DSP_Ctrl3    , 0x00},//15
    {OV7725_DSP_Ctrl4    , 0x00},//16

#if (CameraWidth == 80)             //17
    {OV7725_HOutSize     , 0x14},   //共10位，此为高8位，低2位在OV7725_EXHCH中，为00。80，即0x50,右移2位，即0x14
#elif (CameraWidth == 160)
    {OV7725_HOutSize     , 0x28},
#elif (CameraWidth == 240)
    {OV7725_HOutSize     , 0x3c},
#elif (CameraWidth == 320)
    {OV7725_HOutSize     , 0x50},
#else

#endif

#if (CameraHight == 60 )             //18
    {OV7725_VOutSize     , 0x1E},   //共9位，此为高8位，低1位在OV7725_EXHCH中，为0。 60，即0x3C,右移1位，即0x1E
#elif (CameraHight == 120 )
    {OV7725_VOutSize     , 0x3c},
#elif (CameraHight == 180 )
    {OV7725_VOutSize     , 0x5a},
#elif (CameraHight == 240 )
    {OV7725_VOutSize     , 0x78},
#else

#endif

    {OV7725_EXHCH        , 0x00},//19
    {OV7725_GAM1         , 0x0c},//20
    {OV7725_GAM2         , 0x16},//21
    {OV7725_GAM3         , 0x2a},//22
    {OV7725_GAM4         , 0x4e},//23
    {OV7725_GAM5         , 0x61},//24
    {OV7725_GAM6         , 0x6f},//25
    {OV7725_GAM7         , 0x7b},//26
    {OV7725_GAM8         , 0x86},//27
    {OV7725_GAM9         , 0x8e},//28
    {OV7725_GAM10        , 0x97},//29
    {OV7725_GAM11        , 0xa4},//30
    {OV7725_GAM12        , 0xaf},//31
    {OV7725_GAM13        , 0xc5},//32
    {OV7725_GAM14        , 0xd7},//33
    {OV7725_GAM15        , 0xe8},//34
    {OV7725_SLOP         , 0x20},//35
    {OV7725_LC_RADI      , 0x00},//36
    {OV7725_LC_COEF      , 0x13},//37
    {OV7725_LC_XC        , 0x08},//38
    {OV7725_LC_COEFB     , 0x14},//39
    {OV7725_LC_COEFR     , 0x17},//40
    {OV7725_LC_CTR       , 0x05},//41
    {OV7725_BDBase       , 0x99},//42
    {OV7725_BDMStep      , 0x03},//43
    {OV7725_SDE          , 0x04},//44
    {OV7725_BRIGHT       , 0x00},//45
    {OV7725_CNST         , 0xFF},//46
    {OV7725_SIGN         , 0x06},//47
    {OV7725_UVADJ0       , 0x11},//48
    {OV7725_UVADJ1       , 0x02},//49

};

//OV7620寄存器专用初始值，全局变量
CameraRegs OV7620Regs[] =
{
      //寄存器地址，寄存器值
    {OV7725_COM4         , 0xC1},//01
    {OV7725_CLKRC        , 0x00},//02
};





//函数

//CameraDelay函数
//5步 * 30000 * 1 = 150000 次运算，也就是1ms，不一定准。
void CameraDelay(uint32 time)
{
  volatile uint32 timecount = time * 30000;
  while(timecount--)
  {
    asm("nop");  
    asm("nop");  
    asm("nop");  
    asm("nop");  
  }
}


//摄像头所需端口配置
void CameraPortInit(void)
{
    uint8 i;                                  
    uint8 IMAGE_PIN_IN[8] = {0,1,2,3,4,5,6,7};    
    PORT_MemMapPtr p = PORT_IMAGE_DATA_BASE_PTR;
    
    //图像数据输入口初始化， PTB0~PTB7均设置为摄像头数据输入，下拉电阻 
    //SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
    for(i = 0;i < 8;i++)
    {
        //相应的端口引脚初始化,不用输入，
        //gpio_init(PORT_IMAGE_DATA,IMAGE_PIN_IN[i],0,0);
        //设置端口为GPIO模式，默认上拉电阻
        PORT_PCR_REG(p,IMAGE_PIN_IN[i]) |= PORT_PCR_MUX(0x01) | PORT_PCR_PE_MASK;
        //设置为下拉电阻
        PORT_PCR_REG(p,IMAGE_PIN_IN[i]) &= ~PORT_PCR_PS_MASK;
        
    }
    
    
    //场中断输入引脚初始化, 下降沿触发中断，暂定为PTD1
    //无上下拉电阻，无滤波
    PORT_PCR_REG(PORT_FIELD_BASE_PTR, PIN_FIELD) |= (
                                                    PORT_PCR_MUX(0x01)    //GPIO
                                               //   | PORT_PCR_PE_MASK    //上拉电阻
                                                  | PORT_PCR_IRQC(0xA)   //下降沿触发中断
                                               //   | PORT_PCR_PFE_MASK   //滤波
                                                    );    
    //行中断输入引脚初始化, 上升沿触发中断，暂定为PTC8
    //无上下拉电阻，无滤波
    PORT_PCR_REG(PORT_LINE_BASE_PTR, PIN_LINE) |= (
                                                    PORT_PCR_MUX(0x01)    //GPIO
                                               //   | PORT_PCR_PE_MASK    //上拉电阻
                                                  | PORT_PCR_IRQC(0x9)    //上升沿触发中断
                                               //   | PORT_PCR_PFE_MASK   //滤波
                                                    );   
    
    //PCLK输入引脚初始化，上升沿触发DMA， 暂定为PTB9
    //无上下拉电阻，无滤波
    PORT_PCR_REG(PORT_PCLK_BASE_PTR, PIN_PCLK) |= (
                                                    PORT_PCR_MUX(0x01) //GPIO
                                                //  | PORT_PCR_PE_MASK   //上拉电阻
                                                  | PORT_PCR_IRQC(0x1) //上升沿触发DMA
                                                   );         
}


/*
//OV7725摄像头寄存器初始化 成功返回1 失败返回0
uint8 CameraRegInit(void)
{
    uint8 i;
    uint8 SensorID = 1;
    uint8 Send_Tmp[1] = {"1"};
    
    //SCCB信号引脚配置
    SCCBPortInit();
    
    //复位摄像头所有寄存器
    if(SCCBWriteByte(OV7725_COM7, 0x80) == 0)
    {
        uart_sendN(UART0, (uint8 *)"\nCamera Registers Reset Failed!", 31);
        return 0;
    }
    
    //延时
    CameraDelay(50);
    
    
    
    //读取摄像头芯片的ID号
    if(SCCBReadByte(&SensorID, 1, OV7725_VER) == 0)
    {
        uart_sendN(UART0, (uint8 *)"\nCamera Registers ReadID Failed!", 32);
        return 0;
    }
    
    Send_Tmp[0] = SensorID;
    
    //获取摄像头芯片ID号成功
    uart_sendN(UART0, (uint8 *)"\nCamera Registers ReadID Succeed: ", 34);
    uart_send1(UART0, '\n');
    uart_sendN(UART0, Send_Tmp, sizeof(Send_Tmp));
    
    
    
    //配置各个寄存器
    if(SensorID == OV7725_ID) 
    {
        for( i = 0 ; i < 49 ; i++ )
        {
            if( 0 == SCCBWriteByte(OV7725Regs[i].addr, OV7725Regs[i].val) )
            {
                uart_sendN(UART0, (uint8 *)"\nCamera Writing a Reg Failed!", 29);
                return 0;
            }
        }
    }
    else
    {
        uart_sendN(UART0, (uint8 *)"\nSensor ID Unmatch!", 19);
        return 0;
    }
    
    //配置Camera各寄存器均成功
    uart_sendN(UART0, (uint8 *)"\nCamera Registers Initial Finally Succeed!", 42);
    return 1;
}

*/



//OV7620摄像头寄存器初始化 成功返回1 失败返回0
uint8 CameraRegInit(void)
{
    //uint8 i;
    //uint8 SensorID = 1;
    //uint8 Send_Tmp[1] = {"1"};
    
    //SCCB信号引脚配置
    SCCBPortInit();

    //复位摄像头所有寄存器
    if(SCCBWriteByte(OV7620_COMCTRLA, 0x80) == 0)
    {
        uart_sendN(UART0, (uint8 *)"\nCamera Registers Reset Failed!", 31);
        return 0;
    }
    
    //延时
    CameraDelay(50);    
    
    //PCLK四分频，HSYNC无效，CHSYNC无效，VSYNC有效边沿。
    //PCLK:166ns   HREF:254.6us   VSYN:133.6ms
    /*
    if( 0 == SCCBWriteByte(OV7620_CLKCTRL,0x00))
    {
        uart_sendN(UART0, (uint8 *)"\nCamera Writing OV7620_CLKCTRL Failed!", 38);
        return 0;
    }
    */
    /*
    if( 0 == SCCBWriteByte(OV7620_HS,0x3F))
    {
        uart_sendN(UART0, (uint8 *)"\nCamera Writing OV7620_COMCTRLC Failed!", 39);
        return 0;
    }
    
    if( 0 == SCCBWriteByte(OV7620_HE,0xBF))
    {
        uart_sendN(UART0, (uint8 *)"\nCamera Writing OV7620_COMCTRLC Failed!", 39);
        return 0;
    }    
    */
    //AWB激活阈值选择为 低， QVGA输出选择320*240，VSYNC在奇偶场均输出，HREF正极性，RGB伽马校正开启。
    //PCLK:332ns   HREF:509.6us   VSYN:133.6ms
    if( 0 == SCCBWriteByte(OV7620_COMCTRLC,0x24))
    {
        uart_sendN(UART0, (uint8 *)"\nCamera Writing OV7620_COMCTRLC Failed!", 39);
        return 0;
    }
    
    /*
    //连续采集模式(320*240)
    //PCLK:666ns   HREF:509.6us   VSYN:133.6ms
    if( 0 == SCCBWriteByte(OV7620_COMCTRLH,0x00))
    {
        uart_sendN(UART0, (uint8 *)"\nCamera Writing OV7620_COMCTRLH Failed!", 39);
        return 0;
    }
    */

    
    //配置Camera各寄存器均成功
    uart_sendN(UART0, (uint8 *)"\nCamera Registers Initial Finally Succeed12!", 42);
    return 1;
}





//摄像头初始化函数 成功返回1 失败返回0
uint8 CameraInit(void)
{
  
    if(!CameraRegInit()) return 0; //摄像头寄存器初始化若失败，直接返回0
    CameraPortInit();              //摄像头所需端口配置
    return 1;
}

