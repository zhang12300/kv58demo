/******************************************************************************/
/*******************************************************************************
  文件名：调试文件Debug.c
  功  能：OLED显示，FLASH参数读写。
  日  期：2015.03.13
  作  者：HJZ
  备  注：
*******************************************************************************/
/******************************************************************************/

#include "Debug.h"





//拨码开关。
//拨码开关确认计数,初始化为0.
uint8 DebugSwitchConfirmCount[SwitchNum] = 
{
  0, 0, 0, 0  
};

//拨码开关标志。1某个方向，0另一个方向。1个16ms周期内即可被确认。
uint8 DebugSwitchFlag[SwitchNum] = 
{
  0, 0, 0, 0
};


//按键。
//按键确认计数,初始化为0。
uint8 DebugButtonConfirmCount[ButtonNum] = 
{
  0, 0, 0, 0, 0, 0  
};
//按键标志。1表示连续DebugButtonConfirmNum个16ms周期内被按下，0表示没有。
uint8 DebugButtonFlag[ButtonNum] = 
{
  0, 0, 0, 0, 0, 0
};

/*
//需要修改的14个值。
uint16 DebugServoPIDPMat00 = 167;
uint16 DebugServoPIDPMat01 = 195;
uint16 DebugServoPIDPMat02 = 214;
uint16 DebugServoPIDPMat03 = 225;
uint16 DebugServoPIDPMat04 = 231;
uint16 DebugServoPIDPMat05 = 234;
uint16 DebugServoPIDPMat06 = 255;
uint16 DebugServoPIDPMat07 = 235;
uint16 DebugServoPIDPMat08 = 230;
uint16 DebugServoPIDPMat09 = 217;
//3个关键值的上下界
uint8 DebugWMin10 = 160;
uint8 DebugWMax11 = 190;
uint8 DebugBMin12 = 85;
uint8 DebugBMax13 = 140;
uint8 DebugWBMin = 70;
uint8 DebugWBMax = 100;
*/

//需要修改的60个值。每页15个。不需要赋初值，初始化时会从FLASH中读取出来。
uint16 DebugModifyNum[60];

uint8 DebugOLEDUpdateCount = 0;       //OLED的刷新频率控制，DebugOLEDUpdateCountNum个16ms刷新一次。

uint8 DebugScreenChosen = DebugScreenChosenDisplay;  //界面选择标志，分显示/调参。
uint8 DebugDisplayPage = 0;                          //显示界面，页码。0~DebugDisplayPageNum-1
uint8 DebugModifyPage = 0;                           //调参界面，页码。0,1,2.3
uint8 DebugModifyCursor = 0;                         //调参界面，光标。0~14.
uint8 DebugModifyCursorPre = 0;                      //调参界面，上一次的光标值，0~14.
uint8 DebugModifyFlag = 0;                           //调参界面，修改标志。0表示当前为上下移模式，1表示当前为加减值模式。

//定时跑
uint16 TimeRealRun = 100;                         //定时跑时，时间上限。以0.1s为单位。
uint8 TimeRealRunFlag = 0;                  //定时跑标志。0未跑完，1已跑完。

//Debug用的引脚的初始化。
uint8 DebugPortInit(void)
{
  //4个拨码开关IO口初始化
  
   GPIO_Init(PORTC, 6, 0, 1);                                  
   GPIO_Init(PORTC, 7, 0, 1);
   GPIO_Init(PORTC, 8, 0, 1);
   GPIO_Init(PORTC, 9, 0, 1);
  
  //6个按键的初始化

   GPIO_Init(PORTB, 20, 0, 1); //PTE0,按键0,初始值置1。up上。                                 
   GPIO_Init(PORTB, 21, 0, 1); //PTE1,按键1,初始值置1。down下。
   GPIO_Init(PORTB, 22, 0, 1); //PTE2,按键2,初始值置1。ok确认。
   GPIO_Init(PORTE, 13, 0, 1);  //PTE3,按键3,初始值置1。esc取消。
//  gpio_init(PORTE,0,0,1);  //PTE0,按键0,初始值置1。up上。
//  gpio_init(PORTE,1,0,1);  //PTE1,按键1,初始值置1。down下。
//  gpio_init(PORTE,2,0,1);  //PTE2,按键2,初始值置1。ok确认。
//  gpio_init(PORTE,3,0,1);  //PTE3,按键3,初始值置1。esc取消。
//  gpio_init(PORTE,4,0,1);  //PTE4,按键4,初始值置1
//  gpio_init(PORTE,5,0,1);  //PTE5,按键5,初始值置1
  
  return 1;
}


//Debug专用延时函数。
void DebugDelay(int ms)
{
   int ii,jj;
   if (ms<1) ms=1;
   for(ii=0;ii<ms;ii++)     
     for(jj=0;jj<5341;jj++);     
}


//Debug拨码开关扫描函数。
uint8 DebugSwitchScan(void)
{
  //拨码开关0. 
  if(!(GPIOE_PDIR&0x0001))
  {
    if(!(GPIOE_PDIR&0x0001))
    {    
      DebugSwitchFlag[0] = 1;
    }
    else
    {
      DebugSwitchFlag[0] = 0;
    }
  }
  else
  {
    DebugSwitchFlag[0] = 0;
  }
  
  //拨码开关1. 
  if(!(GPIOE_PDIR&0x0002))
  {
    if(!(GPIOE_PDIR&0x0002))
    {
      DebugSwitchFlag[1] = 1;
    }
    else
    {
      DebugSwitchFlag[1] = 0;
    }
  }
  else
  {
    DebugSwitchFlag[1] = 0;
  }
  
  //拨码开关2.
  if(!(GPIOE_PDIR&0x0004))
  {
    if(!(GPIOE_PDIR&0x0004))
    {
      DebugSwitchFlag[2] = 1;
    }
    else
    {
      DebugSwitchFlag[2] = 0;
    }  
  }
  else
  {
    DebugSwitchFlag[2] = 0;
  }  
  
  //拨码开关3. 
  if(!(GPIOE_PDIR&0x0008))
  {
    if(!(GPIOE_PDIR&0x0008))
    {
      DebugSwitchFlag[3] = 1;
    }
    else
    {
      DebugSwitchFlag[3] = 0;
    }
  }
  else
  {
    DebugSwitchFlag[3] = 0;
  }    
    
  //拨码开关不存在检测成不成功，扫描一遍更新标志位即可。
  return 1;
}

//Debug按键扫描函数。
uint8 DebugButtonScan(void)
{    
  
  //按键0.PTE0.
  if(!(GPIOE_PDIR&0x01))
  {
    //确认次数加1.
    DebugButtonConfirmCount[0]++;
    //连续多个16ms周期内确认，则确认。
    if(DebugButtonConfirmCount[0] > DebugButtonConfirmNum)
    {
      //确认标志置位。
      DebugButtonFlag[0] = 1;    
      //确认次数清0。
      DebugButtonConfirmCount[0] = 0;      
      //一次只允许一个按键，若确认了，则不用检测后面的按键，直接返回正确。
      return 1;
    }
    //还没到次数，仍视为没有按下。继续检测后面的按键。
    else
    {
      DebugButtonFlag[0] = 0;
    }
  }
  else
  {
    //确认次数清0。
    DebugButtonConfirmCount[0] = 0;
    //确认标志置0.
    DebugButtonFlag[0] = 0;
    //继续检测后面的按键。
  }
  
  //按键1.PTE1.
  if(!(GPIOE_PDIR&0x02))
  {
    //确认次数加1.
    DebugButtonConfirmCount[1]++;
    //连续多个16ms周期内确认，则确认。
    if(DebugButtonConfirmCount[1] > DebugButtonConfirmNum)
    {
      //确认标志置位。
      DebugButtonFlag[1] = 1;     
      //确认次数清0。
      DebugButtonConfirmCount[1] = 0;     
      //一次只允许一个按键，若确认了，则不用检测后面的按键，直接返回正确。
      return 1;
    }
    //还没到次数，仍视为没有按下。继续检测后面的按键。
    else
    {
      DebugButtonFlag[1] = 0;
    }
  }
  else
  {
    //确认次数清0。
    DebugButtonConfirmCount[1] = 0;
    //确认标志置0.
    DebugButtonFlag[1] = 0;
    //继续检测后面的按键。
  }
  
  //按键2.PTE2.
  if(!(GPIOE_PDIR&0x04))
  {
    //确认次数加1.
    DebugButtonConfirmCount[2]++;
    //连续多个16ms周期内确认，则确认。
    if(DebugButtonConfirmCount[2] > DebugButtonConfirmNum)
    {
      //确认标志置位。
      DebugButtonFlag[2] = 1;     
      //确认次数清0。
      DebugButtonConfirmCount[2] = 0;           
      //一次只允许一个按键，若确认了，则不用检测后面的按键，直接返回正确。
      return 1;
    }
    //还没到次数，仍视为没有按下。继续检测后面的按键。
    else
    {
      DebugButtonFlag[2] = 0;
    }
  }
  else
  {
    //确认次数清0。
    DebugButtonConfirmCount[2] = 0;
    //确认标志置0.
    DebugButtonFlag[2] = 0;
    //继续检测后面的按键。
  }  
  
  //按键3.PTE3.
  if(!(GPIOE_PDIR&0x08))
  {
    //确认次数加1.
    DebugButtonConfirmCount[3]++;
    //连续多个16ms周期内确认，则确认。
    if(DebugButtonConfirmCount[3] > DebugButtonConfirmNum)
    {
      //确认标志置位。
      DebugButtonFlag[3] = 1;     
      //确认次数清0。
      DebugButtonConfirmCount[3] = 0;           
      //一次只允许一个按键，若确认了，则不用检测后面的按键，直接返回正确。
      return 1;
    }
    //还没到次数，仍视为没有按下。继续检测后面的按键。
    else
    {
      DebugButtonFlag[3] = 0;
    }
  }
  else
  {
    //确认次数清0。
    DebugButtonConfirmCount[3] = 0;
    //确认标志置0.
    DebugButtonFlag[3] = 0;
    //继续检测后面的按键。
  }
  
  
  //按键4.PTE4.
  if(!(GPIOE_PDIR&0x10))
  {
    //确认次数加1.
    DebugButtonConfirmCount[4]++;
    //连续多个16ms周期内确认，则确认。
    if(DebugButtonConfirmCount[4] > DebugButtonConfirmNum)
    {
      //确认标志置位。
      DebugButtonFlag[4] = 1;  
      //确认次数清0。
      DebugButtonConfirmCount[4] = 0;           
      //一次只允许一个按键，若确认了，则不用检测后面的按键，直接返回正确。
      return 1;
    }
    //还没到次数，仍视为没有按下。继续检测后面的按键。
    else
    {
      DebugButtonFlag[4] = 0;
    }
  }
  else
  {
    //确认次数清0。
    DebugButtonConfirmCount[4] = 0;
    //确认标志置0.
    DebugButtonFlag[4] = 0;
    //继续检测后面的按键。
  }
  
  
  //按键5.PTE5.
  if(!(GPIOE_PDIR&0x20))
  {
    //确认次数加1.
    DebugButtonConfirmCount[5]++;
    //连续多个16ms周期内确认，则确认。
    if(DebugButtonConfirmCount[5] > DebugButtonConfirmNum)
    {
      //确认标志置位。
      DebugButtonFlag[5] = 1;     
      //确认次数清0。
      DebugButtonConfirmCount[5] = 0;           
      //一次只允许一个按键，若确认了，则不用检测后面的按键，直接返回正确。
      return 1;
    }
    //还没到次数，仍视为没有按下。继续检测后面的按键。
    else
    {
      DebugButtonFlag[5] = 0;
    }
  }
  else
  {
    //确认次数清0。
    DebugButtonConfirmCount[5] = 0;
    //确认标志置0.
    DebugButtonFlag[5] = 0;
    //继续检测后面的按键。
  }
    
  
  return 0;
}

