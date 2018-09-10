#ifndef _IMAGEGET_H
#define _IMAGEGET_H
/******************************************************************************/
/*******************************************************************************
  文件名：图像采集程序头文件IamgeGet.h
  功  能：图像采集
  日  期：2014.10.09
  作  者：HJZ
  备  注：
*******************************************************************************/
/******************************************************************************/
//1. 头文件
//2. 宏定义
//3. 变量类型定义
//4. 变量声明
//5. 函数声明

//1. 头文件
#include <stdio.h>
#include "common.h"
#include "CameraSet.h"
#include "vectors.h"
#include "ImagePro.h"
#include "Debug.h"

//2. 宏定义
#define ImgNO1 1       //当前可用图像标志位宏定义，第1幅图像
#define ImgNO2 2       //当前可用图像标志位宏定义，第2幅图像

#define ModeFlagRun 0               //模式标志，跑车。
#define ModeFlagSend 1              //模式标志，发图。

//4. 变量声明
extern uint8 ModeFlag;                    //模式标志
extern uint8 ImgPresent;   //当前可用图像的标志位
extern uint16 LineCount;          //当前采集的行的计数值
extern uint16 LineRealCount;      //当前实际的行的计数值;
//extern uint8 ImgNew[CameraHight][CameraRealWidth];       //处理后的图像数据储存数组
extern uint8 ImgRaw[CameraHight][CameraWidth];           //B模式时的图像数据储存数组
//extern uint16 const LineRealCountVal[CameraHeight];  //当前实际的行的初始值

extern uint32 ErrorCountNow7;
extern uint32 ErrorCountNow8;

//5. 函数声明
//图像获取函数
extern void ImageGet(void);
//场中断服务函数,下降沿触发中断，暂定为PTD1
extern void FieldIsr(void);
//行中断服务函数,上升沿触发中断，暂定为PTC8
extern void LineIsr(void);

#endif //end of _IMAGEGET_H 