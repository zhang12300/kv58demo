#ifndef __DEBUG_H__
#define __DEBUG_H__
/******************************************************************************/
/*******************************************************************************
  文件名：调试文件Debug.h
  功  能：OLED显示，FLASH参数读写。
  日  期：2015.03.13
  作  者：HJZ
  备  注：
*******************************************************************************/
/******************************************************************************/
//1. 头文件
#include "Ftm.h"
#include "common.h"
#include "Flash.h"
#include "LQ12864.h"
#include "LQ_MT9V034.h"
#include "include.h"

//2. 宏定义

#define DebugOLEDUpdateCountNum 20    //OLED的刷新频率控制，DebugOLEDUpdateCountNum个16ms刷新一次。

//拨码开关
#define SwitchNum 4                //拨码开关的个数。

//按键。
#define ButtonNum 6                 //按键的个数。
#define DebugButtonConfirmNum 10    //按键的确认次数，可以改变大小以适应手速。
                                    //同时也控制着OLED的刷新频率，即多少次16ms周期刷新一次。刷新时所有中断关闭，刷新后开启。

#define DebugScreenChosenModify 0    //界面选择为 调参
#define DebugScreenChosenDisplay 1   //界面选择为 显示

#define DebugDisplayPageNum 5         //显示界面，页码总数。

//扇区读写
#define FlashSectorIndex 224           //扇区读写时，当前扇区的序号。范围0~255，尽量靠后，以免占了程序的空间。
#define FlashSectorSize 2048           //扇区读写时，扇区的大小。与芯片型号有关。DN512表示512KB，共256个扇区，每个扇区2k大小，即2048.
#define FlashElementSize 4             //扇区写入时，单个元素的大小，一般为4个byte.即4.

//3. 变量类型定义
//4. 变量声明
extern uint8 DebugSwitchFlag[SwitchNum];    //拨码开关标志。
extern uint8 DebugButtonFlag[ButtonNum];    //按键标志。
extern uint16 TimeRealRun;                         //定时跑时，时间上限。以0.1s为单位。
extern uint8 TimeRealRunFlag;                  //定时跑标志。0未跑完，1已跑完。

//5. 函数声明
extern uint8 DebugInit(void);
extern uint8 DebugSwitchScan(void);
extern uint8 DebugDebug(void);
extern uint8 DebugButtonScan(void);
extern uint8 FlashWriteNow(uint32 tempsectorindex, uint32 num, uint32 tempelementsize, uint8 *pointeruint8);
extern uint8 FlashFinalWrite(void);



#endif // end of __DEBUG_H__