//FLASH值的读取函数
//tempsectorindex扇区序号，tempsectorsize扇区大小，num第几个数据
uint32 FlashReadNow(uint32 tempsectorindex, uint32 tempsectorsize, uint32 num)
{
  uint32 temp;
  
  
  temp = (*(byte *)((uint32)(tempsectorindex * tempsectorsize + num * 4))) 
       + (*(byte *)((uint32)(tempsectorindex * tempsectorsize + num * 4 + 1))) * 256 
       + (*(byte *)((uint32)(tempsectorindex * tempsectorsize + num * 4 + 2))) * 65536 
       + (*(byte *)((uint32)(tempsectorindex * tempsectorsize + num * 4 + 3))) * 16777216;
  
  return temp;
}


//FLASH值的写入函数
//tempsectorindex扇区序号，num第几个数据，tempelementsize元素的大小（一般是4），pointeruint8数据指针（必须是uint8 *型的）。
uint8 FlashWriteNow(uint32 tempsectorindex, uint32 num, uint32 tempelementsize, uint8 *pointeruint8)
{
  
  FlashWrite(tempsectorindex, num * 4, tempelementsize, pointeruint8);
  
  return 1;
}

//FLASH模块值初始化读取
uint8 FlashInitRead(void)
{
  uint32 temp;
  uint8 i, j, k;
  
  for(i = 0; i < 60; i++)
  {
    temp = FlashReadNow(FlashSectorIndex, FlashSectorSize, i);
    DebugModifyNum[i] = temp;   //不需要强制类型转换，会自动转换的。
  }
  
  
  //赋给实际值，不直接赋给实际变量而通过中间变量，是为了方便修改。
  //LightHouseWaitStartMaxTime = DebugModifyNum[0];    //发车最多时间
  //SpeedStrategyChosen = DebugModifyNum[1];          //速度档位
  //MotorTakeOffQuadCountLimit = DebugModifyNum[2];   //起飞控制
  //AngleZoneConfirmMatLimit = DebugModifyNum[3];  //直角黑块，标志存储数组中直角黑块的个数大于该值时，则锁存为直角黑块。
  //AngleConfirmMatLimit = DebugModifyNum[4];      //直角弯标志存储数组中直角的个数大于该值时，则锁存为直角。
  //BrickConfirmMatLimit = DebugModifyNum[5];      //障碍标志存储数组中障碍的个数大于该值时，则锁存为障碍。
  //BrickConfirmFinalColCom = DebugModifyNum[6];   //障碍躲避时的列修正量（以15为零，左减右加。）
  //LightHouseWaitStopDelayCountLimit = DebugModifyNum[7]; //停车延时判断阈值，隔这么多个16ms才开始停车判断
  //LightHouseWaitStopGapCountLimit = DebugModifyNum[8];  //停车间隔计数阈值，每隔这么多个16ms就更新一次灯塔采集值

  

  //列优先,共4x8=32个，写完了DebugModifyNum[46]
  k = 15;
  for(i = 0; i <= 7; i++)
  {
    for(j = 0; j <= 3; j++)
    {
      //ControlRow_Weight[j][i] = DebugModifyNum[k];
      k++;
    }
  }
  
  //定时跑时间上限
  //TimeRealRun = DebugModifyNum[47];
  //电机P
  //SpeedPIDPVal = DebugModifyNum[48];
  //电机D
  //SpeedPIDDVal = DebugModifyNum[49];
  //坡道的给定速度
  //SpeedTargetValGryoResult = DebugModifyNum[50];
  //直角弯的给定速度
  //SpeedTargetValAngle = DebugModifyNum[51];
  //长直道的给定速度
  //SpeedTargetValStraight = DebugModifyNum[52];
  //坡道禁止赛道类型判断倒计数载入值
  //GyroResultForbidBackCountNum = DebugModifyNum[53];
  //连续直道计数阈值
  //PathRealStraightCountLimit = DebugModifyNum[54];
//  //灯塔停车亮持续时间
//  LightHouseWaitStopOnCountLimit = DebugModifyNum[55];
  //停车计数阈值。
  //StopFlagMaxCountNum = DebugModifyNum[55];
  //不停车标志
  //LightHouseNoStopFlag = DebugModifyNum[56];
  //直角弯线头差阈值。
  //AngleHeadLineNum = DebugModifyNum[57];
  //灯塔等待停车速度
  //SpeedTargetValWaitStop = DebugModifyNum[58];
  //上坡速度
  //SpeedTargetValGryoUp = DebugModifyNum[59];
  
  
 /*   
  DebugModifyNum[0] = 10;
  DebugModifyNum[1] = 33;
  DebugModifyNum[2] = 37;
  DebugModifyNum[3] = 40;
  DebugModifyNum[4] = 40;
  DebugModifyNum[5] = 40;
  DebugModifyNum[6] = 34;
  DebugModifyNum[7] = 29;
  DebugModifyNum[8] = 0;
  DebugModifyNum[9] = 120;
  DebugModifyNum[10] = 230;
  DebugModifyNum[11] = 150;
  DebugModifyNum[12] = 150;
  DebugModifyNum[13] = 150;
  DebugModifyNum[14] = 150;
  DebugModifyNum[15] = 150;
  DebugModifyNum[16] = 150;
  */
  
  speedOfCar = DebugModifyNum[0];               //校赛的时候，这个参数是决定速度的，后来就没啥用了
  Steer_kp[1] = DebugModifyNum[1];              //舵机的p参数
  Steer_kp[2] = DebugModifyNum[2];
  Steer_kp[3] = DebugModifyNum[3];
  Steer_kp[4] = DebugModifyNum[4];
  Steer_kp[5] = DebugModifyNum[5];
  Steer_kp[6] = DebugModifyNum[6];
  Steer_kp[7] = DebugModifyNum[7];
  Steer_kp[0] = DebugModifyNum[8];
  SpeedPIDPVal = DebugModifyNum[9];             //电机的PI参数
  SpeedPIDDVal = DebugModifyNum[10];
  SpeedTargetMat[0][5] = DebugModifyNum[11];    //给定速度
  SpeedTargetMat[0][4] = DebugModifyNum[12];
  SpeedTargetMat[0][6] = DebugModifyNum[12];
  SpeedTargetMat[0][3] = DebugModifyNum[13];
  SpeedTargetMat[0][7] = DebugModifyNum[13];
  SpeedTargetMat[0][2] = DebugModifyNum[14];
  SpeedTargetMat[0][8] = DebugModifyNum[14];
  SpeedTargetMat[0][1] = DebugModifyNum[15];
  SpeedTargetMat[0][9] = DebugModifyNum[15];
  SpeedTargetMat[0][0] = DebugModifyNum[16];
  SpeedTargetMat[0][10] = DebugModifyNum[16];
  BrickConfirmMatLimit = DebugModifyNum[17];      //障碍标志存储数组中障碍的个数大于该值时，则锁存为障碍。
  BrickConfirmFinalColCom = DebugModifyNum[18];   //障碍躲避时的列修正量（以15为零，左减右加。）
  LimitLeftWBMin = DebugModifyNum[19];          //图像黑白线的阈值
  LimitRightWBMin = DebugModifyNum[19];
  LimitLeftWBMax = DebugModifyNum[20];
  LimitRightWBMax = DebugModifyNum[20];
  LimitLeftWMin = DebugModifyNum[21];
  LimitRightWMin = DebugModifyNum[21];
  LimitLeftWMax = DebugModifyNum[22];
  LimitRightWMax = DebugModifyNum[22]; 
  LimitLeftBMin = DebugModifyNum[23];
  LimitRightBMin = DebugModifyNum[23];
  LimitLeftBMax = DebugModifyNum[24];
  LimitRightBMax = DebugModifyNum[24];
  SpeedTargetValStraight = DebugModifyNum[25];      //长直道速度
  SpeedBrick = DebugModifyNum[26];                  //障碍速度
  SpeedTargetValGryoUp = DebugModifyNum[27];        //上坡速度
  SpeedTargetValGryoResult = DebugModifyNum[28];    //下坡速度
  SpeedXiaoZhiDao = DebugModifyNum[29];             //小直道速度
  limitStopLineBlackXiu = DebugModifyNum[52];       //与停车线判断有关的黑色阈值的设定
  ServoPIDDMat[0] = DebugModifyNum[53];             //舵机d参数
  ServoPIDD2Mat[0] = DebugModifyNum[53];
  ServoPIDDMat[1] = DebugModifyNum[54];
  ServoPIDD2Mat[1] = DebugModifyNum[54];
  ServoPIDDMat[2] = DebugModifyNum[55];
  ServoPIDD2Mat[2] = DebugModifyNum[55];
  ServoPIDDMat[3] = DebugModifyNum[56];
  ServoPIDD2Mat[3] = DebugModifyNum[56];
  ServoPIDDMat[4] = DebugModifyNum[57];
  ServoPIDD2Mat[4] = DebugModifyNum[57];
  ServoPIDDMat[5] = DebugModifyNum[58];
  ServoPIDD2Mat[5] = DebugModifyNum[58];
  ServoPIDDMat[6] = DebugModifyNum[59];
  ServoPIDD2Mat[6] = DebugModifyNum[59];
  

  return 1;
}


//Debug初始化
uint8 DebugInit(void)
{
  //OLED初始化。这里会显示“Hello AE86”
  LCD_Init();
  //FLASH初始化
  FlashInit();
  //FLASH模块值初始化读取。
  //FlashInitRead();
  //Debug用的引脚的初始化。
  DebugPortInit();
  
  return 1;
}



