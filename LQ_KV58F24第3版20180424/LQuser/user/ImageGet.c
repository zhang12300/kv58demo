/******************************************************************************/
/*******************************************************************************
  文件名：图像采集程序IamgeGet.c
  功  能：图像采集
  日  期：2014.10.09
  作  者：HJZ
  备  注：

*******************************************************************************/
/******************************************************************************/

#include "ImageGet.h"

uint32 ErrorCountNow7 = 0;
uint32 ErrorCountNow8 = 0;

uint8 ModeFlag = ModeFlagRun;                    //模式标志

uint8 ImgPresent = ImgNO1; //当前可用图像的标志位，初始化为第1幅图像
uint16 LineCount;          //当前采集的行的计数值
uint16 LineRealCount;      //当前实际的行的计数值

uint8 ImgRaw[CameraHight][CameraWidth];      //B模式时的图像数据储存数组

//uint8 ImgRaw[CameraHight][CameraWidth];           //图像原始储存数组
//uint8 LineCount_Index;
/*
uint8 const LineRealCountVal01[CameraHight] = //当前实际的行的初始值，隔3行取1行
{
3,6,9,12,15,18,21,24,27,30,33,36,39,42,45,48,51,54,57,60,63,66,69,72,75,78,81,
84,87,90,93,96,99,102,105,108,111,114,117,120,123,126,129,132,135,138,141,144,
147,150,153,156,159,162,165,168,171,174,177,180,183,186,189,192,195,198,201,
204,207,210,213,216,219,222,225,228,231,234,237,238
};
uint8 const LineRealCountVal02[CameraHight] = //当前实际的行的初始值，连续取中间的80行，效果不如隔行取好
{
151,152,153,154,155,156,157,158,159,160,161,162,163,164,165,166,167,168,169,170,
171,172,173,174,175,176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,
191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,208,209,210,
211,212,213,214,215,216,217,218,219,220,221,222,223,224,225,226,227,228,229,230  
};
*/

/*
uint8 const LineRealCountVal03[CameraHight] = 
{
15,18,21,24,27,30,33,36,39,42,45,48,51,54,57,60,63,66,69,72,75,78,81,84,87,90,
93,96,99,102,105,108,111,114,117,120,123,126,129,132,135,138,141,144,147,150,
153,156,159,162
};

uint8 const LineRealCountVal120_01[CameraHight] = 
{
8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,
58,60,62,64,66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,104,106
};


uint8 const LineRealCountVal120_02[CameraHight] = 
{
0,4,8,12,16,20,22,24,26,28,
30,32,34,36,38,40,42,44,46,48,
50,52,54,56,58,60,62,64,66,68,
70,72,74,76,78,80,82,84,86,88,
90,92,94,96,98,102,106,110,114,118  
};


uint8 const LineRealCountReverseVal[CameraHight] = 
{
14,16,18,20,22,24,26,28,30,32,34,36,38,40,42,44,46,48,50,52,54,56,58,60,62,64,
66,68,70,72,74,76,78,80,82,84,86,88,90,92,94,96,98,100,102,104,106,108,110,112  
};
*/

/*
uint8 const LineRealCountVal120_40_01[CameraHight] = 
{
0,3,6,9,12,
15,18,21,24,27,
30,33,36,39,42,
45,48,51,54,57,
60,63,66,69,72,
75,78,81,84,87,
90,93,96,99,102,
105,108,111,114,117  
};
*/

uint8 const LineRealCountVal120_60_01[CameraHight] = 
{
60,62,64,66,68,
70,72,74,76,78,
80,82,84,86,88,
90,92,94,96,98,
100,102,104,106,108,
110,112,114,116,118,
120,122,124,126,128,
130,132,134,136,138,
140,142,144,146,148,
150,152,154,156,158,
160,162,164,166,168,
170,172,174,176,178  
};

uint8 const LineRealCountVal120_60_02[CameraHight] = 
{
//    1,3,5,7,9,11,13,15,17,19,
//    21,23,25,27,29,31,33,35,37,39,
//    41,43,45,47,49,51,53,55,57,59,
//    61,63,65,67,69,71,73,75,77,79,
//    81,83,85,87,89,91,93,95,97,99,
//    101,103,105,107,109,
//    111,113,115,117,119
 
    25,27,29,31,33,35,37,39,
    41,43,45,47,49,51,53,55,57,59,
    61,63,65,67,69,71,73,75,77,79,
    81,83,85,87,89,91,93,95,97,99,
    101,103,105,107,109,111,113,115,117,119,
    121,123,125,127,129,
    131,133,135,137,139,141,143   
        
//    21,23,25,27,29,31,33,35,37,39,
//    41,43,45,47,49,51,53,55,57,59,
//    61,63,65,67,69,71,73,75,77,79,
//    81,83,85,87,89,91,93,95,97,99,
//    101,103,105,107,109,111,113,115,117,119,
//    121,123,125,127,129,
//    131,133,135,137,139        
    //60,62,64,66,68,70,72,74,
//76,78,80,82,84,
//86,88,90,92,94,
//96,98,100,102,104,
//106,108,110,112,114,
//116,118,120,122,124,
//126,128,130,132,134,
//136,138,140,142,144,
//146,148,150,152,154,
//156,158,160,162,164,
//166,168,170,172,174,
//176,178
    
//76,78,80,82,84,
//86,88,90,92,94,
//96,98,100,102,104,
//106,108,110,112,114,
//116,118,120,122,124,
//126,128,130,132,134,
//136,138,140,142,144,
//146,148,150,152,154,
//156,158,160,162,164,
//166,168,170,172,174,
//176,178,180,182,184,
//186,188,190,192,194
};







//图像获取函数
void ImageGet(void)
{
    
    ImgStatus = ImgGetStart;        //图像采集标志位置为开始
    enable_irq(FIELD_IRQ);         //开启图像采集
    
    while(ImgStatus != ImgGetFinish) ; //如果图像采集未结束，则一直等待。
        
}






