//对值的修改，temp=0表示减1个单位，temp=1表示加1个单位
uint8 DebugModifyValue(uint8 temp)
{
  
  //temp=0表示减1个单位
  if(temp == 0)
  {
    //当前是第0页。
    if(DebugModifyPage == 0)
    {
      //根据光标位置来改值。
      if(DebugModifyCursor == 0)
      {
        DebugModifyNum[0] --;
        speedOfCar = DebugModifyNum[0];
      }
      else if(DebugModifyCursor == 1)
      {
        DebugModifyNum[1] --;
        Steer_kp[1] = DebugModifyNum[1];
      }
      else if(DebugModifyCursor == 2)
      {
        DebugModifyNum[2] --;
        Steer_kp[2] = DebugModifyNum[2];
      }
      else if(DebugModifyCursor == 3)
      {
        DebugModifyNum[3]--;
        Steer_kp[3] = DebugModifyNum[3];
      }
      else if(DebugModifyCursor == 4)
      {
        DebugModifyNum[4]--;
        Steer_kp[4] = DebugModifyNum[4];
      }
      else if(DebugModifyCursor == 5)
      {
        DebugModifyNum[5]--;
        Steer_kp[5] = DebugModifyNum[5];
      }
      else if(DebugModifyCursor == 6)
      {
        DebugModifyNum[6]--;
        Steer_kp[6] = DebugModifyNum[6];
      }
      else if(DebugModifyCursor == 7)
      {
        DebugModifyNum[7]--;
        Steer_kp[7] = DebugModifyNum[7];
      }
      else if(DebugModifyCursor == 8)
      {
        DebugModifyNum[8]--;
        Steer_kp[0] = DebugModifyNum[8];
        //LightHouseWaitStopGapCountLimit = DebugModifyNum[8];
      }
      else if(DebugModifyCursor == 9)
      {
        DebugModifyNum[9]--;
        SpeedPIDPVal = DebugModifyNum[9];

        //LimitLeftWBMin = DebugModifyNum[9];
        //LimitRightWBMin = DebugModifyNum[9];
      }
      else if(DebugModifyCursor == 10)
      {
        DebugModifyNum[10]--;
        SpeedPIDDVal = DebugModifyNum[10];

        //LimitLeftWBMax = DebugModifyNum[10];
        //LimitRightWBMax = DebugModifyNum[10];
      }
      else if(DebugModifyCursor == 11)
      {
        DebugModifyNum[11]--;
        SpeedTargetMat[0][5] = DebugModifyNum[11];
        //LimitLeftWMin = DebugModifyNum[11];
        //LimitRightWMin = DebugModifyNum[11];
      }
      else if(DebugModifyCursor == 12)
      {
        DebugModifyNum[12]--;
        SpeedTargetMat[0][4] = DebugModifyNum[12];
        SpeedTargetMat[0][6] = DebugModifyNum[12];
        //LimitLeftWMax = DebugModifyNum[12];
        //LimitRightWMax = DebugModifyNum[12];
      }
      else if(DebugModifyCursor == 13)
      {
        DebugModifyNum[13]--;
        SpeedTargetMat[0][3] = DebugModifyNum[13];
        SpeedTargetMat[0][7] = DebugModifyNum[13];
        //LimitLeftBMin = DebugModifyNum[13];
        //LimitRightBMin = DebugModifyNum[13];
      }
      else if(DebugModifyCursor == 14)
      {
        DebugModifyNum[14]--;
        SpeedTargetMat[0][2] = DebugModifyNum[14];
        SpeedTargetMat[0][8] = DebugModifyNum[14];
        //LimitLeftBMax = DebugModifyNum[14];
        //LimitRightBMax = DebugModifyNum[14];
      }
      else
      {
      }
    }
    //当前是第1页。
    else if(DebugModifyPage == 1)
    {
      //根据光标位置来改值。
      if(DebugModifyCursor == 0)
      {
        DebugModifyNum[15]--;
        SpeedTargetMat[0][1] = DebugModifyNum[15];
        SpeedTargetMat[0][9] = DebugModifyNum[15];
        //ControlRow_Weight[0][0] = DebugModifyNum[15];
      }
      else if(DebugModifyCursor == 1)
      {
        DebugModifyNum[16]--;
        SpeedTargetMat[0][0] = DebugModifyNum[16];
        SpeedTargetMat[0][10] = DebugModifyNum[16];
        //ControlRow_Weight[1][0]  = DebugModifyNum[16];
      }
      else if(DebugModifyCursor == 2)
      { 
        DebugModifyNum[17]--;
        BrickConfirmMatLimit = DebugModifyNum[17];      //障碍标志存储数组中障碍的个数大于该值时，则锁存为障碍。
        //ControlRow_Weight[2][0]  = DebugModifyNum[17];
      }
      else if(DebugModifyCursor == 3)
      {
        DebugModifyNum[18]--;
        BrickConfirmFinalColCom = DebugModifyNum[18];   //障碍躲避时的列修正量（以15为零，左减右加。）
        //ControlRow_Weight[3][0]  = DebugModifyNum[18];
      }
      else if(DebugModifyCursor == 4)
      {
        DebugModifyNum[19]--;
        LimitLeftWBMin = DebugModifyNum[19];
        LimitRightWBMin = DebugModifyNum[19];
        //ControlRow_Weight[0][1]  = DebugModifyNum[19];
      }
      else if(DebugModifyCursor == 5)
      {
        DebugModifyNum[20]--;
        LimitLeftWBMax = DebugModifyNum[20];
        LimitRightWBMax = DebugModifyNum[20];
        //ControlRow_Weight[1][1] = DebugModifyNum[20];
      }
      else if(DebugModifyCursor == 6)
      {
        DebugModifyNum[21]--;
        LimitLeftWMin = DebugModifyNum[21];
        LimitRightWMin = DebugModifyNum[21];
        //ControlRow_Weight[2][1] = DebugModifyNum[21];
      }
      else if(DebugModifyCursor == 7)
      {
        DebugModifyNum[22]--;
        LimitLeftWMax = DebugModifyNum[22];
        LimitRightWMax = DebugModifyNum[22]; 
        //ControlRow_Weight[3][1] = DebugModifyNum[22];
      }
      else if(DebugModifyCursor == 8)
      {
        DebugModifyNum[23]--;
        LimitLeftBMin = DebugModifyNum[23];
        LimitRightBMin = DebugModifyNum[23];
        //ControlRow_Weight[0][2] = DebugModifyNum[23];
      }
      else if(DebugModifyCursor == 9)
      {
        DebugModifyNum[24]--;
        LimitLeftBMax = DebugModifyNum[24];
        LimitRightBMax = DebugModifyNum[24];
        //ControlRow_Weight[1][2] = DebugModifyNum[24];
      }
      else if(DebugModifyCursor == 10)
      {
        DebugModifyNum[25]--;
        SpeedTargetValStraight = DebugModifyNum[25];
        //ControlRow_Weight[2][2] = DebugModifyNum[25];
      }
      else if(DebugModifyCursor == 11)
      {
        DebugModifyNum[26]--;
        SpeedBrick = DebugModifyNum[26];
        //ControlRow_Weight[3][2] = DebugModifyNum[26];
      }
      else if(DebugModifyCursor == 12)
      {
        DebugModifyNum[27]--;
        SpeedTargetValGryoUp = DebugModifyNum[27];
        //ControlRow_Weight[0][3] = DebugModifyNum[27];
      }
      else if(DebugModifyCursor == 13)
      {
        DebugModifyNum[28]--;
        SpeedTargetValGryoResult = DebugModifyNum[28];
        //ControlRow_Weight[1][3] = DebugModifyNum[28];
      }
      else if(DebugModifyCursor == 14)
      {
        DebugModifyNum[29]--;
        SpeedXiaoZhiDao = DebugModifyNum[29];
        //ControlRow_Weight[2][3] = DebugModifyNum[29];
      }
      else
      {
      }
    }
    //当前是第2页。
    else if(DebugModifyPage == 2)
    {
      //根据光标位置来改值。
      if(DebugModifyCursor == 0)
      {
        DebugModifyNum[30]--;
        //ControlRow_Weight[3][3] = DebugModifyNum[30];
      }
      else if(DebugModifyCursor == 1)
      {
        DebugModifyNum[31]--;
        //ControlRow_Weight[0][4]  = DebugModifyNum[31];
      }
      else if(DebugModifyCursor == 2)
      {
        DebugModifyNum[32]--;
        //ControlRow_Weight[1][4]  = DebugModifyNum[32];
      }
      else if(DebugModifyCursor == 3)
      {
        DebugModifyNum[33]--;
        //ControlRow_Weight[2][4]  = DebugModifyNum[33];
      }
      else if(DebugModifyCursor == 4)
      {
        DebugModifyNum[34]--;
        //ControlRow_Weight[3][4]  = DebugModifyNum[34];
      }
      else if(DebugModifyCursor == 5)
      {
        DebugModifyNum[35]--;
        //ControlRow_Weight[0][5] = DebugModifyNum[35];
      }
      else if(DebugModifyCursor == 6)
      {
        DebugModifyNum[36]--;
        //ControlRow_Weight[1][5] = DebugModifyNum[36];
      }
      else if(DebugModifyCursor == 7)
      {
        DebugModifyNum[37]--;
        //ControlRow_Weight[2][5] = DebugModifyNum[37];
      }
      else if(DebugModifyCursor == 8)
      {
        DebugModifyNum[38]--;
        //ControlRow_Weight[3][5] = DebugModifyNum[38];
      }
      else if(DebugModifyCursor == 9)
      {
        DebugModifyNum[39]--;
        //ControlRow_Weight[0][6] = DebugModifyNum[39];
      }
      else if(DebugModifyCursor == 10)
      {
        DebugModifyNum[40]--;
        //ControlRow_Weight[1][6] = DebugModifyNum[40];
      }
      else if(DebugModifyCursor == 11)
      {
        DebugModifyNum[41]--;
        //ControlRow_Weight[2][6] = DebugModifyNum[41];
      }
      else if(DebugModifyCursor == 12)
      {
        DebugModifyNum[42]--;
        //ControlRow_Weight[3][6] = DebugModifyNum[42];
      }
      else if(DebugModifyCursor == 13)
      {
        DebugModifyNum[43]--;
        //ControlRow_Weight[0][7] = DebugModifyNum[43];
      }
      else if(DebugModifyCursor == 14)
      {
        DebugModifyNum[44]--;
        //ControlRow_Weight[1][7] = DebugModifyNum[44];
      }
      else
      {
      }
    }
    //当前是第3页。
    else if(DebugModifyPage == 3)
    {
      //根据光标位置来改值。
      if(DebugModifyCursor == 0)
      {
        DebugModifyNum[45]--;
        //ControlRow_Weight[2][7] = DebugModifyNum[45];
      }
      else if(DebugModifyCursor == 1)
      {
        DebugModifyNum[46]--;
        //ControlRow_Weight[3][7]  = DebugModifyNum[46];
      }
      else if(DebugModifyCursor == 2)
      {
        DebugModifyNum[47] -= 10;;
        //TimeRealRun -= 10;
      }
      else if(DebugModifyCursor == 3)
      {
        DebugModifyNum[48] -= 1;
        //SpeedPIDPVal -= 1;
      }
      else if(DebugModifyCursor == 4)
      {
        DebugModifyNum[49] -= 1;
        //SpeedPIDDVal -= 1;
      }
      else if(DebugModifyCursor == 5)
      {
        DebugModifyNum[50]--;
        //SpeedTargetValGryoResult--;
      }
      else if(DebugModifyCursor == 6)
      {
        DebugModifyNum[51]--;
        //SpeedTargetValAngle--;
      }
      else if(DebugModifyCursor == 7)
      {
        DebugModifyNum[52]--;
        limitStopLineBlackXiu = DebugModifyNum[52];
        //SpeedTargetValStraight = DebugModifyNum[52];
      }
      else if(DebugModifyCursor == 8)
      {
        DebugModifyNum[53]--;
        ServoPIDDMat[0] = DebugModifyNum[53];
        ServoPIDD2Mat[0] = DebugModifyNum[53];
        //GyroResultForbidBackCountNum = DebugModifyNum[53];
      }
      else if(DebugModifyCursor == 9)
      {
        DebugModifyNum[54]--;
        ServoPIDDMat[1] = DebugModifyNum[54];
        ServoPIDD2Mat[1] = DebugModifyNum[54];
        //PathRealStraightCountLimit = DebugModifyNum[54];
      }
      else if(DebugModifyCursor == 10)
      {
        DebugModifyNum[55]--;
        ServoPIDDMat[2] = DebugModifyNum[55];
        ServoPIDD2Mat[2] = DebugModifyNum[55];
        //StopFlagMaxCountNum = DebugModifyNum[55];
      }
      else if(DebugModifyCursor == 11)
      {
        DebugModifyNum[56]--;
        ServoPIDDMat[3] = DebugModifyNum[56];
        ServoPIDD2Mat[3] = DebugModifyNum[56];
        //LightHouseNoStopFlag = DebugModifyNum[56];
      }
      else if(DebugModifyCursor == 12)
      {
        DebugModifyNum[57]--;
        ServoPIDDMat[4] = DebugModifyNum[57];
        ServoPIDD2Mat[4] = DebugModifyNum[57];
        //AngleHeadLineNum = DebugModifyNum[57];
      }
      else if(DebugModifyCursor == 13)
      {
        DebugModifyNum[58]--;
        ServoPIDDMat[5] = DebugModifyNum[58];
        ServoPIDD2Mat[5] = DebugModifyNum[58];
        //SpeedTargetValWaitStop = DebugModifyNum[58];
      }
      else if(DebugModifyCursor == 14)
      {
        DebugModifyNum[59]--;
        ServoPIDDMat[6] = DebugModifyNum[59];
        ServoPIDD2Mat[6] = DebugModifyNum[59];
        //SpeedTargetValGryoUp = DebugModifyNum[59];
      }
      else
      {
      }      
      
    }
    else
    {
    }
  }
  //temp=1表示加1个单位
  else
  {
    //当前是第0页。
    if(DebugModifyPage == 0)
    {
      //根据光标位置来改值。
      if(DebugModifyCursor == 0)
      {
        DebugModifyNum[0] ++;
        speedOfCar = DebugModifyNum[0];
      }
      else if(DebugModifyCursor == 1)
      {
        DebugModifyNum[1]++;
        Steer_kp[1] = DebugModifyNum[1];
      }
      else if(DebugModifyCursor == 2)
      {
        DebugModifyNum[2] ++;
        Steer_kp[2] = DebugModifyNum[2];
      }
      else if(DebugModifyCursor == 3)
      {
        DebugModifyNum[3]++;
        Steer_kp[3] = DebugModifyNum[3];
      }
      else if(DebugModifyCursor == 4)
      {
        DebugModifyNum[4]++;
        Steer_kp[4] = DebugModifyNum[4];
      }
      else if(DebugModifyCursor == 5)
      {
        DebugModifyNum[5]++;
        Steer_kp[5] = DebugModifyNum[5];
      }
      else if(DebugModifyCursor == 6)
      {
        DebugModifyNum[6]++;
        Steer_kp[6] = DebugModifyNum[6];
      }
      else if(DebugModifyCursor == 7)
      {
        DebugModifyNum[7]++;
        Steer_kp[7] = DebugModifyNum[7];
      }
      else if(DebugModifyCursor == 8)
      {
        DebugModifyNum[8]++;
        Steer_kp[0] = DebugModifyNum[8];
        //LightHouseWaitStopGapCountLimit = DebugModifyNum[8];
      }
      else if(DebugModifyCursor == 9)
      {
        DebugModifyNum[9]++;
        SpeedPIDPVal = DebugModifyNum[9];
        //LimitLeftWBMin = DebugModifyNum[9];
        //LimitRightWBMin = DebugModifyNum[9];
      }
      else if(DebugModifyCursor == 10)
      {
        DebugModifyNum[10]++;
        SpeedPIDDVal = DebugModifyNum[10];
        //LimitLeftWBMax = DebugModifyNum[10];
        //LimitRightWBMax = DebugModifyNum[10];
      }
      else if(DebugModifyCursor == 11)
      {
        DebugModifyNum[11]++;
        SpeedTargetMat[0][5] = DebugModifyNum[11];
        //LimitLeftWMin = DebugModifyNum[11];
        //LimitRightWMin = DebugModifyNum[11];
      }
      else if(DebugModifyCursor == 12)
      {
        DebugModifyNum[12]++;
        SpeedTargetMat[0][4] = DebugModifyNum[12];
        SpeedTargetMat[0][6] = DebugModifyNum[12];
        //LimitLeftWMax = DebugModifyNum[12];
        //LimitRightWMax = DebugModifyNum[12];
      }
      else if(DebugModifyCursor == 13)
      {
        DebugModifyNum[13]++;
        SpeedTargetMat[0][3] = DebugModifyNum[13];
        SpeedTargetMat[0][7] = DebugModifyNum[13];
        //LimitLeftBMin = DebugModifyNum[13];
        //LimitRightBMin = DebugModifyNum[13];
      }
      else if(DebugModifyCursor == 14)
      {
        DebugModifyNum[14]++;
        SpeedTargetMat[0][2] = DebugModifyNum[14];
        SpeedTargetMat[0][8] = DebugModifyNum[14];
        //LimitLeftBMax = DebugModifyNum[14];
        //LimitRightBMax = DebugModifyNum[14];
      }
      else
      {
      }
    }
    //当前是第1页。
    else if(DebugModifyPage == 1)
    {
      //根据光标位置来改值。
      if(DebugModifyCursor == 0)
      {
        DebugModifyNum[15]++;
        SpeedTargetMat[0][1] = DebugModifyNum[15];
        SpeedTargetMat[0][9] = DebugModifyNum[15];
        //ControlRow_Weight[0][0] = DebugModifyNum[15];
      }
      else if(DebugModifyCursor == 1)
      {
        DebugModifyNum[16]++;
        SpeedTargetMat[0][0] = DebugModifyNum[16];
        SpeedTargetMat[0][10] = DebugModifyNum[16];
        //ControlRow_Weight[1][0]  = DebugModifyNum[16];
      }
      else if(DebugModifyCursor == 2)
      {
        DebugModifyNum[17]++;
        BrickConfirmMatLimit = DebugModifyNum[17];      //障碍标志存储数组中障碍的个数大于该值时，则锁存为障碍。
        //ControlRow_Weight[2][0]  = DebugModifyNum[17];
      }
      else if(DebugModifyCursor == 3)
      {
        DebugModifyNum[18]++;
        BrickConfirmFinalColCom = DebugModifyNum[18];   //障碍躲避时的列修正量（以15为零，左减右加。）
        //ControlRow_Weight[3][0]  = DebugModifyNum[18];
      }
      else if(DebugModifyCursor == 4)
      {
        DebugModifyNum[19]++;
        LimitLeftWBMin = DebugModifyNum[19];
        LimitRightWBMin = DebugModifyNum[19];
        //ControlRow_Weight[0][1]  = DebugModifyNum[19];
      }
      else if(DebugModifyCursor == 5)
      {
        DebugModifyNum[20]++;
        LimitLeftWBMax = DebugModifyNum[20];
        LimitRightWBMax = DebugModifyNum[20];        
        //ControlRow_Weight[1][1] = DebugModifyNum[20];
      }
      else if(DebugModifyCursor == 6)
      {
        DebugModifyNum[21]++;
        LimitLeftWMin = DebugModifyNum[21];
        LimitRightWMin = DebugModifyNum[21];
        //ControlRow_Weight[2][1] = DebugModifyNum[21];
      }
      else if(DebugModifyCursor == 7)
      {
        DebugModifyNum[22]++;
        LimitLeftWMax = DebugModifyNum[22];
        LimitRightWMax = DebugModifyNum[22]; 
        //ControlRow_Weight[3][1] = DebugModifyNum[22];
      }
      else if(DebugModifyCursor == 8)
      {
        DebugModifyNum[23]++;
        LimitLeftBMin = DebugModifyNum[23];
        LimitRightBMin = DebugModifyNum[23];
        //ControlRow_Weight[0][2] = DebugModifyNum[23];
      }
      else if(DebugModifyCursor == 9)
      {
        DebugModifyNum[24]++;
        LimitLeftBMax = DebugModifyNum[24];
        LimitRightBMax = DebugModifyNum[24];
        //ControlRow_Weight[1][2] = DebugModifyNum[24];
      }
      else if(DebugModifyCursor == 10)
      {
        DebugModifyNum[25]++;
        SpeedTargetValStraight = DebugModifyNum[25];
        //ControlRow_Weight[2][2] = DebugModifyNum[25];
      }
      else if(DebugModifyCursor == 11)
      {
        DebugModifyNum[26]++;
        SpeedBrick = DebugModifyNum[26];
        //ControlRow_Weight[3][2] = DebugModifyNum[26];
      }
      else if(DebugModifyCursor == 12)
      {
        DebugModifyNum[27]++;
        SpeedTargetValGryoUp = DebugModifyNum[27];
        //ControlRow_Weight[0][3] = DebugModifyNum[27];
      }
      else if(DebugModifyCursor == 13)
      {
        DebugModifyNum[28]++;
        SpeedTargetValGryoResult = DebugModifyNum[28];
        //ControlRow_Weight[1][3] = DebugModifyNum[28];
      }
      else if(DebugModifyCursor == 14)
      {
        DebugModifyNum[29]++;
        SpeedXiaoZhiDao = DebugModifyNum[29];
        //ControlRow_Weight[2][3] = DebugModifyNum[29];
      }
      else
      {
      }
    }
    //当前是第2页。
    else if(DebugModifyPage == 2)
    {
      //根据光标位置来改值。
      if(DebugModifyCursor == 0)
      {
        DebugModifyNum[30]++;
        //ControlRow_Weight[3][3] = DebugModifyNum[30];
      }
      else if(DebugModifyCursor == 1)
      {
        DebugModifyNum[31]++;
        //ControlRow_Weight[0][4]  = DebugModifyNum[31];
      }
      else if(DebugModifyCursor == 2)
      {
        DebugModifyNum[32]++;
        //ControlRow_Weight[1][4]  = DebugModifyNum[32];
      }
      else if(DebugModifyCursor == 3)
      {
        DebugModifyNum[33]++;
        //ControlRow_Weight[2][4]  = DebugModifyNum[33];
      }
      else if(DebugModifyCursor == 4)
      {
        DebugModifyNum[34]++;
        //ControlRow_Weight[3][4]  = DebugModifyNum[34];
      }
      else if(DebugModifyCursor == 5)
      {
        DebugModifyNum[35]++;
        //ControlRow_Weight[0][5] = DebugModifyNum[35];
      }
      else if(DebugModifyCursor == 6)
      {
        DebugModifyNum[36]++;
        //ControlRow_Weight[1][5] = DebugModifyNum[36];
      }
      else if(DebugModifyCursor == 7)
      {
        DebugModifyNum[37]++;
        //ControlRow_Weight[2][5] = DebugModifyNum[37];
      }
      else if(DebugModifyCursor == 8)
      {
        DebugModifyNum[38]++;
        //ControlRow_Weight[3][5] = DebugModifyNum[38];
      }
      else if(DebugModifyCursor == 9)
      {
        DebugModifyNum[39]++;
        //ControlRow_Weight[0][6] = DebugModifyNum[39];
      }
      else if(DebugModifyCursor == 10)
      {
        DebugModifyNum[40]++;
        //ControlRow_Weight[1][6] = DebugModifyNum[40];
      }
      else if(DebugModifyCursor == 11)
      {
        DebugModifyNum[41]++;
        //ControlRow_Weight[2][6] = DebugModifyNum[41];
      }
      else if(DebugModifyCursor == 12)
      {
        DebugModifyNum[42]++;
        //ControlRow_Weight[3][6] = DebugModifyNum[42];
      }
      else if(DebugModifyCursor == 13)
      {
        DebugModifyNum[43]++;
        //ControlRow_Weight[0][7] = DebugModifyNum[43];
      }
      else if(DebugModifyCursor == 14)
      {
        DebugModifyNum[44]++;
        //ControlRow_Weight[1][7] = DebugModifyNum[44];
      }
      else
      {
      }
    }
    //当前是第3页。
    else if(DebugModifyPage == 3)
    {
      //根据光标位置来改值。
      if(DebugModifyCursor == 0)
      {
        DebugModifyNum[45]++;
        //ControlRow_Weight[2][7] = DebugModifyNum[45];
      }
      else if(DebugModifyCursor == 1)
      {
        DebugModifyNum[46]++;
        //ControlRow_Weight[3][7]  = DebugModifyNum[46];
      }
      else if(DebugModifyCursor == 2)
      {
        DebugModifyNum[47] += 10;
        //TimeRealRun += 10;
      }
      else if(DebugModifyCursor == 3)
      {
        DebugModifyNum[48] += 1;
        //SpeedPIDPVal += 1;
      }
      else if(DebugModifyCursor == 4)
      {
        DebugModifyNum[49] += 1;
        //SpeedPIDDVal += 1;
      }
      else if(DebugModifyCursor == 5)
      {
        DebugModifyNum[50]++;
        //SpeedTargetValGryoResult++;
      }
      else if(DebugModifyCursor == 6)
      {
        DebugModifyNum[51]++;
        //SpeedTargetValAngle++;
      }
      else if(DebugModifyCursor == 7)
      {
        DebugModifyNum[52]++;
        limitStopLineBlackXiu = DebugModifyNum[52];
        //SpeedTargetValStraight = DebugModifyNum[52];
      }
      else if(DebugModifyCursor == 8)
      {
        DebugModifyNum[53]++;
        ServoPIDDMat[0] = DebugModifyNum[53];
        ServoPIDD2Mat[0] = DebugModifyNum[53];
        //GyroResultForbidBackCountNum = DebugModifyNum[53];
      }
      else if(DebugModifyCursor == 9)
      {
        DebugModifyNum[54]++;
        ServoPIDDMat[1] = DebugModifyNum[54];
        ServoPIDD2Mat[1] = DebugModifyNum[54];
        //PathRealStraightCountLimit = DebugModifyNum[54];
      }
      else if(DebugModifyCursor == 10)
      {
        DebugModifyNum[55]++;
        ServoPIDDMat[2] = DebugModifyNum[55];
        ServoPIDD2Mat[2] = DebugModifyNum[55];
        //StopFlagMaxCountNum = DebugModifyNum[55];
      }
      else if(DebugModifyCursor == 11)
      {
        DebugModifyNum[56]++;
        ServoPIDDMat[3] = DebugModifyNum[56];
        ServoPIDD2Mat[3] = DebugModifyNum[56];
        //LightHouseNoStopFlag = DebugModifyNum[56];
      }
      else if(DebugModifyCursor == 12)
      {
        DebugModifyNum[57]++;
        ServoPIDDMat[4] = DebugModifyNum[57];
        ServoPIDD2Mat[4] = DebugModifyNum[57];
        //AngleHeadLineNum = DebugModifyNum[57];
      }
      else if(DebugModifyCursor == 13)
      {
        DebugModifyNum[58]++;
        ServoPIDDMat[5] = DebugModifyNum[58];
        ServoPIDD2Mat[5] = DebugModifyNum[58];
        //SpeedTargetValWaitStop = DebugModifyNum[58];
      }
      else if(DebugModifyCursor == 14)
      {
        DebugModifyNum[59]++;
        ServoPIDDMat[6] = DebugModifyNum[59];
        ServoPIDD2Mat[6] = DebugModifyNum[59];
        //SpeedTargetValGryoUp = DebugModifyNum[59];
      }
      else
      {
      }      
      
    }
    else
    {
    }
  }
  
  return 1;
}


//按键信息处理
uint8 DebugButtonDeal(void)
{
  //当前为显示界面。
  if(DebugScreenChosen == DebugScreenChosenDisplay)
  {
    //1. 首先判断3号按键是否按下，3号按键表示切换显示/调参
    if(DebugButtonFlag[3] == 1)
    {
      //切换到调参界面。
      DebugScreenChosen = DebugScreenChosenModify;
      //清屏准备显示新内容。
      LCD_CLS();
      
      return 1;
    }
    else
    {
    }
    
    //2. 再判断2号按键是否按下，2号按键在显示界面下暂时没有定义////////////////
    if(DebugButtonFlag[2] == 1)
    {
      
      
      return 1;
    }
    else
    {
    }
    
    //3. 再判断1号按键是否按下，1号按键在显示界面下表示下一页。
    if(DebugButtonFlag[1] == 1)
    {
      //显示界面下的页数加1，到顶就回到0.取值范围为0~DebugDisplayPageNum - 1
      if(DebugDisplayPage >= DebugDisplayPageNum - 1)
      {
        DebugDisplayPage = 0;
      }
      else
      {
        DebugDisplayPage++;
      }
      //清屏准备显示新内容。
      LCD_CLS();
      
      return 1;
    }
    else
    {      
    }    
    
    //4. 再判断0号按键是否按下，2号按键在显示界面下表示上一页。
    if(DebugButtonFlag[0] == 1)
    {
      //显示界面下的页数减1，到底就回到顶.取值范围为0~DebugDisplayPageNum - 1   
      if(DebugDisplayPage == 0)
      {
        DebugDisplayPage = DebugDisplayPageNum - 1;
      }
      else
      {
        DebugDisplayPage--;
      }
      //清屏准备显示新内容。
      LCD_CLS();
      
      return 1;
    }
    else
    {
    }
    
  }
  //当前为调参界面。
  else
  {
    //1. 再判断3号按键是否按下，3号按键表示切换显示/调参。
    if(DebugButtonFlag[3] == 1)
    {
      //切换到显示界面。
      DebugScreenChosen = DebugScreenChosenDisplay;
      //清屏准备显示新内容。
      LCD_CLS();
      
      return 1;      
    }
    else
    {     
    }

    //2. 再判断2号按键是否按下，2号按键在调参界面下表示 确认/保存。
    if(DebugButtonFlag[2] == 1)
    {
      //之前修改标志为0，再按下2号键表示马上要进行修改。则屏幕上要出现更改标志。
      if(DebugModifyFlag == 0)
      {
        DebugModifyFlag = 1;
      }
      //之前修改标志为1，再按上2号键表示已经修改完成，需要保存并退出到上下移模式。更改标志消失。
      else
      {
        DebugModifyFlag = 0;
        ///////////////////////////////保存怎么保存？？？？
      }
      
      return 1;
    }
    else
    {
    }
    
    //3. 再判断1号按键是否按下，1号按键在调参界面下表示 光标下移/值减一个单位。
    if(DebugButtonFlag[1] == 1)
    {
      //之前修改标志为0，表示是上下移模式，按下1号键表示下移光标。
      if(DebugModifyFlag == 0)
      {
        //光标的取值范围为0~14
        if(DebugModifyCursor >= 14)
        {
          //记录上一次的光标位置
          DebugModifyCursorPre = 14;
          DebugModifyCursor = 0;         
          //DebugModifyPage有0,1,2,3共4种取值。翻页就在这4页之间翻。
          if(DebugModifyPage == 0)
          {
            DebugModifyPage = 1;
          }
          else if(DebugModifyPage == 1)
          {
            DebugModifyPage = 2;
          }
          else if(DebugModifyPage == 2)
          {
            DebugModifyPage = 3;
          }
          else if(DebugModifyPage == 3)
          {
            DebugModifyPage = 0;
          }          
          else
          {
          }
          //清屏准备显示新内容。
          LCD_CLS();
        }
        else
        {
          //记录上一次的光标位置
          DebugModifyCursorPre = DebugModifyCursor;
          DebugModifyCursor++;
          
        }
        
      }
      //之前修改标志为1，表示是加减值模式，按下1号键表示光标所在值要减一个单位。
      else
      {
        //0表示减1个单位。
        DebugModifyValue(0);
      }
      
      return 1;
    }
    else
    {     
    }

    //4. 再判断0号按键是否按下，0号按键在调参界面下表示 光标上移/值加一个单位。
    if(DebugButtonFlag[0] == 1)
    {
      //之前修改标志为0，表示是上下移模式，按下0号键表示上移光标。
      if(DebugModifyFlag == 0)
      {
        //光标的取值范围为0~14
        if(DebugModifyCursor == 0)
        {
          //记录上一次的光标位置。
          DebugModifyCursorPre = 0;
          DebugModifyCursor = 14;
          //DebugModifyPage有0, 1, 2, 3共4种取值。翻页就在这4页之间翻。
          if(DebugModifyPage == 0)
          {
            DebugModifyPage = 3;
          }
          else if(DebugModifyPage == 1)
          {
            DebugModifyPage = 0;
          }
          else if(DebugModifyPage == 2)
          {
            DebugModifyPage = 1;
          }
          else if(DebugModifyPage == 3)
          {
            DebugModifyPage = 2;
          }          
          else
          {
          }
          //清屏准备显示新内容。
          LCD_CLS();   
        }
        else
        {
          //记录上一次的光标位置。
          DebugModifyCursorPre = DebugModifyCursor;
          DebugModifyCursor--;
        }
     
      }
      //之前修改标志为1，表示是加减值模式，按下0号键表示光标所在值要加一个单位。
      else
      {
        //1表示加1个单位。
        DebugModifyValue(1);
      }
      
      return 1;
    }
    else
    {      
    }
    
  }
  
  return 1;
}

//光标位置标注
//num为光标位置0~14
uint8 DebugModifyCursorMark(void)
{    
  
  if(DebugModifyCursor == 0)
  {
    LCD_P6x8Str(0, 1, "#");
    if(DebugModifyCursorPre == 1)
    {
      LCD_P6x8Str(0, 2, " ");
    }
    else if(DebugModifyCursorPre == 14)
    {
      LCD_P6x8Str(64, 7, " ");
    }
    else
    {     
    }
  }
  else if(DebugModifyCursor == 1)
  {
    LCD_P6x8Str(0, 2, "#");
    if(DebugModifyCursorPre == 2)
    {
      LCD_P6x8Str(0, 3, " ");
    }
    else if(DebugModifyCursorPre == 0)
    {
      LCD_P6x8Str(0, 1, " ");
    }
    else
    {     
    }
  }
  else if(DebugModifyCursor == 2)
  {
    LCD_P6x8Str(0, 3, "#");
    if(DebugModifyCursorPre == 3)
    {
      LCD_P6x8Str(0, 4, " ");
    }
    else if(DebugModifyCursorPre == 1)
    {
      LCD_P6x8Str(0, 2, " ");
    }
    else
    {     
    }
  }  
  else if(DebugModifyCursor == 3)
  {
    LCD_P6x8Str(0, 4, "#");
    if(DebugModifyCursorPre == 4)
    {
      LCD_P6x8Str(0, 5, " ");
    }
    else if(DebugModifyCursorPre == 2)
    {
      LCD_P6x8Str(0, 3, " ");
    }
    else
    {     
    }
  }  
  else if(DebugModifyCursor == 4)
  {
    LCD_P6x8Str(0, 5, "#");
    if(DebugModifyCursorPre == 5)
    {
      LCD_P6x8Str(0, 6, " ");
    }
    else if(DebugModifyCursorPre == 3)
    {
      LCD_P6x8Str(0, 4, " ");
    }
    else
    {     
    }
  } 
  else if(DebugModifyCursor == 5)
  {
    LCD_P6x8Str(0, 6, "#");
    if(DebugModifyCursorPre == 6)
    {
      LCD_P6x8Str(0, 7, " ");
    }
    else if(DebugModifyCursorPre == 4)
    {
      LCD_P6x8Str(0, 5, " ");
    }
    else
    {     
    }
  }  
  else if(DebugModifyCursor == 6)
  {
    LCD_P6x8Str(0, 7, "#");
    if(DebugModifyCursorPre == 7)
    {
      LCD_P6x8Str(64, 0, " ");
    }
    else if(DebugModifyCursorPre == 5)
    {
      LCD_P6x8Str(0, 6, " ");
    }
    else
    {     
    }
  }
  else if(DebugModifyCursor == 7)
  {
    LCD_P6x8Str(64, 0, "#");
    if(DebugModifyCursorPre == 8)
    {
      LCD_P6x8Str(64, 1, " ");
    }
    else if(DebugModifyCursorPre == 6)
    {
      LCD_P6x8Str(0, 7, " ");
    }
    else
    {     
    }
  }
  else if(DebugModifyCursor == 8)
  {
    LCD_P6x8Str(64, 1, "#");
    if(DebugModifyCursorPre == 9)
    {
      LCD_P6x8Str(64, 2, " ");
    }
    else if(DebugModifyCursorPre == 7)
    {
      LCD_P6x8Str(64, 0, " ");
    }
    else
    {     
    }
  }
  else if(DebugModifyCursor == 9)
  {
    LCD_P6x8Str(64, 2, "#");
    if(DebugModifyCursorPre == 10)
    {
      LCD_P6x8Str(64, 3, " ");
    }
    else if(DebugModifyCursorPre == 8)
    {
      LCD_P6x8Str(64, 1, " ");
    }
    else
    {     
    }
  }
  else if(DebugModifyCursor == 10)
  {
    LCD_P6x8Str(64, 3, "#");
    if(DebugModifyCursorPre == 11)
    {
      LCD_P6x8Str(64, 4, " ");
    }
    else if(DebugModifyCursorPre == 9)
    {
      LCD_P6x8Str(64, 2, " ");
    }
    else
    {     
    }
  }
  else if(DebugModifyCursor == 11)
  {
    LCD_P6x8Str(64, 4, "#");
    if(DebugModifyCursorPre == 12)
    {
      LCD_P6x8Str(64, 5, " ");
    }
    else if(DebugModifyCursorPre == 10)
    {
      LCD_P6x8Str(64, 3, " ");
    }
    else
    {     
    }
  }
  else if(DebugModifyCursor == 12)
  {
    LCD_P6x8Str(64, 5, "#");
    if(DebugModifyCursorPre == 13)
    {
      LCD_P6x8Str(64, 6, " ");
    }
    else if(DebugModifyCursorPre == 11)
    {
      LCD_P6x8Str(64, 4, " ");
    }
    else
    {     
    }
  }
  else if(DebugModifyCursor == 13)
  {
    LCD_P6x8Str(64, 6, "#");
    if(DebugModifyCursorPre == 14)
    {
      LCD_P6x8Str(64, 7, " ");
    }
    else if(DebugModifyCursorPre == 12)
    {
      LCD_P6x8Str(64, 5, " ");
    }
    else
    {     
    }
  }
  else if(DebugModifyCursor == 14)
  {
    LCD_P6x8Str(64, 7, "#");
    if(DebugModifyCursorPre == 0)
    {
      LCD_P6x8Str(0, 1, " ");
    }
    else if(DebugModifyCursorPre == 13)
    {
      LCD_P6x8Str(64, 6, " ");
    }
    else
    {     
    }
  }
  else
  {
  }
  
  return 1;
}


//OLED屏显示信息刷新。
uint8 DebugOLEDUpdate(void)
{
  uint32 temp;
  uint8 i;
    
  //当前为显示界面。
  if(DebugScreenChosen == DebugScreenChosenDisplay)
  {
    //第0页，用来显示调节舵机P值所需要的一些参数。
    //序号+英文名+正负号+数值（没有光标和更改标志）
    //0. DF = DeviFuse                   现偏差。            3位数，有正负。
    //1. DD = DeviNow - DeviPre;         偏差变化率。        3位数，有正负。
    //2. DL = DeviNowLevel               偏差等级。          1位数，无正负。
    //3. DR = DeviRowChosen              偏差选取行。        2位数，无正负。
    //4. SP = ServoPIDPMat[DeviNowLevel] 舵机P值。           3位数，无正负。
    //5. SD = ServoPIDDMat[DeviNowLevel] 舵机D值。           3位数，无正负。
    //6. SV = ServoPIDInputVal-ServoPIDCen       舵机实际值。        3位数，有正负。
    if(DebugDisplayPage == 0)
    {
      //显示界面第0页用的，第1个表示符号，中间3个表示值，最后1个表示字符串末尾。
      uint8 DF00[5] = {0, 0, 0, 0, 0};   //偏差DevuFuse
      uint8 DD01[5] = {0, 0, 0, 0, 0};   //偏差变化率DevuFuse-DeviPre
      uint8 DL02[5] = {0, 0, 0, 0, 0};   //偏差等级DeviNowLevel
      uint8 DR03[5] = {0, 0, 0, 0, 0};   //偏差选取行DeviRowChosen
      uint8 SP04[5] = {0, 0, 0, 0, 0};   //舵机P值ServoPIDPMat[DeviNowLevel]
      uint8 SD05[5] = {0, 0, 0, 0, 0};   //舵机D值ServoPIDDMat[DeviNowLevel]
      uint8 SV06[5] = {0, 0, 0, 0, 0};   //舵机实际值FTM1_C0V-ServoPIDCen
      uint8 QL07[5] = {0, 0, 0, 0, 0};   //左编码器返回值QuadLeftCount
      uint8 QR08[5] = {0, 0, 0, 0, 0};   //右编码器返回值QuadRightCount
      uint8 LS09[5] = {0, 0, 0, 0, 0};   //左线扫描起点BlackLeftLastEdgeStartCol
      uint8 RS10[5] = {0, 0, 0, 0, 0};   //右线扫描起点BlackRighttLastEdgeStartCol
      uint8 CS11[5] = {0, 0, 0, 0, 0};   //扫描起点定位点BlackCenEdgeStartCol
      uint8 LH12[5] = {0, 0, 0, 0, 0};   //左线头BlackLeftHeadLine
      uint8 RH13[5] = {0, 0, 0, 0, 0};   //右线头BlackRightHeadLine

      //第0个显示值。
      if(DeviFuse < 0)
      {
        DF00[0] = 45;      //45代表-
        temp = 0 - DeviFuse;
      }
      else
      {
        DF00[0] = 32;   //32代表空格
        temp = DeviFuse;
      }
      DF00[1] = temp / 100 + 48;           //48代表0
      DF00[2] = (temp % 100) / 10 + 48;
      DF00[3] = (temp % 10) / 1 + 48;
      //第1个显示值。
      if(DeviFuse - DeviPre < 0)
      {
        DD01[0] = 45;
        temp = DeviPre - DeviFuse;
      }
      else
      {
        DD01[0] = 32;
        temp = DeviFuse - DeviPre;        
      }
      DD01[1] = temp / 100 + 48;
      DD01[2] = (temp % 100) / 10 + 48;
      DD01[3] = (temp % 10) / 1 + 48;      
      //第2个显示值。
      temp = BatteryCheckVal;
      DL02[0] = 32;
      DL02[1] = temp / 100 + 48;
      DL02[2] = (temp % 100) / 10 + 48;            
      DL02[3] = (temp % 10) / 1 + 48;
      //第3个显示值。
      temp = Steer_kp[2];
      DR03[0] = 32;
      DR03[1] = temp / 100 + 48;
      DR03[2] = (temp % 100) / 10 + 48;
      DR03[3] = (temp % 10) / 1 + 48;
      //第4个显示值。
      temp = Steer_kp[3];
      SP04[0] = 32;
      SP04[1] = temp / 100 + 48;
      SP04[2] = (temp % 100) / 10 + 48;
      SP04[3] = (temp % 10) / 1 + 48;        
      //第5个显示值。
      temp = Steer_kp[4];
      SD05[0] = 32;
      SD05[1] = temp / 100 + 48;
      SD05[2] = (temp % 100) / 10 + 48;
      SD05[3] = (temp % 10) / 1 + 48;        
      //第6个显示值。
      temp = Steer_kp[5];
      SV06[0] = 32;
      SV06[1] = temp / 100 + 48;
      SV06[2] = (temp % 100) / 10 + 48;
      SV06[3] = (temp % 10) / 1 + 48;         
      //第7个显示值。
      temp = Steer_kp[6];
      QL07[0] = 32;  //空格
      QL07[1] = temp / 100 + 48;
      QL07[2] = (temp % 100) / 10 + 48;
      QL07[3] = (temp % 10) / 1 + 48;
      //第8个显示值。
      temp = Steer_kp[7];
      QR08[0] = 32;  //空格
      QR08[1] = temp / 100 + 48;
      QR08[2] = (temp % 100) / 10 + 48;
      QR08[3] = (temp % 10) / 1 + 48;   
      //第9显示值。
      temp = BlackLeftEdgeStartCol;
      LS09[0] = 32;  //空格
      LS09[1] = temp / 100 + 48;
      LS09[2] = (temp % 100) / 10 + 48;
      LS09[3] = (temp % 10) / 1 + 48;  
      //第10个显示值。
      temp = BlackRightEdgeStartCol;
      RS10[0] = 32;  //空格
      RS10[1] = temp / 100 + 48;
      RS10[2] = (temp % 100) / 10 + 48;
      RS10[3] = (temp % 10) / 1 + 48;  
      //第11个显示值。
      temp = BlackCenEdgeStartCol;
      CS11[0] = 32;  //空格
      CS11[1] = temp / 100 + 48;
      CS11[2] = (temp % 100) / 10 + 48;
      CS11[3] = (temp % 10) / 1 + 48;  
      //第10个显示值。
      temp = BlackLeftHeadLine;
      LH12[0] = 32;  //空格
      LH12[1] = temp / 100 + 48;
      LH12[2] = (temp % 100) / 10 + 48;
      LH12[3] = (temp % 10) / 1 + 48;  
      //第11个显示值。
      temp = BlackRightHeadLine;
      RH13[0] = 32;  //空格
      RH13[1] = temp / 100 + 48;
      RH13[2] = (temp % 100) / 10 + 48;
      RH13[3] = (temp % 10) / 1 + 48;        
      //列，行，字符串。
      LCD_P6x8Str(55, 0, "D_0"); 
 
      LCD_P6x8Str(0, 1, "DF");
      LCD_P6x8Str(0, 2, "DD");
      LCD_P6x8Str(0, 3, "P1");
      LCD_P6x8Str(0, 4, "P2");
      LCD_P6x8Str(0, 5, "P3");
      LCD_P6x8Str(0, 6, "P4");
      LCD_P6x8Str(0, 7, "P5");
      
      LCD_P6x8Str(18, 1, DF00);
      LCD_P6x8Str(18, 2, DD01);
      LCD_P6x8Str(18, 3, DL02);
      LCD_P6x8Str(18, 4, DR03);
      LCD_P6x8Str(18, 5, SP04);
      LCD_P6x8Str(18, 6, SD05);
      LCD_P6x8Str(18, 7, SV06);
      
      LCD_P6x8Str(64, 1, "P6");
      LCD_P6x8Str(64, 2, "P7");
      LCD_P6x8Str(64, 3, "LS");
      LCD_P6x8Str(64, 4, "RS");
      LCD_P6x8Str(64, 5, "CS");
      LCD_P6x8Str(64, 6, "LH");
      LCD_P6x8Str(64, 7, "RH"); 
      
      LCD_P6x8Str(64+18, 1, QL07);
      LCD_P6x8Str(64+18, 2, QR08);
      LCD_P6x8Str(64+18, 3, LS09);
      LCD_P6x8Str(64+18, 4, RS10);
      LCD_P6x8Str(64+18, 5, CS11);
      LCD_P6x8Str(64+18, 6, LH12);
      LCD_P6x8Str(64+18, 7, RH13);

    }
    //第1页。用来显示固定6行的左黑线两边的黑点白点的像素值。
    else if(DebugDisplayPage == 1)
    {
      //显示界面第1页用的。
      uint8 D_1_LB[18][4] = {0, 0, 0, 0};
      uint8 D_1_LW[18][4] = {0, 0, 0, 0};
      
      //建立字符串的尾巴。
      for(i = 0; i < 18; i++)
      {
        D_1_LB[i][3] = 0;
        D_1_LW[i][3] = 0;
      }      
      
      //计算左边18个值和右边18个值，左右边关于中间对称。。
      //12 06 00
      //13 07 01
      //14 08 02
      //15 09 03
      //16 10 04
      //17 11 05
      for(i = 0; i <= 5; i++)
      {
        //当前行的左黑线提取失败。
        if(BlackLeftLoc[9 + i * 10][0] ==  MaxValUint8)
        {
          D_1_LB[i][0] = 32;
          D_1_LB[i][1] = 32;
          D_1_LB[i][2] = 32;   

          D_1_LB[i + 6][0] = 32;
          D_1_LB[i + 6][1] = 32;
          D_1_LB[i + 6][2] = 32;         

          D_1_LB[i + 12][0] = 32;
          D_1_LB[i + 12][1] = 32;
          D_1_LB[i + 12][2] = 32;

          D_1_LW[i][0] = 32;
          D_1_LW[i][1] = 32;
          D_1_LW[i][2] = 32;

          D_1_LW[i + 6][0] = 32;
          D_1_LW[i + 6][1] = 32;
          D_1_LW[i + 6][2] = 32;

          D_1_LW[i + 12][0] = 32;
          D_1_LW[i + 12][1] = 32;
          D_1_LW[i + 12][2] = 32;
        }
        //当前行的左黑线提取成功。
        else
        {
          //0-5表示第3列。
          temp = ImgNew[9 + i * 10][BlackLeftLoc[9 + i * 10][0] + 1 + BlackLeftEdgeNum] - ImgNew[9 + i * 10][BlackLeftLoc[9 + i * 10][0]];
          D_1_LB[i][0] = temp / 100 + 48;
          D_1_LB[i][1] = (temp % 100) / 10 + 48;
          D_1_LB[i][2] = (temp % 10) / 1 + 48;   
          
          //6-11表示第2列。
          temp = ImgNew[9 + i * 10][BlackLeftLoc[9 + i * 10][0]];
          D_1_LB[i + 6][0] = temp / 100 + 48;
          D_1_LB[i + 6][1] = (temp % 100) / 10 + 48;
          D_1_LB[i + 6][2] = (temp % 10) / 1 + 48;         
          
          //12-17表示第1列。
          temp = ImgNew[9 + i * 10][BlackLeftLoc[9 + i * 10][0] - 1];
          D_1_LB[i + 12][0] = temp / 100 + 48;
          D_1_LB[i + 12][1] = (temp % 100) / 10 + 48;
          D_1_LB[i + 12][2] = (temp % 10) / 1 + 48;
  
          //0-5表示第4列。
          temp = ImgNew[9 + i * 10][BlackLeftLoc[9 + i * 10][0] + 1 + BlackLeftEdgeNum];
          D_1_LW[i][0] = temp / 100 + 48;
          D_1_LW[i][1] = (temp % 100) / 10 + 48;
          D_1_LW[i][2] = (temp % 10) / 1 + 48;
          
          //6-11表示第5列。
          temp = ImgNew[9 + i * 10][BlackLeftLoc[9 + i * 10][0] + 2 + BlackLeftEdgeNum];
          D_1_LW[i + 6][0] = temp / 100 + 48;
          D_1_LW[i + 6][1] = (temp % 100) / 10 + 48;
          D_1_LW[i + 6][2] = (temp % 10) / 1 + 48;
          
          //12-17表示第6列。
          temp = ImgNew[9 + i * 10][BlackLeftLoc[9 + i * 10][0] + 3 + BlackLeftEdgeNum];
          D_1_LW[i + 12][0] = temp / 100 + 48;
          D_1_LW[i + 12][1] = (temp % 100) / 10 + 48;
          D_1_LW[i + 12][2] = (temp % 10) / 1 + 48;
        }
      }     
      
      //36个值的显示。
      LCD_P6x8Str(55, 0, "D_1");
      LCD_P6x8Str(14, 1, "LB");
      LCD_P6x8Str(48, 1, "C");
      LCD_P6x8Str(86, 1, "LW");
      
      for(i = 0; i <= 5; i++)
      {
        LCD_P6x8Str(42, i+2, D_1_LB[i]);
        LCD_P6x8Str(21, i+2, D_1_LB[i+6]);
        LCD_P6x8Str(0, i+2, D_1_LB[i+12]);
        LCD_P6x8Str(64, i+2, D_1_LW[i]);
        LCD_P6x8Str(85, i+2, D_1_LW[i+6]);
        LCD_P6x8Str(106, i+2, D_1_LW[i+12]);        
      }
    }
    //第2页.用来显示固定6行的右黑线两边的黑点白点的像素值。
    else if(DebugDisplayPage == 2)
    {
      //显示界面第2页用的。
      uint8 D_2_RB[18][4] = {0, 0, 0, 0};  
      uint8 D_2_RW[18][4] = {0, 0, 0, 0};  
      
      //建立字符串的尾巴。
      for(i = 0; i < 18; i++)
      {
        D_2_RB[i][3] = 0;
        D_2_RW[i][3] = 0;
      }
      
      //计算左边18个值和右边18个值，左右边关于中间对称。。
      //12 06 00
      //13 07 01
      //14 08 02
      //15 09 03
      //16 10 04
      //17 11 05
      for(i = 0; i <= 5; i++)
      {
        //当前行的右黑线提取失败。
        if(BlackRightLoc[9 + i * 10][0] ==  MaxValUint8)
        {
          D_2_RB[i][0] = 32;
          D_2_RB[i][1] = 32;
          D_2_RB[i][2] = 32;

          D_2_RB[i + 6][0] = 32;
          D_2_RB[i + 6][1] = 32;
          D_2_RB[i + 6][2] = 32;

          D_2_RB[i + 12][0] = 32;
          D_2_RB[i + 12][1] = 32;
          D_2_RB[i + 12][2] = 32;

          D_2_RW[i][0] = 32;
          D_2_RW[i][1] = 32;
          D_2_RW[i][2] = 32;

          D_2_RW[i + 6][0] = 32;
          D_2_RW[i + 6][1] = 32;
          D_2_RW[i + 6][2] = 32;

          D_2_RW[i + 12][0] = 32;
          D_2_RW[i + 12][1] = 32;
          D_2_RW[i + 12][2] = 32;
        }
        //当前行的右黑线提取成功。
        else
        {
          //0-5表示第4列。
          temp = ImgNew[9 + i * 10][BlackRightLoc[9 + i * 10][0] - 1 - BlackLeftEdgeNum] - ImgNew[9 + i * 10][BlackRightLoc[9 + i * 10][0]];
          D_2_RB[i][0] = temp / 100 + 48;
          D_2_RB[i][1] = (temp % 100) / 10 + 48;
          D_2_RB[i][2] = (temp % 10) / 1 + 48;
          
          //6-11表示第5列。
          temp = ImgNew[9 + i * 10][BlackRightLoc[9 + i * 10][0]];
          D_2_RB[i + 6][0] = temp / 100 + 48;
          D_2_RB[i + 6][1] = (temp % 100) / 10 + 48;
          D_2_RB[i + 6][2] = (temp % 10) / 1 + 48;
          
          //12-17表示第6列。
          temp = ImgNew[9 + i * 10][BlackRightLoc[9 + i * 10][0] + 1];
          D_2_RB[i + 12][0] = temp / 100 + 48;
          D_2_RB[i + 12][1] = (temp % 100) / 10 + 48;
          D_2_RB[i + 12][2] = (temp % 10) / 1 + 48;
  
          //0-5表示第3列。
          temp = ImgNew[9 + i * 10][BlackRightLoc[9 + i * 10][0] - 1 - BlackLeftEdgeNum];
          D_2_RW[i][0] = temp / 100 + 48;
          D_2_RW[i][1] = (temp % 100) / 10 + 48;
          D_2_RW[i][2] = (temp % 10) / 1 + 48;
          
          //6-11表示第2列。
          temp = ImgNew[9 + i * 10][BlackRightLoc[9 + i * 10][0] - 2 - BlackLeftEdgeNum];
          D_2_RW[i + 6][0] = temp / 100 + 48;
          D_2_RW[i + 6][1] = (temp % 100) / 10 + 48;
          D_2_RW[i + 6][2] = (temp % 10) / 1 + 48;
          
          //12-17表示第1列。
          temp = ImgNew[9 + i * 10][BlackRightLoc[9 + i * 10][0] - 3 - BlackLeftEdgeNum];
          D_2_RW[i + 12][0] = temp / 100 + 48;
          D_2_RW[i + 12][1] = (temp % 100) / 10 + 48;
          D_2_RW[i + 12][2] = (temp % 10) / 1 + 48;          
        }
      }     
      
      //36个值的显示。
      LCD_P6x8Str(55, 0, "D_2");
      LCD_P6x8Str(16, 1, "RW");
      LCD_P6x8Str(70, 1, "C");
      LCD_P6x8Str(98, 1, "RB");
      
      for(i = 0; i <= 5; i++)
      {
        LCD_P6x8Str(42, i+2, D_2_RW[i]);
        LCD_P6x8Str(21, i+2, D_2_RW[i+6]);
        LCD_P6x8Str(0, i+2, D_2_RW[i+12]);
        LCD_P6x8Str(64, i+2, D_2_RB[i]);
        LCD_P6x8Str(85, i+2, D_2_RB[i+6]);
        LCD_P6x8Str(106, i+2, D_2_RB[i+12]);        
      }
    }
    //第3页.用来显示地板的像素值，以确定相关的阈值。
    else if(DebugDisplayPage == 3)
    {
      //显示界面第3页用的。
      uint8 D_3_L[18][4] = {0, 0, 0, 0};
      uint8 D_3_R[18][4] = {0, 0, 0, 0};
      
      //建立字符串的尾巴。
      for(i = 0; i < 18; i++)
      {
        D_3_L[i][3] = 0;
        D_3_R[i][3] = 0;
      }
      
      
      //更新36个值。
      for(i = 0; i <= 5; i++)
      {
        //0-5表示第3列。
        temp = ImgNew[9 + i * 10][CameraRealWidth/2];
        D_3_L[i][0] = temp / 100 + 48;
        D_3_L[i][1] = (temp % 100) / 10 + 48;
        D_3_L[i][2] = (temp % 10) / 1 + 48;
        
        //6-11表示第2列。
        temp = ImgNew[9 + i * 10][CameraRealWidth/2 - 1];
        D_3_L[i + 6][0] = temp / 100 + 48;
        D_3_L[i + 6][1] = (temp % 100) / 10 + 48;
        D_3_L[i + 6][2] = (temp % 10) / 1 + 48;
        
        //12-17表示第1列。
        temp = ImgNew[9 + i * 10][CameraRealWidth/2 - 2];
        D_3_L[i + 12][0] = temp / 100 + 48;
        D_3_L[i + 12][1] = (temp % 100) / 10 + 48;
        D_3_L[i + 12][2] = (temp % 10) / 1 + 48;

        //0-5表示第4列。
        temp = ImgNew[9 + i * 10][CameraRealWidth/2 + 1];
        D_3_R[i][0] = temp / 100 + 48;
        D_3_R[i][1] = (temp % 100) / 10 + 48;
        D_3_R[i][2] = (temp % 10) / 1 + 48;
        
        //6-11表示第5列。
        temp = ImgNew[9 + i * 10][CameraRealWidth/2 + 2];
        D_3_R[i + 6][0] = temp / 100 + 48;
        D_3_R[i + 6][1] = (temp % 100) / 10 + 48;
        D_3_R[i + 6][2] = (temp % 10) / 1 + 48;
        
        //12-17表示第6列。
        temp = ImgNew[9 + i * 10][CameraRealWidth/2 + 3];
        D_3_R[i + 12][0] = temp / 100 + 48;
        D_3_R[i + 12][1] = (temp % 100) / 10 + 48;
        D_3_R[i + 12][2] = (temp % 10) / 1 + 48;      
      }
      
      
      //36个值的显示。
      LCD_P6x8Str(55, 0, "D_3");
      LCD_P6x8Str(0,   1, "123");
      LCD_P6x8Str(21,  1, "124");
      LCD_P6x8Str(42,  1, "125");
      LCD_P6x8Str(64,  1, "126");
      LCD_P6x8Str(85,  1, "127");
      LCD_P6x8Str(106, 1, "128");   
      
      for(i = 0; i <= 5; i++)
      {
        LCD_P6x8Str(42, i+2, D_3_L[i]);
        LCD_P6x8Str(21, i+2, D_3_L[i+6]);
        LCD_P6x8Str(0, i+2, D_3_L[i+12]);
        LCD_P6x8Str(64, i+2, D_3_R[i]);
        LCD_P6x8Str(85, i+2, D_3_R[i+6]);
        LCD_P6x8Str(106, i+2, D_3_R[i+12]);        
      }      
    }
    //画图
    else if(DebugDisplayPage == 4)
    {
      uint8 bmp[127*8];  //127列0~126，8页0~7。
      uint32 j;
      uint8 page;
      //页 第0列 第126列
      //0:   0 ~ 126
      //1: 127 ~ 253
      //2: 254 ~ 380
      //3: 381 ~ 507
      //4: 508 ~ 634
      //5: 635 ~ 761
      //6: 762 ~ 888
      //7: 889 ~ 1015
      //数组清零
      for(j = 0; j < 127 * 8; j++)
      {
        bmp[j] = 0;
      }
      //实际图像绘制60x125，有则写1。有点小难度。
      for(i = 0; i < 60; i++)
      {
        //页码。
        page = i / 8;
        //左黑线，有则写1.
        if(BlackLeftLoc[i][0] != MaxValUint8)
        {
          bmp[127 * page + BlackLeftLoc[i][0] / 2] |= 1 << (i - page * 8);
        }
        else
        {
        }
        //右黑线，有则写1.
        if(BlackRightLoc[i][0] != MaxValUint8)
        {
          bmp[127 * page + BlackRightLoc[i][0] / 2] |= 1 << (i - page * 8);
        }
        else
        {
        }                
      }
      //行标识
      bmp[253]  |= 1 << 1;  //第9行
      bmp[380]  |= 1 << 3;  //第19行
      bmp[507]  |= 1 << 5;  //第29行
      bmp[634]  |= 1 << 7;  //第39行
      bmp[888]  |= 1 << 1;  //第49行
      bmp[1015] |= 1 << 3;  //第59行
      //左线头
      if(BlackLeftHeadLine != MaxValUint8)
      {
        page = BlackLeftHeadLine / 8;
        bmp[127 * page] |= 1 << (BlackLeftHeadLine - page * 8);
      }
      else
      {
      }
      //右线头
      if(BlackRightHeadLine != MaxValUint8)
      {
        page = BlackRightHeadLine / 8;
        bmp[127 * page + 125] |= 1 << (BlackRightHeadLine - page * 8);
      }
      else
      {
      }      
      //列标识
      bmp[889]  |= 1 << 6;
      bmp[889]  |= 1 << 7;  //第0列
      bmp[951]  |= 1 << 6;
      bmp[951]  |= 1 << 7;  //第62列
      bmp[1014]  |= 1 << 6;
      bmp[1014]  |= 1 << 7;  //第125列  
      //左扫描起点
      bmp[889 + BlackLeftEdgeStartCol / 2] |= 1 << 4;
      bmp[889 + BlackLeftEdgeStartCol / 2] |= 1 << 5;
      //右扫描起点
      bmp[889 + BlackRightEdgeStartCol / 2] |= 1 << 4;
      bmp[889 + BlackRightEdgeStartCol / 2] |= 1 << 5;      
      //PutPixel函数有误，只能用DrawBMP函数，且更高效。
      Draw_BMP(0, 0, 127, 7, bmp);      
    }
    //待添加页。
    else 
    {
    }
  }
  //当前为调参界面。
  else
  {
    //调参界面用的。前3个表示值，最后1个表示字符串末尾。
    uint8 tempVal[60][4]; 

    //建立字符串的尾巴。
    for(i = 0; i < 60; i++)
    {
      tempVal[i][3] = 0;
    }
    
    //调参界面的页码，0,1,2,3
    if(DebugModifyPage == 0)
    {
      //第0页的标题。
      LCD_P6x8Str(0, 0, "M_0");
      //修改标志。
      if(DebugModifyFlag == 1)
      {
        LCD_P6x8Str(24, 0, "@");
      }
      else
      {
        LCD_P6x8Str(24, 0, " ");
      }
      
      //光标的标注。
      DebugModifyCursorMark();
      
      //15个参数的计算。
      for(i = 0; i <= 14; i++)
      {
        temp = DebugModifyNum[i];
        tempVal[i][0] = temp / 100 + 48;
        tempVal[i][1] = (temp % 100) / 10 + 48;
        tempVal[i][2] = (temp % 10) / 1 + 48;        
      }
      //15个参数名字的显示。
      LCD_P6x8Str(12, 1, "SPD");
      LCD_P6x8Str(12, 2, "Kp1");
      LCD_P6x8Str(12, 3, "Kp2");
      LCD_P6x8Str(12, 4, "Kp3");
      LCD_P6x8Str(12, 5, "Kp4");
      LCD_P6x8Str(12, 6, "Kp5");
      LCD_P6x8Str(12, 7, "Kp6");
      LCD_P6x8Str(76, 0, "Kp7");
      LCD_P6x8Str(76, 1, "Kp0");
      LCD_P6x8Str(76, 2, "MtI");
      LCD_P6x8Str(76, 3, "MtP");
      LCD_P6x8Str(76, 4, "Mt5");
      LCD_P6x8Str(76, 5, "Mt4");
      LCD_P6x8Str(76, 6, "Mt3");
      LCD_P6x8Str(76, 7, "Mt2");
      //15个参数值的显示。
      LCD_P6x8Str(36, 1, tempVal[0]);
      LCD_P6x8Str(36, 2, tempVal[1]);
      LCD_P6x8Str(36, 3, tempVal[2]);
      LCD_P6x8Str(36, 4, tempVal[3]);
      LCD_P6x8Str(36, 5, tempVal[4]);
      LCD_P6x8Str(36, 6, tempVal[5]);
      LCD_P6x8Str(36, 7, tempVal[6]);
      LCD_P6x8Str(100, 0, tempVal[7]);
      LCD_P6x8Str(100, 1, tempVal[8]);
      LCD_P6x8Str(100, 2, tempVal[9]);
      LCD_P6x8Str(100, 3, tempVal[10]);
      LCD_P6x8Str(100, 4, tempVal[11]);
      LCD_P6x8Str(100, 5, tempVal[12]);
      LCD_P6x8Str(100, 6, tempVal[13]);
      LCD_P6x8Str(100, 7, tempVal[14]);
      
    }
    //调参界面的页码，0,1,2,3
    else if(DebugModifyPage == 1)
    {
      //第1页的标题。
      LCD_P6x8Str(0, 0, "M_1");
      //修改标志。
      if(DebugModifyFlag == 1)
      {
        LCD_P6x8Str(24, 0, "@");
      }
      else
      {
        LCD_P6x8Str(24, 0, " ");
      }
      
      //光标的标注。
      DebugModifyCursorMark();
      
      //15个参数的计算。
      for(i = 15; i <= 29; i++)
      {
        temp = DebugModifyNum[i];
        tempVal[i][0] = temp / 100 + 48;
        tempVal[i][1] = (temp % 100) / 10 + 48;
        tempVal[i][2] = (temp % 10) / 1 + 48;        
      }
      //15个参数名字的显示。
      LCD_P6x8Str(12, 1, "Mt1");
      LCD_P6x8Str(12, 2, "Mt0");
      LCD_P6x8Str(12, 3, "Bci");
      LCD_P6x8Str(12, 4, "Bxu");
      LCD_P6x8Str(12, 5, "WBi");
      LCD_P6x8Str(12, 6, "WBa");
      LCD_P6x8Str(12, 7, "Win");
      LCD_P6x8Str(76, 0, "Wax");
      LCD_P6x8Str(76, 1, "Bin");
      LCD_P6x8Str(76, 2, "Bax");
      LCD_P6x8Str(76, 3, "Zhi");
      LCD_P6x8Str(76, 4, "Zha");
      LCD_P6x8Str(76, 5, "sPo");
      LCD_P6x8Str(76, 6, "xPo");
      LCD_P6x8Str(76, 7, "xZi");
      //15个参数值的显示。
      LCD_P6x8Str(36, 1, tempVal[0+15]);
      LCD_P6x8Str(36, 2, tempVal[1+15]);
      LCD_P6x8Str(36, 3, tempVal[2+15]);
      LCD_P6x8Str(36, 4, tempVal[3+15]);
      LCD_P6x8Str(36, 5, tempVal[4+15]);
      LCD_P6x8Str(36, 6, tempVal[5+15]);
      LCD_P6x8Str(36, 7, tempVal[6+15]);
      LCD_P6x8Str(100, 0, tempVal[7+15]);
      LCD_P6x8Str(100, 1, tempVal[8+15]);
      LCD_P6x8Str(100, 2, tempVal[9+15]);
      LCD_P6x8Str(100, 3, tempVal[10+15]);
      LCD_P6x8Str(100, 4, tempVal[11+15]);
      LCD_P6x8Str(100, 5, tempVal[12+15]);
      LCD_P6x8Str(100, 6, tempVal[13+15]);
      LCD_P6x8Str(100, 7, tempVal[14+15]);  
    }
    //当前是第2页。
    else if(DebugModifyPage == 2)
    {
      //第2页的标题。
      LCD_P6x8Str(0, 0, "M_2");
      //修改标志。
      if(DebugModifyFlag == 1)
      {
        LCD_P6x8Str(24, 0, "@");
      }
      else
      {
        LCD_P6x8Str(24, 0, " ");
      }
      
      //光标的标注。
      DebugModifyCursorMark();
      
      //15个参数的计算。
      for(i = 30; i <= 44; i++)
      {
        temp = DebugModifyNum[i];
        tempVal[i][0] = temp / 100 + 48;
        tempVal[i][1] = (temp % 100) / 10 + 48;
        tempVal[i][2] = (temp % 10) / 1 + 48;        
      }
      //15个参数名字的显示。
      LCD_P6x8Str(12, 1, "C33");
      LCD_P6x8Str(12, 2, "C04");
      LCD_P6x8Str(12, 3, "C14");
      LCD_P6x8Str(12, 4, "C24");
      LCD_P6x8Str(12, 5, "C34");
      LCD_P6x8Str(12, 6, "C05");
      LCD_P6x8Str(12, 7, "C15");
      LCD_P6x8Str(76, 0, "C25");
      LCD_P6x8Str(76, 1, "C35");
      LCD_P6x8Str(76, 2, "C06");
      LCD_P6x8Str(76, 3, "C16");
      LCD_P6x8Str(76, 4, "C26");
      LCD_P6x8Str(76, 5, "C36");
      LCD_P6x8Str(76, 6, "C07");
      LCD_P6x8Str(76, 7, "C17");
      //15个参数值的显示。
      LCD_P6x8Str(36, 1, tempVal[0+30]);
      LCD_P6x8Str(36, 2, tempVal[1+30]);
      LCD_P6x8Str(36, 3, tempVal[2+30]);
      LCD_P6x8Str(36, 4, tempVal[3+30]);
      LCD_P6x8Str(36, 5, tempVal[4+30]);
      LCD_P6x8Str(36, 6, tempVal[5+30]);
      LCD_P6x8Str(36, 7, tempVal[6+30]);
      LCD_P6x8Str(100, 0, tempVal[7+30]);
      LCD_P6x8Str(100, 1, tempVal[8+30]);
      LCD_P6x8Str(100, 2, tempVal[9+30]);
      LCD_P6x8Str(100, 3, tempVal[10+30]);
      LCD_P6x8Str(100, 4, tempVal[11+30]);
      LCD_P6x8Str(100, 5, tempVal[12+30]);
      LCD_P6x8Str(100, 6, tempVal[13+30]);
      LCD_P6x8Str(100, 7, tempVal[14+30]);
    }
    //当前是第3页。
    else if(DebugModifyPage == 3)
    {
      //第2页的标题。
      LCD_P6x8Str(0, 0, "M_2");
      //修改标志。
      if(DebugModifyFlag == 1)
      {
        LCD_P6x8Str(24, 0, "@");
      }
      else
      {
        LCD_P6x8Str(24, 0, " ");
      }
      
      //光标的标注。
      DebugModifyCursorMark();
      
      //15个参数的计算。
      for(i = 45; i <= 59; i++)
      {
        temp = DebugModifyNum[i];
        tempVal[i][0] = temp / 100 + 48;
        tempVal[i][1] = (temp % 100) / 10 + 48;
        tempVal[i][2] = (temp % 10) / 1 + 48;        
      }
      //15个参数名字的显示。
      LCD_P6x8Str(12, 1, "C27");
      LCD_P6x8Str(12, 2, "C37");
      LCD_P6x8Str(12, 3, "Tim");
      LCD_P6x8Str(12, 4, "MoP");
      LCD_P6x8Str(12, 5, "MoD");
      LCD_P6x8Str(12, 6, "STG");
      LCD_P6x8Str(12, 7, "STA");
      LCD_P6x8Str(76, 0, "STL");
      LCD_P6x8Str(76, 1, "Kd1");
      LCD_P6x8Str(76, 2, "Kd2");
      LCD_P6x8Str(76, 3, "Kd3");
      LCD_P6x8Str(76, 4, "Kd4");
      LCD_P6x8Str(76, 5, "Kd5");
      LCD_P6x8Str(76, 6, "Kd6");
      LCD_P6x8Str(76, 7, "Kd7");
      //15个参数值的显示。
      LCD_P6x8Str(36, 1, tempVal[0+45]);
      LCD_P6x8Str(36, 2, tempVal[1+45]);
      LCD_P6x8Str(36, 3, tempVal[2+45]);
      LCD_P6x8Str(36, 4, tempVal[3+45]);
      LCD_P6x8Str(36, 5, tempVal[4+45]);
      LCD_P6x8Str(36, 6, tempVal[5+45]);
      LCD_P6x8Str(36, 7, tempVal[6+45]);
      LCD_P6x8Str(100, 0, tempVal[7+45]);
      LCD_P6x8Str(100, 1, tempVal[8+45]);
      LCD_P6x8Str(100, 2, tempVal[9+45]);
      LCD_P6x8Str(100, 3, tempVal[10+45]);
      LCD_P6x8Str(100, 4, tempVal[11+45]);
      LCD_P6x8Str(100, 5, tempVal[12+45]);
      LCD_P6x8Str(100, 6, tempVal[13+45]);
      LCD_P6x8Str(100, 7, tempVal[14+45]);
    }
    else
    {
    }
  }
  return 1;
}



//Debug调试函数。
uint8 DebugDebug(void)
{
  
  //1. 拨码开关扫描，里面会对DebugSwitchFlag[]标志进行更新。
  DebugSwitchScan();
  
  //2. 按键扫描。里面会对DebugButtonFlag[]标志进行更新。返回确认后的按键值。
  if(DebugButtonScan() == 1)
  {
    //有某个键被按下。
    //3. 按键信息处理
    DebugButtonDeal();
  }
  else
  {
    //没有键被按下，可以不进行第3步操作。
  }
  
  DebugOLEDUpdateCount++;
  if(DebugOLEDUpdateCount > DebugOLEDUpdateCountNum)
  {
    DebugOLEDUpdateCount = 0;
    //4. OLED屏显示信息刷新。刷新时不需要关中断，被打断不会影响它。
    //DisableInterrupts;
    DebugOLEDUpdate();
    //EnableInterrupts;  
  }
  else
  {    
  }
  
  return 1;
}


//将最后的值写入FLASH。
uint8 FlashFinalWrite(void)
{
  uint8 *pointeruint8;         //uint8型数据的指针。
  uint8 i;
  
  //先擦除整个扇区。
  FlashEraseSector(FlashSectorIndex);
  
  for(i = 0; i < 60; i++)
  {
    pointeruint8 = (uint8 *)&DebugModifyNum[i];
    FlashWriteNow(FlashSectorIndex, i, FlashElementSize, pointeruint8);      
  }
  
  return 1;
}
