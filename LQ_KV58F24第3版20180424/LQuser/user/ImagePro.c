/******************************************************************************/
/*******************************************************************************
  文件名：图像处理程序文件ImagePro.c
  功  能：图像处理
  日  期：2014.10.09
  作  者：HJZ
  备  注：
*******************************************************************************/
/******************************************************************************/

#include "ImagePro.h"

//uint16 SW0;
//uint16 SW1;
//uint16 SW2;
//uint16 SW3;
//uint16 SW4;
//uint16 SW5;
//uint16 SW6;
//uint16 SW7;
//uint16 SW8;

uint8 limitStopLineBlackXiu = 30;       //与停车线黑色阈值有关的参数
uint8 ShiZhiFlag = 0;               //十字标志
uint8 ShiZhiCount = 0;              //十字计数
uint16 buXianCount = 0;             //补线修正计数


uint8 xiaoZhiDaoFlag = 0;                 //小直道

uint8 ImgProFailCount = 0;          //图像失败计数
uint8 RunOutFlag = 0;               //跑出赛道标志

uint8 StopLineCheckBeginFlag = 0;       //开始判断停车线标志
uint8 StopLineCheckTimer = 0;           //停车线计时
uint8 StopLineCheckFlag = 0;            //停车线标志
int16 ErrorCountNow5 = 0;
uint8 flag000;
int32 flag111;
int flag222;
//uint8 countcount;

int ServoPIDDGetVal;
int ServoPIDDGetValOld;

//中心线提取时，实际赛道宽度的一半对应的像素点数。是固定角度后测得的值。
uint8 const CenterLineHalfWidth[CameraHight] =  
{
64, 68, 72, 76, 80, 84, 88, 92, 96, 100,
104,108,112,116,120,124,128,132,136,140,
143,147,150,153,156,159,162,165,168,172,
177,179,181,183,185,187,189,191,193,195,
213,216,219,222,225,228,231,234,237,240,
100,100,100,100,100,100,100,100,100,100
};


uint8 const BlackLeftEdgeStartColOffset[CameraHight] = //向左检测跳变沿的时候，起始列相对CameraRealWidth/2的向右偏移的列数 
{
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,         //远处20行
40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,         //中间20行
43,43,45,45,48,48,50,50,53,53,55,55,58,58,60,60,63,63,65,65          //近处20行
};


uint8 const BlackRightEdgeStartColOffset[CameraHight] = //向右检测跳变沿的时候，起始列相对CameraRealWidth/2的向左偏移的列数 
{
10,10,10,10,10,10,10,10,10,10,20,20,20,20,20,20,20,20,30,40,         //远处20行
40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,         //中间20行
43,43,45,45,48,48,50,50,53,53,55,55,58,58,60,60,63,63,65,65          //近处20行
};

//直角弯
//直角弯特殊处理时，对DeviNow的特殊赋值。影响舵机控制。
//应使之较大，使舵机打角较大。
//最多用AngleSpeDealBackCountNumMax次，越高速用的次数越少。
int16 const AngleSpeDealDeviNow[AngleSpeDealBackCountNum] = 
{
  //100, 100, 100, 100, 100,
  //100, 100, 100, 100, 100
50,100,130,130,130,
130,130,130,130,130,
130,130,100,100,100
};

//直角弯特殊处理时，对DeviNowLevel的特殊赋值。0~6，0偏差最小，6偏差最大。影响舵机和电机控制。
//应使之较大，使速度较慢。
//最多用AngleSpeDealBackCountNumMax次，越高速用的次数越少。
/*
uint8 const AngleSpeDealDeviNowLevel[AngleSpeDealBackCountNum] = 
{
  6, 6, 6, 6, 6,
  6, 6, 6, 6, 6
};
*/

//直角弯特殊处理时，对SlopeC的特殊赋值。影响电机控制。
//影响SlopeCLevel.0~5，0最直，5最斜。
//应使之较斜，使速度较慢。
//最多用AngleSpeDealBackCountNumMax次，越高速用的次数越少。
int32 const AngleSpeDealSlopeC[AngleSpeDealBackCountNum] = 
{
  //3000, 3000, 3000, 3000, 3000,
  //3000, 3000, 3000, 3000, 3000
1500,2400,3000,3000,3000,
3000,3000,3000,2850,2550,
2250,2040,1950,1860,1800
};

//DeviNow低通滤波时的权重,相加之和为100.不可修改。
uint8 const DeviNowArrayWight[DeviNowArrayNum] = 
{
  0, 0, 100
};

//DeviNow低通滤波时的储存数组,初始化为0;可修改。
int16 DeviNowArray[DeviNowArrayNum] = 
{
  0, 0, 0
};


//单线处理
//单线处理时，允许的黑线宽度的最小值。越远则越小。
uint8 const SingleBlackWidthMin[CameraHight] = 
{
5,5,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,5,5,5,
6,6,6,6,6,6,6,6,6,6,
6,6,6,6,6,6,6,6,6,6,
7,7,7,7,7,7,7,7,7,7,
7,7,7,7,7,7,7,7,7,7
};

//单线处理时，允许的黑线宽度的最大值。越远则越小。
uint8 const SingleBlackWidthMax[CameraHight] = 
{
20,20,20,20,20,20,20,20,20,20,
20,20,20,20,20,20,20,20,20,20,
20,20,20,20,20,20,20,20,20,20,
20,20,20,20,20,20,20,20,20,20,
20,20,20,20,20,20,20,20,20,20,
20,20,20,20,20,15,15,15,15,15    //最近的几行15列就够了，太多了会采到旁边的赛道。
};

/*
uint8 const SingleBlackWidthMax[CameraHight] = 
{
1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,
};
*/

//畸变矫正
 //远处的黑线畸变矫正数组
int32 const BlackUdisMatrixFar[8] = 
{
        2950,
        4062,
     -243750,
           0,
       23420,
      -22387,
           0,
          32,
};
        //中间处的黑线畸变矫正数组
int32 const BlackUdisMatrixMiddle[8] = 
{
        2950,
        4062,
     -243750,
           0,
       23420,
      -22387,
           0,
          32,
};

        //近处的黑线畸变矫正数组
int32 const BlackUdisMatrixLow[8] = 
{
       2950,
        4062,
     -243750,
           0,
       23420,
      -22387,
           0,
          32,
};

//对左黑线差值阈值的微调，因为越远的地方差值会越小。
uint8 const LimitLeftWBAdjust[CameraHight] = 
{
10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
 5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

//对右黑线差值阈值的微调，因为越远的地方差值会越小。
uint8 const LimitRightWBAdjust[CameraHight] = 
{
10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
 5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};



uint8 CameraRealLeftCol = CameraWidth/2 - CameraRealWidth/2;    //ImgPut()函数中，320列图像取250列的真正起始列，用以修正机械偏差。
//只有图像处理成功了，才会进行赛道类型判断，才会进行偏差获取，舵机电机控制值才会更新。
uint8 ImgProSucceedFlag = 1;                 //图像处理成功标志位，默认为1，若黑线或中心线提取失败，则置0.
uint8 ImgBlackSucceedFlag = 1;                 //黑线提取成功标志位，默认为1，若黑线提取失败，则置0.
uint8 ImgCenterSucceedFlag = 1;                //中心线提取成功标志位，默认为1，若中心线提取失败，则置0.

uint8 ImgNew[CameraHight][CameraRealWidth];       //处理后的图像数据储存数组
uint8 BlackLeftLoc[CameraHight][3];       //左黑线位置存储数组,每行3个可疑点， 255为无效值
uint8 BlackRightLoc[CameraHight][3];      //右黑线位置存储数组,每行3个可疑点， 255为无效值
//uint8 MaxValUint8 = 255;             //自定义的无效值
uint8 CenterLineResult = 1;             //中心线提取成功标志
uint8 CenterKeyLine = 25;                  //固定的要选取的行。
uint8 CenterLocStore = MaxValUint8;         //左右黑线都提取到的时候，保存的CenterLineLoc[CameraHight - 1]。
uint8 CenterHeadLine = MaxValUint8;         //中心线的线头。
uint8 CenterEndLine = MaxValUint8;          //中心线的线尾。
uint8 BlackCenEdgeStartCol = 0;             //扫描起点定位点
uint8 BlackGetPreDir;                       //黑线预扫描方向。0左线头成功，1右线头成功，2左右线头均失败。

uint8 CurveLineChosenC1 = 0;               //曲率求取时，中心线选取的第1行。
uint8 CurveLineChosenC2 = 0;               //曲率求取时，中心线选取的第1行。
uint8 CurveLineChosenC3 = 0;               //曲率求取时，中心线选取的第1行。
uint8 CurveLineChosenL1 = 0;               //曲率求取时，左黑线选取的第1行。
uint8 CurveLineChosenL2 = 0;               //曲率求取时，左黑线选取的第1行。
uint8 CurveLineChosenL3 = 0;               //曲率求取时，左黑线选取的第1行。
uint8 CurveLineChosenR1 = 0;               //曲率求取时，右黑线选取的第1行。
uint8 CurveLineChosenR2 = 0;               //曲率求取时，右黑线选取的第1行。
uint8 CurveLineChosenR3 = 0;               //曲率求取时，右黑线选取的第1行。
int16 SABCL = 0;                          //曲率求取时，左黑线曲率三角形的面积，顺时针为负，逆时针为正。
int16 SABCC = 0;                          //曲率求取时，中心线曲率三角形的面积，顺时针为负，逆时针为正。
int16 SABCR = 0;                          //曲率求取时，右黑线曲率三角形的面积，顺时针为负，逆时针为正。
int16 CURVEL = 0;                          //曲率求取时，左黑线的曲率，顺时钟为负，逆时针为正。
int16 CURVEC = 0;                          //曲率求取时，中心线的曲率，顺时钟为负，逆时针为正。
int16 CURVER = 0;                          //曲率求取时，右黑线的曲率，顺时钟为负，逆时针为正。
//int16 CurveLeftLimitL = 8;               //赛道类型判断时，通过左黑线曲率判断赛道为左弯的阈值。很敏感，任何硬软件改变后需手动改动此值。
//int16 CurveLeftLimitC = 10;              //赛道类型判断时，通过中心线曲率判断赛道为左弯的阈值。很敏感，任何硬软件改变后需手动改动此值。
//int16 CurveLeftLimitR = 12;              //赛道类型判断时，通过右黑线曲率判断赛道为左弯的阈值。很敏感，任何硬软件改变后需手动改动此值。
//int16 CurveRightLimitL = 12;             //赛道类型判断时，通过左黑线曲率判断赛道为右弯的阈值。很敏感，任何硬软件改变后需手动改动此值。
//int16 CurveRightLimitC = 10;             //赛道类型判断时，通过中心线曲率判断赛道为右弯的阈值。很敏感，任何硬软件改变后需手动改动此值。
//int16 CurveRightLimitR = 8;              //赛道类型判断时，通过右黑线曲率判断赛道为右弯的阈值。很敏感，任何硬软件改变后需手动改动此值。
int32 SlopeL = MaxValInt16;                           //斜率求取时，左黑线的斜率。
int32 SlopeC = MaxValInt16;                           //斜率求取时，中心线的斜率。
int32 SlopeR = MaxValInt16;                           //斜率求取时，右黑线的斜率。

uint8 CurveSlopeFlagL = 1;                  //左黑线曲率、斜率求取的标志位，1成功，0失败(CURVE和Slope均置了无效值)
uint8 CurveSlopeFlagC = 1;                  //中心线曲率、斜率求取的标志位，1成功，0失败(CURVE和Slope均置了无效值)
uint8 CurveSlopeFlagR = 1;                  //右黑线曲率、斜率求取的标志位，1成功，0失败(CURVE和Slope均置了无效值)

uint8 ErrorGetSelf;                 //平均值与自身的偏差，也就是curve
uint8 ErrorGetCen;                 //平均值与物理中心线的偏差。

//左
uint8 BlackLeftRealWB[2];                  //黑线的实际黑白差值的存储数组，用于计算实际黑白差值的阈值。替代LimitLeftWB
uint8 BlackLeftRealB[2];                   //黑线的实际黑点值存储数组，用于计算实际黑点阈值。替代LimitLeftB
uint8 BlackLeftRealW[2];                   //黑线的实际白点值存储数组，用于计算实际白点阈值。替代LimitLeftW
//3个关键值的上下界
uint8 LimitLeftWBMin = 35;
uint8 LimitLeftWBMax = 90;
uint8 LimitLeftWMin = 90;
uint8 LimitLeftWMax = 140;
uint8 LimitLeftBMin = 80;
uint8 LimitLeftBMax = 110;


//右
uint8 BlackRightRealWB[2];                  //黑线的实际黑白差值的存储数组，用于计算实际黑白差值的阈值。替代LimitRightWB
uint8 BlackRightRealB[2];                   //黑线的实际黑点值存储数组，用于计算实际黑点阈值。替代LimitRightB
uint8 BlackRightRealW[2];                   //黑线的实际白点值存储数组，用于计算实际白点阈值。替代LimitRightW
//3个关键值的上下界
uint8 LimitRightWBMin = 35;
uint8 LimitRightWBMax = 90;
uint8 LimitRightWMin = 90;
uint8 LimitRightWMax = 140;
uint8 LimitRightBMin = 80;
uint8 LimitRightBMax = 110;

 
//左
//小5点斜率储存
uint8 BlackLeft5SlopeIndex = 0;          //左线的小5点斜率储存数组的下标。
uint8 BlackLeft5SlopeRow[CameraHight];      //右线的小5点斜率储存，计算小5点斜率时的最远点所在行。
int32 BlackLeft5Slope[CameraHight];      //左线的小5点斜率储存，从近往远储存。大跳变点不会储存进来。
//3个关键值的可改值，可由自适应性算法更改。
uint8 LimitLeftWB;    //黑白像素点的差值，两个点的差必须要大于这个值，才能认为有跳变。
uint8 LimitLeftW;      //白点必须要大于此值
uint8 LimitLeftB;      //黑点必须要小于此值

//每幅有效图像的左线线头对应的3个阈值，单独储存，每幅有效图像刷新一次。
uint8 LimitLeftHeadWB;    
uint8 LimitLeftHeadW;      
uint8 LimitLeftHeadB;      

uint8 BlackLeftCrossDone = 0;    //左黑线十字检测标志位 0未进入 1失败 2成功
uint8 BlackLeftDone = 1;          //左黑线提取成功标示位 1成功 0失败
uint8 BlackLeft3ResetCount = 0;       //由于光线变化太大引起的3个关键值重置的次数。
uint8 BlackLeftCrossTooLowLine = 3;    //十字再检测时，低于该行就没必要启动再检测程序了。
uint8 BlackLeftStep4Flag = 0;          //左黑线需要执行第4阶段的标志位，0未进入，1失败，2成功，3出错。
uint8 BlackLeftStep4StartLine = MaxValUint8;     //左黑线第4阶段起始行
uint8 BlackLeftHeadLine = MaxValUint8;   //左黑线的线头，从上往下。
uint8 BlackLeftEndLine = MaxValUint8;    //左黑线的线尾，从上往下。
uint8 BlackLeftWinPredictStore0 = 0;         //左黑线窗口内检测黑点时，防止预测点更新失败而储存的预测点值0.
uint8 BlackLeftWinPredictStore1 = 0;         //左黑线窗口内检测黑点时，防止预测点更新失败而储存的预测点值1.
uint8 BlackLeftWinPredictStore2 = 0;         //左黑线窗口内检测黑点时，防止预测点更新失败而储存的预测点值2.
uint8 BlackLeftContinueFlag = 1;           //左黑线连续性标志，1连续或采线未成功，0不连续，2出错。
uint8 BlackLeftCrossConfirmFlag = 0;  //左十字线头标志。0未进入，1超范围，2成功。默认为未进入。  
uint8 BlackLeftEdgeStartCol;      //左黑线检测跳变沿时的下一幅图的起始列，每幅图根据上一幅有效图像来估计，需要考虑到单线的宽度。
uint8 BlackLeftLastEdgeStartCol;  //左黑线检测跳变沿时的上一幅图的起始列（也就是这一幅图），每幅图根据上一幅有效图像来估计，需要考虑到单线的宽度。
uint8 BlackLeftStep1ScanRow;      //左黑线第1阶段线头检测时的返回行。
uint8 BlackLeftRow1RealLastLine;       //左黑线十字检测，第1段黑线的真实结束行，有可能因为大跳变被删掉，但差别不会超过3行。

//右
//小5点斜率储存
uint8 BlackRight5SlopeIndex = 0;          //右线的小5点斜率储存数组的下标。
uint8 BlackRight5SlopeRow[CameraHight];      //右线的小5点斜率储存，计算小5点斜率时的最远点所在行。
int32 BlackRight5Slope[CameraHight];      //右线的小5点斜率储存，从近往远储存。大跳变点不会储存进来。
//3个关键值的可改值，可由自适应性算法更改。
uint8 LimitRightWB;    //黑白像素点的差值，两个点的差必须要大于这个值，才能认为有跳变。
uint8 LimitRightW;      //白点必须要大于此值
uint8 LimitRightB;      //黑点必须要小于此值

//每幅有效图像的右线线头对应的3个阈值，单独储存，每幅有效图像刷新一次。
uint8 LimitRightHeadWB;    
uint8 LimitRightHeadW;      
uint8 LimitRightHeadB;      
uint8 BlackRightCrossDone = 0;   //右黑线十字检测标志位 0未进入 1失败 2成功
uint8 BlackRightDone = 1;          //右黑线提取成功标示位 1成功 0失败
uint8 BlackRight3ResetCount = 0;       //由于光线变化太大引起的3个关键值重置的次数。
uint8 BlackRightCrossTooLowLine = 3;    //十字再检测时，低于该行就没必要启动再检测程序了。
uint8 BlackRightStep4Flag = 0;          //右黑线需要执行第4阶段的标志位，0未进入，1失败，2成功，3出错。
uint8 BlackRightStep4StartLine = MaxValUint8;     //右黑线第4阶段起始行
uint8 BlackRightHeadLine = MaxValUint8;    //右黑线的线头，从上往下。
uint8 BlackRightEndLine = MaxValUint8;     //右黑线的线尾，从上往下。
uint8 BlackRightWinPredictStore0 = 0;         //右黑线窗口内检测黑点时，防止预测点更新失败而储存的预测点值0.
uint8 BlackRightWinPredictStore1 = 0;         //右黑线窗口内检测黑点时，防止预测点更新失败而储存的预测点值1.
uint8 BlackRightWinPredictStore2 = 0;         //右黑线窗口内检测黑点时，防止预测点更新失败而储存的预测点值2.
uint8 BlackRightContinueFlag = 1;           //右黑线连续性标志，1连续或采线未成功，0不连续，2出错。
uint8 BlackRightCrossConfirmFlag = 0;  //右十字线头标志。0未进入，1超范围，2成功。默认为未进入。
uint8 BlackRightEdgeStartCol;      //右黑线检测跳变沿时的下一幅图的起始列，每幅图根据上一幅有效图像来估计，需要考虑到单线的宽度。
uint8 BlackRightLastEdgeStartCol;  //右黑线检测跳变沿时的上一幅图的起始列（也就是这一幅图），每幅图根据上一幅有效图像来估计，需要考虑到单线的宽度。
uint8 BlackRightStep1ScanRow;      //右黑线第1阶段线头检测时的返回行。
uint8 BlackRightRow1RealLastLine;       //右黑线十字检测，第1段黑线的真实结束行，有可能因为大跳变被删掉，但差别不会超过3行。

//紧急停车
uint8 EmergencyStopCount = 0;     //紧急停车计数器。每当图像处理失败时加1，连续计EmergencyStopCountNum个则置标志位。
uint8 EmergencyStopFlag = 0;      //紧急停车标志。1表示紧急停车，会将电机输出置0，只有掉电重开才能置0.


//偏差获取
//int16 DeviNow = 0;                  //当前偏差
uint8 DeviFlag = 0;                //偏差求取标志位，告诉程序这次应该写入第几个偏差。
uint8 DeviRowChosen = 0;               //求取偏差时选取的行，若指定范围没选取到则置无效值，若偏差过小，则置为0。
uint8 DeviNowLevel = 0;      //当前偏差的绝对值的等级，10个等级，0偏差最小，9偏差最大。

//直角黑色区域
//判断
uint8 AngleZoneJudgeRowL[AngleZoneJudgeRowNum];  //直角黑块，根据线头选中的要检测的左线行。
uint8 AngleZoneJudgeColL[AngleZoneJudgeRowNum];  //直角黑块，左线定位的所在列。
uint8 AngleZoneJudgeColLFlag[AngleZoneJudgeRowNum]; //直角黑块，左线该行的有效黑点满足要求时置位。
uint8 AngleZoneJudgeCountColL[AngleZoneJudgeRowNum]; //直角黑块，左线，记录该行的有效黑点数，以供确定合适的阈值。
uint8 AngleZoneJudgeRowR[AngleZoneJudgeRowNum];  //直角黑块，根据线头选中的要检测的右线行。
uint8 AngleZoneJudgeColR[AngleZoneJudgeRowNum];  //直角黑块，右线定位的所在列。
uint8 AngleZoneJudgeColRFlag[AngleZoneJudgeRowNum]; //直角黑块，右线该行的有效黑点满足要求时置位。
uint8 AngleZoneJudgeCountColR[AngleZoneJudgeRowNum]; //直角黑块，右线，记录该行的有效黑点数，以供确定合适的阈值。
//确认
uint8 AngleZoneConfirmLockMeterCount = 0; //直角黑块，锁存后进行的累加计数，用以直角黑块误判后的恢复正常。
uint8 AngleZoneConfirmCount = 0;   //对直角黑块确认次数的计数。
uint8 AngleZoneConfirmLockFlag = 0;    //直角黑块判断成功锁存标志位,确认是直角黑块后置位。
                                       //有两种情况清零，一是之后的线头差检测成功后，二是一定距离后仍没有检测到线头差。
uint8 AngleZoneConfirmMat[AngleZoneConfirmMatNum];  //直角黑块确认标志存储数组，1表示本次为直角黑块，0表示不是直角黑块。
uint8 AngleZoneConfirmMatCollect = 0;      //直角黑块，AngleZoneConfirmMat数组中1的个数。
//可修改
uint8 AngleZoneConfirmMatLimit = 3;         //直角黑块，标志存储数组中直角黑块的个数大于该值时，则锁存为直角黑块。

//直角弯处理
uint8 AngleConfirmCount = 0;   //对直角弯确认次数的计数。
uint8 AngleSpeDealBackCount = 0;  //直角弯最终确认后，特殊情况次数的倒数计数。
uint8 AngleSpeDealDir = 0;        //直角弯方向标志，0左直角弯，1右直角弯，2错误。
uint8 AngleSpeDealFlag = 0;       //直角弯特殊处理标志，1表示该周期进行了直角弯特殊处理，0表示该周期没有进行直角弯特殊处理。
uint16 AngleConfirmCountMax = 0;   //记录实际直角弯确认次数的最大值，方便调节阈值。
uint8 AngleConfirmLockFlag = 0;    //直角弯判断成功锁存标志位,确认是直角弯后置位，用以确认直角弯特殊处理开始的位置。
                                   //直角弯锁存期间，不进行特殊赛道判断。
uint8 AngleSpeDealExitJudgeFlag = 0;  //出直角弯判断标志，1已出，0未出
uint8 AngleConfirmMat[AngleConfirmMatNum];  //直角弯确认标志存储数组，1表示本次为直角，0表示不是直角。
uint8 AngleConfirmMatCollect = 0;      //直角弯 ，AngleConfirmMat数组中1的个数。

uint8 AngleConfirmMatLimit = 3;         //直角弯标志存储数组中直角的个数大于该值时，则锁存为直角。
uint8 AngleLongHeadLineStrategy = 1;                //直角弯长线头策略，1表示去掉长线头，0表示不去长线头。
uint8 AngleHeadLineNum = 5;              //左右线头之差大于该值，则有可能为直角弯。


//直角弯非障碍物确认。
uint8 AngleStep1NotBrickEndColL;    //直角弯非障碍物确认，左直角梯形，起始行（近行）黑点所在列。
uint8 AngleStep1NotBrickBorderColL; //直角弯非障碍物确认，左直角梯形，直角边所在列。
uint8 AngleStep1NotBrickEndColR;    //直角弯非障碍物确认，右直角梯形，起始行（近行）黑点所在列。
uint8 AngleStep1NotBrickBorderColR; //直角弯非障碍物确认，右直角梯形，直角边所在列。

//障碍物
uint8 BrickConfirmCount = 0;   //对障碍物确认次数的计数。
uint8 BrickSpeDealBackCount = 0;  //障碍物最终确认后，特殊情况次数的倒数计数。
uint8 BrickSpeDealDir = 0;        //障碍物位置，0左障碍物，1右障碍物，2错误。
uint8 BrickSpeDealFlag = 0;       //障碍物特殊处理标志，1表示该周期进行了障碍物特殊处理，0表示该周期没有进行障碍物特殊处理。
uint16 BrickConfirmCountMax = 0;   //记录实际障碍物确认次数的最大值，方便调节阈值。
uint8 BrickConfirmLockFlag = 0;    //障碍物判断成功锁存标志位,确认是障碍物后置位，用以确认障碍物特殊处理开始的位置。
                                   //障碍物锁存期间，不进行特殊赛道判断。
uint8 BrickSpeDealExitJudgeFlag = 1;  //出障碍物判断标志，1表示初始态，2表示当前已经与障碍平行，0退出特殊控制，把控制权交还给普通控制。
                                      //不用每幅图更新，每次过完障碍，会手动写成1.
uint8 BrickConfirmMat0[BrickConfirmMatNum];  //左障碍物确认标志存储数组，1表示本次为障碍物，0表示不是障碍物。
uint8 BrickConfirmMatCollect0 = 0;      //左障碍，BrickConfirmMat数组中1的个数。
uint8 BrickConfirmMat1[BrickConfirmMatNum];  //右障碍物确认标志存储数组，1表示本次为障碍物，0表示不是障碍物。
uint8 BrickConfirmMatCollect1 = 0;      //右障碍，BrickConfirmMat数组中1的个数。

uint8 BrickSpeDealStartJudgeCol;     //障碍，判定是否开始特殊控制时，特定行对应的最小二乘法算出来的列。
uint8 BrickEndColL;    //障碍物，左直角梯形，起始行（近行）黑点所在列。
uint8 BrickHeadColL;   //障碍物，左直角梯形，结束行（远行）黑点所在列。
uint8 BrickBorderColL; //障碍物，左直角梯形，直角边所在列。
uint8 BrickEndColR;    //障碍物，右直角梯形，起始行（近行）黑点所在列。
uint8 BrickHeadColR;   //障碍物，右直角梯形，结束行（远行）黑点所在列。
uint8 BrickBorderColR; //障碍物，右直角梯形，直角边所在列。
uint8 BrickSpeDealCenFlag = 1; //障碍物，中心线特殊处理标志位，每幅图像会预置1。1表示无特殊处理或处理成功，0表示处理失败。
uint8 BrickLineCountNumRecord;
//障碍，特殊控制时，偏移的列数。从数字大的开始执行，到数字小的。
const int BrickSpeDealBias[BrickSpeDealBackCountNum] = 
{
  30,30,30,25,25,  
  25,25,25,20, 20,
   20, 15, 5, 0, -2,
   -5, -10, -10, -19, -19
};

uint8 BrickConfirmMatLimit = 3;         //障碍标志存储数组中障碍的个数大于该值时，则锁存为障碍。
uint8 BrickConfirmFinalColCom = 50;     //障碍躲避时的列修正量（以50为零，左减右加。）比50小表示往中心线靠，比50大表示往外走。

//单线处理
uint8 SingleBlackAllFlag = 0;   //总幅图像的单线标志，只要有一行检测单线成功，则置1.每幅图像处理前置0.
uint8 SingleBlackLastAllFlag = 0;      //上一幅图像的单线标志。
uint8 SingleDoneFlag = 0;              //单线处理时，单线成功标志位。
                         //////////////////////目前还不完善，是每个有效行验证一次，一次成功则视为成功。
uint8 SingleFloorFlag = 0;             //单线处理时，地板判断成功标志位。1该幅图画左线是地板，0不是或没有判断。
uint8 SingleFloorConfirmCount = 0;       //单线处理时，地板判断确认计数值。每幅图归零一次。
uint8 SingleFloorConfirmCountMax = 0;   //单线处理时，地板判断确认计数值的最大值，用于确认最佳确认阈值。每幅图归零一次。
uint8 SingleBlackFlag[CameraHight];     //单线处理时，每行的单线判断成功标志。1该行为单线，0该行不是单线。每次图像处理前会置0.

//赛道类型
uint8 PathJudgeDone = 1;                   //赛道类型判断成功标志位，1成功，0失败。
uint8 PathType = PathTypeUnknown;          //未知 直道 左弯 右弯 左直 右直 小S
uint8 PathSmallSDoneFlag = 0;              //赛道类型判断时，小S弯判断成功标志位。1成功，0失败。每幅图赛道类型判断时初始化为0。
uint8 PathSmallSDir = 1;                   //赛道类型判断时，小S弯起始方向。

//畸变矫正
int16 CenterLineLoc[CameraHight];      //中心线位置存储数组, 255为无效值
LocationUdis BlackUdisLeftLocation[CameraHight];  //畸变矫正后，左黑线的位置储存数组。有正负，包括了行和列。
LocationUdis BlackUdisRightLocation[CameraHight]; //畸变矫正后，左黑线的位置储存数组。有正负，包括了行和列。
//多行控制
int32 DeviFuse = 0;            //多行融合后的偏差。
int32 DeviFuse_old = 0;
uint32 DeviFuseArray[10];
int32 Devi_near,Devi_far;      //近远行偏移量
int32 DeviPre = 0;                  //上一次偏差
int32 DeviPrePre=0;              //上上次偏差
int32 DeviPrePrePre=0;              //上上上次偏差
int16 ControlRowDeviInit[4];    //各控制行的初始偏差,归一的0~100,0~3分别是base，low，middle，high
int16 ControlRowDeviOld[4]; 
uint8 BaseRowControlNum;        //近行控制序号，对应于权重数组列下标
uint8 FuzzyDegree;              //隶属度计算辅助变量
uint8 ControlRowWeight[4];      //控制行权重系数,模糊计算
uint16 Weight_sum = 0;            //权重系数的和



//大斜率检测。
uint8 BlackLeftBigChangeNumx[5];                     //大斜率检测时，横坐标X的储存数组。储存行号。
uint8 BlackLeftBigChangeNumy[5];                     //大斜率检测时，横坐标X的储存数组。储存有效行的黑点所在列。
uint8 BlackLeftBigChangeFlag = 0;              //大斜率检测时，检测到大斜率的标志。每幅图像会清零，但只要有一次检测到则置位。1该幅图像检测到大斜率，0该幅图像没有检测到。
int32 BlackLeftBigChangeSlopeStore = MaxValInt16;    //大斜率检测时，储存的上一次的斜率值。
uint8 BlackLeftBigChangeJumpFlag = 0;                //大斜率检测时，用来跳过程序内的一个小循环。

uint8 BlackRightBigChangeNumx[5];                     //大斜率检测时，横坐标X的储存数组。储存行号。
uint8 BlackRightBigChangeNumy[5];                     //大斜率检测时，横坐标X的储存数组。储存有效行的黑点所在列。
uint8 BlackRightBigChangeFlag = 0;              //大斜率检测时，检测到大斜率的标志。每幅图像会清零，但只要有一次检测到则置位。
int32 BlackRightBigChangeSlopeStore = MaxValInt16;    //大斜率检测时，储存的上一次的斜率值。
uint8 BlackRightBigChangeJumpFlag = 0;                //大斜率检测时，用来跳过程序内的一个小循环。

uint8 BlackLeftBigChangeCheckRow[CameraHight];   //大斜率检测时，每行的标志。1表示该行左黑线未通过大跳变检测，0表示通过。
uint8 BlackLeftBigChangeCheckAllRow;             //大斜率检测时，1该幅图像的左黑线存在有某行未通过大跳变检测，0该幅图像每行左黑线均通过大跳变检测。
uint8 BlackRightBigChangeCheckRow[CameraHight];  //大斜率检测时，每行的标志。1表示该行右黑线未通过大跳变检测，0表示通过。
uint8 BlackRightBigChangeCheckAllRow;            //大斜率检测时，1该幅图像的右黑线存在有某行未通过大跳变检测，0该幅图像每行右黑线均通过大跳变检测。

//回弯检测
uint8 BlackLeftRollCheckFlag = 0;             //回弯检测标志位，0未检测到回弯，1检测到回弯。
uint8 BlackRightRollCheckFlag = 0;             //回弯检测标志位，0未检测到回弯，1检测到回弯。



//十字外尾
uint8 BlackLeftCrossTailFlag = 2;       //十字外尾标志，0失败，1确认，2未进入。
uint8 BlackLeftCrossTailEndLine;        //十字外尾，待删除的最远行。
uint8 BlackRightCrossTailFlag = 2;       //十字外尾标志，0失败，1确认，2未进入。
uint8 BlackRightCrossTailEndLine;        //十字外尾，待删除的最远行。

//窗口预测值管理
uint8 BlackLeftPredictL[CameraHight];          //窗口预测值，左线的左边界。
uint8 BlackLeftPredictR[CameraHight];          //窗口预测值，左线的右边界。
uint8 BlackRightPredictL[CameraHight];          //窗口预测值，右线的左边界。
uint8 BlackRightPredictR[CameraHight];          //窗口预测值，右线的右边界。

//赛道半宽
uint8 RoadHalfWidth;                              //双线赛道半宽
uint8 RoadHalfWidthLast;                          //上一幅图最终的双线赛道半宽
uint8 RoadHalfWidthRecord[CameraHight];           //双线赛道半宽记录
uint8 RoadHalfWidthSingle;                        //单线赛道半宽
uint8 RoadHalfWidthWrongFlag = 2;                 //通过赛道半宽判断出来的单线错误标志，2未进入，1错误，0正确。
uint8 RoadHalfWidthWrongDir;                      //单线错误时，0左线悬空，1右线悬空。
float RoadHalfWidthCompen;                        //单双线切换的修正系数
uint8 RoadHalfWidthSwitch = 1;                        //路宽开关，1单线算斜率，取最大值，0单线直接用上次双线的


//单线错误
uint8 BlackSingleFlag = 0;   //单线错误标志。0没错误，1有错误。每幅图初始化为0.
uint8 BlackSingleDir;        //单线错误的方向标志，0表示左线悬空，1表示右线悬空。

//最终大跳变检测
uint8 BlackBigChangeFinalCheckFlag = 0;  //最终大跳变检测标志位。0未检测到或没必要检测，1检测到左线最终大跳变，2检测到右线最终大跳变

//长直道
uint8 PathRealStraightCount = 0;   //连续直道计数
uint8 PathRealStraightCountLimit = 10;  //连续直道计数阈值，大于该值则确认为长直道。
uint8 PathRealStraightFlag = 0;        //长直道标志，1是长直道，0不是。




//绝对值求取
int32 AbsInt(int32 val)
{
  if(val >= 0)
  {
    return val;
  }
  else
  {
    return (0-val);
  }
}

//左右边界限制
int32 BoundaryLimitRe(int32 x, int32 left, int32 right)
{
  if(x < left)
  {
    return left;
  }
  else if(x > right)
  {
    return right;
  }
  else
  {
    return x;
  }
}

//======================================================================
//函数名：MaxRe
//功  能：求取两数最大值
//参  数：第1个数num1, 第2个数num2。
//返  回：2个数中的最大值
//影  响：无
//说  明：1. 两个输入参数无先后顺序。
//======================================================================
int32 MaxRe(int32 a, int32 b)
{
  return (a > b ? a : b);
}

//======================================================================
//函数名：MinRe
//功  能：求取两数最小值
//参  数：第1个数num1, 第2个数num2。
//返  回：2个数中的最小值
//影  响：无
//说  明：1. 两个输入参数无先后顺序。   
//======================================================================
int32 MinRe(int32 a, int32 b)
{
  return (a < b ? a : b);
}

//======================================================================
//函数名：AbsRe
//功  能：求取两数差值的绝对值
//参  数：第1个数num1, 第2个数num2。
//返  回：2个数的绝对值。
//影  响：无
//说  明：1. 两个输入参数无先后顺序。 
//======================================================================
int32 AbsRe(int32 num1, int32 num2)
{
  if(num1 >= num2)
  {
    return (num1 - num2);  
  }
  else
  {
    return (num2 - num1);  
  }
}



//十字检测
int ShiZhiCheck()
{
    uint8 startRow = 59;
    uint8 row;
    uint8 col;
    uint32 limitL;
    uint32 limitR;
    uint32 limit;

    limitL = 0;
    limitR = 0;
    limit = 0;
    if(ShiZhiFlag==0)
    {
        //要保证黑线采集成功
        if (BlackLeftDone != 1 || BlackRightDone != 1) { return 0; }
        //要保证是在直道
        if (AbsInt(SlopeL) < 1700 && AbsInt(SlopeR) < 1700
            && BlackRightCrossDone != 2 && BlackLeftCrossDone != 2
                && BlackRightEndLine > 57 && BlackLeftEndLine > 57)
        {
            if (BlackRightHeadLine >= 15 && BlackLeftHeadLine >= 15 && BlackRightHeadLine <= 59 && BlackLeftHeadLine <= 59 && AbsInt(BlackLeftHeadLine - BlackRightHeadLine) <= 10) { }
            else { return 0; }
        }
        else { return 0; }

        if (BlackLeftHeadLine >= BlackLeftEndLine) { return 0; }
        //线所在的列必须在一定范围内
        for (row = BlackLeftEndLine; row >= BlackLeftHeadLine; row--)
        {
            if (BlackLeftLoc[row][0] < 10 || BlackLeftLoc[row][0] > 240) { return 0; }
            else
            {
                limitL += ImgNew[row][BlackLeftLoc[row][0] - 9];
            }
        }
        for (row = BlackRightEndLine ; row >= BlackRightHeadLine; row--)
        {
            if (BlackRightLoc[row][0] < 10 || BlackRightLoc[row][0] > 240) { return 0; }
            else
            {
                limitR += ImgNew[row][BlackRightLoc[row][0] + 9];
            }
        }

        limit = (limitL + limitR) / (BlackLeftEndLine - BlackLeftHeadLine + 1) / 2;
        startRow = MinRe(BlackLeftHeadLine, BlackRightHeadLine) - 3;
        limit = MaxRe(limit, LimitLeftB) + 10;

        for (row = startRow; row > startRow - 3; row--)
        {
            for (col = 30; col <= 220; col++)
            {
                if (ImgNew[row][col] <= limit) { return 0; }
            }
        }
        ShiZhiCount = 0;
        return 1;
    }
    else
    {
        if(BlackLeftLoc[20][0]!=MaxValUint8 && BlackRightLoc[20][0]!=MaxValUint8){return 0;}
        else if(BlackRightHeadLine<15 && BlackLeftHeadLine<15){return 0;}
        else if(++ShiZhiCount>=6){return 0;}
        else{return 1;}
    }
}




//新加入的停车线检测
uint8 StopLineCheck()
{
    uint8 row;
    uint8 col;
    uint16 leftCount = 0;
    uint16 rightCount = 0;
    uint16 centerErrorCount = 0;
    uint32 limitStopLineBlack = 0;
    
    
    if(StopLineCheckBeginFlag == 0)
    {
        if(++StopLineCheckTimer >= 50)
        {
            StopLineCheckBeginFlag = 1;
        }
        else {return 0;}
    }

    
    limitStopLineBlack = 0;
    centerErrorCount = 0;
    //要保证黑线采集成功
    if (BlackLeftDone != 1 || BlackRightDone != 1) { return 0; }
    //要保证是在直道
    if (AbsInt(SlopeL) < 1500 && AbsInt(SlopeR) < 1500 && AbsInt(CURVEL) < 15 && AbsInt(CURVER) < 15
        && BlackRightCrossDone == 0 && BlackLeftCrossDone == 0
            && BlackRightHeadLine < 3 && BlackLeftHeadLine < 3 && BlackRightEndLine > 57 && BlackLeftEndLine > 57
                && BlackLeftCrossDone == 0 && BlackRightCrossDone == 0
                 && ShiZhiFlag == 0) { }
    else { return 0; }

    centerErrorCount = 0;
    //检测20-35行之间有没有可疑的停车线
    for (row = 59; row >= 20; row--)
    {
        //防止下面的判断溢出
        if (BlackLeftLoc[row][0] == MaxValUint8 || BlackRightLoc[row][0] == MaxValUint8)
        {
            centerErrorCount++;
            if (centerErrorCount > 5)
            {
                return 0;
            }
            continue;
        }
        limitStopLineBlack += ImgNew[row][BlackLeftLoc[row][0]];
        limitStopLineBlack += ImgNew[row][BlackRightLoc[row][0]];
    }
    limitStopLineBlack /= (59 - 20 + 1 - centerErrorCount)*2;

    centerErrorCount = 0;
    //检测20-35行之间有没有可疑的停车线
    for (row = 55; row >= 40; row--)
    {
        //防止下面的判断溢出
        if (ImgNew[row][BlackLeftLoc[row][0]] >= LimitLeftW || ImgNew[row][BlackRightLoc[row][0]] >= LimitRightW)
        {
            centerErrorCount++;
            if (centerErrorCount > 2)
            {
                return 0;
            }
            continue;
        }
    }

    centerErrorCount = 0;
    //检测20-35行之间有没有可疑的停车线
    for (row = 59; row >= 20; row--)
    {
        //防止下面的判断溢出
        if (BlackLeftLoc[row][0] == MaxValUint8 || BlackRightLoc[row][0] == MaxValUint8)
        {
            centerErrorCount++;
            if (centerErrorCount > 5)
            {
                return 0;
            }
            continue;
        }
        //从中间线的位置开始找线，一直找到中间线-40
        for (col = BlackLeftLoc[row][0] + 3; col <= (BlackLeftLoc[row][0] + BlackRightLoc[row][0]) / 2; col++)
        {
            if (ImgNew[row][col] < limitStopLineBlack+ limitStopLineBlackXiu)
            {
                leftCount++;
            }
        }

        //从中间线的位置开始找线，一直找到中间线+40
        for (col = BlackRightLoc[row][0] - 3; col >= (BlackLeftLoc[row][0] + BlackRightLoc[row][0]) / 2; col--)
        {
            if (ImgNew[row][col] < limitStopLineBlack + limitStopLineBlackXiu)
            {
                rightCount++;
            }
        }
    }


    //最后看左右停止线的线头是不是满足水平距离要求
    if (leftCount + rightCount > 10 && leftCount >= 5 && rightCount >= 5)//(stopLineLeft!=MaxValUint8)&&(stopLineRight!=MaxValUint8)&&(stopLineRight-stopLineLeft<=40)&&(stopLineRight-stopLineLeft>=20)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}






//======================================================================
//函数名：LeastSquareInt16a1
//功  能：针对y是int16型数据的最小二乘法的a1值的求取
//参  数：x数组，y数组，num基于的有效数据个数
//返  回：a1
//影  响：无
//说  明：1. y = a0 + a1 * x;
//        2. a1 = (N * Σxy - Σx * Σy) / (N * Σx^2 - Σx * Σx);
//        3. a1乘以了LeastSquareMulti倍。
//======================================================================
int32 LeastSquareInt16a1(uint8 x[], int16 y[], uint8 num)
{
  int32 andxy = 0, andx = 0, andy = 0, andx2 = 0;
  int32 a1;
  uint8 i;
 
  for(i = 0; i < num; i++)
  {
    andxy += x[i] * y[i];
    andx += x[i];
    andy += y[i];
    andx2 += x[i] * x[i];
  }
  
  //运算时为整型，最后变为int32型，且放大了LeastSquareMulti倍。
  a1 = (int32)(LeastSquareMulti * (num * andxy - andx * andy) / (num * andx2 - andx * andx));    
  
  return a1;  
  
}

//======================================================================
//函数名：LeastSquareInt16a0
//功  能：针对y是int16型数据的最小二乘法的a0值的求取
//参  数：x数组，y数组，a1参数，num基于的有效数据个数
//返  回：a0
//影  响：无
//说  明：1. y = a0 + a1 * x;
//        2. a0 = Σy / N - a1 * Σx / N;
//        3. a0乘以了LeastSquareMulti倍。      
//======================================================================
int32 LeastSquareInt16a0(uint8 x[], int16 y[], int32 a1, uint8 num)
{
  int32 andx = 0, andy = 0;
  int32 a0;
  uint8 i;
  for(i = 0; i < num; i++)
  {
    andx += x[i];
    andy += y[i];
  }
  
  //运算时为整型，最后变为int32型，且放大了LeastSquareMulti倍。
  a0 = (int32)(LeastSquareMulti * andy / num - a1 * andx / num);
  return a0;
}

//======================================================================
//函数名：LeastSquarea1
//功  能：最小二乘法的a1值的求取
//参  数：x数组，y数组，num基于的有效数据个数
//返  回：a1
//影  响：无
//说  明：1. y = a0 + a1 * x;
//        2. a1 = (N * Σxy - Σx * Σy) / (N * Σx^2 - Σx * Σx);
//        3. a1乘以了LeastSquareMulti倍。
//======================================================================
int32 LeastSquarea1(uint8 x[], uint8 y[], uint8 num)
{
  int32 andxy = 0, andx = 0, andy = 0, andx2 = 0;
  int32 a1;
  uint8 i;
 
  for(i = 0; i < num; i++)
  {
    andxy += x[i] * y[i];
    andx += x[i];
    andy += y[i];
    andx2 += x[i] * x[i];
  }
  
  //运算时为整型，最后变为int32型，且放大了LeastSquareMulti倍。
  a1 = (int32)(LeastSquareMulti * (num * andxy - andx * andy) / (num * andx2 - andx * andx));    
  return a1;  
}


//======================================================================
//函数名：LeastSquarea0
//功  能：最小二乘法的a0值的求取
//参  数：x数组，y数组，a1参数，num基于的有效数据个数
//返  回：a0
//影  响：无
//说  明：1. y = a0 + a1 * x;
//        2. a0 = Σy / N - a1 * Σx / N;
//        3. a0乘以了LeastSquareMulti倍。      
//======================================================================
int32 LeastSquarea0(uint8 x[], uint8 y[], int32 a1, uint8 num)
{
  int32 andx = 0, andy = 0;
  int32 a0;
  uint8 i;
  for(i = 0; i < num; i++)
  {
    andx += x[i];
    andy += y[i];
  }
  
  //运算时为整型，最后变为int32型，且放大了LeastSquareMulti倍。
  a0 = (int32)(LeastSquareMulti * andy / num - a1 * andx / num);
  return a0;
}




/*
//二值化图像解压函数
//dst             图像解压目的地址
//src             图像解压源地址
//srclen          二值化图像的占用空间大小
void ImageExtract(uint8 *dst, uint8 *src, uint32 srclen)
{
    uint8 colour[2] = {255, 0}; //0 和 1 分别对应的颜色
    //注：山外的摄像头 0 表示 白色，1表示 黑色
    uint8 tmpsrc;
    while(srclen --)
    {
        tmpsrc = *src++;
        *dst++ = colour[ (tmpsrc >> 7 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 6 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 5 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 4 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 3 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 2 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 1 ) & 0x01 ];
        *dst++ = colour[ (tmpsrc >> 0 ) & 0x01 ];
    }
}


*/

/*          //OV7725的图像发送函数
//图像发送到上位机
void SendImage(uint8 *imgaddr, uint32 imgsize)
{                                                                                   
    uint8 cmd[4] = {0, 255, 1, 0 };    //yy_摄像头串口调试 使用的命令

    //uint8 cmd[1] = {255};       //DEMOK上位机使用的命令   
  
    uart_sendN(UART0, cmd, sizeof(cmd));    //先发送命令

    uart_sendN(UART0, imgaddr, imgsize); //再发送图像
}

*/

//======================================================================
//函数名：SendCenterLineLoc
//功  能：中心线数组发送函数
//参  数：img待发送的一维图像数组
//返  回：无
//影  响：无
//说  明：1. 这里的命令头和图像数组发送函数的命令头不一样。
//      
//          
//======================================================================
void SendCenterLineLoc(uint8 img[CameraHight])
{
    uint8 i;
    uint8 cmd[4] = {0, 255, 1, 255 };   //中心线数组的数据头
    uart_sendN(UART0, cmd, sizeof(cmd));    //先发送命令
    for(i = 0; i < CameraHight; i++)
    {
      uart_send1(UART0, img[i]); //发送中心线数组
    }
    
}




//======================================================================
//函数名：SendImage
//功  能：OV7620的图像发送函数
//参  数：待发送的二维图像数组imgaddr
//返  回：无
//影  响：无
//说  明：1. 修改图像数组大小时需要把内部两个for的内容也改掉
//        2. 这里的命令头与中心线数组发送函数的命令头不一样。
//          
//======================================================================
void SendImage(uint8 imgaddr[CameraHight][CameraRealWidth])
{                            
    uint16 i, j;
    uint8 cmd[4] = {0, 255, 1, 0 };    //图像数组的数据头

    //uint8 cmd[1] = {255};       //DEMOK上位机使用的命令   
  
    uart_sendN(UART0, cmd, sizeof(cmd));    //先发送命令
    
    //这里不能用uart_sendN(UART0, imgaddr, CameraSize)来代替，因为uart_sendN函数里的buff[i].
    for(i = 0; i < CameraHight; i++)
        for(j = 0; j < CameraRealWidth; j++)
            uart_send1(UART0, imgaddr[i][j]); //发送图像
}



//====================================左左左左左左左左左左左左左左左左左============================




//======================================================================
//函数名：BlackLeftRealClear
//功  能：左黑线提取中，对3个关键值的实际值清零
//参  数：无
//返  回：无
//影  响：BlackLeftRealWB[], BlackLeftRealW[], BlackLeftRealB[]
//说  明：
//      
//             
//======================================================================
void BlackLeftRealClear(void)
{
    BlackLeftRealWB[0] = 0;
    BlackLeftRealWB[1] = 0;
    BlackLeftRealW[0] = 0;
    BlackLeftRealW[1] = 0;
    BlackLeftRealB[0] = 0;
    BlackLeftRealB[1] = 0;
}


//黑线提取成功，找左黑线的线头线尾。
uint8 BlackLeftHeadEndSearch(void)
{
  uint8 i;
  //左线线头和线尾的查找
  if(BlackLeftDone == 1)
  { //左线线头的查找。
    for(i = 0; ; i++)
    {
      if(BlackLeftLoc[i][0] != MaxValUint8)
      { //找到了线头
        BlackLeftHeadLine = i;
        break;
      }
      if(i == CameraHight - 1) 
      { //直到最近一行还没有找到线头
        BlackLeftHeadLine = MaxValUint8;
        break;
      }
    }
    
    //若线头已经查找失败，就不用查找线尾了。
    if(BlackLeftHeadLine == MaxValUint8)
    {
      BlackLeftEndLine = MaxValUint8;
      BlackLeftDone = 0;
    }
    //若线头查找成功，就开始找线尾。
    else
    {
      //左线线尾的查找。
      for(i = CameraHight - 1; ; i--)
      { //左线线尾查找成功。
        if(BlackLeftLoc[i][0] != MaxValUint8)
        {
          BlackLeftEndLine = i;
          break;
        }
        //左线线尾查找失败。左线不可能有头无尾，所以均置无效值。
        if(i == BlackLeftHeadLine)
        {
          BlackLeftHeadLine = MaxValUint8;
          BlackLeftEndLine = MaxValUint8;
          break;
        }
      }
    }
  }
  //如果左黑线提取失败，就不用找线头和线尾了。
  else
  {
    BlackLeftHeadLine = MaxValUint8;
    BlackLeftEndLine = MaxValUint8;
  }
  
  //本幅图像有单线时，删掉最远一个点。
  if(SingleBlackAllFlag == 1)
  {
    BlackLeftLoc[BlackLeftHeadLine][0] = MaxValUint8; //Udis数组不用改，因为线头线尾查找之后才轮到矫正函数。
    BlackLeftHeadLine += 1;
  }
  else
  {
  }
  
  return 1;
}








//======================================================================
//函数名：BlackLeftBigChangeCheck
//功  能：左黑线提取时，大斜率检测。
//参  数：newrow新加进来的需要判定是否有效的行,use调用时是连续2行的第几行。
//返  回：1该行黑线成功，0该行黑线失败。
//影  响：
//说  明：1. 用来防止大斜率突变的坏点。
//        2. 共5个行，取1，3，5行进行斜率计算，每进一个新行，移出一个旧行。
//             
//======================================================================
uint8 BlackLeftBigChangeCheck(uint8 newrow, uint8 use)
{
  uint8 tempstorex, tempstorey;
  uint8 i;
  int32 tempslope1, tempslope2;
  
  //tempstorex用来储存将要移出的一行，用来检测失败时复原5行数组。
  tempstorex = BlackLeftBigChangeNumx[0];
  BlackLeftBigChangeNumx[0] = BlackLeftBigChangeNumx[1];
  BlackLeftBigChangeNumx[1] = BlackLeftBigChangeNumx[2];
  BlackLeftBigChangeNumx[2] = BlackLeftBigChangeNumx[3];
  BlackLeftBigChangeNumx[3] = BlackLeftBigChangeNumx[4];
  BlackLeftBigChangeNumx[4] = newrow;

  tempstorey = BlackLeftBigChangeNumy[0];
  BlackLeftBigChangeNumy[0] = BlackLeftBigChangeNumy[1];
  BlackLeftBigChangeNumy[1] = BlackLeftBigChangeNumy[2];
  BlackLeftBigChangeNumy[2] = BlackLeftBigChangeNumy[3];
  BlackLeftBigChangeNumy[3] = BlackLeftBigChangeNumy[4];
  BlackLeftBigChangeNumy[4] = BlackLeftLoc[newrow][0];
  
  //每幅图最多只进一次这个for循环，可以省时间。
  if(BlackLeftBigChangeJumpFlag == 0)
  {
    //储存数组的有效性判断，若不足5个数，则无法进行大斜率检测，直接返回成功。
    for(i = 0; i <= 4; i++)
    {
      if(BlackLeftBigChangeNumx[i] == MaxValUint8)
      {
        return 1;
      }
    }
    //改变标志位，下次就不进来了。
    BlackLeftBigChangeJumpFlag = 1;
  }
  //走到这里说明通过了数组有效性判断。
  
  
  //取1，3点进行斜率计算。
  //因为只有两个点，所以斜率的计算采取斜率的初始定义的方式，不采取最小二乘法，可以省很多时间。
  tempslope1 = (BlackLeftBigChangeNumy[2] - BlackLeftBigChangeNumy[0]) 
             * LeastSquareMulti   //乘以了这么多倍。跟最小二乘法一样。
             / (BlackLeftBigChangeNumx[2] - BlackLeftBigChangeNumx[0]);
  tempslope2 = (BlackLeftBigChangeNumy[4] - BlackLeftBigChangeNumy[2]) 
             * LeastSquareMulti   //乘以了这么多倍。跟最小二乘法一样。
             / (BlackLeftBigChangeNumx[4] - BlackLeftBigChangeNumx[2]);
  BlackLeftBigChangeSlopeStore = tempslope1;
  //小5点斜率储存在数组中，由于储存的是tempslope1，所以大跳变点不会记录，会出现多行相同值。
  BlackLeft5Slope[BlackLeft5SlopeIndex] = BlackLeftBigChangeSlopeStore;
  BlackLeft5SlopeRow[BlackLeft5SlopeIndex] = newrow;  //记录计算5点斜率的最远点所在行
  BlackLeft5SlopeIndex++;
  
  //1. 前后两次的斜率是相反的。
  if(tempslope1 >= 0 && tempslope2 < 0)
  {
    //2. 两者差值很大很大。
    if(tempslope1 >= tempslope2 + BlackLeftBigChangeSlopeLimit)
    {
      //两者差值的确很大。不会刷新储存值。
      //if(use == 2)                    //另一种方案是在连续2行的第2行调用时才置成功标志位。
      //{
        //置成功标志位。
        BlackLeftBigChangeFlag = 1;
      //}
      //else
      //{
      //}
      //此次行不进入队列。
      BlackLeftBigChangeNumx[4] = BlackLeftBigChangeNumx[3];
      BlackLeftBigChangeNumx[3] = BlackLeftBigChangeNumx[2];
      BlackLeftBigChangeNumx[2] = BlackLeftBigChangeNumx[1];
      BlackLeftBigChangeNumx[1] = BlackLeftBigChangeNumx[0];
      BlackLeftBigChangeNumx[0] = tempstorex;
      BlackLeftBigChangeNumy[4] = BlackLeftBigChangeNumy[3];
      BlackLeftBigChangeNumy[3] = BlackLeftBigChangeNumy[2];
      BlackLeftBigChangeNumy[2] = BlackLeftBigChangeNumy[1];
      BlackLeftBigChangeNumy[1] = BlackLeftBigChangeNumy[0];
      BlackLeftBigChangeNumy[0] = tempstorey;
      
      //将前面3个左黑线(共4个)的值置无效值。图像上更连续。
      BlackLeftLoc[BlackLeftBigChangeNumx[4]][0] = MaxValUint8;
      BlackLeftLoc[BlackLeftBigChangeNumx[3]][0] = MaxValUint8;
      //BlackLeftLoc[BlackLeftBigChangeNumx[2]][0] = MaxValUint8;    
      
      return 0;
    }
    else
    {
      //刷新斜率储存值。
      //BlackLeftBigChangeSlopeStore = tempslope;
      return 1;
    }
  }
  //1. 前后两次的斜率是相反的。
  else if(tempslope1 < 0 && tempslope2 >= 0)
  {
    //2. 两者差值很大很大。
    if(tempslope2 >= tempslope1 + BlackLeftBigChangeSlopeLimit)
    {
      //两者差值的确很大。不会刷新储存值。
      //if(use == 2)                    //另一种方案是在连续2行的第2行调用时才置成功标志位。
      //{
        //置成功标志位。
        BlackLeftBigChangeFlag = 1;
      //}
      //else
      //{
      //}
      //此次行不进入队列。
      BlackLeftBigChangeNumx[4] = BlackLeftBigChangeNumx[3];
      BlackLeftBigChangeNumx[3] = BlackLeftBigChangeNumx[2];
      BlackLeftBigChangeNumx[2] = BlackLeftBigChangeNumx[1];
      BlackLeftBigChangeNumx[1] = BlackLeftBigChangeNumx[0];
      BlackLeftBigChangeNumx[0] = tempstorex;
      BlackLeftBigChangeNumy[4] = BlackLeftBigChangeNumy[3];
      BlackLeftBigChangeNumy[3] = BlackLeftBigChangeNumy[2];
      BlackLeftBigChangeNumy[2] = BlackLeftBigChangeNumy[1];
      BlackLeftBigChangeNumy[1] = BlackLeftBigChangeNumy[0];
      BlackLeftBigChangeNumy[0] = tempstorey;
      
      //将前面3个左黑线(共4个)的值置无效值。图像上更连续。
      BlackLeftLoc[BlackLeftBigChangeNumx[4]][0] = MaxValUint8;
      BlackLeftLoc[BlackLeftBigChangeNumx[3]][0] = MaxValUint8;
      //BlackLeftLoc[BlackLeftBigChangeNumx[2]][0] = MaxValUint8; 
      
      return 0;
    }
    else
    {
      //刷新斜率储存值。
      //BlackLeftBigChangeSlopeStore = tempslope;
      return 1;
    }
  }
  //若不相反，则判定无大斜率的跳变。返回成功。
  else
  {
    //刷新斜率储存值。
    //BlackLeftBigChangeSlopeStore = tempslope;
    return 1;
  }
  
}


//单纯的、从右往左的、一定窗口内的、扫描跳变沿的函数.可以精简Step2Win, CrossSpeWin等诸多内容。
//返回的是跳变沿的白点所在位置。
uint8 BlackGetLeftJustWin(uint8 row, int32 colleft, int32 colright)
{
  uint8 temp1, abs1;
  uint8 tempadd1, tempadd2, tempadd3, tempadd4;
  
  //左右边界限幅
  colleft = BoundaryLimitRe(colleft, 0, CameraRealWidth - 1);
  colright = BoundaryLimitRe(colright, 0, CameraRealWidth - 1);
  
  //判断条件里的">"很关键，没有写">="是考虑temp = 0的情况，这样可以防止产生负数（uint8中为正数），即无限循环。
  for( ; colright > colleft; colright--) 
  {
    //右白点与左黑线的差。
    if(ImgNew[row][colright] > ImgNew[row][colright-1-BlackLeftEdgeNum])
    {
      abs1 = ImgNew[row][colright] - ImgNew[row][colright-1-BlackLeftEdgeNum];
    }
    else
    {
      abs1 = 0;
    }
   
    if( //跳变沿的第一批条件：
        //黑够黑，白够白，差够大。
        //这里的3个阈值是自适应之后调整的值
          (abs1 > LimitLeftWB - LimitLeftWBAdjust[row]) //两者的差够大
        &&(ImgNew[row][colright] > LimitLeftW)                     //白点够白
        &&(ImgNew[row][colright-1-BlackLeftEdgeNum] < LimitLeftB)                   //黑点够黑
       )
    {
            //多几个白点的确认。用来防止右黑线右边的白边。
            if(colright < CameraRealWidth - 1 - 6)
            {
              tempadd1 = (ImgNew[row][colright+3] > LimitLeftW);
              tempadd2 = (ImgNew[row][colright+4] > LimitLeftW);
              tempadd3 = (ImgNew[row][colright+5] > LimitLeftW);
              tempadd4 = (ImgNew[row][colright+6] > LimitLeftW);
            }
            else
            {
              tempadd1 = 1;
              tempadd2 = 1;
              tempadd3 = 1;
              tempadd4 = 1;
            }       
      
          //近处的行，向左3个点仍为黑点，向右4个点仍为白点。
          if(row > BlackMiddleLine)
          {                        
            temp1 = (
                      (ImgNew[row][colright-2-BlackLeftEdgeNum] < LimitLeftB)
                    &&(ImgNew[row][colright-3-BlackLeftEdgeNum] < LimitLeftB)
                    //&&(ImgNew[row][colright-4-BlackLeftEdgeNum] < LimitLeftB)
                    &&(ImgNew[row][colright+1] > LimitLeftW)
                    &&(ImgNew[row][colright+2] > LimitLeftW)
                    &&(tempadd1 == 1)
                    &&(tempadd2 == 1)
                    &&(tempadd3 == 1)
                    &&(tempadd4 == 1)
                    );
          }
          else
          {
            //中间的行 ，向左2个点为仍为黑点，向右4个点仍为白点。
            if(row > BlackFarLine)
            {
              temp1 = (
                        (ImgNew[row][colright-2-BlackLeftEdgeNum] < LimitLeftB)
                      //&&(ImgNew[row][colright-3-BlackLeftEdgeNum] < LimitLeftB)
                      &&(ImgNew[row][colright+1] > LimitLeftW)
                      &&(tempadd1 == 1)
                      &&(tempadd2 == 1)
                      &&(tempadd3 == 1)
                      &&(tempadd4 == 1)                        
                      );
            }
            else
            {
              if(row > BlackFarfarLine)
              {
                //远处的行 ，向左1个点为仍为黑点，向右4个点仍为白点。
                temp1 = (
                        //(ImgNew[row][colright-2-BlackLeftEdgeNum] < LimitLeftB)
                        (tempadd1 == 1)
                      &&(tempadd2 == 1)
                      &&(tempadd3 == 1)
                      &&(tempadd4 == 1)
                        );                   
              }
              else
              {
                //很远处的行，只对跳变和周围白点有要求，对周围黑点没有要求。
                temp1 = (
                        (tempadd1 == 1)
                      &&(tempadd2 == 1)
                      &&(tempadd3 == 1)
                      &&(tempadd4 == 1)
                        );    
              }
                    
            }
          }
          
          //跳变沿的第二批条件：
          if(temp1 == 1)
          {
            //两批条件均满足，返回白点位置，不是黑点位置！           
            return colright;              
            
            //break;          
          }// end of 跳变沿检测的第2批条件
    }// end of 跳变沿检测的第1批条件
  }// end of for循环  
  
  
  
  //如果一直到窗口最左边还没有找到跳变沿，返回无效值。
  return MaxValUint8;  
  
}




//======================================================================
//函数名：BlackGetLeftStep1Edge
//功  能：左黑线提取中，第1阶段跳变检测函数,处理该行左线的可疑黑点
//参  数：待检测的行号row, 第几次使用use, 黑线预处理标志preflag(1是预处理)
//返  回：1成功 0失败
//影  响：BlackLeftRealWB[], BlackLeftRealW[], BlackLeftRealB[]
//        BlackLeftLoc[][]
//说  明：1. 对第row行的跳变向左进行白到黑检测，成功就赋相应的值（有3个备用值），失败则赋255
//        2. 两批条件均满足时。则检测成功。会有最多3个值进行记录，现阶段只用到了第1个值
//        3. 里面有涉及3个关键值的实际值的记录，CrossFlag的情况就不用记录（也就不用更改）
//        4. 若1个黑点也没有，或者超过3个黑点，则检测失败。
//        5. 若要改成黑到白的检测，或者任意检测，只需更改第一批条件即可，很方便。
//======================================================================
uint8 BlackGetLeftStep1Edge(uint8 row, uint8 use, uint8 preflag)
{
  uint8 index, col, temp1;
  uint8 abs1;
  uint8 tempadd1, tempadd2, tempadd3, tempadd4;
  uint8 avgr1, avgl1;
  uint8 i;
  
  //根据上一幅有效图像的中线来预估这一幅图像的左右黑线扫描起点，左线起点需要在中线往右一个黑线宽度。 
  col = (uint8)MinRe((int32)BlackLeftEdgeStartCol+10, CameraRealWidth - 1);

  //右线开始扫描。
  //如果是预处理，也不用考虑这个情况。直接往下走就好。
  if((BlackGetPreDir == 1)
  &&(BlackRightLoc[row][0] != MaxValUint8)
  &&(preflag == 0)
    )
  {
    //如果上一幅图是单线，则左线扫描起点必须在右线的右边一个单线宽度。
    if(
      (row > CameraHight - 5)
    &&(SingleBlackLastAllFlag == 1)
       )
    {
      col = MaxRe(col, MinRe(CameraRealWidth - 1, BlackRightLoc[row][0] + SingleBlackWidthMax[row]));
    }
    //如果上一幅图不是单线，且最近四行没有采集到单线，那就视为不可能出现单线。所以左线扫描起点必须在右线的左边
    else if(
           (row <= CameraHight - 5)
         //&&(SingleBlackLastAllFlag == 0)
            )
    {
      col = MinRe(col, BlackRightLoc[row][0]);
    }
    else
    {
      col = MinRe(col, BlackRightLoc[row][0]);
    }
  }
  else
  {    
  }
  
  //窗口预测值记录
  BlackLeftPredictL[row] = 4 + BlackLeftEdgeNum;
  BlackLeftPredictR[row] = col;
  
  index = 0;         //可疑点的序号
  //从中间到最左边扫描，隔1个点求差
  for(; col >= 4+BlackLeftEdgeNum; col--) 
  {
      //右白点与左黑点的差。
      if(ImgNew[row][col] > ImgNew[row][col-1-BlackLeftEdgeNum])
      {
        abs1 = ImgNew[row][col] - ImgNew[row][col-1-BlackLeftEdgeNum];
      }
      else
      {
        abs1 = 0;
      }
      
      if( //跳变沿的第一批条件：
          //黑够黑，白够白，差够大。
          //这里的3个阈值为初始值，不是自适应之后调整的值。
          (abs1 > LimitLeftWB - LimitLeftWBAdjust[row]) //两者的差够大
        //后面两个条件改一下，就可以实现黑到白的跳变检测，或者任意检测。
        &&(ImgNew[row][col] > LimitLeftW)                     //白点够白
        &&(ImgNew[row][col-1-BlackLeftEdgeNum] < LimitLeftB)                   //黑点够黑
         )
      {
            //多几个白点的确认。用来防止右黑线右边的白边。
            if(col < CameraRealWidth - 1 - 6)
            {
              tempadd1 = (ImgNew[row][col+3] > LimitLeftW);
              tempadd2 = (ImgNew[row][col+4] > LimitLeftW);
              tempadd3 = (ImgNew[row][col+5] > LimitLeftW);
              tempadd4 = (ImgNew[row][col+6] > LimitLeftW);
            }
            else
            {
              tempadd1 = 1;
              tempadd2 = 1;
              tempadd3 = 1;
              tempadd4 = 1;
            }       
          //跳变沿的第二批条件：
          //近处的行，向左3个点仍为黑点，向右4个点仍为白点。
          if(row > BlackMiddleLine)
          {                                
            temp1 = (
                      (ImgNew[row][col-2-BlackLeftEdgeNum] < LimitLeftB)
                    &&(ImgNew[row][col-3-BlackLeftEdgeNum] < LimitLeftB)
                    &&(ImgNew[row][col-4-BlackLeftEdgeNum] < LimitLeftB)
                    &&(ImgNew[row][col+1] > LimitLeftW)
                    &&(ImgNew[row][col+2] > LimitLeftW)
                    &&(tempadd1 == 1)
                    &&(tempadd2 == 1)
                    &&(tempadd3 == 1)
                    &&(tempadd4 == 1)
                    );
          }          
          else
          {
            //远处的行 ，向左2个点为仍为黑点，向右4个点仍为白点。
            if(row > BlackFarLine)
            {
              temp1 = (
                        (ImgNew[row][col-2-BlackLeftEdgeNum] < LimitLeftB)
                      &&(ImgNew[row][col-3-BlackLeftEdgeNum] < LimitLeftB)
                      &&(ImgNew[row][col+1] > LimitLeftW)
                      &&(tempadd1 == 1)
                      &&(tempadd2 == 1)
                      &&(tempadd3 == 1)
                      &&(tempadd4 == 1)
                      );
            }            
            else
            {
              if(row > BlackFarfarLine)
              {
                //远处的行 ，向左1个点为仍为黑点，向右4个点仍为白点。
                temp1 = (
                        (ImgNew[row][col-2-BlackLeftEdgeNum] < LimitLeftB)
                      &&(tempadd1 == 1)
                      &&(tempadd2 == 1)
                      &&(tempadd3 == 1)
                      &&(tempadd4 == 1)
                        );
              }
              else
              {
                //很远的行，有跳变，有周围白点确认就行，对周围黑点没有要求。
                temp1 = (
                        (tempadd1 == 1)
                      &&(tempadd2 == 1)
                      &&(tempadd3 == 1)
                      &&(tempadd4 == 1)
                        );
              }
            }
          }

          if(temp1)
          {            
              //跳变检测成功，取黑点
              BlackLeftLoc[row][index] = col - 1 - BlackLeftEdgeNum;          
              index++;
              break;
            
          }    //end of 跳变沿的第二批条件
      }   //end of 跳变沿的第一批条件
  }// end of 从中间到最左边扫描，隔1个点求差
  
  //1个可疑黑点也没有，则报错。
  if(index == 0)
  {
    //如果是黑线预处理的调用，也不用更新这几个值。
    if(preflag == 0)
    {
      //对3个关键值的实际值清零
      BlackLeftRealClear();
    }
    //该行定位黑线失败，赋无效值
    BlackLeftLoc[row][0] = MaxValUint8;
    return 0;
  }
  
  //不是黑线预处理的调用，则进行大斜率检测。
  if(preflag == 0)
  {
    //最后确认前，需要进行大斜率的检测。若不成功，则说明检测到坏点。
    if(BlackLeftBigChangeCheck(row, use) == 1)
    {
      //从右线开始的扫描
      //若已经确认为单线了，就不用再次确认了。
      if(
        (SingleBlackAllFlag == 0) 
      &&(BlackGetPreDir == 1)   
        )
      {
        //若该行左右黑线均检测成功，且右线在左线的左边一定范围内，则该行判定为单线。
        if((BlackLeftLoc[row][0] != MaxValUint8)
         &&(BlackRightLoc[row][0] != MaxValUint8)
         &&(BlackRightLoc[row][0] < BlackLeftLoc[row][0])
         &&(BlackRightLoc[row][0] - BlackLeftLoc[row][0] < SingleBlackWidthMax[row]) //结果是负数也会满足，所以必须小于。
        )
        {
          //检查5个白点的平均值是否相近，若相近，则是单线，若不相近，则置无效值。
          if(
            (BlackRightLoc[row][0] - 1 - BlackRightEdgeNum - 5 < 0)
          ||(BlackLeftLoc[row][0] + 1 + BlackLeftEdgeNum + 5 > CameraRealWidth - 1)
            )
          {
            //不哆5个点。
            //因为进行过大跳变，所以要清除。只针对最近几行。
            if(row > CameraHight - 5)
            {
                for(i = 0; i <= 4; i++)
                {
                    BlackLeftBigChangeNumx[i] = MaxValUint8;
                    BlackLeftBigChangeNumy[i] = MaxValUint8;
                }
                BlackLeftBigChangeJumpFlag = 0;
            }
            else
            {
            }
            
            //该行定位黑线失败，赋无效值
            BlackLeftLoc[row][0] = MaxValUint8;
            return 0;
          }
          else
          {
            //该行右线的白点平均值
            avgr1 = (ImgNew[row][BlackRightLoc[row][0] - 1 - BlackRightEdgeNum - 1]
                   + ImgNew[row][BlackRightLoc[row][0] - 1 - BlackRightEdgeNum - 2]
                   + ImgNew[row][BlackRightLoc[row][0] - 1 - BlackRightEdgeNum - 3]
                   + ImgNew[row][BlackRightLoc[row][0] - 1 - BlackRightEdgeNum - 4]
                   + ImgNew[row][BlackRightLoc[row][0] - 1 - BlackRightEdgeNum - 5])
                   / 5;
            //该行左线的白点平均值
            avgl1 = (ImgNew[row][BlackLeftLoc[row][0] + 1 + BlackLeftEdgeNum + 1]
                   + ImgNew[row][BlackLeftLoc[row][0] + 1 + BlackLeftEdgeNum + 2]
                   + ImgNew[row][BlackLeftLoc[row][0] + 1 + BlackLeftEdgeNum + 3]
                   + ImgNew[row][BlackLeftLoc[row][0] + 1 + BlackLeftEdgeNum + 4]
                   + ImgNew[row][BlackLeftLoc[row][0] + 1 + BlackLeftEdgeNum + 5])
                   / 5;    
            if(AbsInt(avgr1 - avgl1) < 20)
            {
              //5个白点之间的差别够小，单线判断成功。
              SingleBlackFlag[row] = 1;      
              //SingleBlackAllFlag = 1;   //总幅图像的单线标志，只要有一行检测单线成功，则置1.
            }
            else
            {
              //5个白点之间的差别太大，单线判断失败。
              //因为进行过大跳变，所以要清除。只针对最近几行。
              if(row > CameraHight - 5)
              {
                  for(i = 0; i <= 4; i++)
                  {
                      BlackLeftBigChangeNumx[i] = MaxValUint8;
                      BlackLeftBigChangeNumy[i] = MaxValUint8;
                  }
                  BlackLeftBigChangeJumpFlag = 0;
              }
              else
              {
              }             
              
              //该行定位黑线失败，赋无效值
              BlackLeftLoc[row][0] = MaxValUint8;
              return 0;              
            }
          }          
        }
        else
        {
          //否则该行单线标志为0.
          SingleBlackFlag[row] = 0;
        }
      }
      //从右线开始的扫描，右线不用检测是否有单线，左线去检测即可。
      else
      {
      }
      
      //没有报错，就会走到这里，黑点查找成功，会有1，2，3个黑点
      //return 1;
    }
    else
    {
      //没通过大斜率检测。
      BlackLeftBigChangeCheckRow[row] = 1;
      BlackLeftBigChangeCheckAllRow = 1;
      
      //return 1;    
      
      
      //该行定位黑线失败，赋无效值
      BlackLeftLoc[row][0] = MaxValUint8;
      return 0;    
    }
  }
  //不是黑线预处理的调用，不需进行大斜率检测，直接返回成功。
  else
  {
    //return 1;    
  }
  
  //如果是黑线预处理，也不用更新这几个值
  if(preflag == 0)
  {
    //进入这里说明跳变沿的二批条件均满足，进入黑点记录阶段
    //记录3个关键值的实际信息，作为后面的参考。
    if(use == 1)
    {
      BlackLeftRealWB[0] = ImgNew[row][col] - ImgNew[row][col-1-BlackLeftEdgeNum]; 
      BlackLeftRealW[0] = ImgNew[row][col];
      BlackLeftRealB[0] = ImgNew[row][col-1-BlackLeftEdgeNum];
    }
    else if(use == 2)
    {
      BlackLeftRealWB[1] = ImgNew[row][col] - ImgNew[row][col-1-BlackLeftEdgeNum]; 
      BlackLeftRealW[1] = ImgNew[row][col];
      BlackLeftRealB[1] = ImgNew[row][col-1-BlackLeftEdgeNum];
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



//======================================================================
//函数名：BlackGetLeftStep2Win
//功  能：左黑线提取中，第2阶段窗口内跳变检测函数
//参  数：待处理行号row, 窗口大小win, 预测的跳变点的位置predict, 第几次使用use
//返  回：1成功  0失败
//影  响：BlackLeftLoc[][0]
//说  明：1. 满足两批条件，才能检测成功。只有一个点的记录。
//        2. 窗口内没有检测到，则检测失败。
//        3. 窗缩小可以减少一定的时间，但如果遇到斜率过大的情况，就有可能检测不到。     
//======================================================================
uint8 BlackGetLeftStep2Win(uint8 row, uint8 win, uint8 predict, uint8 use)
{
  uint8 col, temp;
  
  //在规划好的窗口内，从右往左查找跳变沿
  //防止溢出
  
  //对左边界的限制
  if(predict <= win)
  {
    temp = 4 + BlackLeftEdgeNum;
  }  
  else
  {
    temp = predict - win;
    if(temp < 4 + BlackLeftEdgeNum)
    {
      temp = 4 + BlackLeftEdgeNum;
    }
    else 
    {
    } 
  }
  
  //对右边界的限制
  if(predict + win > CameraRealWidth - 1)
  {
    col = CameraRealWidth - 1;  
  }
  else
  {
    col = predict + win;  
  }
  
  //右线开始扫描时，左线的扫描右边界必须在该行有效右黑线的左边。
  if((BlackGetPreDir == 1)
  &&(SingleBlackAllFlag == 0)   
  &&(BlackRightLoc[row][0] != MaxValUint8)
    )
  {
    col = MinRe(col, BlackRightLoc[row][0]);
  }
  else
  {
  }
  
  //窗口预测值记录
  BlackLeftPredictL[row] = temp;
  BlackLeftPredictR[row] = col;
  
  //右边界小于左边界，返回失败
  if(col <= temp) return 0;
  
  //开窗扫描跳变沿
  col = BlackGetLeftJustWin(row, temp, col);
  if(col != MaxValUint8)
  {
    //两批条件均满足，直接记录该点为黑线，返回即可
    BlackLeftLoc[row][0] = col - 1 - BlackLeftEdgeNum;

    //最后确认前，需要进行大斜率的检测。若不成功，则说明检测到坏点。
    if(BlackLeftBigChangeCheck(row, use) == 0)
    {              
      //没通过大斜率检测。
      BlackLeftBigChangeCheckRow[row] = 1;
      BlackLeftBigChangeCheckAllRow = 1;
   
      
      //该行定位黑线失败，赋无效值
      BlackLeftLoc[row][0] = MaxValUint8;
      return 0;
    }
    else
    {
    }
    
    //通过了大斜率检测。
    //记录3个关键值的实际信息，作为后面的参考。
    if(use == 1)
    {
      BlackLeftRealWB[0] = ImgNew[row][col] - ImgNew[row][col-1-BlackLeftEdgeNum]; 
      BlackLeftRealW[0] = ImgNew[row][col];
      BlackLeftRealB[0] = ImgNew[row][col-1-BlackLeftEdgeNum];
    }
    else if(use == 2)
    {
      BlackLeftRealWB[1] = ImgNew[row][col] - ImgNew[row][col-1-BlackLeftEdgeNum]; 
      BlackLeftRealW[1] = ImgNew[row][col];
      BlackLeftRealB[1] = ImgNew[row][col-1-BlackLeftEdgeNum];
    }
    else
    {
    }     
    
    return 1;                  
  }
  else
  {
    //窗口内没有扫描到跳变沿，该行的黑线位置记为无效值，返回0失败.
    BlackLeftLoc[row][0] = MaxValUint8;
    return 0;   
  }
  
}


//======================================================================
//函数名：BlackGetLeftStep1Scan
//功  能：左黑线提取中，第1阶段的行扫描函数
//参  数：扫描起始行FirstLine, 扫描结束行LastLine
//返  回：0~CameraHight-1行数 255采集失败
//影  响：BlackLeftRealWB[], BlackLeftRealW[], BlackLeftRealB[], BlackLeftLoc[][],
//        LimitLeftWB, LimitLeftW, LimitLeftB
//说  明：1. 可用于正常地第1阶段行扫描，也可用于十字再检测时的行扫描
//        2. 正常第1阶段扫描时，会更改3个关键阈值的值。
//             
//======================================================================
uint8 BlackGetLeftStep1Scan(uint8 FirstLine, uint8 LastLine, uint8 CrossFlag)
{
    uint8 row, temp1, temp2;
    uint8 abs1;
    uint8 tempWB, tempW, tempB;
    uint8 flag, i;
    
    flag = 0;
    //最近的BlackStep1Size行用差值法找黑线
    for(row = FirstLine; row > LastLine; row -= 2)
    {
      //判断是否连续两行检测到跳变，可疑黑点有3个点，暂取其中一个。非黑线预处理。
      temp1 = BlackGetLeftStep1Edge(row, 1, 0);
      temp2 = BlackGetLeftStep1Edge(row-1, 2, 0);
      if(temp1 == 1 && temp2 == 1)//这里可以这么写，不用嵌套if，因为&&的特点
      {
        abs1 = AbsRe(BlackLeftLoc[row-1][0], BlackLeftLoc[row][0]);
        //进一步判断连续两行的黑点的间隔是否够小，且暂取第1个可疑黑点。
        if(abs1 < BlackLeftStep1ScanMinus)
        {
          //检测到了连续的黑线，停止差值法找跳变沿，节省时间，下面进入第2阶段
          //如果不是十字再检测的情况，就更新一下实际的3个关键值，否则不更新。
          if(CrossFlag == 1)
            {
              //差值的处理，留有裕度，且有上下界。
              tempWB = (BlackLeftRealWB[0] + BlackLeftRealWB[1]) / 2 - LimitLeftWBMargin; 
              if(tempWB < LimitLeftWBMin) tempWB = LimitLeftWBMin;
              else if(tempWB > LimitLeftWBMax) tempWB = LimitLeftWBMax;
              else ;
              LimitLeftWB = tempWB;
              
              //白点的处理，留有裕度，且有上下界。
              tempW = (BlackLeftRealW[0] + BlackLeftRealW[1]) / 2 - LimitLeftWMargin;
              if(tempW < LimitLeftWMin) tempW = LimitLeftWMin;
              else if(tempW > LimitLeftWMax) tempW = LimitLeftWMax;
              else ;
              LimitLeftW = tempW;
              
              //黑点的处理，留有裕度，且有上下界。
              tempB = (BlackLeftRealB[0] + BlackLeftRealB[1]) / 2 + LimitLeftBMargin;
              if(tempB < LimitLeftBMin) tempB = LimitLeftBMin;
              else if(tempB > LimitLeftBMax) tempB = LimitLeftBMax;
              else ;
              //黑色阈值不能超过白色
              if(tempB >= LimitLeftW)
              {
                tempB = LimitLeftW - 1;
              }
              else
              {
              }
              
              LimitLeftB = tempB;
            }
          break;
        }
        else
        {
          //连续两行黑点差别太大，视为错误的点。
          BlackLeftLoc[row][0] = MaxValUint8;
          BlackLeftLoc[row-1][0] = MaxValUint8;
          //大跳变检测数组的复位。
          BlackLeftBigChangeNumx[3] = MaxValUint8;
          BlackLeftBigChangeNumx[4] = MaxValUint8;
          BlackLeftBigChangeNumy[3] = MaxValUint8;
          BlackLeftBigChangeNumy[4] = MaxValUint8;
          BlackLeftBigChangeJumpFlag = 0;
        }
      }
      else if(temp1 == 0 && temp2 == 1)
      {
        //走到这里，说明双黑线检测成功之前有双线之一采集成功。
        flag = 1;
      }
      else if(temp1 == 1 && temp2 == 0)
      {
        //走到这里，说明双黑线检测成功之前有双线之一采集成功。
        flag = 1;
      }
      else
      {
      }
    }   
    
    //检测了BlackStep1Size行也没有检测到连续的黑线，说明黑线检测失败
    if(row <= LastLine)
    {
      return MaxValUint8;
    }               
    else
    {
        //双黑线检测成功 且 双黑线检测成功之前有双线之一采集成功。
        if(flag == 1)
        {
            //把前面的双线之一都去掉。
            for(i = FirstLine; ; i--)
            {
                if(i == row) break;
                BlackLeftLoc[i][0] = MaxValUint8;                
            }
            
            //第一次scan
            if(CrossFlag == 1)
            {
              //把前3个大斜率检测储存值都去掉，后面2个不用，因为是对的。
              for(i = 0; i <= 2; i++)
              {
                  BlackLeftBigChangeNumx[i] = MaxValUint8;
                  BlackLeftBigChangeNumy[i] = MaxValUint8;
                  BlackLeftBigChangeJumpFlag = 0;   //小细节
              }
            }
            else
            {
            }
        }
        else
        {

        }
        
        return row;
    }
  
}

//======================================================================
//函数名：BlackLeftCrossStep3
//功  能：左线提取中，十字检测第3阶段，脑补线函数
//参  数：第1段有效黑线的最后一行FirstLine， 第2段有效黑线的第一行LastLine
//返  回：1成功 0失败
//影  响：BlackLeftLoc[][0]
//说  明：1. 这里有一些浮点运算，会占时间，在调用本函数时要慎重，尽量不要用。
//        2. 这里的两点间连线的思路很不错，float强转和uint8强转用得很好。
//             
//======================================================================
uint8 BlackLeftCrossStep3(uint8 FirstLine, uint8 LastLine)
{
  int32 delta_x, delta_y, delta, index;
  uint8 row, temp1, temp2;
  uint8 i;
  //uint8 temp3, temp4, flag1, flag2;
  
  //超始行比结束行还高，报错！
  if(LastLine >= FirstLine)
  {
    //uart_sendN(UART0, (uint8 *)"\nError In BlackLeftCrossStep3!", 30);
    return 0;  
  }
  
  //有可能出现被大斜率检测删掉的情况，这种情况下该行黑线判断为错误，就不进行十字补线了。
  if(BlackLeftLoc[LastLine][0] == MaxValUint8)
  {
    //返回失败前扫屁股
    for(i = FirstLine - 1; ; i--)
    {
      BlackLeftLoc[i][0] = MaxValUint8;
      if(i == 0) break;
    }
    
    return 0;
  }
  else
  {
  }
  
  temp1 = BlackLeftLoc[FirstLine][0];
  temp2 = BlackLeftLoc[LastLine][0];
  
  /*
  flag = 1;
  flag = 2;
  //确认这两个点是直角处的点，也就是往左6个点仍为黑点
  if(temp1 <= (CameraRealWidth - 9))
  {
    temp3 = (
            (ImgNew[FirstLine+1][temp1+6] < LimitLeftB)
          &&(ImgNew[FirstLine+1][temp1+7] < LimitLeftB)
          &&(ImgNew[FirstLine+1][temp1+8] < LimitLeftB)
          );
    if(temp3)
    {
      flag1 = 0;  
    }
  }
  
  if(temp2 <= (CameraRealWidth - 9))
  {
    temp4 = (
            (ImgNew[LastLine-1][temp2+6] < LimitLeftB)
          &&(ImgNew[LastLine-1][temp2+7] < LimitLeftB)
          &&(ImgNew[LastLine-1][temp2+8] < LimitLeftB)
          );
    if(temp4)
    {
      flag2 = 0;  
    }
  }
  
  if((flag1 == 0) || (flag2 == 0))
  {
    return 0;  
  }
  
  */
  
  delta_y = FirstLine - LastLine;
  
  
  //斜率向右
  if(temp1 < temp2)
  {
    //放大BlackLeftCrossStep3Multi倍。
    delta_x = temp2 - temp1;  
    delta = delta_x * BlackLeftCrossStep3Multi / delta_y; 
    
    index = 1;
    for(row = FirstLine - 1; row >= LastLine + 1; row-- )
    {
      //每次右移一点点
      BlackLeftLoc[row][0] = (uint8)(BlackLeftLoc[FirstLine][0] + delta * index / BlackLeftCrossStep3Multi);
      index += 1;
    }
  }
  //斜率向左
  else if(temp1 > temp2)
  {
    //放大BlackLeftCrossStep3Multi倍。
    delta_x = temp1 - temp2;
    delta = delta_x * BlackLeftCrossStep3Multi / delta_y;
    
    index = 1;
    for(row = FirstLine - 1; row >= LastLine + 1; row-- )
    {
      //每次右移一点点
      BlackLeftLoc[row][0] = (uint8)(BlackLeftLoc[FirstLine][0] - delta * index / BlackLeftCrossStep3Multi);
      index += 1;
    }
  }
  //斜率直走，这个不太可能出现
  else
  {
    for(row = FirstLine - 1; row >= LastLine + 1; row-- )
    {
      //赋相同的值
      BlackLeftLoc[row][0] = temp1;
    }
  }
  
  return 1;
    
}

//======================================================================
//函数名：BlackLeftCrossConfirm
//功  能：十字再检测时 ，线头有效性的确认。
//参  数：第1段黑线的真实结束行Row1RealLastLine，第2段黑线的真实起始行Row2RealFirstLine
//返  回：1成功 0失败
//影  响：无
//说  明：1. 为了应对270度弯道十字误检测而诞生，编写过程中没有过多考虑其它情况。
//      
//             
//======================================================================
uint8 BlackLeftCrossConfirm(uint8 Row1RealLastLine, uint8 Row2RealFirstLine)
{
  uint8 row, index;
  uint8 x[BlackCrossConfirmNum];
  uint8 y[BlackCrossConfirmNum];
  
  int32 a0, a1, temp;

  //1. 先进行第二段黑线起始点方向判定，若不满足左十字该有方向，则判定为错误点。直接返回0.
  if(BlackLeftLoc[Row2RealFirstLine][0] > BlackLeftLoc[Row2RealFirstLine-1][0]) 
  {
    return 0;
  }
  //2. 再进行第二段黑线起始两点的斜率判定，也就是对两点的列差作限定，太大则有可能是大弯回弯。针对回弯十字误补。
  else if((BlackLeftLoc[Row2RealFirstLine][0] < BlackLeftLoc[Row2RealFirstLine-1][0])
   &&(BlackLeftLoc[Row2RealFirstLine-1][0] - BlackLeftLoc[Row2RealFirstLine][0] >= 3)
     )
  {
    return 0;
  }
  //3. 再对上一幅图的右线斜率和右线头进行检查。这里的右线斜率与右线头因为还没更新，所以是上一幅图的。针对回弯十字误补。
  else if((SlopeR > BlackCrossConfirmSlopeLimit) //上一幅图的右线斜率很大。
   &&(BlackRightHeadLine < BlackCrossConfirmHeadLineLimit) //上一幅图的右线线头比较高。（防止因为线特别短而造成的斜率不准确）
     )
  {
    return 0;
  }
  else
  {
  }
  
  //第一段黑线太短，无法检测正确性时，检查要补的行是不是够长，如果够长，就视为正确。
  //if(Row1RealLastLine >= CameraHight - 5 && Row2RealFirstLine > Row1RealLastLine + BlackLeftCrossConfirmGapLine) return 1;
  //if(Row1RealLastLine >= CameraHight - 5) return 0;
  
  //以左上角为原点，向下为x轴正方向（0~49），向右为y轴正方向(0~249)
  index = 0;
  for(row = Row1RealLastLine; ; row++)
  {
    //有效行的记录
    if(BlackLeftLoc[row][0] != MaxValUint8)
    {
      y[index] = BlackLeftLoc[row][0];
      x[index] = row;
      index++;
    }
    //点数够多，则跳出。
    if(index == BlackCrossConfirmNum) break;
    //直到最低行仍没有凑够足够的点数，说明第一段有效黑线太短，无法检测正确性，直接返回1.
    if(row == CameraHight - 1) 
    {
      if(index == 0) return 0;
      //判断第二段黑点起点是不是在第一段黑点末尾的右边一定范围内
      if(BlackLeftLoc[Row2RealFirstLine][0] > y[0] && BlackLeftLoc[Row2RealFirstLine][0] - y[0] < 50)
      {
        return 1;
      }
      else
      {
        return 0;
      }      
    }
    else
    {
    }
  }
  
  //最小二乘法
  //y = a0 + a1 * x
  a1 = LeastSquarea1(x, y, BlackCrossConfirmNum);
  
  //判断斜率是不是从右下到左上，如果是，则返回失败。
  if(a1 > BlackLeftCrossConfirmSlopeLimit) return 0;
  
  a0 = LeastSquarea0(x, y, a1, BlackCrossConfirmNum);
  
  //temp为第2段黑线起始行本应该在的位置。
  temp = (a0 + a1 * Row2RealFirstLine) / LeastSquareMulti;

  //把这个点限定在窗内
  if(
    (BlackLeftLoc[Row2RealFirstLine][0] > temp - BlackCrossConfirmWin) 
  &&(BlackLeftLoc[Row2RealFirstLine][0] < temp + BlackCrossConfirmWin)  //向右的值太大的话，会在“下双上单”的时候出现十字误补线。
    )
  {
    return 1;
  }
  else
  {
    return 0;
  }
    
   
}




//======================================================================
//函数名：BlackGetLeftCrossSpeWin
//功  能：左黑线十字再检测时，窗口的特殊扫描
//参  数：待处理行号row, 窗口左边界colleft, 窗口右边界colright, 第几次使用use.
//返  回：1成功  0失败
//影  响：BlackLeftLoc[][0]
//说  明：1. 满足两批条件，才能检测成功。只有一个点的记录。
//        2. 窗口内没有检测到，则检测失败。  
//        3. 跟普通窗口扫描不一样，前者输入变量是预测值和窗口大小，后者是左右边界。
//======================================================================
uint8 BlackGetLeftCrossSpeWin(uint8 row, uint8 colleft, uint8 colright, uint8 use)
{
  uint8 col;
  
  //在规划好的窗口内，从右往左查找跳变沿
  //防止溢出
  
  //对左边界的限制
  if(colleft < 4 + BlackLeftEdgeNum)
  {
    colleft = 4 + BlackLeftEdgeNum;
  }
  else 
  {
  } 

  //对右边界的限制
  if(colright > CameraRealWidth - 1)
  {
    colright = CameraRealWidth - 1;  
  }
  else
  {
  }
  
  //右线开始扫描时，左线扫描的右边界必须在该行有效右黑线的左边。
  if((BlackGetPreDir == 1)
   &&(BlackRightLoc[row][0] != MaxValUint8)
     )
  {
    colright = MinRe(colright, BlackRightLoc[row][0]);
  }
  else
  {
  }
  
  //窗口预测值记录
  BlackLeftPredictL[row] = colleft;
  BlackLeftPredictR[row] = colright;
  
  //左边界在右边界右边，直接返回失败。
  if(colleft >= colright) return 0;
  
  //开窗扫描跳变沿
  col = BlackGetLeftJustWin(row, colleft, colright);
  if(col != MaxValUint8)
  {
    //两批条件均满足，直接记录该点为黑线，返回即可
    BlackLeftLoc[row][0] = col - 1 - BlackLeftEdgeNum;

    //最后确认前，需要进行大斜率的检测。若不成功，则说明检测到坏点。
    if(BlackLeftBigChangeCheck(row, use) == 0)
    {              
      //没通过大斜率检测。
      BlackLeftBigChangeCheckRow[row] = 1;
      BlackLeftBigChangeCheckAllRow = 1;
   
      
      //该行定位黑线失败，赋无效值
      BlackLeftLoc[row][0] = MaxValUint8;
      return 0;
    }
    else
    {
    }
    
    //通过了大斜率检测。
    //记录3个关键值的实际信息，作为后面的参考。
    if(use == 1)
    {
      BlackLeftRealWB[0] = ImgNew[row][col] - ImgNew[row][col-1-BlackLeftEdgeNum]; 
      BlackLeftRealW[0] = ImgNew[row][col];
      BlackLeftRealB[0] = ImgNew[row][col-1-BlackLeftEdgeNum];
    }
    else if(use == 2)
    {
      BlackLeftRealWB[1] = ImgNew[row][col] - ImgNew[row][col-1-BlackLeftEdgeNum]; 
      BlackLeftRealW[1] = ImgNew[row][col];
      BlackLeftRealB[1] = ImgNew[row][col-1-BlackLeftEdgeNum];
    }
    else
    {
    }     
    
    return 1;   
  }
  else
  {
    //窗口内没有扫描到跳变沿，该行的黑线位置记为无效值，返回0失败.
    BlackLeftLoc[row][0] = MaxValUint8;
    return 0; 
  }
}



//======================================================================
//函数名：BlackGetLeftCrossSpeScan
//功  能：十字再检测时，线头的特殊扫描
//参  数：第2段黑线的预测起始行Row2FirstLine， 第2段黑线的预测结束行Row2LastLine，
//        最近一次的窗口预测值LastWinPredict
//返  回：MaxValUint8失败，否则成功。
//影  响：
//说  明：
//      
//             
//======================================================================
uint8 BlackGetLeftCrossSpeScan(uint8 Row2FirstLine, uint8 Row2LastLine, uint8 LastWinPredict)
{
  uint8 row;
  uint8 temp1, temp2, temppredict, tempminus;
  uint8 colleft, colright;
  uint8 i;
  
  //最近一次的窗口预测值。
  temppredict = LastWinPredict;
  
  //colleft的确定
  //对于左黑线扫描，colleft表示之前所有有效黑线的最左点。
  //找第1个有效左黑线。
  for(i = Row2FirstLine+1; ; i++)
  {
    if(BlackLeftLoc[i][0] != MaxValUint8)
    {
      colleft = BlackLeftLoc[i][0];
      break;
    }
    else
    {
    }
    //若一直到最近一行仍没有有效左黑线，则返回失败。理论上不可能走到这里。
    if(i == CameraHight - 1) return MaxValUint8;
  }
  //暂时不用最左边的左黑线，也就相当于用最右边的左黑线。
//  //找最左的有效左黑线。
//  for( ; ; i++)
//  {
//    if(BlackLeftLoc[i][0] != MaxValUint8)
//    {
//      if(BlackLeftLoc[i][0] < colleft)
//      {
//        colleft = BlackLeftLoc[i][0];
//      }
//      else
//      {
//      }
//    }
//    else
//    { 
//    }
//    
//    if(i == CameraHight - 1) break;
//  }

  //逐行开窗扫描左黑线。
  for(row = Row2FirstLine; ; row -= 2)
  {
    //colright的确定
    colright = BoundaryLimitRe(temppredict + BlackLeftWinVal * 2,
                               0,
                               CameraRealWidth - 1);        
    
    //为了防止窗口有错，所以最左边不得小于BlackLeftEdgeStartColOffset。
    colright = MaxRe(colright, CameraRealWidth / 2 - 1 + BlackLeftEdgeStartColOffset[row]);
    
    //连续2行检测黑点,特殊开窗扫描。
    temp1 = BlackGetLeftCrossSpeWin(row, colleft, colright, 1);
    temp2 = BlackGetLeftCrossSpeWin(row-1, colleft, colright, 2);
    //连续2行开窗检测黑点成功，则线头检测成功。跳出。
    if(temp1 == 1 && temp2 == 1)
    {
      return row;
    }
    //连续2行开窗检测黑线失败，则窗口预测点继续变化，继续开窗检测。
    else if(temp1 == 0 && temp2 == 0)
    {
      //预测点按趋势走也要限幅。
      tempminus = BoundaryLimitRe(BlackLeftWinPredictStore2 + BlackLeftWinPredictStore2 - BlackLeftWinPredictStore1,
                                  0,
                                  CameraRealWidth - 1);
      //temp在这里表示窗口预测点位置。
      temppredict = tempminus;
      BlackLeftWinPredictStore0 = BlackLeftWinPredictStore1;
      BlackLeftWinPredictStore1 = BlackLeftWinPredictStore2;
      BlackLeftWinPredictStore2 = temppredict;      
    }
    //其中一行开窗检测成功，则当作干扰点，窗口预测点继续变化，继续开窗检测。
    else
    {
      //当作干扰点滤掉。
      BlackLeftLoc[row][0] = MaxValUint8;
      BlackLeftLoc[row-1][0] = MaxValUint8;
      
      //预测点按趋势走也要限幅。
      tempminus = BoundaryLimitRe(BlackLeftWinPredictStore2 + BlackLeftWinPredictStore2 - BlackLeftWinPredictStore1,
                                  0,
                                  CameraRealWidth - 1);
      //temp在这里表示窗口预测点位置。
      temppredict = tempminus;
      BlackLeftWinPredictStore0 = BlackLeftWinPredictStore1;
      BlackLeftWinPredictStore1 = BlackLeftWinPredictStore2;
      BlackLeftWinPredictStore2 = temppredict;        
    }
      
      
      
    //一直到第Row2LastLine行也没有连续2行检测成功，则返回失败。
    if(row == Row2LastLine) 
    {
      return MaxValUint8;
    }
    else
    {
    }
  }
 



}

//======================================================================
//函数名：BlackLeftAgainForCross
//功  能：左黑线提取中，针对十字的再检测程序
//参  数：第2段黑线的预测起始行Row2FirstLine， 第2段黑线的预测结束行Row2LastLine，
//        第1段黑线的实际结束行Row1RealLastLine， 最近一次的窗口预测值LastWinPredict
//返  回：1成功 0失败
//影  响：BlackLeftLoc[][]。 （不会影响那几个阈值。）
//说  明：1. 本函数会调用浮点运算的函数，预计花费较多时间，调用入口需谨慎。
//        2. 本函数仅影响了BlackLeftLoc[][]，以后也应该保持。
//             
//======================================================================
uint8 BlackLeftAgainForCross(uint8 Row2FirstLine, uint8 Row2LastLine, uint8 Row1RealLastLine, uint8 LastWinPredict)
{
    uint8 row, temp, temp1, temp2;
    uint8 Row2RealFirstLine; //第2段黑线的真实起始行
    uint8 i;
    
    uint8 tempWB, tempW, tempB;
    uint8 tempminus;
    
    //第1段黑线的真实结束行有可能因为大跳变被删掉，所以需要重新查找并更新。
    for(i = Row1RealLastLine; i <= CameraHight - 1; i++)
    {
      if(BlackLeftLoc[i][0] != MaxValUint8)
      {
        Row1RealLastLine = i;
        break;
      }
      else
      {
      }
    }
    //由于直角黑块不允许检测十字再补线，所以当第1段黑线真实结束行的斜上方有黑块时，不允许补线。
    //取5列。
    for(i = 0; i <= 5; i++)
    {
      if(ImgNew[Row1RealLastLine - 4][BlackLeftLoc[Row1RealLastLine][0] + 5 + i] < LimitLeftB) //往远4行。斜右上方。
      {
        return 0;
      }
      else if(ImgNew[Row1RealLastLine - 5][BlackLeftLoc[Row1RealLastLine][0] + 5 + i] < LimitLeftB) //往远5行。斜右上方。
      {
        return 0;
      }
      else
      {
      }      
    }
    
    //十字再检测第1阶段，找线头。
    //temp = BlackGetLeftStep1Scan(Row2FirstLine, Row2LastLine, 0); //第3个参数0表示是十字再检测，不用更新3个关键值
    
    //十字再检测第1阶段，找线头，不采用Scan的方式，采用Win的方式。
    temp = BlackGetLeftCrossSpeScan(Row2FirstLine, Row2LastLine, LastWinPredict);
    
    //线头查找失败，或找到最远两行（继续执行后面会数组超出界限），就直接判定为失败。
    if(
      (temp == MaxValUint8)
    ||(temp <= 1)
    ||(BlackAgainForCrossFar10CutSwitch == 1 && temp <= 9)  //当远10行删除开关开启时，且十字再检测时的第2段黑线线头在最远的10行，则十字再检测失败。
      )
    {
      //扫屁股工作。       //i >= Row2LastLine
      for(i = Row2FirstLine; ; i--)
      {
        BlackLeftLoc[i][0] = MaxValUint8;
        if(i == 0) break;
      }
      return 0;  
    }
    else 
    {
      ////////////////如果要注释，就从这里开始注释
      
      //十字再检测的线头不在限定的范围内，认为是干扰点，不用顺着线头找线了，直接退出。
      if(BlackLeftCrossConfirm(Row1RealLastLine+1, temp) == 0)
      { //扫屁股工作。       //i >= (temp-1)
        BlackLeftCrossConfirmFlag = 1;  //左十字线头标志确认失败，超出预想范围。
        for(i = Row2FirstLine; ; i--)
        {
          BlackLeftLoc[i][0] = MaxValUint8;
          if(i == 0) break;
        }
        return 0;
      }
      //十字再检测的线头的确在限定的范围内，可以继续。
      else
      {
        BlackLeftCrossConfirmFlag = 2;  //左十字线头标志确认成功。
        row = temp;
      }
      
      ////////////////如果要注释，就从这里结束注释
      
      //暂时不用confirm函数，如果要用的话 ，就把后面这句话去掉，前面注释去掉。
      //row = temp;
    }
    
    Row2RealFirstLine = row; //记录第2段黑线的真实起始行
    
    //十字再检测第2阶段，顺着线头找线。
    //temp为预测的黑点位置
    temp = BoundaryLimitRe(BlackLeftLoc[row-1][0] + BlackLeftLoc[row-1][0] - BlackLeftLoc[row][0],
                           0,
                           CameraRealWidth - 1);
    //预测值更新后就刷新一次堆栈，堆栈顶部丢失。
    BlackLeftWinPredictStore0 = BlackLeftWinPredictStore1;
    BlackLeftWinPredictStore1 = BlackLeftWinPredictStore2;
    BlackLeftWinPredictStore2 = temp;    
    
    //从最近的认证行到取最远的行
    for(row -= 2; ; row -= 2)
    {
      //连续2行检测黑点。
      temp1 = BlackGetLeftStep2Win(row, BlackLeftWinVal, temp, 1);
      temp2 = BlackGetLeftStep2Win(row-1, BlackLeftWinVal, temp, 2);
      
      //若连续2行黑点检测成功，则对3个阈值进行更新，且更新预测点位置。
      if(temp1 && temp2) 
      {
        //差值的处理，留有裕度，且有上下界。
        tempWB = (BlackLeftRealWB[0] + BlackLeftRealWB[1]) / 2 - LimitLeftWBMargin; 
        if(tempWB < LimitLeftWBMin) tempWB = LimitLeftWBMin;
        else if(tempWB > LimitLeftWBMax) tempWB = LimitLeftWBMax;
        else ;
        LimitLeftWB = tempWB;
        
        //白点的处理，留有裕度，且有上下界。
        tempW = (BlackLeftRealW[0] + BlackLeftRealW[1]) / 2 - LimitLeftWMargin;
        if(tempW < LimitLeftWMin) tempW = LimitLeftWMin;
        else if(tempW > LimitLeftWMax) tempW = LimitLeftWMax;
        else ;
        LimitLeftW = tempW;
        
        //黑点的处理，留有裕度，且有上下界。
        tempB = (BlackLeftRealB[0] + BlackLeftRealB[1]) / 2 + LimitLeftBMargin;
        if(tempB < LimitLeftBMin) tempB = LimitLeftBMin;
        else if(tempB > LimitLeftBMax) tempB = LimitLeftBMax;
        else ;
        //黑色阈值不能超过白色
        if(tempB >= LimitLeftW)
        {
          tempB = LimitLeftW - 1;
        }
        else
        {
        }
        LimitLeftB = tempB;
        
        //预测点更新，并限幅
        temp = BoundaryLimitRe(BlackLeftLoc[row-1][0] + BlackLeftLoc[row-1][0] - BlackLeftLoc[row][0],
                               0,
                               CameraRealWidth - 1);
        //预测值更新后就刷新一次堆栈，堆栈顶部丢失。
        BlackLeftWinPredictStore0 = BlackLeftWinPredictStore1;
        BlackLeftWinPredictStore1 = BlackLeftWinPredictStore2;
        BlackLeftWinPredictStore2 = temp;
      }
      //如果没能连续两行检测黑点成功，原本打算用原位置的预测点，后来发现效果不好，所以决定按预测点的趋势走。
      else
      {
        //预测点按趋势走也要限幅。
        tempminus = BoundaryLimitRe(BlackLeftWinPredictStore2 + BlackLeftWinPredictStore2 - BlackLeftWinPredictStore1,
                                    0,
                                    CameraRealWidth - 1);
        //temp在这里表示窗口预测点位置。
        temp = tempminus;
        BlackLeftWinPredictStore0 = BlackLeftWinPredictStore1;
        BlackLeftWinPredictStore1 = BlackLeftWinPredictStore2;
        BlackLeftWinPredictStore2 = temp;
      }       
      
      if((row == Row2LastLine) || (row == Row2LastLine+1)) break;      
    }
    
    //检查是否因大斜率而被削头。如果有，则更新头的位置，如果没有，则正常补线。
    if (BlackLeftLoc[Row1RealLastLine][0] == MaxValUint8)
    {

        //头足够高，找回头。
        for (i = Row1RealLastLine; ; i++)
        {
            if (BlackLeftLoc[i][0] != MaxValUint8)
            {
                Row1RealLastLine = i;
                break;
            }
            else
            {
            }
            //直到最后一行也没有找到有效行，不用补线，直接返回1.
            if (i == CameraHight - 1) return 1;
        }

        //十字再检测第3阶段，脑补两段线的接线。
        if (BlackLeftCrossStep3(Row1RealLastLine, Row2RealFirstLine) == 0)
        {
            return 0;
        }

    }
    else
    {
        //没有被大斜率削头。
        //十字再检测第3阶段，脑补两段线的接线。
        if (BlackLeftCrossStep3(Row1RealLastLine, Row2RealFirstLine) == 0)
        {
            return 0;
        }
    }

    
    return 1;
        
}

//======================================================================
//函数名：BlackLeftStep4BackLine
//功  能：左黑线提取中，第4阶段十字向后补线函数。
//参  数：Step4StartLine开始行，flag标志0表示正常调用，1表示单线错误的调用
//返  回：1成功 0失败
//影  响：BlackLeftLoc[][0]
//说  明：1. 这里引用了float型的函数，有可能占用较长的时间。
//        2. 想要减少时间，可以把宏定义BlackBackLineNum减至3或者2.
//        3. 单线错误的调用时，不用检测十字外尾     
//======================================================================
uint8 BlackLeftStep4BackLine(uint8 Step4StartLine, uint8 flag)
{
  uint8 row;
  uint8 i, index;
  uint8 x[BlackBackLineNum];   //取多少个点在一开始有定义。
  uint8 y[BlackBackLineNum];
  uint8 j;
  uint8 temp1;
  uint8 tempend;
  
  int32 temp;
  int32 a0;
  int32 a1;
  
  //正常调用时，需要检测十字外尾
  if(flag == 0)
  {
    //十字外尾处理
    //第1阶段的线头比较高的话，就开始十字外尾检测。否则不用管。
    if(BlackLeftStep1ScanRow < BlackLeftCrossTailRowLimit)
    {
      //检查线头处的斜率是不是很大，若很大，则怀疑有十字外尾。否则不用管。
      //这里用的斜率是十字外尾专用斜率。
      if(BlackLeft5Slope[0] + BlackLeftCrossTailSlopeLimit < 0 || AbsInt(BlackLeft5Slope[0])>BlackLeftCrossTailSlopeLimit)
      {
        //十字外尾标志置位。
        BlackLeftCrossTailFlag = 1;
        BlackLeftHeadEndSearch();
        if (BlackLeftEndLine - BlackLeftHeadLine<=8 && BlackLeftHeadLine!=MaxValUint8 && BlackLeftEndLine!=MaxValUint8)
        {
            //这种情况有可能是回弯导致的，过滤掉
            for (i = BlackLeftHeadLine; i <= BlackLeftEndLine; i++)
            {
                BlackLeftLoc[i][0] = MaxValUint8;
            }
            return 0;
        }
        else if (BlackLeft5Slope[0] == MaxValInt16)
        {
            //这种情况有可能是回弯导致的，过滤掉
            for (i = 0; i <= 59; i++)
            {
                BlackLeftLoc[i][0] = MaxValUint8;
            }
            return 0;
        }
        //5Slope数组有可能已经删掉了一些黑点，所以也要参考实际的有效黑点数。
        if(BlackLeftEndLine - BlackLeftHeadLine - 4 < 0 || BlackLeft5SlopeIndex - 1 < 0)
        {
          return 0;
        }
        else
        {
        }        
        tempend = MinRe(BlackLeftEndLine - BlackLeftHeadLine - 4, BlackLeft5SlopeIndex - 1);
        //开始消除十字外尾，找十字外尾的处理最远行。              
        for(i = 0; ; i++)
        {
          //找斜率在直道范围内的。这里用的斜率是判断为直道的斜率。
          if(BlackLeft5Slope[i] + PathJudgeStraightSlopeLimit > 0)
          {
            BlackLeftCrossTailEndLine = BlackLeft5SlopeRow[i];
            break;
          }
          
          if(i == tempend) return 0;  //一直都很斜，则有可能是回弯，直接返回失败。
          //按理说不会走到这里，因为回弯会导致向后补线出错，向后补线出错后进来本函数时会先检查出错的原因BlackLeftStep4ErrorCheck()，会检测斜率是不是都是直线，若不是则不会进来。
        }
        //向近处4个点，也就是“5”点斜率的计算末尾。
        BlackLeftCrossTailEndLine += 4;
        //取最先采集到的行。
        i = MinRe(BlackLeft5SlopeRow[0] + 4, CameraHight - 1);
        i = MaxRe(BlackLeftStep1ScanRow, i);
        if(i >= BlackLeftCrossTailEndLine)
        {
          //开始删除十字外尾。
          for(; ; i--)
          {
            BlackLeftLoc[i][0] = MaxValUint8;
            if(i == BlackLeftCrossTailEndLine) break;
          }
          //修改向后补线的起点。
          Step4StartLine = BlackLeftCrossTailEndLine - 1;
        }
        else
        {
        }
      }
      else
      {
        //没有十字外尾。
        BlackLeftCrossTailFlag = 0;      
      }
    }
    else
    {    
      //没有十字外尾。
      BlackLeftCrossTailFlag = 0;
    }
  }
  //单线错误调用时，就不用检测十字外尾了。
  else
  {
  }
 
  //正常的十字向后补线。
  if(Step4StartLine - BlackBackLineNum - BlackBackLineNum < 0)
  {
    temp1 = 0;    
  }
  else
  {
    temp1 = Step4StartLine - BlackBackLineNum - BlackBackLineNum;
  }
  
  //取BlackBackLineNum个点作为最小二乘法的基准点。
  //以左上角为原点，向下为x轴正方向，向右为y轴正方向
  index = 0;
  for(i = Step4StartLine; ; i--)
  {
    if(BlackLeftLoc[i][0] != MaxValUint8)
    {
      y[index] = BlackLeftLoc[i][0];
      x[index] = i;
      index++;
    }
    if(index == BlackBackLineNum)
    {
      break;  
    }
    if(i == temp1)
    {
      return 0;
    }
  }
  
  //最小二乘法
  //y = a0 + a1 * x
  a1 = LeastSquarea1(x, y, BlackBackLineNum);
  /*
  //如果直线是从左上到右下，则说明是取到了错误的线，不必再补线，全数组清零。
  if(a1 > BlackLeftBackLineSlope)
  {
    for(row = Step4StartLine; ; row--)
    {
      BlackLeftLoc[row][0] = MaxValUint8;
      if(row == 0) break;
    }
    return 0;
  }
  //否则继续。
  else
  {
  }
  */
  
  
  //直着补，不是顺着补。
  /*
  for (row = Step4StartLine + 1; row <= CameraHight - 1; row++)
  {
    BlackLeftLoc[row][0] = BlackLeftLoc[Step4StartLine][0];
  }
  */
  
  //顺着补，不是直着补。
  a0 = LeastSquarea0(x, y, a1, BlackBackLineNum);
  
  for(row = Step4StartLine + 1; row <= CameraHight - 1; row++)
  {
    //补线也有个度，向左向右都不能补出范围。
    temp = (a0 + a1 * row) / LeastSquareMulti;
    if(temp >= CameraRealWidth-1)
    {
      //BlackLeftLoc[row][0] = CameraRealWidth - 1;
      //超过左右界限时，原策略为置边界值，现在的策略是从该行起置无效值。
      for(j = row; ; j++)
      {
        BlackLeftLoc[j][0] = CameraRealWidth-1;
        if(j == CameraHight - 1) break;
      }
      break;
    }
    else if(temp <= 0)
    {
      //BlackLeftLoc[row][0] = 0;
      //超过左右界限时，原策略为置边界值，现在的策略是从该行起置无效值。
      for(j = row; ; j++)
      {
        BlackLeftLoc[j][0] = 0;
        if(j == CameraHight - 1) break;
      }
      break;
    }
    else
    {
      BlackLeftLoc[row][0] = (uint8)(temp); 
    }
  }
  
  
  
  return 1;
  
}




//左黑线连续性补线
uint8 BlackLeftContinueAdd(void)
{
    uint8 row, i, j;
    uint8 tempfirst, templast;
    uint8 fangHuiWanFlag =0;
    uint8 fangDuanLieFlag = 0;
    
    //左黑线连续性补线
    //只有左黑线采集成功才补线
    if(BlackLeftDone == 1)
    {        
        //找线尾
        for(row = CameraHight - 1; ; row--)
        {
          if(BlackLeftLoc[row][0] != MaxValUint8)
          {
            break;
          }
          else
          {
          }
          
          if(row == 0)
          {
            BlackLeftDone = 0;
            return 0;
          }
        }
        
        //给线头线尾初始化为无效值。
        tempfirst = MaxValUint8;
        templast = MaxValUint8;
                
        //开始补线
        for (; ; row--)
        {
            if (BlackLeftLoc[row][0] == MaxValUint8)
            {
                if (tempfirst == MaxValUint8)
                {
                    //若线头为无效值，就记录为线头。
                    tempfirst = row + 1;
                }
                else
                {
                }
            }
            else
            {
                if (tempfirst != MaxValUint8)
                {
                    //若线头已经有赋值，则开始记录线尾。
                    templast = row;
                    if(templast > 5)
                    {
                        if (BlackLeftLoc[templast - 1][0] != MaxValUint8 && BlackLeftLoc[templast - 2][0] != MaxValUint8 && BlackLeftLoc[templast - 3][0] != MaxValUint8
                            && BlackLeftLoc[templast - 4][0] != MaxValUint8)
                        {
                            fangDuanLieFlag = 0;
                        }
                        else
                        {
                            fangDuanLieFlag = 1;
                        }
                    }
                    else
                    {
                        fangDuanLieFlag = 1;
                    }
                    
                    if (templast >= 8)
                    {
                        if (BlackLeftLoc[templast - 1][0] != MaxValUint8 && BlackLeftLoc[templast - 2][0] != MaxValUint8 && BlackLeftLoc[templast - 3][0] != MaxValUint8
                            && BlackLeftLoc[templast - 4][0] != MaxValUint8 && BlackLeftLoc[templast - 5][0] != MaxValUint8
                                && BlackLeftLoc[templast - 6][0] != MaxValUint8 && BlackLeftLoc[templast - 7][0] != MaxValUint8 && BlackLeftLoc[templast - 8][0] != MaxValUint8)
                        {
                            fangHuiWanFlag = 0;
                        }
                        else
                        {
                            fangHuiWanFlag = 1;
                        }
                    }
                    else
                    {
                        fangHuiWanFlag = 1;
                    }
                    
                    if ((BlackLeftLoc[tempfirst][0]<=15 && BlackLeftLoc[templast][0]<=15) && (tempfirst - templast) <= 12)
                    {
                        //这种情况是防止黑线太靠镜头边界导致黑线不成功的
                        for (j = tempfirst - 1; ; j--)
                        {
                            BlackLeftLoc[j][0] = (BlackLeftLoc[tempfirst][0] + BlackLeftLoc[templast][0]) / 2;
                            if (j == templast + 1) break;
                        }
                    }
                    else if ((BlackLeftLoc[tempfirst][0] <= 15 && BlackLeftLoc[templast][0] <= 15) && (tempfirst - templast) <20
                                && (BlackLeftLoc[10][0] != MaxValUint8 || BlackLeftLoc[9][0] != MaxValUint8 || BlackLeftLoc[8][0] != MaxValUint8 || BlackLeftLoc[7][0] != MaxValUint8 || BlackLeftLoc[6][0] != MaxValUint8)
                                && fangHuiWanFlag == 0)
                    {
                        //这种情况是防止黑线太靠镜头边界导致黑线不成功的
                        for (j = tempfirst - 1; ; j--)
                        {
                            BlackLeftLoc[j][0] = (BlackLeftLoc[tempfirst][0] + BlackLeftLoc[templast][0]) / 2;
                            if (j == templast + 1) break;
                        }
                    }
                    else if ((BlackLeftLoc[tempfirst][0] <= 15 && BlackLeftLoc[templast][0] <= 15) && (tempfirst - templast) <= 35
                                && (BlackLeftLoc[6][0] != MaxValUint8 || BlackLeftLoc[7][0] != MaxValUint8 || BlackLeftLoc[8][0] != MaxValUint8 || BlackLeftLoc[10][0] != MaxValUint8 || BlackLeftLoc[9][0] != MaxValUint8)
                                && fangHuiWanFlag == 0)
                    {
                        //这种情况是防止黑线太靠镜头边界导致黑线不成功的
                        for (j = tempfirst - 1; ; j--)
                        {
                            BlackLeftLoc[j][0] = (BlackLeftLoc[tempfirst][0] + BlackLeftLoc[templast][0]) / 2;
                            if (j == templast + 1) break;
                        }
                    }
                    //防边线断裂的
                    else if (tempfirst - templast <= 5 && fangDuanLieFlag == 0)
                    {
                        for (j = tempfirst - 1; ; j--)
                        {
                            BlackLeftLoc[j][0] = (BlackLeftLoc[tempfirst][0] + BlackLeftLoc[templast][0]) / 2;
                            if (j == templast + 1) break;
                        }
                    }
                    else if(tempfirst - templast >= 3) 
                    {
                      //两者相差太多，则后面的行全部视为错误。
                      for(i = tempfirst - 1; ; i--)
                      {
                        BlackLeftLoc[i][0] = MaxValUint8;
                        if(i == 0) break;
                      }
                      //左黑线连续性标志出错。
                      BlackLeftContinueFlag = 2;
                      return 0;
                    }
                    else
                    {
                      //一共才1或2个点，直接插值就行。
                      for(j = tempfirst - 1; ; j--)
                      {
                        BlackLeftLoc[j][0] = (BlackLeftLoc[tempfirst][0] + BlackLeftLoc[templast][0]) / 2;
                        if(j == templast + 1) break;
                      }
                    }
                    tempfirst = MaxValUint8;
                    templast = MaxValUint8;
                    //左黑线连续性标志置0，不连续。
                    BlackLeftContinueFlag = 0;
                }
                else
                {
                }
            }

            if (row == 0) break;
        }
    }
    else
    {
    }    
    
    return 1;
}






//左黑线的回弯特殊检测。进来前BlackLeftDone已经置为1了。
//进来前已经进行左、右黑线的斜曲率检测。
uint8 BlackLeftRollCheck(void)
{
  uint8 temp1, temp2, temp3;
  
  //当前为单线，不进行回弯特殊检测。
  if(SingleBlackAllFlag == 1) return 1;
  
  //斜率确认。
  //若无效，直接确认。
  if(SlopeL == MaxValInt16)
  {
    temp1 = 1;
  }
  else
  {
    //斜率的确太大。
    if(SlopeL + BlackLeftRollSlopeLimit < 0)
    {
      temp1 = 1;
    }
    //斜率不大。
    else
    {
      temp1 = 0;
    }
  }
  
  //线长度确认。
  //若无效，直接确认。
  if(BlackLeftHeadLine == MaxValUint8 || BlackLeftEndLine == MaxValUint8)
  {
    temp2 = 1;
  }
  else
  {
    //线确实很短。
    if(BlackLeftEndLine - BlackLeftHeadLine < BlackLeftRollLengthLimit)
    {
      temp2 = 1;
    }
    //线很长。
    else
    {
      temp2 = 0;
    }
  }
  
  /*
  //右线斜率确认。
  //右线必须是左上到右下的，也就是正的斜率。而且斜率要求比较大。才能说明是270等大弯。
  if(SlopeR == MaxValInt16)
  {
    temp3 = 1;    
  }
  else
  {
    if(SlopeR > BlackLeftRollSlopeRLimit)
    {
      temp3 = 1;
    }
    else
    {
      temp3 = 0;
    }
  }
  */
  temp3 = 1;

  if(
    (BlackLeftStep4Flag == 3 || SingleBlackLastAllFlag == 1)   //左向后补线标志，出错。或者因为上幅图像是单线，无法开启十字，无法置向后补线出错。
  &&(BlackLeftCrossDone == 1 || BlackLeftCrossDone == 0)   //左十字标志，失败。或者未进入（线尾过高不会进行十字再检测）。
  &&(temp1 == 1)                //斜率很大。
  &&(temp2 == 1)                //线很短。
  &&(temp3 == 1)                //右线斜率很大。
    )
  {
    //检测到回弯，返回0.
    return 0;   
  }
  else
  {
    //没检测到回弯，返回1.
    return 1;
  }

}

//向后补线标志出错时的检查，这个函数是防止回弯的很重要的函数。
uint8 BlackLeftStep4ErrorCheck(void)
{
  uint8 i;
  
  //检查斜率是不是都满足直线
  for(i = 0; ; i++)
  {
    if(BlackLeft5Slope[i] == MaxValInt16) return 0;
    
    if(AbsInt(BlackLeft5Slope[i]) > PathJudgeStraightSlopeLimit)
    {
      return 0;
    }
    else
    {      
    }
    
    if(i == 5) break;
  }
  
  return 1;
  
}


//左黑线提取前的参数初始化赋值
uint8 BlackGetLeftParaInit(void)
{
  uint8 i;
  
  //左线连续性标志。默认为1连续。
  BlackLeftContinueFlag = 1;  
  
  //采集成功标志位
  BlackLeftDone = 1;  
 
  //左十字线头标志。0未进入，1超范围，2成功。
  BlackLeftCrossConfirmFlag = 0;  //默认为未进入。  
  
  //小5点斜率储存数组置初始值。
  BlackLeft5SlopeIndex = 0;  
  for(i = 0; ; i++)
  {
      BlackLeft5Slope[i] = MaxValInt16;
      BlackLeft5SlopeRow[i] = MaxValUint8;
      if(i == CameraHight - 1) break;
  }  
  
  //回弯检测
  BlackLeftRollCheckFlag = 0;        //检测到回弯。
  BlackLeftBigChangeJumpFlag = 0;    //回弯跳过标志。

  //大斜率检测时，每幅图像的初始值初始化为无效值。
  for(i = 0; i <= 4; i++)
  {
    BlackLeftBigChangeNumx[i] = MaxValUint8;
    BlackLeftBigChangeNumy[i] = MaxValUint8; 
  }
  BlackLeftBigChangeFlag = 0;
  BlackLeftBigChangeSlopeStore = MaxValInt16;

  //大斜率检测时，每行的大跳变标志置0.
  for(i = 0; ; i++)
  {
    BlackLeftBigChangeCheckRow[i] = 0;
    if(i == CameraHight - 1) break;
  }
  BlackLeftBigChangeCheckAllRow = 0;

  //左黑线储存数组初始化为无效值。
  for(i = 0; ; i++)
  {
    BlackLeftLoc[i][0] = MaxValUint8;
    if(i == CameraHight - 1) break;
  }

  //十字外尾
  BlackLeftCrossTailFlag = 2;       //十字外尾标志，0失败，1确认，2未进入。

  //窗口预测值管理
  for(i = 0; ; i++)
  {
    BlackLeftPredictL[i] = MaxValUint8;
    BlackLeftPredictR[i] = MaxValUint8; 
    if(i == CameraHight - 1) break;
  }
  
  //窗口预测储存值
  BlackLeftWinPredictStore0 = MaxValUint8;
  BlackLeftWinPredictStore1 = MaxValUint8;
  BlackLeftWinPredictStore2 = MaxValUint8;

  //第4阶段进入标志置为未进入
  BlackLeftStep4Flag = 0;
  BlackLeftStep4StartLine = MaxValUint8;
  //默认十字检测标志置为未进入。
  BlackLeftCrossDone = 0;
  
  return 1;
}






//======================================================================
//函数名：BlackGetLeft
//功  能：左黑线提取算法
//参  数：无
//返  回：1成功 0失败
//影  响：BlackLeftCrossDone，BlackLeftLoc[][],
//        BlackLeftRealWB[], BlackLeftRealW[], BlackLeftRealB[],
//        LimitLeftWB, LimitLeftW, LimitLeftB
//说  明：1. 暂时可以处理直道，弯道，十字路口3种情况。2014.11.10
//      
//             
//======================================================================
uint8 BlackGetLeft(void)
{
    uint8 row, temp, temp1, temp2, temp3, i;
    uint8 GapCount, Row1RealLastLine, index, index1;
    uint8 tempWB, tempW, tempB;
    uint8 tempminus;
    uint8 successRow = MaxValUint8;
    
    int temp001;
    
    //左黑线提取前的参数初始化赋值
    BlackGetLeftParaInit();
    
    //第1阶段。这1阶段有可能会返回黑线提取失败
    
    //每幅有效图像的线头的3个阈值单独储存。Step1Scan前调用前一幅有效图像保存的3个阈值。
    LimitLeftWB = LimitLeftHeadWB;
    LimitLeftW = LimitLeftHeadW;
    LimitLeftB = LimitLeftHeadB;
    
    temp = BlackGetLeftStep1Scan(CameraHight - 1, CameraHight - BlackStep1Size - 1, 1);
    BlackLeftStep1ScanRow = temp;
    
    //右线开始扫描的情况下，左线尾比右线头高，直接返回失败。增加后面这个条件是为了防止十字误判。
    if(
       (BlackGetPreDir == 1)
     &&(temp <= BlackRightHeadLine)
     &&(BlackRightHeadLine < CameraHight - 7)
      )
    {
        BlackLeftLoc[temp][0] = MaxValUint8;
        BlackLeftLoc[temp - 1][0] = MaxValUint8;
        return 0;
    }
    else
    {
    }
    
    if(temp == MaxValUint8)
    {
      return 0;
    }    
    else 
    {     
              
      //每幅有效图像的线头的3个阈值单独储存。若线头提取成功，则该幅图像有效，线头3个阈值更新。
      LimitLeftHeadWB = LimitLeftWB;
      LimitLeftHeadW = LimitLeftW;
      LimitLeftHeadB = LimitLeftB;
      
      row = temp;
      //如果起始线头的位置非常高，就认为是十字的前两个角，需要进入第4阶段向后补线。    
      if(row < CameraHight - BlackLeftBackLineStart)
      {
        BlackLeftStep4Flag = 1;               //默认为失败
        BlackLeftStep4StartLine = row;
      }
    }
    
    //第2阶段。到了这一阶段，已经不会返回黑线提取失败 
    //temp为预测的黑点位置   

    //对预测值的大小限定
    temp = BoundaryLimitRe(BlackLeftLoc[row-1][0] + BlackLeftLoc[row-1][0] - BlackLeftLoc[row][0], 
                           0, 
                           CameraRealWidth - 1);
    //预测值更新后就刷新一次堆栈，堆栈顶部丢失。
    BlackLeftWinPredictStore0 = BlackLeftWinPredictStore1;
    BlackLeftWinPredictStore1 = BlackLeftWinPredictStore2;
    BlackLeftWinPredictStore2 = temp;

    index = 0;
    index1 = 0;
    
    GapCount = 0;  //连续多少行没有采集到黑线
    successRow = MaxValUint8; //记录最后一次成功的行
    //从最近的认证行到取最远的行
    for(row -= 2; ; row -= 2)
    {      
      //连续2行检测黑点。
      temp1 = BlackGetLeftStep2Win(row, BlackLeftWinVal, temp, 1);
      temp2 = BlackGetLeftStep2Win(row-1, BlackLeftWinVal, temp, 2);
      
      temp3 = (
              (temp1 == 0)
            &&(temp2 == 0)
            //&&(BlackLeftStep4Flag == 0)
              );
      
      //若连续2行黑点检测成功，则对3个阈值进行更新，且更新预测点位置。
      if(temp1 == 1 && temp2 == 1) 
      {
        //只要连续采集到双黑线，则两个标志位重置为0.
        index = 0;
        index1 = 0;
        successRow = row;
        
        //差值的处理，留有裕度，且有上下界。
        tempWB = (BlackLeftRealWB[0] + BlackLeftRealWB[1]) / 2 - LimitLeftWBMargin; 
        if(tempWB < LimitLeftWBMin) tempWB = LimitLeftWBMin;
        else if(tempWB > LimitLeftWBMax) tempWB = LimitLeftWBMax;
        else ;
        LimitLeftWB = tempWB;
        
        //白点的处理，留有裕度，且有上下界。
        tempW = (BlackLeftRealW[0] + BlackLeftRealW[1]) / 2 - LimitLeftWMargin;
        if(tempW < LimitLeftWMin) tempW = LimitLeftWMin;
        else if(tempW > LimitLeftWMax) tempW = LimitLeftWMax;
        else ;
        LimitLeftW = tempW;
        
        //黑点的处理，留有裕度，且有上下界。
        tempB = (BlackLeftRealB[0] + BlackLeftRealB[1]) / 2 + LimitLeftBMargin;
        if(tempB < LimitLeftBMin) tempB = LimitLeftBMin;
        else if(tempB > LimitLeftBMax) tempB = LimitLeftBMax;
        else ;
        //黑色阈值不能超过白色
        if(tempB >= LimitLeftW)
        {
          tempB = LimitLeftW - 1;
        }
        else
        {
        }
        LimitLeftB = tempB; 
         
        //对预测值的大小限定
        temp = BoundaryLimitRe(BlackLeftLoc[row-1][0] + BlackLeftLoc[row-1][0] - BlackLeftLoc[row][0], 
                               0, 
                               CameraRealWidth - 1);
        
        //预测值更新后就刷新一次堆栈，堆栈顶部丢失。
        BlackLeftWinPredictStore0 = BlackLeftWinPredictStore1;
        BlackLeftWinPredictStore1 = BlackLeftWinPredictStore2;
        BlackLeftWinPredictStore2 = temp;
      }
      //如果没能连续两行检测黑点成功，原本打算用原位置的预测点，后来发现效果不好，所以决定按预测点的趋势走。
      //如果连续两行均采集黑线失败，则记数，记到一定值就再来一次Step1
      //如果第4阶段检测标志置位，就不用进入十字检测了。
      else if(temp3 == 1)
      {        
        //若上一幅图是单线，则不允许开启十字。
        if(SingleBlackLastAllFlag == 1) break;        
        //直角黑块锁存期间，为了防止一边十字成功另一边不成功造成的中心线倾斜，所以不允许开启十字。
        if(AngleZoneConfirmLockFlag == 1) break;
        
        //第一次检测到双线失败时，最后的几条线可能已经不太正确。
        if(index1 == 0)
        {
          //如果储存的3个值趋势相同，也就是变化量同号，那么就按正常的来。否则就认为最后一个值已经错误。
          if(row <= CameraHight - 3)
          {
              //若前面两行没有被大跳变删除掉。
              if (BlackLeftLoc[row + 1][0] != MaxValUint8 && BlackLeftLoc[row + 2][0] != MaxValUint8)
              {


                  //如果黑线点够，最好还是根据实际的黑点来更新窗口值。
                  tempminus = BoundaryLimitRe(BlackLeftLoc[row + 1][0] * 3 - BlackLeftLoc[row + 2][0] * 2,
                                              0,
                                              CameraRealWidth - 1);
                  //斜入十字不管怎么摆，左十字的上段线一定在下段线的右边。
                  tempminus = MaxRe(tempminus, BlackLeftLoc[row + 1][0]);
              }
              else
              {
                  temp001 = BlackLeftWinPredictStore1 - BlackLeftWinPredictStore0;
                  BlackLeftWinPredictStore0 += temp001;
                  BlackLeftWinPredictStore1 += temp001;
                  BlackLeftWinPredictStore2 = BlackLeftWinPredictStore1 + BlackLeftWinPredictStore1 - BlackLeftWinPredictStore0;
                  tempminus = BoundaryLimitRe(BlackLeftWinPredictStore2 + BlackLeftWinPredictStore2 - BlackLeftWinPredictStore1,
                                              0,
                                              CameraRealWidth - 1);       
              }
          }
          else
          {
              //理论上不会走到这里。
              return 0;
          }
          
          //进来一次后就再也不进来了。
          index1++;          
        }
        else
        {
          //预测点按趋势走也要限幅。
          tempminus = BoundaryLimitRe(BlackLeftWinPredictStore2 + BlackLeftWinPredictStore2 - BlackLeftWinPredictStore1,
                                      0,
                                      CameraRealWidth - 1);          
        }
        //temp在这里表示窗口预测点位置。
        temp = tempminus;
        BlackLeftWinPredictStore0 = BlackLeftWinPredictStore1;
        BlackLeftWinPredictStore1 = BlackLeftWinPredictStore2;
        BlackLeftWinPredictStore2 = temp;
              
        //Row1RealLastLine记录第1段黑线的最后一行。
        if(index == 0)
        {
          //进来一次后，就再也不会进来了。
          index++;
          for(i = row; i <= CameraHight - 1; i++)
          {
            if(BlackLeftLoc[i][0] != MaxValUint8)
            {
              Row1RealLastLine = i;
              break;
            }
          }
          if(Row1RealLastLine >= CameraHight - 1)
          {
            Row1RealLastLine = CameraHight - 1;
          }
          else
          {
          }
          //记录为全局变量
          BlackLeftRow1RealLastLine = Row1RealLastLine;
        }
        
        GapCount += 2;
        if (successRow != MaxValUint8 && BlackLeftLoc[successRow][0]<10 && BlackLeftLoc[successRow - 1][0]<10) { }
        ///////////////////////////////进入十字重新检测的条件还要再严格一些，减少没必要的时间，比如说前面最多只允许检测到多少行。
        else if(GapCount > BlackCrossGapLine)
        {
          //再检测时的行太远了，就没必要检测了。
          if(row < BlackLeftCrossTooLowLine)
          {
            //扫屁股。
            for(i = row; ; i--)
            {
              BlackLeftLoc[i][0] = MaxValUint8;
              if(i == 0) break;
            }
            //跳出来。
            break;
          }
          else
          {
          }
          
          //进入十字重新检测后，就不允许出现第4阶段向后补线的情况。
          if(BlackLeftStep4Flag == 1)
          {
            BlackLeftStep4Flag = 3;   //向后补线出错。
          }
          else
          {
          }
          
          //十字再检测的话，Step1Scan的线头必须够近。
          //最后只剩第0，1行没有检测的话，就不用开启十字再检测了。
          if(BlackLeftStep1ScanRow >= CameraHight - 3 && row > 3)
          {            
            //正常的十字再检测。
            if(BlackLeftAgainForCross(row - 2, 1, Row1RealLastLine, temp) == 0)
            {
              BlackLeftCrossDone = 1;  //十字检测失败           
            }
            else
            {
              BlackLeftCrossDone = 2;  //十字检测成功
            }
            break;  //十字再检测里会开窗检测到指定的最后一行，所以不用再循环了，直接跳出
          }
          //线头太远，或者只剩第0，1行的时候，直接break就好，远处的行在初始化时已经置为无效值。
          else
          {
            break;
          }
          
        }
        else
        {
          BlackLeftCrossDone = 0;   //十字检测未进入
        }
      }
      //连续两线只有一线检测成功的情况，仍然要按趋势更新窗口预测值。
      else if((temp1 == 1 && temp2 == 0) || (temp1 == 0 && temp2 == 1)) 
      {        
        
        //第一次检测到有单条线失败时，最后的几条线可能已经不太正确。
        if(index1 == 0)
        {
          //如果储存的3个值趋势相同，也就是变化量同号，那么就按正常的来。否则就认为最后一个值已经错误。
          if(row <= CameraHight - 3)
          {
              //若前面两行没有被大跳变删除掉。
              if (BlackLeftLoc[row + 1][0] != MaxValUint8 && BlackLeftLoc[row + 2][0] != MaxValUint8)
              {


                  //如果黑线点够，最好还是根据实际的黑点来更新窗口值。
                  tempminus = BoundaryLimitRe(BlackLeftLoc[row + 1][0] * 3 - BlackLeftLoc[row + 2][0] * 2,
                                              0,
                                              CameraRealWidth - 1);
                  //斜入十字不管怎么摆，左十字的上段线一定在下段线的右边。
                  tempminus = MaxRe(tempminus, BlackLeftLoc[row + 1][0]);
              }
              else
              {
                  temp001 = BlackLeftWinPredictStore1 - BlackLeftWinPredictStore0;
                  BlackLeftWinPredictStore0 += temp001;
                  BlackLeftWinPredictStore1 += temp001;
                  BlackLeftWinPredictStore2 = BlackLeftWinPredictStore1 + BlackLeftWinPredictStore1 - BlackLeftWinPredictStore0;
                  tempminus = BoundaryLimitRe(BlackLeftWinPredictStore2 + BlackLeftWinPredictStore2 - BlackLeftWinPredictStore1,
                                              0,
                                              CameraRealWidth - 1);       
              }
          }
          else
          {
              //理论上不会走到这里。
              return 0;
          }
          
          //进来一次后就再也不进来了。
          index1++;          
        }
        else
        {
          //预测点按趋势走也要限幅。
          tempminus = BoundaryLimitRe(BlackLeftWinPredictStore2 + BlackLeftWinPredictStore2 - BlackLeftWinPredictStore1,
                                      0,
                                      CameraRealWidth - 1);          
        }
        //temp在这里表示窗口预测点位置。
        temp = tempminus;
        BlackLeftWinPredictStore0 = BlackLeftWinPredictStore1;
        BlackLeftWinPredictStore1 = BlackLeftWinPredictStore2;
        BlackLeftWinPredictStore2 = temp;
      }
      //理论上不会走到这里。
      else
      {
      }
      
      if((row == 0) || (row == 1)) break;
      
    }
    
    //第4阶段，采集到十字的前两个角，向后补线。这里有浮点运算，要严格控制调用。
    //向后补线时，必须考虑的就是十字外尾的问题。
    if(BlackLeftStep4Flag == 1)
    {
      if(BlackLeftStep4BackLine(BlackLeftStep4StartLine, 0) == 0)
      {
        BlackLeftStep4Flag = 1;    //向后补线失败，有可能是点取不够，有可能是斜率不对。
        //return 0;  
      }
      else
      {
        BlackLeftStep4Flag = 2;    // 向后补线成功。
      }
    }
    //BlackLeftStep4Flag出错的情况。
    else if(BlackLeftStep4Flag == 3)
    {
      //检查出错到底是为什么
      if(BlackLeftStep4ErrorCheck() == 1)
      {        
        if(BlackLeftStep4BackLine(BlackLeftStep4StartLine, 0) == 0)
        {
          //BlackLeftStep4Flag = 1;    //向后补线失败，有可能是点取不够，有可能是斜率不对。
          //return 0;  
        }
        else
        {
          BlackLeftStep4Flag = 2;    // 向后补线成功。
        }
      }
      else
      {
      }
      
    }
    //BlackLeftStep4Flag未进入的情况。
    else
    {       
    }

    return 1;
}



//====================================右右右右右右右右右右右右右右右右右============================

//======================================================================
//函数名：BlackRightRealClear
//功  能：右黑线提取中，对3个关键值的实际值清零
//参  数：无
//返  回：无
//影  响：BlackRightRealWB[], BlackRightRealW[], BlackRightRealB[]
//说  明：
//      
//             
//======================================================================
void BlackRightRealClear(void)
{
    BlackRightRealWB[0] = 0;
    BlackRightRealWB[1] = 0;
    BlackRightRealW[0] = 0;
    BlackRightRealW[1] = 0;
    BlackRightRealB[0] = 0;
    BlackRightRealB[1] = 0;
}




//黑线提取成功，找右黑线的线头线尾。
uint8 BlackRightHeadEndSearch(void)
{
  uint8 i;
  //右线线头和线尾的查找
  if(BlackRightDone == 1)
  { //右线线头的查找。
    for(i = 0; ; i++)
    { //找到了右线线头。
      if(BlackRightLoc[i][0] != MaxValUint8)
      {
        BlackRightHeadLine = i;
        break;
      }
      //直到最近一行仍没有找到线头，则查找失败。
      if(i == CameraHight - 1) 
      {
        BlackRightHeadLine = MaxValUint8;
        break;
      }
    }
    //若右线线头查找失败，就不用找右线线尾了。
    if(BlackRightHeadLine == MaxValUint8)
    {
      BlackRightEndLine = MaxValUint8;
      BlackRightDone = 0;
    }
    //若右线线头查找成功，则开始找右线线尾。
    else
    {
      for(i = CameraHight - 1; ; i--)
      { //找到了右线的线尾。
        if(BlackRightLoc[i][0] != MaxValUint8)
        {
          BlackRightEndLine = i;
          break;
        }
        //没找到右线的线尾。右线不可能有头无尾，所以均置无效值。
        if(i == BlackRightHeadLine) 
        {
          BlackRightHeadLine = MaxValUint8;
          BlackRightEndLine = MaxValUint8;
          break;
        }
      }
    }
  }
  //若右线黑线提取失败，就不用找线头和线尾了。
  else
  {
    BlackRightHeadLine = MaxValUint8;
    BlackRightEndLine = MaxValUint8;
  }
  
  //本幅图像有单线时，删掉最远一个点。
  if(SingleBlackAllFlag == 1)
  {
    BlackRightLoc[BlackRightHeadLine][0] = MaxValUint8; //Udis数组不用改，因为线头线尾查找之后才轮到矫正函数。
    BlackRightHeadLine += 1;
  }
  else
  {
  }
  
  return 1;
}


//======================================================================
//函数名：BlackRightBigChangeCheck
//功  能：右黑线提取时，大斜率检测。
//参  数：newrow新加进来的需要判定是否有效的行, use调用时是连续2行的第几行。
//返  回：1该行黑线成功，0该行黑线失败。
//影  响：
//说  明：1. 用来防止大斜率突变的坏点。
//        2. 共5个行，取1，3，5行进行斜率计算，每进一个新行，移出一个旧行。
//             
//======================================================================
uint8 BlackRightBigChangeCheck(uint8 newrow, uint8 use)
{
  uint8 tempstorex, tempstorey;
  uint8 i;
  int32 tempslope1, tempslope2;
  
  //tempstorex用来储存将要移出的一行，用来检测失败时复原5行数组。
  tempstorex = BlackRightBigChangeNumx[0];
  BlackRightBigChangeNumx[0] = BlackRightBigChangeNumx[1];
  BlackRightBigChangeNumx[1] = BlackRightBigChangeNumx[2];
  BlackRightBigChangeNumx[2] = BlackRightBigChangeNumx[3];
  BlackRightBigChangeNumx[3] = BlackRightBigChangeNumx[4];
  BlackRightBigChangeNumx[4] = newrow;

  tempstorey = BlackRightBigChangeNumy[0];
  BlackRightBigChangeNumy[0] = BlackRightBigChangeNumy[1];
  BlackRightBigChangeNumy[1] = BlackRightBigChangeNumy[2];
  BlackRightBigChangeNumy[2] = BlackRightBigChangeNumy[3];
  BlackRightBigChangeNumy[3] = BlackRightBigChangeNumy[4];
  BlackRightBigChangeNumy[4] = BlackRightLoc[newrow][0];

  //每幅图最多只进一次这个for循环，可以省时间。
  if(BlackRightBigChangeJumpFlag == 0)
  {  
    //储存数组的有效性判断，若不足5个数，则无法进行大斜率检测，直接返回成功。
    for(i = 0; i <= 4; i++)
    {
      if(BlackRightBigChangeNumx[i] == MaxValUint8)
      {
        return 1;
      }
    }
    //改变标志位，下次就不进来了。
    BlackRightBigChangeJumpFlag = 1;   
  }
  //走到这里说明通过了数组有效性判断。
  
  //取1，3，5点进行斜率计算。
  //因为只有两个点，所以斜率的计算采取斜率的初始定义的方式，不采取最小二乘法，可以省很多时间。
  tempslope1 = (BlackRightBigChangeNumy[2] - BlackRightBigChangeNumy[0]) 
            * LeastSquareMulti   //乘以了这么多倍。跟最小二乘法一样。
            / (BlackRightBigChangeNumx[2] - BlackRightBigChangeNumx[0]);
  tempslope2 = (BlackRightBigChangeNumy[4] - BlackRightBigChangeNumy[2]) 
            * LeastSquareMulti   //乘以了这么多倍。跟最小二乘法一样。
            / (BlackRightBigChangeNumx[4] - BlackRightBigChangeNumx[2]);
  BlackRightBigChangeSlopeStore = tempslope1;
  //小5点斜率储存在数组中，由于储存的是tempslope1，所以大跳变点不会记录，会出现多行相同值。
  BlackRight5Slope[BlackRight5SlopeIndex] = BlackRightBigChangeSlopeStore;
  BlackRight5SlopeRow[BlackRight5SlopeIndex] = newrow;
  BlackRight5SlopeIndex++;  
  //1. 前后两次的斜率是相反的。
  if(tempslope1 >= 0 && tempslope2 < 0)
  {
    //2. 两者差值很大很大。
    if(tempslope1 >= tempslope2 + BlackRightBigChangeSlopeLimit)
    {
      //两者差值的确很大。不会刷新储存值。
      //if(use == 2)                    //另一种方案是在连续2行的第2行调用时才置成功标志位。
      //{
        //置成功标志位。
        BlackRightBigChangeFlag = 1;
      //}
      //else
      //{
      //}
      //此次行不进入队列。
      BlackRightBigChangeNumx[4] = BlackRightBigChangeNumx[3];
      BlackRightBigChangeNumx[3] = BlackRightBigChangeNumx[2];
      BlackRightBigChangeNumx[2] = BlackRightBigChangeNumx[1];
      BlackRightBigChangeNumx[1] = BlackRightBigChangeNumx[0];
      BlackRightBigChangeNumx[0] = tempstorex;
      BlackRightBigChangeNumy[4] = BlackRightBigChangeNumy[3];
      BlackRightBigChangeNumy[3] = BlackRightBigChangeNumy[2];
      BlackRightBigChangeNumy[2] = BlackRightBigChangeNumy[1];
      BlackRightBigChangeNumy[1] = BlackRightBigChangeNumy[0];
      BlackRightBigChangeNumy[0] = tempstorey;

      //将前面3个右黑线(共4个)的值置无效值。图像上更连续。
      BlackRightLoc[BlackRightBigChangeNumx[4]][0] = MaxValUint8;
      BlackRightLoc[BlackRightBigChangeNumx[3]][0] = MaxValUint8;
      //BlackRightLoc[BlackRightBigChangeNumx[2]][0] = MaxValUint8;
      
      return 0;
    }
    else
    {
      //刷新斜率储存值。
      //BlackRightBigChangeSlopeStore = tempslope;
      return 1;
    }
  }
  //1. 前后两次的斜率是相反的。
  else if(tempslope1 < 0 && tempslope2 >= 0)
  {
    //2. 两者差值很大很大。
    if(tempslope2 >= tempslope1 + BlackRightBigChangeSlopeLimit)
    {
      //两者差值的确很大。不会刷新储存值。
      //if(use == 2)                    //另一种方案是在连续2行的第2行调用时才置成功标志位。
      //{
        //置成功标志位。
        BlackRightBigChangeFlag = 1;
      //}
      //else
      //{
      //}
      //此次行不进入队列。
      BlackRightBigChangeNumx[4] = BlackRightBigChangeNumx[3];
      BlackRightBigChangeNumx[3] = BlackRightBigChangeNumx[2];
      BlackRightBigChangeNumx[2] = BlackRightBigChangeNumx[1];
      BlackRightBigChangeNumx[1] = BlackRightBigChangeNumx[0];
      BlackRightBigChangeNumx[0] = tempstorex;
      BlackRightBigChangeNumy[4] = BlackRightBigChangeNumy[3];
      BlackRightBigChangeNumy[3] = BlackRightBigChangeNumy[2];
      BlackRightBigChangeNumy[2] = BlackRightBigChangeNumy[1];
      BlackRightBigChangeNumy[1] = BlackRightBigChangeNumy[0];
      BlackRightBigChangeNumy[0] = tempstorey;
      
      //将前面3个右黑线(共4个)的值置无效值。图像上更连续。
      BlackRightLoc[BlackRightBigChangeNumx[4]][0] = MaxValUint8;
      BlackRightLoc[BlackRightBigChangeNumx[3]][0] = MaxValUint8;
      //BlackRightLoc[BlackRightBigChangeNumx[2]][0] = MaxValUint8;   
      
      return 0;
    }
    else
    {
      //刷新斜率储存值。
      //BlackRightBigChangeSlopeStore = tempslope;
      return 1;
    }    
  }
  //若不相反，则判定无大斜率的跳变。返回成功。
  else
  {
    //刷新斜率储存值。
    //BlackRightBigChangeSlopeStore = tempslope;
    return 1;
  }
  
}


//单纯的、从左往右的、一定窗口内的、扫描跳变沿的函数.可以精简Step2Win, CrossSpeWin等诸多内容。
//返回的是跳变沿的白点所在位置。
uint8 BlackGetRightJustWin(uint8 row, int32 colleft, int32 colright)
{
  uint8 abs1;
  uint8 tempwhite;
  uint8 temp1;
  
  //左右边界限幅
  colleft = BoundaryLimitRe(colleft, 0, CameraRealWidth - 1);
  colright = BoundaryLimitRe(colright, 0, CameraRealWidth - 1);
  
  for(; colleft < colright; colleft++) 
  {
    //左白点与右黑点的像素差
    if(ImgNew[row][colleft] > ImgNew[row][colleft+1+BlackRightEdgeNum])
    {
      abs1 = ImgNew[row][colleft] - ImgNew[row][colleft+1+BlackRightEdgeNum];
    }
    else
    {
      abs1 = 0;
    }

    if( //跳变沿的第一批条件：
        //黑够黑，白够白，差够大。
        //这里的3个阈值是自适应之后调整的值
          (abs1 > LimitRightWB - LimitRightWBAdjust[row]) //两者的差够大
        &&(ImgNew[row][colleft] > LimitRightW)                     //白点够白
        &&(ImgNew[row][colleft+1+BlackRightEdgeNum] < LimitRightB)                   //黑点够黑
       )
    {
          //如果左边白点不够话，就不用判断白点是否满足阈值了。
          if(colleft > 2)
          {
            tempwhite = ((ImgNew[row][colleft-1] > LimitRightW) && (ImgNew[row][colleft-2] > LimitRightW));
          }
          else
          {
            tempwhite = 1;
          }
          
          //近处的行，向右3个点仍为黑点，向左2个点仍为白点。
          if(row > BlackMiddleLine)
          {
            temp1 = (
                      (ImgNew[row][colleft+2+BlackRightEdgeNum] < LimitRightB)
                    &&(ImgNew[row][colleft+3+BlackRightEdgeNum] < LimitRightB)
                    //&&(ImgNew[row][colleft+4+BlackRightEdgeNum] < LimitRightB)
                    &&(tempwhite == 1)
                    );
          }
          else
          {
            //中间的行 ，向右2个点为仍为黑点，向左1个点仍为白点。
            if(row > BlackFarLine)
            {
              temp1 = (
                        (ImgNew[row][colleft+2+BlackRightEdgeNum] < LimitRightB)
                      //&&(ImgNew[row][colleft+3+BlackRightEdgeNum] < LimitRightB)
                      &&(tempwhite == 1)
                      );
            }          
            else
            {
              //远处的行，向右1个点仍为黑点，向左0个点仍为白点。
              if(row > BlackFarfarLine)
              {
                //temp1 = (ImgNew[row][colleft+2+BlackRightEdgeNum] < LimitRightB);
                temp1 = 1;
              }
              //很远处的行，只对跳变有要求，对周围黑点无要求。
              else
              {
                temp1 = 1;
              }
            }
          }
          
          //跳变沿的第二批条件：
          if(temp1 == 1)
          {
            //两批条件均满足，返回白点位置，不是黑点位置！
            return colleft;
            
          //break;               
          }// end of 跳变沿检测的第2批条件
    }// end of 跳变沿检测的第1批条件
    
    
    
  }// end of for循环
  
  //如果一直到窗口最左边还没有找到跳变沿，返回无效值。
  return MaxValUint8;
    
}


//======================================================================
//函数名：BlackGetRightStep1Edge
//功  能：右黑线提取中，第1阶段跳变检测函数,处理该右左线的可疑黑点
//参  数：待检测的行号row, 第几次使用use, 黑线预处理标志preflag(1是预处理)
//返  回：1成功 0失败
//影  响：BlackRightRealWB[], BlackRightRealW[], BlackRightRealB[]
//        BlackRightLoc[][]
//说  明：1. 对第row行的跳变向右进行检测，成功就赋相应的值（有3个备用值），失败则赋255
//        2. 两批条件均满足时。则检测成功。会有最多3个值进行记录，现阶段只用到了第1个值
//        3. 里面有涉及3个关键值的实际值的记录，CrossFlag的情况就不用记录（也就不用更改）
//        4. 若1个黑点也没有，或者超过3个黑点，则检测失败。
//        5. 若要检测黑到白的跳变，或者任意跳变，改变第一批条件即可。
//======================================================================
uint8 BlackGetRightStep1Edge(uint8 row, uint8 use, uint8 preflag)
{
  uint8 index, col, temp1;
  uint8 abs1;
  uint8 RightBeginCol; //右线跳变检测起点
  uint8 tempwhite;
  uint8 avgr1, avgl1;
  uint8 i;
    
  //根据上一幅有效图像的中线来预估这一幅图像的左右黑线扫描起点，右线起点需要在中线往左一个黑线宽度。 
  RightBeginCol = (uint8)MaxRe((int32)BlackRightEdgeStartCol-10,0);
  
  //左线开始扫描。
  //如果是预处理，也不用考虑这个情况。直接往下走就好。
  if((BlackGetPreDir == 0 || BlackGetPreDir == 2)
  &&(BlackLeftLoc[row][0] != MaxValUint8)
  &&(preflag == 0)
    )
  {
    //如果上一幅图是单线，则右线扫描起点必须在左线的左边一个单线宽度。
    if(
      (row > CameraHight - 5)
    &&(SingleBlackLastAllFlag == 1)
       )
    {
      RightBeginCol = MinRe(RightBeginCol, MaxRe(0, BlackLeftLoc[row][0] - SingleBlackWidthMax[row]));
    }
    //如果上一幅图不是单线，且最近四行没有采集到单线，那就视为不可能出现单线。所以右线扫描起点必须在左线的右边
    else if(
           (row <= CameraHight - 5)
         //&&(SingleBlackLastAllFlag == 0)
            )
    {
      RightBeginCol = MaxRe(RightBeginCol, BlackLeftLoc[row][0]);
    }
    else
    {
      RightBeginCol = MaxRe(RightBeginCol, BlackLeftLoc[row][0]);
    }
    
  }
  else
  {    
  }
  
  //窗口预测值记录
  BlackRightPredictL[row] = RightBeginCol;
  BlackRightPredictR[row] = CameraRealWidth - 5 - BlackRightEdgeNum;
  
  index = 0;         //可疑点的序号
  //从中间到最右边扫描，隔1个点求差
  for(col = RightBeginCol; col <= CameraRealWidth - 5 - BlackRightEdgeNum; col++) 
  {
      //左白点与右黑点的像素差
      if(ImgNew[row][col] > ImgNew[row][col+1+BlackRightEdgeNum])
      {
        abs1 = ImgNew[row][col] - ImgNew[row][col+1+BlackRightEdgeNum];
      }
      else
      {
        abs1 = 0;
      }
      
      if( //跳变沿的第一批条件：
          //黑够黑，白够白，差够大。
          //这里的3个阈值为初始值，不是自适应之后调整的值。
          (abs1 > LimitRightWB - LimitRightWBAdjust[row]) //两者的差够大
        &&(ImgNew[row][col] > LimitRightW)                     //白点够白
        &&(ImgNew[row][col+1+BlackRightEdgeNum] < LimitRightB) //黑点够黑
         )
      {         
          //左边没有白点的话就不会判断白点是否满足阈值了。
          if(col > 2)
          {
            tempwhite = ((ImgNew[row][col-1] > LimitRightW) && (ImgNew[row][col-2] > LimitRightW));
          }
          else
          {
            tempwhite = 1;
          }
          
          //跳变沿的第二批条件：
          //近处的行，向右3个点仍为黑点，向左2个点仍为白点。
          if(row > BlackMiddleLine)
          {
            temp1 = (
                      (ImgNew[row][col+2+BlackRightEdgeNum] < LimitRightB)
                    &&(ImgNew[row][col+3+BlackRightEdgeNum] < LimitRightB)
                    &&(ImgNew[row][col+4+BlackRightEdgeNum] < LimitRightB)
                    &&(tempwhite == 1)
                    );
          }
          else
          {
            if(row > BlackFarLine)
            {
              //中间的行，向右2个点仍为黑点，向左1个点仍为白点。
              temp1 = (
                        (ImgNew[row][col+2+BlackRightEdgeNum] < LimitRightB)
                      &&(ImgNew[row][col+3+BlackRightEdgeNum] < LimitRightB)
                      &&(tempwhite == 1)
                      );
            }
            else
            {
              //远处的行，向右1个点仍为黑点，向左0个点仍为白点。
              if(row > BlackFarfarLine)
              {
                temp1 = (ImgNew[row][col+2+BlackRightEdgeNum] < LimitRightB);
              }
              //很远处的行，只对跳变有要求，对周围黑点无要求。
              else
              {
                temp1 = 1;
              }             
            }
          }
          
          if(temp1)
          {
              //跳变检测成功，取黑点
              BlackRightLoc[row][index] = col + 1 + BlackRightEdgeNum;          
              index++;
              break;
            
          }    //end of 跳变沿的第二批条件
      }   //end of 跳变沿的第一批条件
  }// end of 从中间到最左边扫描，隔1个点求差
  
  //1个可疑黑点也没有，则报错。
  if(index == 0)
  {
    //如果是黑线预处理的调用，也不用更新这几个值。
    if(preflag == 0)
    {
      //对3个关键值的实际值清零
      BlackRightRealClear();
    }
      
    //该行定位黑线失败，赋无效值
    BlackRightLoc[row][0] = MaxValUint8;
    return 0;
  }
  
  //不是黑线预处理的调用，则需要进行大跳变的检测。
  if(preflag == 0)
  {
    //最终确认前，需要对大斜率进行检测。
    if(BlackRightBigChangeCheck(row, use) == 1)
    {
      //从左线开始的扫描。单线一定出现在BlackGetPreDir == 0的情况。
      //若已经确认为单线了，就不用再次确认了。
      if(
        (SingleBlackAllFlag == 0) 
      &&(BlackGetPreDir == 0 || BlackGetPreDir == 2)   //0713文件夹里的error002.txt里的第31幅图过渡到第32幅图时，若BlackGetPreDir不等于2的话，会出问题。
        )
      {
        //若该行左右黑线均检测成功，且右线在左线的左边一定范围内，则该行判定为单线。
        if((BlackLeftLoc[row][0] != MaxValUint8)
         &&(BlackRightLoc[row][0] != MaxValUint8)
         &&(BlackRightLoc[row][0] < BlackLeftLoc[row][0])
         &&(BlackRightLoc[row][0] - BlackLeftLoc[row][0] < SingleBlackWidthMax[row]) //结果是负数也会满足，所以必须小于。
        )
        {
          //检查5个白点的平均值是否相近，若相近，则是单线，若不相近，则置无效值。
          if(
            (BlackRightLoc[row][0] - 1 - BlackRightEdgeNum - 5 < 0)
          ||(BlackLeftLoc[row][0] + 1 + BlackLeftEdgeNum + 5 > CameraRealWidth - 1)
            )
          {
            //不哆5个点。
            //因为进行过大跳变，所以要清除。只针对最近几行。
            if(row > CameraHight - 5)
            {
                for(i = 0; i <= 4; i++)
                {
                    BlackRightBigChangeNumx[i] = MaxValUint8;
                    BlackRightBigChangeNumy[i] = MaxValUint8;
                }
                BlackRightBigChangeJumpFlag = 0;
            }
            else
            {
            }
            //该行定位黑线失败，赋无效值
            BlackRightLoc[row][0] = MaxValUint8;
            return 0;
          }
          else
          {
            //该行右线的白点平均值
            avgr1 = (ImgNew[row][BlackRightLoc[row][0] - 1 - BlackRightEdgeNum - 1]
                   + ImgNew[row][BlackRightLoc[row][0] - 1 - BlackRightEdgeNum - 2]
                   + ImgNew[row][BlackRightLoc[row][0] - 1 - BlackRightEdgeNum - 3]
                   + ImgNew[row][BlackRightLoc[row][0] - 1 - BlackRightEdgeNum - 4]
                   + ImgNew[row][BlackRightLoc[row][0] - 1 - BlackRightEdgeNum - 5])
                   / 5;
            //该行左线的白点平均值
            avgl1 = (ImgNew[row][BlackLeftLoc[row][0] + 1 + BlackLeftEdgeNum + 1]
                   + ImgNew[row][BlackLeftLoc[row][0] + 1 + BlackLeftEdgeNum + 2]
                   + ImgNew[row][BlackLeftLoc[row][0] + 1 + BlackLeftEdgeNum + 3]
                   + ImgNew[row][BlackLeftLoc[row][0] + 1 + BlackLeftEdgeNum + 4]
                   + ImgNew[row][BlackLeftLoc[row][0] + 1 + BlackLeftEdgeNum + 5])
                   / 5;    
            if(AbsInt(avgr1 - avgl1) < 20)
            {
              //5个白点之间的差别够小，单线判断成功。
              SingleBlackFlag[row] = 1;      
              //SingleBlackAllFlag = 1;   //总幅图像的单线标志，只要有一行检测单线成功，则置1.
            }
            else
            {
              //5个白点之间的差别太大，单线判断失败。
              //因为进行过大跳变，所以要清除。只针对最近几行。
              if(row > CameraHight - 5)
              {
                  for(i = 0; i <= 4; i++)
                  {
                      BlackRightBigChangeNumx[i] = MaxValUint8;
                      BlackRightBigChangeNumy[i] = MaxValUint8;
                  }
                  BlackRightBigChangeJumpFlag = 0;
              }
              else
              {
              }
              
              //该行定位黑线失败，赋无效值
              BlackRightLoc[row][0] = MaxValUint8;
              return 0;              
            }
          }          
        }
        else
        {
          //否则该行单线标志为0.
          SingleBlackFlag[row] = 0;
        }
      }
      //从右线开始的扫描，右线不用检测是否有单线，左线去检测即可。
      else
      {
      }
      
      //没有报错，就会走到这里，黑点查找成功，会有1，2，3个黑点
      //return 1;
    }
    else
    {
      //没通过大跳变检测。
      BlackRightBigChangeCheckRow[row] = 1;
      BlackRightBigChangeCheckAllRow = 1;
      
      //return 1;
      
      //该行定位黑线失败，赋无效值
      BlackRightLoc[row][0] = MaxValUint8;
      return 0;
    }
  }
  //是黑线预处理的调用，不需大跳变的检测，直接返回成功。
  else
  {
    //return 1;
  }

  //如果是黑线预处理的调用，也不用更新这几个值。
  if(preflag == 0)
  {
    //进入这里说明跳变沿的二批条件均满足，进入黑点记录阶段
    //记录3个关键值的实际信息，作为后面的参考。
    if(use == 1)
    {
      BlackRightRealWB[0] = ImgNew[row][col] - ImgNew[row][col+1+BlackRightEdgeNum]; 
      BlackRightRealW[0] = ImgNew[row][col];
      BlackRightRealB[0] = ImgNew[row][col+1+BlackRightEdgeNum];
    }
    else if(use == 2)
    {
      BlackRightRealWB[1] = ImgNew[row][col] - ImgNew[row][col+1+BlackRightEdgeNum]; 
      BlackRightRealW[1] = ImgNew[row][col];
      BlackRightRealB[1] = ImgNew[row][col+1+BlackRightEdgeNum];
    }
    else
    {
    }
  }
  
  return 1;
}


//======================================================================
//函数名：BlackGetRightStep2Win
//功  能：右黑线提取中，第2阶段窗口内跳变检测函数
//参  数：待处理行号row, 窗口大小win, 预测的跳变点的位置predict, 第几次使用use.
//返  回：1成功  0失败
//影  响：BlackRightLoc[][0]
//说  明：1. 满足两批条件，才能检测成功。只有一个点的记录。
//        2. 窗口内没有检测到，则检测失败。
//        3. 窗缩小可以减少一定的时间，但如果遇到斜率过大的情况，就有可能检测不到。     
//======================================================================
uint8 BlackGetRightStep2Win(uint8 row, uint8 win, uint8 predict, uint8 use)
{
  uint8 col, temp;

  //在规划好的窗口内，从左往右查找跳变沿
  //防止溢出
  //对右边界的限制 
  if((predict + win) >= (CameraRealWidth - 5 - BlackRightEdgeNum)) 
  {
    temp = CameraRealWidth - 5 - BlackRightEdgeNum;
  }  
  else
  {
    temp = predict + win;
  }
  //对左边界的限制
  if(predict <= win)
  {
    col = 0;
  }
  else
  {
    col = predict - win;  
  }
  //左线开始扫描且没有出现单线时，右线的扫描左边界必须在该行有效左黑线的右边。
  if((BlackGetPreDir == 0 || BlackGetPreDir == 2)
  &&(SingleBlackAllFlag == 0)
  &&(BlackLeftLoc[row][0] != MaxValUint8)
    )
  {
    col = MaxRe(col, BlackLeftLoc[row][0]);
  }
  else
  {
  }
  
  //窗口预测值记录
  BlackRightPredictL[row] = col;
  BlackRightPredictR[row] = temp;
  
  //左边界在右边界的右边，直接返回失败。
  if(col >= temp) return 0;
   
  //开窗扫描跳变沿
  col = BlackGetRightJustWin(row, col, temp);
  if(col != MaxValUint8)
  {
    //两批条件均满足，直接记录该点为黑线，返回即可
    BlackRightLoc[row][0] = col + 1 + BlackRightEdgeNum;
             
    //最终确认前，需要对大斜率进行检测。
    if(BlackRightBigChangeCheck(row, use) == 0)
    {
      //没通过大跳变检测。
      BlackRightBigChangeCheckRow[row] = 1;
      BlackRightBigChangeCheckAllRow = 1;             
      
      BlackRightLoc[row][0] = MaxValUint8;
      return 0;
    }
    else
    {
    }
    
    
    //记录3个关键值的实际信息，作为后面的参考。
    if(use == 1)
    {
      BlackRightRealWB[0] = ImgNew[row][col] - ImgNew[row][col+1+BlackRightEdgeNum]; 
      BlackRightRealW[0] = ImgNew[row][col];
      BlackRightRealB[0] = ImgNew[row][col+1+BlackRightEdgeNum];
    }
    else if(use == 2)
    {
      BlackRightRealWB[1] = ImgNew[row][col] - ImgNew[row][col+1+BlackRightEdgeNum]; 
      BlackRightRealW[1] = ImgNew[row][col];
      BlackRightRealB[1] = ImgNew[row][col+1+BlackRightEdgeNum];
    }
    else
    {
    }
      
    return 1;   
  }
  else
  {
    //窗口内没有扫描到跳变沿，该行的黑线位置记为无效值，返回0失败.
    BlackRightLoc[row][0] = MaxValUint8;
    return 0;
  }
}



//======================================================================
//函数名：BlackGetRightStep1Scan
//功  能：右黑线提取中，第1阶段的行扫描函数
//参  数：扫描起始行FirstLine, 扫描结束行LastLine
//返  回：0~CameraHight - 1行数 255采集失败
//影  响：BlackRightRealWB[], BlackRightRealW[], BlackRightRealB[], BlackRightLoc[][],
//        LimitRightWB, LimitRightW, LimitRightB
//说  明：1. 可用于正常地第1阶段行扫描，也可用于十字再检测时的行扫描
//        2. 正常第1阶段扫描时，会更改3个关键阈值的值。
//             
//======================================================================
uint8 BlackGetRightStep1Scan(uint8 FirstLine, uint8 LastLine, uint8 CrossFlag)
{
    uint8 row, temp1, temp2;
    uint8 abs1;
    uint8 tempWB, tempW, tempB;
    uint8 flag, i;
    
    flag = 0;
    //最近的BlackStep1Size行用差值法找黑线
    for(row = FirstLine; row > LastLine; row -= 2)
    {
      //判断是否连续两行检测到跳变，可疑黑点有3个点，暂取其中一个
      temp1 = BlackGetRightStep1Edge(row, 1, 0);
      temp2 = BlackGetRightStep1Edge(row-1, 2, 0);
      if(temp1 == 1 && temp2 == 1)//这里可以这么写，不用嵌套if，因为&&的特点
      {
        abs1 = AbsRe(BlackRightLoc[row-1][0], BlackRightLoc[row][0]);
        //进一步判断连续两行的黑点的间隔是否够小，且暂取第1个可疑黑点。
        if(abs1 < BlackRightStep1ScanMinus)
        {
          //检测到了连续的黑线，停止差值法找跳变沿，节省时间，下面进入第2阶段                                     
          //如果不是十字再检测的情况，就更新一下实际的3个关键值，否则不更新。
          if(CrossFlag == 1)
            {           
              //差值的处理，留有裕度，且有上下界。
              tempWB = (BlackRightRealWB[0] + BlackRightRealWB[1]) / 2 - LimitRightWBMargin; 
              if(tempWB < LimitRightWBMin) tempWB = LimitRightWBMin;
              else if(tempWB > LimitRightWBMax) tempWB = LimitRightWBMax;
              else ;
              LimitRightWB = tempWB;
              
              //白点的处理，留有裕度，且有上下界。
              tempW = (BlackRightRealW[0] + BlackRightRealW[1]) / 2 - LimitRightWMargin;
              if(tempW < LimitRightWMin) tempW = LimitRightWMin;
              else if(tempW > LimitRightWMax) tempW = LimitRightWMax;
              else ;
              LimitRightW = tempW;
              
              //黑点的处理，留有裕度，且有上下界。
              tempB = (BlackRightRealB[0] + BlackRightRealB[1]) / 2 + LimitRightBMargin;
              if(tempB < LimitRightBMin) tempB = LimitRightBMin;
              else if(tempB > LimitRightBMax) tempB = LimitRightBMax;
              else ;
              //黑色阈值不能超过白色
              if(tempB >= LimitRightW)
              {
                tempB = LimitRightW - 1;
              }
              else
              {
              }             
              LimitRightB = tempB;           
            }
          break;
        }
        else
        {
          //连续两行相差的列太大，视为错误点。
          BlackRightLoc[row][0] = MaxValUint8;
          BlackRightLoc[row-1][0] = MaxValUint8;
          //大跳变检测数组的复位。
          BlackRightBigChangeNumx[3] = MaxValUint8;
          BlackRightBigChangeNumx[4] = MaxValUint8;
          BlackRightBigChangeNumy[3] = MaxValUint8;
          BlackRightBigChangeNumy[4] = MaxValUint8;
          BlackRightBigChangeJumpFlag = 0;
        }
      }
      else if(temp1 == 0 && temp2 == 1)
      {
        //走到这里，说明双黑线检测成功之前有双线之一采集成功。
        flag = 1;
      }
      else if(temp1 == 1 && temp2 == 0)
      {
        //走到这里，说明双黑线检测成功之前有双线之一采集成功。
        flag = 1;
      }
      else
      {
      }
    }   
    
    //检测了BlackStep1Size行也没有检测到连续的黑线，说明黑线检测失败
    if(row <= LastLine)
    {
      return MaxValUint8;
    }               
    else
    {
        //双黑线检测成功 且 双黑线检测成功之前有双线之一采集成功。
        if(flag == 1)
        {
            //把前面的双线之一都去掉。
            for(i = FirstLine; ; i--)
            {
                if(i == row) break;
                BlackRightLoc[i][0] = MaxValUint8;
                
            }
            
            //第一次scan
            if(CrossFlag == 1)
            {
              //把前3个大斜率检测储存值都去掉，后面2个不用，因为是对的。
              for(i = 0; i <= 2; i++)
              {
                  BlackRightBigChangeNumx[i] = MaxValUint8;
                  BlackRightBigChangeNumy[i] = MaxValUint8;
                  BlackRightBigChangeJumpFlag = 0; //小细节。
              }
            }
            else
            {
            }
            
        }
        else
        {

        }
        
        return row;
    }
  
}


//======================================================================
//函数名：BlackRightCrossStep3
//功  能：右线提取中，十字检测第3阶段，脑补线函数
//参  数：第1段有效黑线的最后一行FirstLine， 第2段有效黑线的第一行LastLine
//返  回：1成功 0失败
//影  响：BlackRightLoc[][0]
//说  明：1. 这里有一些浮点运算，会占时间，在调用本函数时要慎重，尽量不要用。
//        2. 这里的两点间连线的思路很不错，float强转和uint8强转用得很好。
//             
//======================================================================
uint8 BlackRightCrossStep3(uint8 FirstLine, uint8 LastLine)
{
  int32 delta_x, delta_y, delta, index;
  uint8 row, temp1, temp2;
  uint8 i;
  //uint8 temp3, temp4, flag1, flag2;
  
  //超始行比结束行还高，报错！
  if(LastLine >= FirstLine)
  {
    //uart_sendN(UART0, (uint8 *)"\nError In BlackRightCrossStep3!", 31);
    return 0;  
  }
  
  //有可能出现被大斜率检测删掉的情况，这种情况下该行黑线判断为错误，就不进行十字补线了。
  if(BlackRightLoc[LastLine][0] == MaxValUint8)
  {
    //返回失败前扫屁股
    for(i = FirstLine - 1; ; i--)
    {
      BlackRightLoc[i][0] = MaxValUint8;
      if(i == 0) break;
    }
    
    return 0;
  }
  else
  {
  }
  
  temp1 = BlackRightLoc[FirstLine][0];
  temp2 = BlackRightLoc[LastLine][0];
  
  /*
  flag1 = 1;
  flag2 = 1;
  //确认这两个点是直角处的点，也就是往右6个点仍为黑点
  if(temp1 <= (CameraRealWidth - 9))
  {
    temp3 = (
            (ImgNew[FirstLine+1][temp1+6] < LimitRightB)
          &&(ImgNew[FirstLine+1][temp1+7] < LimitRightB)
          &&(ImgNew[FirstLine+1][temp1+8] < LimitRightB)
          );
    if(temp3)
    {
      flag1 = 0;  
    }
  }
  
  if(temp2 <= (CameraRealWidth - 9))
  {
    temp4 = (
            (ImgNew[LastLine-1][temp2+6] < LimitRightB)
          &&(ImgNew[LastLine-1][temp2+7] < LimitRightB)
          &&(ImgNew[LastLine-1][temp2+8] < LimitRightB)
          );
    if(temp4)
    {
      flag2 = 0;  
    }
  }
  
  if((flag1 == 0) || (flag2 == 0))
  {
    return 0;  
  }
  
  */
  
  delta_y = FirstLine - LastLine;
  
  
  //斜率向右
  if(temp1 < temp2)
  {
    //放大BlackRightCrossStep3Multi倍
    delta_x = temp2 - temp1;
    delta = delta_x * BlackRightCrossStep3Multi / delta_y; 
    
    index = 1;
    for(row = FirstLine - 1; row >= LastLine + 1; row-- )
    {
      //每次右移一点点
      BlackRightLoc[row][0] = (uint8)(BlackRightLoc[FirstLine][0] + delta * index / BlackRightCrossStep3Multi);
      index += 1;
    }
  }
  //斜率向左
  else if(temp1 > temp2)
  {
    //放大BlackRightCrossStep3Multi倍
    delta_x = temp1 - temp2;
    delta = delta_x * BlackRightCrossStep3Multi / delta_y;
    
    index = 1;
    for(row = FirstLine - 1; row >= LastLine + 1; row-- )
    {
      //每次右移一点点
      BlackRightLoc[row][0] = (uint8)(BlackRightLoc[FirstLine][0] - delta * index / BlackRightCrossStep3Multi);
      index += 1;
    }
  }
  //斜率直走，这个不太可能出现
  else
  {
    for(row = FirstLine - 1; row >= LastLine + 1; row-- )
    {
      //赋相同的值
      BlackRightLoc[row][0] = temp1;
    }
  }
  
  return 1;
    
}

//======================================================================
//函数名：BlackRightCrossConfirm
//功  能：十字再检测时 ，线头有效性的确认。
//参  数：第1段黑线的真实结束行Row1RealLastLine，第2段黑线的真实起始行Row2RealFirstLine
//返  回：1成功 0失败
//影  响：无
//说  明：1. 为了应对270度弯道十字误检测而诞生，编写过程中没有过多考虑其它情况。
//      
//             
//======================================================================
uint8 BlackRightCrossConfirm(uint8 Row1RealLastLine, uint8 Row2RealFirstLine)
{
  uint8 row, index;
  uint8 x[BlackCrossConfirmNum];
  uint8 y[BlackCrossConfirmNum];
  
  int32 a0, a1, temp;
  
  //1. 先进行第二段黑线起始点方向判定，若不满足右十字该有方向，则判定为错误点。直接返回0.
  if(BlackRightLoc[Row2RealFirstLine][0] < BlackRightLoc[Row2RealFirstLine-1][0]) 
  {
    return 0;
  }
  //2. 再进行第二段黑线起始两点的斜率判定，也就是对两点的列差作限定，太大则有可能是大弯回弯。针对回弯十字误补。
  else if((BlackRightLoc[Row2RealFirstLine][0] > BlackRightLoc[Row2RealFirstLine-1][0])
   &&(BlackRightLoc[Row2RealFirstLine][0] - BlackRightLoc[Row2RealFirstLine-1][0] >= 3)
    )
  {
    return 0;
  }
  //3. 再对上一幅图的左线斜率和左线头进行检查。这里的左线斜率与左线头因为还没更新，所以是上一幅图的。针对回弯十字误补。
  else if((SlopeL + BlackCrossConfirmSlopeLimit < 0) //上一幅图的左线斜率很大。
   &&(BlackLeftHeadLine < BlackCrossConfirmHeadLineLimit) //上一幅图的左线线头比较高。（防止因为线特别短而造成的斜率不准确）
     )
  {
    return 0;
  }
  else
  {
  }
  
  //第一段黑线太短，无法检测正确性时，检查要补的行是不是够长，如果够长，就视为正确。
  //if(Row1RealLastLine >= CameraHight - 5 && Row2RealFirstLine > Row1RealLastLine + BlackRightCrossConfirmGapLine) return 1;
  //if(Row1RealLastLine >= CameraHight - 5) return 0;
  
  //以左上角为原点，向下为x轴正方向（0~49），向右为y轴正方向(0~249)
  index = 0;
  for(row = Row1RealLastLine; ; row++)
  {
    //有效行的记录
    if(BlackRightLoc[row][0] != MaxValUint8)
    {
      y[index] = BlackRightLoc[row][0];
      x[index] = row;
      index++;
    }
    //点数够多，则跳出。
    if(index == BlackCrossConfirmNum) break;
    //直到最低行仍没有凑够足够的点数，说明第一段有效黑线太短.
    if(row == CameraHight - 1) 
    {
      if(index == 0) return 0;
      //判断第二段黑点起点是不是在第一段黑点末尾的左边一定范围内
      if(BlackRightLoc[Row2RealFirstLine][0] < y[0] && y[0] - BlackRightLoc[Row2RealFirstLine][0] < 50)
      {
        return 1;
      }
      else
      {
        return 0;
      }      
    }
    else
    {
    }
  }
  
  //最小二乘法
  //y = a0 + a1 * x
  a1 = LeastSquarea1(x, y, BlackCrossConfirmNum);
  
  //判断斜率是不是从左下到右上，如果是，则返回失败。
  if(a1 + BlackRightCrossConfirmSlopeLimit < 0) return 0;
  
  a0 = LeastSquarea0(x, y, a1, BlackCrossConfirmNum);
  
  //temp为第2段黑线起始行本应该在的位置。
  temp = (a0 + a1 * Row2RealFirstLine) / LeastSquareMulti;

  //把这个点限定在窗内
  if(
    (BlackRightLoc[Row2RealFirstLine][0] > temp - BlackCrossConfirmWin)  //向左的值太大的话，会在“下双上单”的时候出现十字误补线。
  &&(BlackRightLoc[Row2RealFirstLine][0] < temp + BlackCrossConfirmWin)  
    )
  {
    return 1;
  }
  else
  {
    return 0;
  }
    
  
}



//======================================================================
//函数名：BlackGetRightCrossSpeWin
//功  能：右黑线十字再检测时，窗口的特殊扫描
//参  数：待处理行号row, 窗口左边界colleft, 窗口右边界colright, 第几次使用use.
//返  回：1成功  0失败
//影  响：BlackRightLoc[][0]
//说  明：1. 满足两批条件，才能检测成功。只有一个点的记录。
//        2. 窗口内没有检测到，则检测失败。
//        3. 跟普通窗口扫描不一样，前者输入变量是预测值和窗口大小，后者是左右边界。  
//======================================================================
uint8 BlackGetRightCrossSpeWin(uint8 row, uint8 colleft, uint8 colright, uint8 use)
{
  uint8 col;

  //在规划好的窗口内，从左往右查找跳变沿
  //防止溢出
  //对右边界的限制 
  if(colright >= (CameraRealWidth - 5 - BlackRightEdgeNum)) 
  {
    colright = CameraRealWidth - 5 - BlackRightEdgeNum;
  }  
  else
  {
  }
  
  //左线开始扫描时，若该幅图像没有单线，则右线扫描的左边界必须在该行有效左黑线的右边。
  if((BlackGetPreDir == 0 || BlackGetPreDir == 2)
   &&(SingleBlackAllFlag == 0)
   &&(BlackLeftLoc[row][0] != MaxValUint8)
     )
  {
    colleft = MaxRe(colleft, BlackLeftLoc[row][0]);
  }
  else
  {
  }
  
  //窗口预测值记录
  BlackRightPredictL[row] = colleft;
  BlackRightPredictR[row] = colright;
  
  //若左边界在右边界右边，直接返回失败。
  if(colleft >= colright) return 0;
  
  //开窗扫描跳变沿
  col = BlackGetRightJustWin(row, colleft, colright);
  if(col != MaxValUint8)
  {
    //两批条件均满足，直接记录该点为黑线，返回即可
    BlackRightLoc[row][0] = col + 1 + BlackRightEdgeNum;
    
    //最终确认前，需要对大斜率进行检测。
    if(BlackRightBigChangeCheck(row, use) == 0)
    {
      //没通过大跳变检测。
      BlackRightBigChangeCheckRow[row] = 1;
      BlackRightBigChangeCheckAllRow = 1;             
      
      BlackRightLoc[row][0] = MaxValUint8;
      return 0;
    }
    else
    {
    }
    
    //记录3个关键值的实际信息，作为后面的参考。
    if(use == 1)
    {
      BlackRightRealWB[0] = ImgNew[row][col] - ImgNew[row][col+1+BlackRightEdgeNum]; 
      BlackRightRealW[0] = ImgNew[row][col];
      BlackRightRealB[0] = ImgNew[row][col+1+BlackRightEdgeNum];
    }
    else if(use == 2)
    {
      BlackRightRealWB[1] = ImgNew[row][col] - ImgNew[row][col+1+BlackRightEdgeNum]; 
      BlackRightRealW[1] = ImgNew[row][col];
      BlackRightRealB[1] = ImgNew[row][col+1+BlackRightEdgeNum];
    }
    else
    {
    }
      
    return 1;
  }
  else
  {
    //窗口内没有检测到跳变沿，该行黑线置无效值，返回0失败。
    BlackRightLoc[row][0] = MaxValUint8;
    return 0;   
  }

}



//======================================================================
//函数名：BlackGetRightCrossSpeScan
//功  能：十字再检测时，右线头的特殊扫描
//参  数：第2段黑线的预测起始行Row2FirstLine， 第2段黑线的预测结束行Row2LastLine，
//        最近一次的窗口预测值LastWinPredict
//返  回：MaxValUint8失败，否则成功。
//影  响：
//说  明：
//      
//             
//======================================================================
uint8 BlackGetRightCrossSpeScan(uint8 Row2FirstLine, uint8 Row2LastLine, uint8 LastWinPredict)
{
  uint8 row;
  uint8 temp1, temp2, temppredict, tempminus;
  uint8 colleft, colright;
  uint8 i;
  
  //最近一次的窗口预测值。
  temppredict = LastWinPredict;
  
  //colright的确定
  //对于右黑线扫描，colright表示之前所有有效右黑线的最右点。
  //找第1个有效右黑线。
  for(i = Row2FirstLine+1; ; i++)
  {
    if(BlackRightLoc[i][0] != MaxValUint8)
    {
      colright = BlackRightLoc[i][0];
      break;
    }
    else
    {
    }
    //若一直到最近一行仍没有有效左黑线，则返回失败。理论上不可能走到这里。
    if(i == CameraHight - 1) return MaxValUint8;
  }
  //暂时不用最右边的左黑线，也就相当于用最左边的左黑线。
//  //找最右的有效右黑线。
//  for( ; ; i++)
//  {
//    if(BlackRightLoc[i][0] != MaxValUint8)
//    {
//      if(BlackRightLoc[i][0] > colright)
//      {
//        colright = BlackRightLoc[i][0];
//      }
//      else
//      {
//      }
//    }
//    else
//    {
//    }
//    
//    if(i == CameraHight - 1) break;
//  }
  
  for(row = Row2FirstLine; ; row -= 2)
  {
    //colleft的确定
    colleft = BoundaryLimitRe(temppredict - BlackRightWinVal * 2,
                              0,
                              CameraRealWidth - 1); 
    //为了防止窗口有错，所以最右边不得大于BlackRightEdgeStartColOffset。
    colleft = MinRe(colleft, CameraRealWidth / 2 - 1 - BlackRightEdgeStartColOffset[row]);
    
    //连续2行检测黑点。
    temp1 = BlackGetRightCrossSpeWin(row, colleft, colright, 1);
    temp2 = BlackGetRightCrossSpeWin(row-1, colleft, colright, 2);
    //连续2行开窗检测黑点成功，则线头检测成功。跳出。
    if(temp1 == 1 && temp2 == 1)
    {
      return row;
    }
    //连续2行开窗检测黑线失败，则窗口预测点继续变化，继续开窗检测。
    else if(temp1 == 0 && temp2 == 0)
    {
      //预测点按趋势走也要限幅。
      tempminus = BoundaryLimitRe(BlackRightWinPredictStore2 + BlackRightWinPredictStore2 - BlackRightWinPredictStore1,
                                  0,
                                  CameraRealWidth - 1);
      //temp在这里表示窗口预测点位置。
      temppredict = tempminus;
      BlackRightWinPredictStore0 = BlackRightWinPredictStore1;
      BlackRightWinPredictStore1 = BlackRightWinPredictStore2;
      BlackRightWinPredictStore2 = temppredict;      
    }
    //其中一行开窗检测成功，则当作干扰点，窗口预测点继续变化，继续开窗检测。
    else
    {
      //当作干扰点滤掉。
      BlackRightLoc[row][0] = MaxValUint8;
      BlackRightLoc[row-1][0] = MaxValUint8;
      
      //预测点按趋势走也要限幅。
      tempminus = BoundaryLimitRe(BlackRightWinPredictStore2 + BlackRightWinPredictStore2 - BlackRightWinPredictStore1,
                                  0,
                                  CameraRealWidth - 1);
      //temp在这里表示窗口预测点位置。
      temppredict = tempminus;
      BlackRightWinPredictStore0 = BlackRightWinPredictStore1;
      BlackRightWinPredictStore1 = BlackRightWinPredictStore2;
      BlackRightWinPredictStore2 = temppredict;         
    }
      
      
      
    //一直到第Row2LastLine行也没有连续2行检测成功，则返回失败。
    if(row == Row2LastLine) 
    {
      return MaxValUint8;
    }
    else
    {
    }
  }
 

}




//======================================================================
//函数名：BlackRightAgainForCross
//功  能：右黑线提取中，针对十字的再检测程序
//参  数：第2段黑线的预测起始行Row2FirstLine， 第2段黑线的预测结束行Row2LastLine，
//        第1段黑线的实际结束行Row1RealLastLine, 最近一次的窗口预测值LastWinPredict
//返  回：1成功 0失败
//影  响：BlackRightLoc[][]。 （不会影响那几个阈值。）
//说  明：1. 本函数会调用浮点运算的函数，预计花费较多时间，调用入口需谨慎。
//        2. 本函数仅影响了BlackRightLoc[][]，以后也应该保持。
//             
//======================================================================
uint8 BlackRightAgainForCross(uint8 Row2FirstLine, uint8 Row2LastLine, uint8 Row1RealLastLine, uint8 LastWinPredict)
{
    uint8 row, temp, temp1, temp2;
    uint8 Row2RealFirstLine; //第2段黑线的真实起始行
    uint8 i;
    
    uint8 tempWB, tempW, tempB;
    uint8 tempminus;
    
    //第1段黑线的真实结束行有可能因为大跳变被删掉，所以需要重新查找并更新。
    for(i = Row1RealLastLine; i <= CameraHight - 1; i++)
    {
      if(BlackRightLoc[i][0] != MaxValUint8)
      {
        Row1RealLastLine = i;
        break;
      }
      else
      {
      }
    }
    //由于直角黑块不允许检测十字再补线，所以当第1段黑线真实结束行的斜上方有黑块时，不允许补线。
    //取5列。
    for(i = 0; i <= 5; i++)
    {
      if(ImgNew[Row1RealLastLine - 4][BlackRightLoc[Row1RealLastLine][0] - 5 - i] < LimitRightB) //往远5行。斜左上方。
      {
        return 0;
      }
      else if(ImgNew[Row1RealLastLine - 5][BlackRightLoc[Row1RealLastLine][0] - 5 - i] < LimitRightB) //往远5行。斜左上方。
      {
        return 0;
      }
      else
      {
      }      
    }
    
    //十字再检测第1阶段，找线头。
    //temp = BlackGetRightStep1Scan(Row2FirstLine, Row2LastLine, 0); //第3个参数0表示是十字再检测，不用更新3个关键值
    
    //十字再检测第1阶段，找线头，不采用Scan的方式，采用Win的方式。
    temp = BlackGetRightCrossSpeScan(Row2FirstLine, Row2LastLine, LastWinPredict);
    
    //线头查找失败，或找到最远两行（继续执行后面会数组超出界限），就直接判定为失败。
    if(
      (temp == MaxValUint8)
    ||(temp <= 1)
    ||(BlackAgainForCrossFar10CutSwitch == 1 && temp <= 9)  //当远10行删除开关开启时，且十字再检测时的第2段黑线线头在最远的10行，则十字再检测失败。
      )
    {
      //扫屁股工作。        //i >= Row2LastLine
      for(i = Row2FirstLine; ; i--)
      {
        BlackRightLoc[i][0] = MaxValUint8;
        if(i == 0) break;
      }
      return 0;  
    }
    else 
    {
      ////////////////如果要注释，就从这里开始注释
      
      //十字再检测的线头不在限定的范围内，认为是干扰点，不用顺着线头找线了，直接退出。
      if(BlackRightCrossConfirm(Row1RealLastLine+1, temp) == 0)
      { //扫屁股工作。         //i >= (temp-1)
        BlackRightCrossConfirmFlag = 1;  //右十字线头标志确认失败，超出预想范围。
        for(i = Row2FirstLine; ; i--)
        {
          BlackRightLoc[i][0] = MaxValUint8;
          if(i == 0) break;
        }
        return 0;
      }
      //十字再检测的线头的确在限定的范围内，可以继续。
      else
      {
        BlackRightCrossConfirmFlag = 2;  //右十字线头标志确认成功。
        row = temp;
      }
      
      ////////////////如果要注释，就从这里结束注释
      
      //暂时不用confirm函数，如果要用的话 ，就把后面这句话去掉，前面注释去掉。
      //row = temp;
    }
    
    Row2RealFirstLine = row; //记录第2段黑线的真实起始行
    
    //十字再检测第2阶段，顺着线头找线。
    //temp为预测的黑点位置
    temp = BoundaryLimitRe(BlackRightLoc[row-1][0] + BlackRightLoc[row-1][0] - BlackRightLoc[row][0],
                           0,
                           CameraRealWidth - 1);
    //预测值更新后就刷新一次堆栈，堆栈顶部丢失。
    BlackRightWinPredictStore0 = BlackRightWinPredictStore1;
    BlackRightWinPredictStore1 = BlackRightWinPredictStore2;
    BlackRightWinPredictStore2 = temp;
    
    //从最近的认证行到取最远的行
    for(row -= 2; ; row -= 2)
    {
      //连续2行检测黑点
      temp1 = BlackGetRightStep2Win(row, BlackRightWinVal, temp, 1);
      temp2 = BlackGetRightStep2Win(row-1, BlackRightWinVal, temp, 2);
      
      //若连续2行的黑点检测成功，则更新3个阈值，更新预测点的位置
      if(temp1 && temp2) 
      {
        //差值的处理，留有裕度，且有上下界。
        tempWB = (BlackRightRealWB[0] + BlackRightRealWB[1]) / 2 - LimitRightWBMargin; 
        if(tempWB < LimitRightWBMin) tempWB = LimitRightWBMin;
        else if(tempWB > LimitRightWBMax) tempWB = LimitRightWBMax;
        else ;
        LimitRightWB = tempWB;
        
        //白点的处理，留有裕度，且有上下界。
        tempW = (BlackRightRealW[0] + BlackRightRealW[1]) / 2 - LimitRightWMargin;
        if(tempW < LimitRightWMin) tempW = LimitRightWMin;
        else if(tempW > LimitRightWMax) tempW = LimitRightWMax;
        else ;
        LimitRightW = tempW;
        
        //黑点的处理，留有裕度，且有上下界。
        tempB = (BlackRightRealB[0] + BlackRightRealB[1]) / 2 + LimitRightBMargin;
        if(tempB < LimitRightBMin) tempB = LimitRightBMin;
        else if(tempB > LimitRightBMax) tempB = LimitRightBMax;
        else ;
        //黑色阈值不能超过白色
        if(tempB >= LimitRightW)
        {
          tempB = LimitRightW - 1;
        }
        else
        {
        }              
        LimitRightB = tempB;     
        
        //对预测值的更新。
        temp = BoundaryLimitRe(BlackRightLoc[row - 1][0] + BlackRightLoc[row - 1][0] - BlackRightLoc[row][0],
                               0,
                               CameraRealWidth - 1);
        //预测值更新后就刷新一次堆栈，堆栈顶部丢失。
        BlackRightWinPredictStore0 = BlackRightWinPredictStore1;
        BlackRightWinPredictStore1 = BlackRightWinPredictStore2;
        BlackRightWinPredictStore2 = temp;
      }
      //如果没能连续两行检测黑点成功，原本打算用原位置的预测点，后来发现效果不好，所以决定按预测点的趋势走。
      else
      {        
        //预测点按趋势走也要限幅。
        tempminus = BoundaryLimitRe(BlackRightWinPredictStore2 + BlackRightWinPredictStore2 - BlackRightWinPredictStore1,
                                    0,
                                    CameraRealWidth - 1);
        //temp在这里表示窗口预测点位置。
        temp = tempminus;
        BlackRightWinPredictStore0 = BlackRightWinPredictStore1;
        BlackRightWinPredictStore1 = BlackRightWinPredictStore2;
        BlackRightWinPredictStore2 = temp;
      }
      
      if((row == Row2LastLine) || (row == Row2LastLine+1)) break;      
    }
    
    //检查是否因大斜率而被削头。如果有，则更新头的位置，如果没有，则正常补线。
    if (BlackRightLoc[Row1RealLastLine][0] == MaxValUint8)
    {

        //头足够高，找回头。
        for (i = Row1RealLastLine; ; i++)
        {
            if (BlackRightLoc[i][0] != MaxValUint8)
            {
                Row1RealLastLine = i;
                break;
            }
            else
            {
            }
            //直到最后一行也没有找到有效行，不用补线，直接返回1.
            if (i == CameraHight - 1) return 1;
        }

        //十字再检测第3阶段，脑补两段线的接线。
        if (BlackRightCrossStep3(Row1RealLastLine, Row2RealFirstLine) == 0)
        {
            return 0;
        }

    }
    else
    {
        //没有被大斜率削头。
        //十字再检测第3阶段，脑补两段线的接线。
        if (BlackRightCrossStep3(Row1RealLastLine, Row2RealFirstLine) == 0)
        {
            return 0;
        }
    }
    
    return 1;
        
}


//======================================================================
//函数名：BlackRightStep4BackLine
//功  能：右黑线提取中，第4阶段十字向后补线函数。
//参  数：Step4StartLine开始行,flag标志0表示正常调用，1表示单线错误的调用
//返  回：1成功 0失败
//影  响：BlackRightLoc[][0]
//说  明：1. 这里引用了float型的函数，有可能占用较长的时间。
//        2. 想要减少时间，可以把宏定义BlackBackLineNum减至3或者2.
//        3. 单线错误的调用时，不用检测十字外尾
//======================================================================
uint8 BlackRightStep4BackLine(uint8 Step4StartLine, uint8 flag)
{
  uint8 row;
  uint8 i, index;
  uint8 x[BlackBackLineNum];   //取多少个点在一开始有定义。
  uint8 y[BlackBackLineNum];
  uint8 j;
  uint8 temp1;
  uint8 tempend;
  
  int32 temp;
  int32 a0;
  int32 a1;
  
  //正常调用时，需要检测十字外尾
  if(flag == 0)
  {
    //十字外尾处理
    //第1阶段的线头比较高的话，就开始十字外尾检测。否则不用管。
    if(BlackRightStep1ScanRow < BlackRightCrossTailRowLimit)
    {
      //检查线头处的斜率是不是很大，若很大，则怀疑有十字外尾。否则不用管。
      //这里用的斜率是十字外尾专用斜率。
      if(BlackRight5Slope[0] > BlackRightCrossTailSlopeLimit)
      {
        //十字外尾标志置位。
        BlackRightCrossTailFlag = 1;
        BlackRightHeadEndSearch();
        if (BlackRightEndLine - BlackRightHeadLine <= 5 && BlackLeftHeadLine != MaxValUint8 && BlackLeftEndLine != MaxValUint8)
        {
            //这种情况有可能是回弯导致的，过滤掉
            for (i = BlackRightHeadLine; i <= BlackRightEndLine; i++)
            {
                BlackRightLoc[i][0] = MaxValUint8;
            }
            return 0;
        }
        else if (BlackRight5Slope[0] == MaxValInt16)
        {
            //这种情况有可能是回弯导致的，过滤掉
            for (i = 0; i <= 59; i++)
            {
                BlackRightLoc[i][0] = MaxValUint8;
            }
            return 0;
        }
        //5Slope数组有可能已经删掉了一些黑点，所以也要参考实际的有效黑点数。
        if(BlackRightEndLine - BlackRightHeadLine - 4 < 0 || BlackRight5SlopeIndex - 1 < 0)
        {
          return 0;
        }
        else
        {
        }        
        tempend = MinRe(BlackRightEndLine - BlackRightHeadLine - 4, BlackRight5SlopeIndex - 1);
        //开始消除十字外尾，找十字外尾的处理最远行。
        for(i = 0; ; i++)
        {
          //找斜率在直道范围内的。这里用的斜率是判断为直道的斜率。
          if(BlackRight5Slope[i] < PathJudgeStraightSlopeLimit)
          {
            BlackRightCrossTailEndLine = BlackRight5SlopeRow[i];
            break;
          }
          
          if(i == tempend) return 0;  //一直都很斜，则有可能是回弯，直接返回失败。
          //按理说不会走到这里，因为回弯会导致向后补线出错，向后补线出错后进来本函数时会先检查出错的原因BlackRightStep4ErrorCheck()，会检测斜率是不是都是直线，若不是则不会进来。
        }
        //向近处4个点，也就是“5”点斜率的计算末尾。
        BlackRightCrossTailEndLine += 4;
        //取最开始采到的行
        i = MinRe(BlackRight5SlopeRow[0] + 4, CameraHight - 1);
        i = MaxRe(BlackRightStep1ScanRow, i);
        if(i >= BlackRightCrossTailEndLine)
        {
          //开始删除十字外尾。
          for(; ; i--)
          {
            BlackRightLoc[i][0] = MaxValUint8;
            if(i == BlackRightCrossTailEndLine) break;
          }
          //修改向后补线的起点。
          Step4StartLine = BlackRightCrossTailEndLine - 1;
        }
        else
        {
        }
      }
      else
      {
        //没有十字外尾。
        BlackRightCrossTailFlag = 0;      
      }
    }
    else
    {    
      //没有十字外尾。
      BlackRightCrossTailFlag = 0;
    }
  }
  //单线错误调用时，就不用检测十字外尾了。
  else
  {
  }  
  

  //正常的十字向后补线
  if(Step4StartLine - BlackBackLineNum - BlackBackLineNum < 0)
  {
    temp1 = 0;  
  }
  else
  {
    temp1 = Step4StartLine - BlackBackLineNum - BlackBackLineNum;
  }
  
  //取BlackBackLineNum个点作为最小二乘法的基准点。
  //以左上角为原点，向下为x轴正方向，向右为y轴正方向
  index = 0;
  for(i = Step4StartLine; ; i--)
  {
    if(BlackRightLoc[i][0] != MaxValUint8)
    {
      y[index] = BlackRightLoc[i][0];
      x[index] = i;
      index++;
    }
    if(index == BlackBackLineNum)
    {
      break;  
    }
    if(i == temp1)
    {
      return 0;
    }
  }
  
  //最小二乘法
  //y = a0 + a1 * x
  a1 = LeastSquarea1(x, y, BlackBackLineNum);
  /*
   //如果直线是从左上到右下，则说明是取到了错误的线，不必再补线，全数组清零。
  if(a1 + BlackRightBackLineSlope < 0)
  {
    for(row = Step4StartLine; ; row--)
    {
      BlackRightLoc[row][0] = MaxValUint8;
      if(row == 0) break;
    }
    return 0;
  }
  //否则继续。
  else
  {
  }
  */
  
  //直着补，不是顺着补。
  /*
  for (row = Step4StartLine + 1; row <= CameraHight - 1; row++)
  {
    BlackRightLoc[row][0] = BlackRightLoc[Step4StartLine][0];
  }
  */
  
  //顺着补，不是直着补。
  a0 = LeastSquarea0(x, y, a1, BlackBackLineNum);
  
  for(row = Step4StartLine + 1; row <= CameraHight - 1; row++)
  {
    //补线也有个度，向右和向左都不能补出范围。
    temp = (a0 + a1 * row) / LeastSquareMulti;
    if(temp >= CameraRealWidth-1)
    {
      //BlackRightLoc[row][0] = CameraRealWidth - 1;
      //超过左右界限时，原策略为置边界值，现在的策略是从该行起置无效值。
      for(j = row; ; j++)
      {
        BlackRightLoc[j][0] = CameraRealWidth-1;
        if(j == CameraHight - 1) break;
      }
      break;
    }
    else if(temp <= 0)
    {
      //BlackRightLoc[row][0] = 0;
      //超过左右界限时，原策略为置边界值，现在的策略是从该行起置无效值。我
      for(j = row; ; j++)
      {
        BlackRightLoc[j][0] = 0;
        if(j == CameraHight - 1) break;
      }
      break;
    }
    else
    {
      BlackRightLoc[row][0] = (uint8)(temp); 
    }
  }
  
  
  
  return 1;
  
}




//右黑线连续性补线
uint8 BlackRightContinueAdd(void)
{
    uint8 row, i, j;
    uint8 tempfirst, templast;
    uint8 fangHuiWanFlag = 0;
    uint8 fangDuanLieFlag = 0;
    
    //右黑线连续性补线
    //只有右黑线采集成功才补线
    if(BlackRightDone == 1)
    {        
        //找线尾
        for(row = CameraHight - 1; ; row--)
        {
          if(BlackRightLoc[row][0] != MaxValUint8)
          {
            break;
          }
          else
          {
          }
          
          if(row == 0)
          {
            BlackRightDone = 0;
            return 0;
          }
        }
        
        //给线头线尾初始化为无效值。
        tempfirst = MaxValUint8;
        templast = MaxValUint8;
                
        //开始补线
        for (; ; row--)
        {
            if (BlackRightLoc[row][0] == MaxValUint8)
            {
                if (tempfirst == MaxValUint8)
                {
                    //若线头为无效值，就记录为线头。
                    tempfirst = row + 1;
                    break;
                }
                else
                {
                }
            }
            else
            {
                if (tempfirst != MaxValUint8)
                {
                    //若线头已经有赋值，则开始记录线尾。
                    templast = row;
                    
                    if (templast > 5)
                    {
                        if (BlackRightLoc[templast - 1][0] != MaxValUint8 && BlackRightLoc[templast - 2][0] != MaxValUint8 && BlackRightLoc[templast - 3][0] != MaxValUint8
                            && BlackRightLoc[templast - 4][0] != MaxValUint8 )
                        {
                            fangDuanLieFlag = 0;
                        }
                        else
                        {
                            fangDuanLieFlag = 1;
                        }
                    }
                    else
                    {
                        fangDuanLieFlag = 1;
                    }
                    if (templast >= 8)
                    {
                        if (BlackRightLoc[templast - 1][0] != MaxValUint8 && BlackRightLoc[templast - 2][0] != MaxValUint8 && BlackRightLoc[templast - 3][0] != MaxValUint8
                            && BlackRightLoc[templast - 4][0] != MaxValUint8 && BlackRightLoc[templast - 5][0] != MaxValUint8
                                && BlackRightLoc[templast - 6][0] != MaxValUint8 && BlackRightLoc[templast - 7][0] != MaxValUint8 && BlackRightLoc[templast - 8][0] != MaxValUint8)
                        {
                            fangHuiWanFlag = 0;
                        }
                        else
                        {
                            fangHuiWanFlag = 1;
                        }
                    }
                    else
                    {
                        fangHuiWanFlag = 1;
                    }
                    
                    if((BlackRightLoc[tempfirst][0]>=235&&BlackRightLoc[templast][0]>=235) && (tempfirst - templast) <= 12)
                    {
                        //这种情况是防止黑线太靠镜头边界导致黑线不成功的
                        for (j = tempfirst - 1; ; j--)
                        {
                            BlackRightLoc[j][0] = (BlackRightLoc[tempfirst][0] + BlackRightLoc[templast][0]) / 2;
                            if (j == templast + 1) break;
                        }
                    }
                    else if ((BlackRightLoc[tempfirst][0] >= 235 && BlackRightLoc[templast][0] >= 235) && (tempfirst - templast) <20
                             && (BlackRightLoc[10][0] != MaxValUint8 || BlackRightLoc[9][0] != MaxValUint8 || BlackRightLoc[8][0] != MaxValUint8 || BlackRightLoc[7][0] != MaxValUint8 || BlackRightLoc[6][0] != MaxValUint8)
                             && fangHuiWanFlag == 0)
                    {
                        //这种情况是防止黑线太靠镜头边界导致黑线不成功的
                        for (j = tempfirst - 1; ; j--)
                        {
                            BlackRightLoc[j][0] = (BlackRightLoc[tempfirst][0] + BlackRightLoc[templast][0]) / 2;
                            if (j == templast + 1) break;
                        }
                    }
                    else if ((BlackRightLoc[tempfirst][0] >= 235 && BlackRightLoc[templast][0] >= 235) && (tempfirst - templast) <= 35
                             && (BlackRightLoc[6][0] != MaxValUint8 || BlackRightLoc[7][0] != MaxValUint8 || BlackRightLoc[8][0] != MaxValUint8 || BlackRightLoc[10][0] != MaxValUint8 || BlackRightLoc[9][0] != MaxValUint8)
                             && fangHuiWanFlag == 0)
                    {
                        //这种情况是防止黑线太靠镜头边界导致黑线不成功的
                        for (j = tempfirst - 1; ; j--)
                        {
                            BlackRightLoc[j][0] = (BlackRightLoc[tempfirst][0] + BlackRightLoc[templast][0]) / 2;
                            if (j == templast + 1) break;
                        }
                    }
                    //防边线断裂的
                    else if (tempfirst - templast <= 5 && fangDuanLieFlag == 0)
                    {
                        for (j = tempfirst - 1; ; j--)
                        {
                            BlackRightLoc[j][0] = (BlackRightLoc[tempfirst][0] + BlackRightLoc[templast][0]) / 2;
                            if (j == templast + 1) break;
                        }
                    }
                    else if(tempfirst - templast >= 3) 
                    {
                      //两者相差太多，则后面的行全部视为错误。
                      for(i = tempfirst - 1; ; i--)
                      {
                        BlackRightLoc[i][0] = MaxValUint8;
                        if(i == 0) break;
                      }
                      //右黑线连续性标志出错。
                      BlackRightContinueFlag = 2;
                      return 0;
                    }
                    else
                    {
                      //一共才1或2个点，直接插值就行。
                      for(j = tempfirst - 1; ; j--)
                      {
                        BlackRightLoc[j][0] = (BlackRightLoc[tempfirst][0] + BlackRightLoc[templast][0]) / 2;
                        if(j == templast + 1) break;
                      }
                    }
                    tempfirst = MaxValUint8;
                    templast = MaxValUint8;
                    //右黑线连续性标志置0，不连续。
                    BlackRightContinueFlag = 0;
                }
                else
                {
                }
            }

            if (row == 0) break;
        }
    }
    else
    {
    }    
    
    return 1;
}



//右黑线的回弯特殊检测。进来前BlackRightDone已经置为1了。
//进来前已经进行左、右黑线的斜曲率检测。
uint8 BlackRightRollCheck(void)
{
  uint8 temp1, temp2, temp3;
  
  //当前为单线，不进行回弯特殊检测。
  if(SingleBlackAllFlag == 1) return 1;
  
  //斜率确认。
  //若无效，直接确认。
  if(SlopeR == MaxValInt16)
  {
    temp1 = 1;
  }
  else
  {
    //斜率的确太大。
    if(SlopeR > BlackRightRollSlopeLimit)
    {
      temp1 = 1;
    }
    //斜率不大。
    else
    {
      temp1 = 0;
    }
  }
  
  //线长度确认。
  //若无效，直接确认。
  if(BlackRightHeadLine == MaxValUint8 || BlackRightEndLine == MaxValUint8)
  {
    temp2 = 1;
  }
  else
  {
    //线确实很短。
    if(BlackRightEndLine - BlackRightHeadLine < BlackRightRollLengthLimit)
    {
      temp2 = 1;
    }
    //线很长。
    else
    {
      temp2 = 0;
    }
  }
  
  /*
  //左线斜率确认。
  //左线必须是右上到左下的，也就是负的斜率。而且斜率要求绝对值比较大。才能说明是270等大弯。
  if(SlopeL == MaxValInt16)
  {
    temp3 = 1;    
  }
  else
  {
    if(SlopeL + BlackRightRollSlopeLLimit < 0)
    {
      temp3 = 1;
    }
    else
    {
      temp3 = 0;
    }
  }  
  */
  temp3 = 1;
  
  if(
    (BlackRightStep4Flag == 3 || SingleBlackLastAllFlag == 1)   //右向后补线标志，出错。或者因为上幅图像是单线，无法开启十字，无法置向后补线出错。
  &&(BlackRightCrossDone == 1 || BlackRightCrossDone == 0)   //右十字标志，失败。或者未进入（线尾过高不会进行十字再检测）。
  &&(temp1 == 1)                //斜率很大。
  &&(temp2 == 1)                //线很短。
  &&(temp3 == 1)                //右线
    )
  {
    //检测到回弯，返回0.
    return 0;   
  }
  else
  {
    //没检测到回弯，返回1.
    return 1;
  }

}

//向后补线标志出错时的检查，这个函数是防止回弯的很重要的函数。
uint8 BlackRightStep4ErrorCheck(void)
{
  uint8 i;
  
  //检查斜率是不是都满足直线
  for(i = 0; ; i++)
  {
    if(BlackRight5Slope[i] == MaxValInt16) return 0;
    
    if(AbsInt(BlackRight5Slope[i]) > PathJudgeStraightSlopeLimit)
    {
      return 0;
    }
    else
    {      
    }
    
    if(i == 5) break;
  }
  
  return 1;
  
}



//右黑线提取前的参数初始化赋值
uint8 BlackGetRightParaInit(void)
{
  uint8 i;  
  
  //右线连续性标志。默认为1连续。
  BlackRightContinueFlag = 1;  
  
  //采集成功标志位
  BlackRightDone = 1;  
 
  //右十字线头标志。0未进入，1超范围，2成功。
  BlackRightCrossConfirmFlag = 0;  //默认为未进入。  
  
  //小5点斜率储存数组置初始值。
  BlackRight5SlopeIndex = 0;  
  for(i = 0; ; i++)
  {
      BlackRight5Slope[i] = MaxValInt16;
      BlackRight5SlopeRow[i] = MaxValUint8;
      if(i == CameraHight - 1) break;
  }  
  
  //回弯检测
  BlackRightRollCheckFlag = 0;        //检测到回弯。
  BlackRightBigChangeJumpFlag = 0;    //回弯跳过标志。

  //大斜率检测时，每幅图像的初始值初始化为无效值。
  for(i = 0; i <= 4; i++)
  {
    BlackRightBigChangeNumx[i] = MaxValUint8;
    BlackRightBigChangeNumy[i] = MaxValUint8; 
  }
  BlackRightBigChangeFlag = 0;
  BlackRightBigChangeSlopeStore = MaxValInt16;

  //大斜率检测时，每行的大跳变标志置0.
  for(i = 0; ; i++)
  {
    BlackRightBigChangeCheckRow[i] = 0;
    if(i == CameraHight - 1) break;
  }
  BlackRightBigChangeCheckAllRow = 0;

  //右黑线储存数组初始化为无效值。
  for(i = 0; ; i++)
  {
    BlackRightLoc[i][0] = MaxValUint8;
    if(i == CameraHight - 1) break;
  }

  //十字外尾
  BlackRightCrossTailFlag = 2;       //十字外尾标志，0失败，1确认，2未进入。

  //窗口预测值管理
  for(i = 0; ; i++)
  {
    BlackRightPredictL[i] = MaxValUint8;
    BlackRightPredictR[i] = MaxValUint8; 
    if(i == CameraHight - 1) break;
  }

  //窗口预测储存值
  BlackRightWinPredictStore0 = MaxValUint8;
  BlackRightWinPredictStore1 = MaxValUint8;
  BlackRightWinPredictStore2 = MaxValUint8;
  
  //第4阶段进入标志置为未进入。
  BlackRightStep4Flag = 0;
  BlackRightStep4StartLine = MaxValUint8;
  //默认十字检测标志置为未进入。
  BlackRightCrossDone = 0;
    
  return 1;
}


//======================================================================
//函数名：BlackGetRight
//功  能：右黑线提取算法
//参  数：无
//返  回：1成功 0失败
//影  响：BlackRightCrossDone，BlackRightLoc[][],
//        BlackRightRealWB[], BlackRightRealW[], BlackRightRealB[],
//        LimitRightWB, LimitRightW, LimitRightB
//说  明：1. 暂时可以处理直道，弯道，十字路口3种情况。2014.11.10
//      
//       
//       
//======================================================================
uint8 BlackGetRight(void)
{
    uint8 row, temp, temp1, temp2, temp3, i;
    uint8 GapCount, Row1RealLastLine, index, index1;
    uint8 tempWB, tempW, tempB;
    uint8 tempminus;
    uint8 successRow = MaxValUint8;
    
    int temp001;
    
    //右黑线提取时的参数初始化赋值
    BlackGetRightParaInit();     
    
    //第1阶段。这1阶段有可能会返回黑线提取失败
    
    //每幅有效图像的线头的3个阈值单独储存。Step1Scan前调用前一幅有效图像保存的3个阈值。
    LimitRightWB = LimitRightHeadWB;
    LimitRightW = LimitRightHeadW;
    LimitRightB = LimitRightHeadB;
            
    temp = BlackGetRightStep1Scan(CameraHight - 1, CameraHight - BlackStep1Size - 1, 1);
    BlackRightStep1ScanRow = temp;
    
    //左线开始扫描的情况下，右线尾比左线头高，直接返回失败。增加后面这个条件是为了防止十字误判。
//    if(
//       (BlackGetPreDir == 0 || BlackGetPreDir == 2)
//     &&(temp <= BlackLeftHeadLine)
//     &&(BlackLeftHeadLine < CameraHight - 7)
//      )
//    {
//        BlackRightLoc[temp][0] = MaxValUint8;
//        BlackRightLoc[temp - 1][0] = MaxValUint8;
//        return 0;
//    }
//    else
//    {
//    }
    
    if(temp == MaxValUint8)
    {
      return 0;
    }
    else
    {   
      //每幅有效图像的线头的3个阈值单独储存。若线头提取成功，则该幅图像有效，线头3个阈值更新。
      LimitRightHeadWB = LimitRightWB;
      LimitRightHeadW = LimitRightW;
      LimitRightHeadB = LimitRightB;
      
      row = temp;
      //如果起始线头的位置非常高，就认为是十字的前两个角，需要进入第4阶段向后补线。    
      if(row < CameraHight - BlackRightBackLineStart)
      {
        BlackRightStep4Flag = 1;             //默认为失败。
        BlackRightStep4StartLine = row;
      }

    }
      
    //第2阶段。到了这一阶段，已经不会返回黑线提取失败 
    //下面开始利用斜率找黑点。
    //上次已经处理完了第row-1行，下一次要处理第row-2行
    //BlackRightSlope();
    
    //temp为预测的黑点位置
    //对预测值进行限定。
    temp = BoundaryLimitRe(BlackRightLoc[row - 1][0] + BlackRightLoc[row - 1][0] - BlackRightLoc[row][0],
                           0,
                           CameraRealWidth - 1);    
    //预测值更新后就刷新一次堆栈，堆栈顶部丢失。
    BlackRightWinPredictStore0 = BlackRightWinPredictStore1;
    BlackRightWinPredictStore1 = BlackRightWinPredictStore2;
    BlackRightWinPredictStore2 = temp;
        
    index = 0;
    index1 = 0;
    
    GapCount = 0;  //连续多少行没有采到黑线
    successRow = MaxValUint8; //记录最后一次成功的行
    //从最近的认证行到取最远的行
    for(row -= 2; ; row -= 2)
    {  
      //连续2行检测黑点位置。     
      //近处的行已经将单线标志置位，则检测到左黑线头即可返回。
      if(SingleBlackAllFlag == 1 && row < BlackLeftHeadLine) return 1;
      temp1 = BlackGetRightStep2Win(row, BlackRightWinVal, temp, 1);
      if(SingleBlackAllFlag == 1 && row - 1 < BlackLeftHeadLine) return 1;
      temp2 = BlackGetRightStep2Win(row-1, BlackRightWinVal, temp, 2);
      
      temp3 = (
              (temp1 == 0)
            &&(temp2 == 0)
            //&&(BlackRightStep4Flag == 0)
              );
      //若连续2行的黑点检测成功，则更新3个阈值，更新预测点的位置
      if(temp1 == 1 && temp2 == 1) 
      {
        //只要连续采集到双黑线，则两个标志位重置为0.
        index = 0;
        index1 = 0;
        successRow = row;
      
        //差值的处理，留有裕度，且有上下界。
        tempWB = (BlackRightRealWB[0] + BlackRightRealWB[1]) / 2 - LimitRightWBMargin; 
        if(tempWB < LimitRightWBMin) tempWB = LimitRightWBMin;
        else if(tempWB > LimitRightWBMax) tempWB = LimitRightWBMax;
        else ;
        LimitRightWB = tempWB;
        
        //白点的处理，留有裕度，且有上下界。
        tempW = (BlackRightRealW[0] + BlackRightRealW[1]) / 2 - LimitRightWMargin;
        if(tempW < LimitRightWMin) tempW = LimitRightWMin;
        else if(tempW > LimitRightWMax) tempW = LimitRightWMax;
        else ;
        LimitRightW = tempW;
        
        //黑点的处理，留有裕度，且有上下界。
        tempB = (BlackRightRealB[0] + BlackRightRealB[1]) / 2 + LimitRightBMargin;
        if(tempB < LimitRightBMin) tempB = LimitRightBMin;
        else if(tempB > LimitRightBMax) tempB = LimitRightBMax;
        else ;
        //黑色阈值不能超过白色
        if(tempB >= LimitRightW)
        {
          tempB = LimitRightW - 1;
        }
        else
        {
        }             
        LimitRightB = tempB;   
         
        //对预测值进行限定。
        temp = BoundaryLimitRe(BlackRightLoc[row - 1][0] + BlackRightLoc[row - 1][0] - BlackRightLoc[row][0],
                               0,
                               CameraRealWidth - 1); 
        //预测值更新后就刷新一次堆栈，堆栈顶部丢失。
        BlackRightWinPredictStore0 = BlackRightWinPredictStore1;
        BlackRightWinPredictStore1 = BlackRightWinPredictStore2;
        BlackRightWinPredictStore2 = temp;        
      }
      //如果没能连续两行检测黑点成功，原本打算用原位置的预测点，后来发现效果不好，所以决定按预测点的趋势走。
      //如果连续两行均采集黑线失败，则记数，记到一定值就再来一次Step1
      //如果第4阶段检测标志置位，就不用进入十字检测了。
      else if(temp3 == 1)
      {
        //若上一幅图是单线，则不允许开启十字。
        if(SingleBlackLastAllFlag == 1) break;        
        //直角黑块锁存期间，为了防止一边十字成功另一边不成功造成的中心线倾斜，所以不允许开启十字。
        if(AngleZoneConfirmLockFlag == 1) break;
        
        //第一次检测到双线失败时，最后的几条线可能已经不太正确。
        if(index1 == 0)
        {
          //如果储存的3个值趋势相同，也就是变化量同号，那么就按正常的来。否则就认为最后一个值已经错误。
          if(row <= CameraHight - 3)
          {
              if (BlackRightLoc[row + 1][0] != MaxValUint8 && BlackRightLoc[row + 2][0] != MaxValUint8)
              {
                  //如果黑线点够，最好还是根据实际的黑点来更新窗口值。
                  tempminus = BoundaryLimitRe(BlackRightLoc[row + 1][0] * 3 - BlackRightLoc[row + 2][0] * 2,
                                              0,
                                              CameraRealWidth - 1);
                  //斜入十字不管怎么摆，右十字的上段线一定在下段线的左边。
                  tempminus = MinRe(tempminus, BlackRightLoc[row + 1][0]);
              }
              else
              {
                  temp001 = BlackRightWinPredictStore1 - BlackRightWinPredictStore0;
                  BlackRightWinPredictStore0 += temp001;
                  BlackRightWinPredictStore1 += temp001;
                  BlackRightWinPredictStore2 = BlackRightWinPredictStore1 + BlackRightWinPredictStore1 - BlackRightWinPredictStore0;
                  tempminus = BoundaryLimitRe(BlackRightWinPredictStore2 + BlackRightWinPredictStore2 - BlackRightWinPredictStore1,
                                              0,
                                              CameraRealWidth - 1);
              }
          }
          else
          {
              //理论上不会走到这里。
              return 0;
              
              /*
              //实在不行就根据Store的值来更新吧。
              //以上上次的窗口预测值为基准，加上2个上上次和上上上次的差。
              tempminus = BoundaryLimitRe(BlackRightWinPredictStore1 * 3 - BlackRightWinPredictStore0 * 2,
                                          0,
                                          CameraRealWidth - 1);
              //斜入十字不管怎么摆，右十字的上段线一定在下段线的左边。
              tempminus = MinRe(tempminus, BlackRightLoc[row + 1][0]);
            */
          }
          
          //进来一次后就再也不进来了。
          index1++;          
        }
        else
        {
          //预测点按趋势走也要限幅。
          tempminus = BoundaryLimitRe(BlackRightWinPredictStore2 + BlackRightWinPredictStore2 - BlackRightWinPredictStore1,
                                      0,
                                      CameraRealWidth - 1);          
        }
        //temp在这里表示窗口预测点位置。
        temp = tempminus;
        BlackRightWinPredictStore0 = BlackRightWinPredictStore1;
        BlackRightWinPredictStore1 = BlackRightWinPredictStore2;
        BlackRightWinPredictStore2 = temp;        
        
        //Row1RealLastLine记录第1段黑线的最后一行。
        if(index == 0)
        {          
          //进来一次后，就再也不会进来了。
          index++;
          for(i = row; i <= CameraHight - 1; i++)
          {
            if(BlackRightLoc[i][0] != MaxValUint8)
            {
              Row1RealLastLine = i;
              break;
            }
          }
          if(Row1RealLastLine >= CameraHight - 1)
          {
            Row1RealLastLine = CameraHight - 1;
          }
          else
          {
          }
          //记录为全局变量
          BlackRightRow1RealLastLine = Row1RealLastLine;
        }
        
        GapCount += 2;
        if (successRow != MaxValUint8 && BlackRightLoc[successRow][0] >240 && BlackRightLoc[successRow - 1][0] >240) { }
        ///////////////////////////////进入十字重新检测的条件还要再严格一些，减少没必要的时间，比如说前面最多只允许检测到多少行。
        else if(GapCount > BlackCrossGapLine)
        {
          //再检测时的行太远了，就没必要检测了。
          if(row < BlackRightCrossTooLowLine)
          {
            //扫屁股。
            for(i = row; ; i--)
            {
              BlackRightLoc[i][0] = MaxValUint8;
              if(i == 0) break;
            }
            //跳出来。
            break;
          }
          else
          {
          }
          
          //进入十字重新检测后，就不允许出现第4阶段向后补线的情况。
          if(BlackRightStep4Flag == 1)
          {
            //BlackRightStep4Flag = 3;   //向后补线出错。
            
            //直接跳出来的作法。不太好。
            /*
            BlackRightCrossDone = 1;   //十字检测失败。
            //扫屁股。
            for(i = row; ; i--)
            {
              BlackRightLoc[i][0] = MaxValUint8;
              if(i == 0) break;
            }
            //跳出来。
            break;
            */
          }
          else
          {
          }
          
          //十字再检测的话，Step1Scan的线头必须够近。
          //最后只剩第0，1行没有检测的话，就不用开启十字再检测了。
          if(BlackRightStep1ScanRow >= CameraHight - 3 && row > 3)
          {
            //正常的十字再检测。
            if(BlackRightAgainForCross(row - 2, 1, Row1RealLastLine, temp) == 0)
            {
              BlackRightCrossDone = 1;  //十字检测失败            
            }
            else
            {
              BlackRightCrossDone = 2;  //十字检测成功
            }
            break;  //十字再检测里会开窗检测到指定的最后一行，所以不用再循环了，直接跳出
          }
          //线头太远，或者只剩第0，1行的时候，直接break就好，远处的行在初始化时已经置为无效值。
          else
          {
            break;
          }
        }
        else
        {
           BlackRightCrossDone = 0;    //十字检测未进入
        }
      
      }
      //连续两线只有一线检测成功的情况，仍然要按趋势更新窗口预测值。
      else if((temp1 == 1 && temp2 == 0) || (temp1 == 0 && temp2 == 1)) 
      {   
        //第一次检测到有单线失败时，最后的几条线可能已经不太正确。
        if(index1 == 0)
        {
          //如果储存的3个值趋势相同，也就是变化量同号，那么就按正常的来。否则就认为最后一个值已经错误。
          if(row <= CameraHight - 3)
          {
              if (BlackRightLoc[row + 1][0] != MaxValUint8 && BlackRightLoc[row + 2][0] != MaxValUint8)
              {
                  //如果黑线点够，最好还是根据实际的黑点来更新窗口值。
                  tempminus = BoundaryLimitRe(BlackRightLoc[row + 1][0] * 3 - BlackRightLoc[row + 2][0] * 2,
                                              0,
                                              CameraRealWidth - 1);
                  //斜入十字不管怎么摆，右十字的上段线一定在下段线的左边。
                  tempminus = MinRe(tempminus, BlackRightLoc[row + 1][0]);
              }
              else
              {
                  temp001 = BlackRightWinPredictStore1 - BlackRightWinPredictStore0;
                  BlackRightWinPredictStore0 += temp001;
                  BlackRightWinPredictStore1 += temp001;
                  BlackRightWinPredictStore2 = BlackRightWinPredictStore1 + BlackRightWinPredictStore1 - BlackRightWinPredictStore0;
                  tempminus = BoundaryLimitRe(BlackRightWinPredictStore2 + BlackRightWinPredictStore2 - BlackRightWinPredictStore1,
                                              0,
                                              CameraRealWidth - 1);
              }
          }
          else
          {
              //理论上不会走到这里。
              return 0;
              
              /*
              //实在不行就根据Store的值来更新吧。
              //以上上次的窗口预测值为基准，加上2个上上次和上上上次的差。
              tempminus = BoundaryLimitRe(BlackRightWinPredictStore1 * 3 - BlackRightWinPredictStore0 * 2,
                                          0,
                                          CameraRealWidth - 1);
              //斜入十字不管怎么摆，右十字的上段线一定在下段线的左边。
              tempminus = MinRe(tempminus, BlackRightLoc[row + 1][0]);
              */
          }
          
          //进来一次后就再也不进来了。
          index1++;          
        }
        else
        {
          //预测点按趋势走也要限幅。
          tempminus = BoundaryLimitRe(BlackRightWinPredictStore2 + BlackRightWinPredictStore2 - BlackRightWinPredictStore1,
                                      0,
                                      CameraRealWidth - 1);          
        }
        //temp在这里表示窗口预测点位置。
        temp = tempminus;
        BlackRightWinPredictStore0 = BlackRightWinPredictStore1;
        BlackRightWinPredictStore1 = BlackRightWinPredictStore2;
        BlackRightWinPredictStore2 = temp;
      }
      //理论上不会走到这里。
      else
      {       
      }
      
      
      if((row == 0) || (row == 1)) break;
      
    }
    
    
    //第4阶段，采集到十字的前两个角，向后补线。这里有浮点运算，要严格控制调用。
    if(BlackRightStep4Flag == 1)
    {
      if(BlackRightStep4BackLine(BlackRightStep4StartLine, 0) == 0)
      {
        BlackRightStep4Flag = 1;   //向后补线失败，有可能是点取不够，有可能是斜率不对。
        //return 0;  
      }
      else
      {
        BlackRightStep4Flag = 2;   //向后补线成功。
      }
    }
    //BlackRightStep4Flag出错的情况。
    else if(BlackRightStep4Flag == 3)
    {
      //检查向后补线出错到底是为什么
      if(BlackRightStep4ErrorCheck() == 1)
      {      
        if(BlackRightStep4BackLine(BlackRightStep4StartLine, 0) == 0)
        {
          //BlackRightStep4Flag = 1;   //向后补线失败，有可能是点取不够，有可能是斜率不对。
          //return 0;  
        }
        else
        {
          BlackRightStep4Flag = 2;   //向后补线成功。
        }
      }
      else
      {
      }
    }
    //BlackRightStep4Flag未进入的情况。
    else
    {
    }

    return 1;

}




//黑线提取预处理
uint8 BlackGetPre(void)
{
  uint8 temp1, temp2;
  
  //如果是障碍特殊处理期间，黑线从控制线开始扫描。
  if(BrickSpeDealFlag == 1)
  {
    //左障碍，固定从右线开始扫描。
    if(BrickSpeDealDir == 0)
    {
      BlackGetPreDir = 1;
    }
    //右障碍，固定从左线开始扫描。
    else
    {
      BlackGetPreDir = 0;
    }
    
    return 1;
  }
  else
  {
  }
  
  //先预扫描左线
  BlackGetLeftParaInit();  //参数初始化
  temp1 = BlackGetLeftStep1Edge(CameraHight - 1, 1, 1);  //第三个参数1表示是黑线预处理
  temp2 = BlackGetLeftStep1Edge(CameraHight - 2, 2, 1);  //第三个参数1表示是黑线预处理
  if(
    (temp1 == 1)
  &&(temp2 == 1)
  &&(AbsInt(BlackLeftLoc[CameraHight - 1][0] - BlackLeftLoc[CameraHight - 2][0]) < BlackLeftStep1ScanMinus)  //两点列差不能太大。
    )
  {
    //左线采集线头成功，从左线开始扫描。 
    BlackGetPreDir = 0;
    return 1;
  }
  else
  {
    //有可能一行采集成功，所以要扫尾。
    BlackLeftLoc[CameraHight - 1][0] = MaxValUint8;
    BlackLeftLoc[CameraHight - 2][0] = MaxValUint8;
  }
  
  //左线头扫描失败，则扫描右线头
  BlackGetRightParaInit();  //参数初始化
  temp1 = BlackGetRightStep1Edge(CameraHight - 1, 1, 1);  //第三个参数1表示是黑线预处理
  temp2 = BlackGetRightStep1Edge(CameraHight - 2, 2, 1);  //第三个参数1表示是黑线预处理
  if(
    (temp1 == 1)
  &&(temp2 == 1)
  &&(AbsInt(BlackRightLoc[CameraHight - 1][0] - BlackRightLoc[CameraHight - 2][0]) < BlackRightStep1ScanMinus)  //两点列差不能太大。
    )
  {
    //右线头采集成功，从右线开始扫描。
    BlackGetPreDir = 1;
    return 1;    
  }
  else
  {
    //有可能一行采集成功，所以要扫尾。
    BlackRightLoc[CameraHight - 1][0] = MaxValUint8;
    BlackRightLoc[CameraHight - 2][0] = MaxValUint8;    
    
    //两线线头均采集失败，则应该是十字，仍然从左线开始扫描。
    BlackGetPreDir = 2;
    return 1;
  }
}



//黑线检测最后一步，找单线错误。///////////////////////////////这种检测单线错误的方法对浅色或者白色地板，效果会很差。
//1正常（没检测到单线错误，或检测到单线错误并成功处理），0出错（这幅图像直接会判定为失败）
uint8 BlackSingleSpeCheck(void)   
{
  uint8 temp1, temp2, index1;
  uint8 tempstep1row = MaxValUint8;
  int avgr1, avgr2, avgl1, avgl2;
  int i;
  
  //两线都成功，不是单线。
  if(
     (BlackLeftDone == 1)
   &&(BlackRightDone == 1)
     )
  {
    //检查左线悬空，注意不要在十字误判。
    if(
       (BlackRightStep1ScanRow == CameraHight - 1 && BlackLeftStep1ScanRow < CameraHight - 1) //左线尾较远，右线尾最近行。 
     &&(BlackRightLoc[BlackRightStep1ScanRow][0] != MaxValUint8 && BlackLeftLoc[BlackLeftStep1ScanRow][0] != MaxValUint8) //没有被大跳变滤掉 
     &&(BlackRightLoc[BlackRightStep1ScanRow-1][0] != MaxValUint8 && BlackLeftLoc[BlackLeftStep1ScanRow-1][0] != MaxValUint8) //没有被大跳变滤掉 
     &&(BlackRightLoc[BlackRightStep1ScanRow-2][0] != MaxValUint8 && BlackLeftLoc[BlackLeftStep1ScanRow-2][0] != MaxValUint8) //没有被大跳变滤掉   
       )
    {
      //1. 检查左线的左边一个单线宽度内是不是有右线。      
      if(
        (BlackLeftLoc[BlackLeftStep1ScanRow-1][0] > SingleBlackWidthMax[BlackLeftStep1ScanRow-1] * 1)
      &&(BlackLeftLoc[BlackLeftStep1ScanRow-2][0] > SingleBlackWidthMax[BlackLeftStep1ScanRow-2] * 1) //左线头不能太靠左，至少要留出1个单线宽度。 
      &&(BlackLeftLoc[BlackLeftStep1ScanRow-1][0] < CameraRealWidth / 2 + 60)  
      &&(BlackLeftLoc[BlackLeftStep1ScanRow-2][0] < CameraRealWidth / 2 + 60)  //左线不能太靠右，大致要在左半边屏内。
        )
      {
        //开一个单线宽度的窗来检测是不是有右线。temp1和temp2为右黑线的白点位置。
        temp1 = BlackGetRightJustWin(BlackLeftStep1ScanRow-1, 
                                     BlackLeftLoc[BlackLeftStep1ScanRow-1][0] - SingleBlackWidthMax[BlackLeftStep1ScanRow-1], //最多最多跨过15列，所以20列应该够。
                                     BlackLeftLoc[BlackLeftStep1ScanRow-1][0]);
        temp2 = BlackGetRightJustWin(BlackLeftStep1ScanRow-2, 
                                     BlackLeftLoc[BlackLeftStep1ScanRow-2][0] - SingleBlackWidthMax[BlackLeftStep1ScanRow-2], //最多最多跨过15列，所以20列应该够。
                                     BlackLeftLoc[BlackLeftStep1ScanRow-2][0]);
        //必须连续两行有右黑线。而且两个黑点所在列不能相差太大。 
        if(temp1 != MaxValUint8 && temp2 != MaxValUint8 && AbsInt(temp1 - temp2) < 8)
        {          
          //第一行右线的白点平均值
          avgr1 = (ImgNew[BlackLeftStep1ScanRow-1][temp1 - 1]
                 + ImgNew[BlackLeftStep1ScanRow-1][temp1 - 2]
                 + ImgNew[BlackLeftStep1ScanRow-1][temp1 - 3]
                 + ImgNew[BlackLeftStep1ScanRow-1][temp1 - 4]
                 + ImgNew[BlackLeftStep1ScanRow-1][temp1 - 5])
                 / 5;
          //第一行左线的白点平均值
          avgl1 = (ImgNew[BlackLeftStep1ScanRow-1][BlackLeftLoc[BlackLeftStep1ScanRow-1][0] + 1 + BlackLeftEdgeNum + 1]
                 + ImgNew[BlackLeftStep1ScanRow-1][BlackLeftLoc[BlackLeftStep1ScanRow-1][0] + 1 + BlackLeftEdgeNum + 2]
                 + ImgNew[BlackLeftStep1ScanRow-1][BlackLeftLoc[BlackLeftStep1ScanRow-1][0] + 1 + BlackLeftEdgeNum + 3]
                 + ImgNew[BlackLeftStep1ScanRow-1][BlackLeftLoc[BlackLeftStep1ScanRow-1][0] + 1 + BlackLeftEdgeNum + 4]
                 + ImgNew[BlackLeftStep1ScanRow-1][BlackLeftLoc[BlackLeftStep1ScanRow-1][0] + 1 + BlackLeftEdgeNum + 5])
                 / 5;          
          //第二行右线的白点平均值
          avgr2 = (ImgNew[BlackLeftStep1ScanRow-2][temp2 - 1]
                 + ImgNew[BlackLeftStep1ScanRow-2][temp2 - 2]
                 + ImgNew[BlackLeftStep1ScanRow-2][temp2 - 3]
                 + ImgNew[BlackLeftStep1ScanRow-2][temp2 - 4]
                 + ImgNew[BlackLeftStep1ScanRow-2][temp2 - 5])
                 / 5;
          //第二行左线的白点平均值
          avgl2 = (ImgNew[BlackLeftStep1ScanRow-2][BlackLeftLoc[BlackLeftStep1ScanRow-2][0] + 1 + BlackLeftEdgeNum + 1]
                 + ImgNew[BlackLeftStep1ScanRow-2][BlackLeftLoc[BlackLeftStep1ScanRow-2][0] + 1 + BlackLeftEdgeNum + 2]
                 + ImgNew[BlackLeftStep1ScanRow-2][BlackLeftLoc[BlackLeftStep1ScanRow-2][0] + 1 + BlackLeftEdgeNum + 3]
                 + ImgNew[BlackLeftStep1ScanRow-2][BlackLeftLoc[BlackLeftStep1ScanRow-2][0] + 1 + BlackLeftEdgeNum + 4]
                 + ImgNew[BlackLeftStep1ScanRow-2][BlackLeftLoc[BlackLeftStep1ScanRow-2][0] + 1 + BlackLeftEdgeNum + 5])
                 / 5;          
          
          //最后一步，确认两者的白色点的平均值差别非常小。  /////////////////////此时若地板为浅色或者白色，就挂了。
          if(AbsInt(avgr1 - avgl1) < 15 && AbsInt(avgr2 - avgl2) < 15)
          {
            BlackSingleFlag = 1;        //单线错误标志置位。
            BlackSingleDir = 0;         //表示是左线悬空的情况。
          }
          else
          {
          }
        }
        else
        {
        }
      }
      else
      {
      }
      
      //查看单线错误标志
      if(BlackSingleFlag == 1)
      {
        //确定是单线错误后，先把右黑线全置无效值。
        for(i = 0; ; i++)
        {
          BlackRightLoc[i][0] = MaxValUint8;           
          if(i == CameraHight - 1) break;
        }
        
        index1 = 0;
        //然后从左线尾所在行开始开窗扫右线，一直到左线头所在行或者有未采到的行。
        for(i = BlackLeftStep1ScanRow; ; i--)
        {
          temp1 = BlackGetRightJustWin(i, 
                                       BlackLeftLoc[i][0] - SingleBlackWidthMax[i], //最多最多跨过15列，所以20列应该够。
                                       BlackLeftLoc[i][0]);            
          if(temp1 != MaxValUint8)
          {
            //该行扫描右黑线成功
            BlackRightLoc[i][0] = temp1 + 1 + BlackRightEdgeNum;
            //记录右黑线扫描成功的最近行。
            if(index1 == 0)
            {
              tempstep1row = i;
              index1++;
            }
            else
            {
            }
          }
          else
          {
            //一旦没有采到，再远的行就不要了，直接退出。
            break;
          }
          //到左线头了，退出
          if(i == BlackLeftHeadLine) break;
        }
        
        //检测不到有效右黑线，返回失败。
        if(tempstep1row == MaxValUint8) return 0;
        
        //往远几行开始，向后补线。第2个输入参数是1。
        //不管左线之间的向后补线是否成功，都要放远几个点重新向后补一次
        if(BlackLeftStep4BackLine(BlackLeftStep1ScanRow - 2, 1) == 1 && BlackRightStep4BackLine(tempstep1row - 2, 1) == 1)
        {
        }
        else
        {
          //如果补线没有成功（有可能是点太少），那就直接返回失败，这幅图像直接判定为失败。
          return 0;
        }           
        
        //置单线标志位SingleBlackAllFlag(用于CenterLineGet)。     
        //SingleBlackAllFlag = 1;     
        
      }
      //单线错误标志没有置位，不用作任何处理。
      else
      {
      }
        
    }
    //检查右线悬空，注意不要在十字误判。
    else if(
       (BlackLeftStep1ScanRow == CameraHight - 1 && BlackRightStep1ScanRow < CameraHight - 1) //右线尾较远，左线尾最近行。 
     &&(BlackRightLoc[BlackRightStep1ScanRow][0] != MaxValUint8 && BlackLeftLoc[BlackLeftStep1ScanRow][0] != MaxValUint8) //没有被大跳变滤掉 
     &&(BlackRightLoc[BlackRightStep1ScanRow-1][0] != MaxValUint8 && BlackLeftLoc[BlackLeftStep1ScanRow-1][0] != MaxValUint8) //没有被大跳变滤掉 
     &&(BlackRightLoc[BlackRightStep1ScanRow-2][0] != MaxValUint8 && BlackLeftLoc[BlackLeftStep1ScanRow-2][0] != MaxValUint8) //没有被大跳变滤掉   
       )
    {
      //1. 检查右线的右边一个单线宽度内是不是有左线。      
      if(
        (BlackRightLoc[BlackRightStep1ScanRow-1][0] < CameraRealWidth - 1 - SingleBlackWidthMax[BlackRightStep1ScanRow-1] * 1)
      &&(BlackRightLoc[BlackRightStep1ScanRow-2][0] < CameraRealWidth - 1 - SingleBlackWidthMax[BlackRightStep1ScanRow-2] * 1) //右线头不能太靠右，至少要留出1个单线宽度。 
      &&(BlackRightLoc[BlackRightStep1ScanRow-1][0] > CameraRealWidth / 2 - 60)  
      &&(BlackRightLoc[BlackRightStep1ScanRow-2][0] > CameraRealWidth / 2 - 60)  //右线不能太靠左，大致要在右半边屏内。
        )
      {
        //开一个单线宽度的窗来检测是不是有左线。temp1和temp2为左黑线的白点位置。
        temp1 = BlackGetLeftJustWin(BlackRightStep1ScanRow-1, 
                                     BlackRightLoc[BlackRightStep1ScanRow-1][0], //最多最多跨过15列，所以20列应该够。
                                     BlackRightLoc[BlackRightStep1ScanRow-1][0] + SingleBlackWidthMax[BlackRightStep1ScanRow-1]);
        temp2 = BlackGetLeftJustWin(BlackRightStep1ScanRow-2, 
                                     BlackRightLoc[BlackRightStep1ScanRow-2][0], //最多最多跨过15列，所以20列应该够。
                                     BlackRightLoc[BlackRightStep1ScanRow-2][0] + SingleBlackWidthMax[BlackRightStep1ScanRow-2]);
        //必须连续两行有左黑线。而且两个黑点所在列不能相差太大。 
        if(temp1 != MaxValUint8 && temp2 != MaxValUint8 && AbsInt(temp1 - temp2) < 8)
        {          
          //第一行左线的白点平均值
          avgr1 = (ImgNew[BlackRightStep1ScanRow-1][temp1 + 1]
                 + ImgNew[BlackRightStep1ScanRow-1][temp1 + 2]
                 + ImgNew[BlackRightStep1ScanRow-1][temp1 + 3]
                 + ImgNew[BlackRightStep1ScanRow-1][temp1 + 4]
                 + ImgNew[BlackRightStep1ScanRow-1][temp1 + 5])
                 / 5;
          //第一行右线的白点平均值
          avgl1 = (ImgNew[BlackRightStep1ScanRow-1][BlackRightLoc[BlackRightStep1ScanRow-1][0] - 1 - BlackRightEdgeNum - 1]
                 + ImgNew[BlackRightStep1ScanRow-1][BlackRightLoc[BlackRightStep1ScanRow-1][0] - 1 - BlackRightEdgeNum - 2]
                 + ImgNew[BlackRightStep1ScanRow-1][BlackRightLoc[BlackRightStep1ScanRow-1][0] - 1 - BlackRightEdgeNum - 3]
                 + ImgNew[BlackRightStep1ScanRow-1][BlackRightLoc[BlackRightStep1ScanRow-1][0] - 1 - BlackRightEdgeNum - 4]
                 + ImgNew[BlackRightStep1ScanRow-1][BlackRightLoc[BlackRightStep1ScanRow-1][0] - 1 - BlackRightEdgeNum - 5])
                 / 5;
          //第二行左线的白点平均值
          avgr2 = (ImgNew[BlackRightStep1ScanRow-2][temp2 + 1]
                 + ImgNew[BlackRightStep1ScanRow-2][temp2 + 2]
                 + ImgNew[BlackRightStep1ScanRow-2][temp2 + 3]
                 + ImgNew[BlackRightStep1ScanRow-2][temp2 + 4]
                 + ImgNew[BlackRightStep1ScanRow-2][temp2 + 5])
                 / 5;
          //第二行右线的白点平均值
          avgl2 = (ImgNew[BlackRightStep1ScanRow-2][BlackRightLoc[BlackRightStep1ScanRow-2][0] - 1 - BlackRightEdgeNum - 1]
                 + ImgNew[BlackRightStep1ScanRow-2][BlackRightLoc[BlackRightStep1ScanRow-2][0] - 1 - BlackRightEdgeNum - 2]
                 + ImgNew[BlackRightStep1ScanRow-2][BlackRightLoc[BlackRightStep1ScanRow-2][0] - 1 - BlackRightEdgeNum - 3]
                 + ImgNew[BlackRightStep1ScanRow-2][BlackRightLoc[BlackRightStep1ScanRow-2][0] - 1 - BlackRightEdgeNum - 4]
                 + ImgNew[BlackRightStep1ScanRow-2][BlackRightLoc[BlackRightStep1ScanRow-2][0] - 1 - BlackRightEdgeNum - 5])
                 / 5;      
          
          //最后一步，确认两者的白色点的平均值差别非常小。  /////////////////////此时若地板为浅色或者白色，就挂了。
          if(AbsInt(avgr1 - avgl1) < 15 && AbsInt(avgr2 - avgl2) < 15)
          {
            BlackSingleFlag = 1;        //单线错误标志置位。
            BlackSingleDir = 1;         //表示是右线悬空的情况。
          }
          else
          {
          }
        }
        else
        {
        }
      }
      else
      {
      }
      
      //查看单线错误标志
      if(BlackSingleFlag == 1)
      {
        //确定是单线错误后，先把左黑线全置无效值。
        for(i = 0; ; i++)
        {
          BlackLeftLoc[i][0] = MaxValUint8;           
          if(i == CameraHight - 1) break;
        }
        
        index1 = 0;
        //然后从右线尾所在行开始开窗扫右线，一直到左线头所在行或者有未采到的行。
        for(i = BlackRightStep1ScanRow; ; i--)
        {
          temp1 = BlackGetLeftJustWin(i, 
                                      BlackRightLoc[i][0], //最多最多跨过15列，所以20列应该够。
                                      BlackRightLoc[i][0] + SingleBlackWidthMax[i]);            
          if(temp1 != MaxValUint8)
          {
            //该行扫描左黑线成功
            BlackLeftLoc[i][0] = temp1 - 1 - BlackLeftEdgeNum;
            //记录左黑线扫描成功的最近行。
            if(index1 == 0)
            {
              tempstep1row = i;
              index1++;
            }
            else
            {
            }
          }
          else
          {
            //一旦没有采到，再远的行就不要了，直接退出。
            break;
          }
          //到右线头了，退出
          if(i == BlackRightHeadLine) break;
        }
        
        //检测不到有效左黑线，返回失败。
        if(tempstep1row == MaxValUint8) return 0;
         
        //往远几行开始，向后补线。第2个输入参数是1。
        //不管右线之间的向后补线是否成功，都要放远几个点重新向后补一次
        if(BlackRightStep4BackLine(BlackRightStep1ScanRow - 2, 1) == 1 && BlackLeftStep4BackLine(tempstep1row - 2, 1) == 1)
        {
        }
        else
        {
          //如果补线没有成功（有可能是点太少），那就直接返回失败，这幅图像直接判定为失败。
          return 0;
        }           
        
        //置单线标志位SingleBlackAllFlag(用于CenterLineGet)。     
        //SingleBlackAllFlag = 1;
        
      }
      //单线错误标志没有置位，不用作任何处理。
      else
      {
      }
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
 



//大跳变最终检测，基本上是针对直角黑块斜入。
//返回1表示检测到大跳变，0表示没有检测到或没必要检测。 
//有FLASH烧写进来的开关控制，如果发现比赛赛道的直角黑块基本没有斜入的情况，那就关掉。
//直角黑块误判
//1. 5Slope数组，同号（左线同为负号，右线同为正号），但相差大于等于2500。
//   离直角黑块比较近的时候，一般相差3000及以上，但离得比较远的时候，有可能相差2500。
//2. 在5SlopeRow数组里可以找到相应的行，从该行（比如说是第13行），则最好把13、14、15、16（17）都删掉。
uint8 BlackBigChangeFinalCheck(void)
{
  //BlackLeft5Slope[]数组和BlackLeft5SlopeRow[]数组的下标为0~BlackLeft5SlopeIndex-1
  //普通的BigChange中，出错的阈值是BlackLeftBigChangeSlopeLimit=4000左右（不是绝对值之差，就是两数之差）。
  uint8 i, row;
  uint8 tempend1, tempend2;
  
  return 0;         //不需要这个检测了，所以关闭
  
  //1. 要求两线都成功
  if(BlackLeftDone == 1 && BlackRightDone == 1)
  {
  }
  else
  {
    return 0;
  }
  
  //2. 要求线头必须都在最近行。
  //左线开始扫描时
  if(BlackGetPreDir == 0 || BlackGetPreDir == 2)
  {
    if(BlackLeftStep1ScanRow == CameraHight - 1 && BlackRightStep1ScanRow >= CameraHight - 5)
    {
    }
    else
    {
      //线头太高，不用检测大跳变错误。
      return 0;
    }
  }
  //右线开始扫描时
  else
  {
    if(BlackRightStep1ScanRow == CameraHight - 1 && BlackLeftStep1ScanRow >= CameraHight - 5)
    {
    }
    else
    {
      //线头太高，不用检测大跳变错误。
      return 0;
    }    
  }
  
  //左线
  //左线出现直角黑块的大跳变，左线一定是右上到左下。斜率一定是负的。
  if(SlopeL < 0 && BlackLeftEndLine - BlackLeftHeadLine - 5 > 0 && BlackLeft5SlopeIndex - 2 > 0)
  {
    //5Slope数组有可能已经删掉了一些黑点，所以也要参考实际的有效黑点数。
    tempend1 = MinRe(BlackLeftEndLine - BlackLeftHeadLine - 5, BlackLeft5SlopeIndex - 2);
    //轮询5Slope数组
    for(i = 0; ; i++)
    {
      //左线的直角黑块误判都是负号。
      if(
        (BlackLeft5Slope[i] < 0)
      &&(BlackLeft5Slope[i+1] < 0)
      &&(BlackLeft5Slope[i] - BlackLeft5Slope[i+1] >= BlackBigChangeFinalCheckSlopeLimit)     //i+1比i小2500以上。不能是">"
        )
      {
        //检测到直角黑块大跳变，跳出。
        break;
      }
      else
      {      
      }
      
      //到数组最后一个，跳出。
      if(i == tempend1) break;
    }
    
    //检测到直角黑块大跳变。
    if(i != tempend1)
    {
      //要删掉4行，至少要有4行。
      if(i > 4)
      {
        row = BlackLeft5SlopeRow[i+1] + 3;
        //十字检测未进入或失败时，是需要检测最终大跳变的。
        //若十字检测成功，且row在第1段黑线真实结束行的远处时，不需要检测，防止因十字补线带来的斜率突变。
        if(BlackLeftCrossDone == 2 && row < BlackLeftRow1RealLastLine)
        {
          //返回不需要检测
          return 0;
        }
        else
        {
        }
        
        //删掉从倒退4行开始的往远处的所有行。
        for(; ; row--)
        {
          BlackLeftLoc[row][0] = MaxValUint8;
          
          if(row == 0) break;
        }
  
        //置标志位。左线最终大跳变。
        BlackBigChangeFinalCheckFlag = 1;
        //返回1检测成功。
        return 1;
      }
      else
      {     
      }
    }
    //没检测到直角黑块大跳变，不作任何处理。
    else
    {    
    }
  }
  else
  {
  }
  
  //右线
  //右线出现直角黑块的大跳变，右线一定是左上到右下。斜率一定是正的。
  if(SlopeR > 0 && BlackRightEndLine - BlackRightHeadLine - 5 > 0 && BlackRight5SlopeIndex - 2 > 0)
  {
    //5Slope数组有可能已经删掉了一些黑点，所以也要参考实际的有效黑点数。
    tempend2 = MinRe(BlackRightEndLine - BlackRightHeadLine - 5, BlackRight5SlopeIndex - 2);
    //轮询5Slope数组
    for(i = 0; ; i++)
    {
      //右线的直角黑块误判都是正号。
      if(
        (BlackRight5Slope[i] > 0)
      &&(BlackRight5Slope[i+1] > 0)
      &&(BlackRight5Slope[i+1] - BlackRight5Slope[i] >= BlackBigChangeFinalCheckSlopeLimit)     //i+1比i大2500以上。不能是">"
        )
      {
        //检测到直角黑块大跳变，跳出。
        break;
      }
      else
      {      
      }
      
      //到数组最后一个，跳出。
      if(i == tempend2) break;
    }
    
    //检测到直角黑块大跳变。
    if(i != tempend2)
    {
      //要删掉4行，至少要有4行。
      if(i > 4)
      {        
        row = BlackRight5SlopeRow[i+1] + 3;
        //十字检测未进入或失败时，是需要检测最终大跳变的。
        //若十字检测成功，且row在第1段黑线真实结束行的远处时，不需要检测，防止因十字补线带来的斜率突变。
        if(BlackRightCrossDone == 2 && row < BlackRightRow1RealLastLine)
        {
          //返回不需要检测
          return 0;
        }
        else
        {
        }
        
        //删掉从倒退4行开始的往远处的所有行。
        for(; ; row--)
        {
          BlackRightLoc[row][0] = MaxValUint8;
          
          if(row == 0) break;
        }
        
        //置标志位。右线最终大跳变。
        BlackBigChangeFinalCheckFlag = 2;
        //返回1检测成功。      
        return 1;
      }
      else
      {     
      }
    }
    //没检测到直角黑块大跳变，不作任何处理。
    else
    {    
    }  
  }
  else
  {
  }
  
  //常态为0，即没检测到或没必要检测
  return 0;
}


////斜入直角黑块
////返回1表示检测到，0表示没有检测到。
//uint8 BlackAngleZoneSpeCheck(void)
//{
//  uint8 temp1, temp2, i;
//  
//  //先检测右边有没有斜入的直角黑块。
//  //1. 初步检测
//  if(
//    (BlackLeftDone == 1 && BlackRightDone == 1)
//  &&(SlopeL + 3000 < 0)
//  &&(SlopeR > 0)
//  &&(CURVEL > 0)
//  &&(CURVER < 0)
//  &&(BlackGetPreDir == 0)
//  &&(BlackLeftStep1ScanRow == CameraHight - 1)
//  &&(AbsInt(BlackLeftHeadLine - BlackRightHeadLine) < 5)
//    )
//  {
//    //2. 左线头之前的5Slope均为负
//    //左线
//    //考虑到了Black5Slope数组的个数与有效黑点不一定相同，大大减少了误判的机率。
//    if(BlackLeftEndLine - BlackLeftHeadLine - 4 < 0 || BlackLeft5SlopeIndex - 1 < 0)
//    {
//      return 0;
//    }
//    else
//    {        
//    }
//    temp1 = MinRe(BlackLeftEndLine - BlackLeftHeadLine - 4, BlackLeft5SlopeIndex - 1);
//    
//    //逐个判断小5点斜率是否满足条件
//    for(i = 0; ; i++)
//    {
//      if(BlackLeft5Slope[i] > 0) //一旦有正的，马上返回失败。
//      {
//        return 0;
//      }
//
//      if(i == temp1) break;
//    }
//    //3. 右线头之前的5Slope均为正
//    //右线
//    //考虑到了Black5Slope数组的个数与有效黑点不一定相同，大大减少了误判的机率。
//    if(BlackRightEndLine - BlackRightHeadLine - 4 < 0 || BlackRight5SlopeIndex - 1 < 0)
//    {
//      return 0;
//    }
//    else
//    {        
//    }
//    temp2 = MinRe(BlackRightEndLine - BlackRightHeadLine - 4, BlackRight5SlopeIndex - 1);
//    
//    //逐个判断小5点斜率是否满足条件
//    for(i = 0; ; i++)
//    {
//      if(BlackRight5Slope[i] < 0) //一旦有负的，马上返回失败。
//      {
//        return 0;
//      }
//
//      if(i == temp2) break;
//    }
//    //4. 存在有行的宽度很小。
//    temp1 = MinRe(BlackLeftEndLine, BlackRightEndLine);   //尾
//    temp2 = MaxRe(BlackLeftHeadLine, BlackRightHeadLine); //头
//    if(temp1 < temp2) 
//    {
//      return 0;
//    }
//    else
//    {
//      for(i = temp1; ; i--)
//      {
//        if(BlackRightLoc[i][0] - BlackLeftLoc[i][0] < 80) break;
//        
//        if(i == temp2) break;
//      }
//      
//      //确认存在宽度很小的行
//      if(i != temp2)
//      {
//        BlackAngleZoneSpeCheckFlag = 1; //标志位
//        BlackAngleZoneSpeCheckDir = 1;  //右边有这样的情况
//        return 1;
//      }
//      else
//      {
//      }
//    }
//  }
//  //右边没有斜入的黑块，接着检测左边有没有斜入的黑块。
//  else
//  {
//  }
//  
//
//  //先检测左边有没有斜入的直角黑块。
//  //1. 初步检测
//  if(
//    (BlackLeftDone == 1 && BlackRightDone == 1)
//  &&(SlopeL < 0)
//  &&(SlopeR > 3000)
//  &&(CURVEL > 0)
//  &&(CURVER < 0)
//  &&(BlackRightStep1ScanRow >= CameraHight - 5)
//  &&(AbsInt(BlackLeftHeadLine - BlackRightHeadLine) < 5)
//    )
//  {
//    //2. 左线头之前的5Slope均为负
//    //左线
//    //考虑到了Black5Slope数组的个数与有效黑点不一定相同，大大减少了误判的机率。
//    if(BlackLeftEndLine - BlackLeftHeadLine - 4 < 0 || BlackLeft5SlopeIndex - 1 < 0)
//    {
//      return 0;
//    }
//    else
//    {        
//    }
//    temp1 = MinRe(BlackLeftEndLine - BlackLeftHeadLine - 4, BlackLeft5SlopeIndex - 1);
//    
//    //逐个判断小5点斜率是否满足条件
//    for(i = 0; ; i++)
//    {
//      if(BlackLeft5Slope[i] > 0) //一旦有正的，马上返回失败。
//      {
//        return 0;
//      }
//
//      if(i == temp1) break;
//    }
//    //3. 右线头之前的5Slope均为正
//    //右线
//    //考虑到了Black5Slope数组的个数与有效黑点不一定相同，大大减少了误判的机率。
//    if(BlackRightEndLine - BlackRightHeadLine - 4 < 0 || BlackRight5SlopeIndex - 1 < 0)
//    {
//      return 0;
//    }
//    else
//    {        
//    }
//    temp2 = MinRe(BlackRightEndLine - BlackRightHeadLine - 4, BlackRight5SlopeIndex - 1);
//    
//    //逐个判断小5点斜率是否满足条件
//    for(i = 0; ; i++)
//    {
//      if(BlackRight5Slope[i] < 0) //一旦有负的，马上返回失败。
//      {
//        return 0;
//      }
//
//      if(i == temp2) break;
//    }
//    //4. 存在有行的宽度很小。
//    temp1 = MinRe(BlackLeftEndLine, BlackRightEndLine);   //尾
//    temp2 = MaxRe(BlackLeftHeadLine, BlackRightHeadLine); //头
//    if(temp1 < temp2) 
//    {
//      return 0;
//    }
//    else
//    {
//      for(i = temp1; ; i--)
//      {
//        if(BlackRightLoc[i][0] - BlackLeftLoc[i][0] < 80) break;
//        
//        if(i == temp2) break;
//      }
//      
//      //确认存在宽度很小的行
//      if(i != temp2)
//      {
//        BlackAngleZoneSpeCheckFlag = 1; //标志位
//        BlackAngleZoneSpeCheckDir = 0;  //左边有这样的情况
//        return 1;
//      }
//      else
//      {
//      }
//    }
//  }
//  //右边没有斜入的黑块，接着检测左边有没有斜入的黑块。
//  else
//  {
//  }
//  
//  
//  //常态为没检测到。
//  return 0;
//}



//======================================================================
//函数名：BlackGet
//功  能：黑线提取程序
//参  数：无
//返  回：1成功 0失败
//影  响：见具体的左、右黑线算法
//说  明：1. 学长的黑线提取算法有很多输入参数，用以对不同环境下的黑线进行提取，可以效仿
//        2. 左右黑线分开提取，但右黑线提取的起始点以左黑线提取的结果为准（前提是左黑线提取成功）。
//        3. 为了防止光线的突变，左右黑线各有2次机会，第2次机会将会把3个关键值赋成原始值再试一次。
//           如果不是光线的原因，3个关键值会变回来。
//======================================================================
uint8 BlackGet(void)
{
    uint8 i;
    uint8 tempBlackAngleZoneSpeCheck;       //斜入直角黑块储存位
    uint8 tempBlackBigChangeFinalCheck; //最终大跳变检测储存位。
    uint8 tempBlackSingleSpeCheck;      //单线错误检测储存位。
    //uint8 tempLWB, tempLW, tempLB;
    //uint8 tempRWB, tempRW, tempRB;

    //黑线提取预处理。决定先扫描哪条线
    BlackGetPre();    
    
    //预处理左线头成功或左右线头均失败时，从左线开始扫描。
    if(BlackGetPreDir == 0 || BlackGetPreDir == 2)
    {
      //左黑线提取，成功则返回1，失败则返回0.
      if(BlackGetLeft() == 0)
      {
        BlackLeftDone = 0;                  //左黑线提取失败
        BlackLeftHeadLine = MaxValUint8;    //左线头置无效值。
        BlackLeftEndLine = MaxValUint8;    //左线尾置无效值。
      }
      else
      {
        //先进行左黑线连续性补线
        BlackLeftContinueAdd();
        //再进行左黑线线头线尾查找
        BlackLeftHeadEndSearch();
  
      }    
      
      //右黑线提取，成功则返回1，失败则返回0.
      //如果是右障碍特殊处理，则右黑线函数不用执行，直接置为失败。
      if(
        (BrickSpeDealFlag == 1 && BrickSpeDealDir == 1)
      ||(BlackGetRight() == 0)
        )
      {
        BlackRightDone = 0;                  //右黑线提取失败
        BlackRightHeadLine = MaxValUint8;    //右线头置无效值。      
      }
      else
      {
        //先进行右黑线连续性补线
        BlackRightContinueAdd();
        //再进行右黑线线头线尾查找
        BlackRightHeadEndSearch();
      }
    }
    //右线头成功，从右线开始扫描。
    else
    {
      //右黑线提取，成功则返回1，失败则返回0.
      if(BlackGetRight() == 0)
      {
        BlackRightDone = 0;                  //右黑线提取失败
        BlackRightHeadLine = MaxValUint8;    //右线头置无效值。      
      }
      else
      {
        //先进行右黑线连续性补线
        BlackRightContinueAdd();
        //再进行右黑线线头线尾查找
        BlackRightHeadEndSearch();
      }
      
      //左黑线提取，成功则返回1，失败则返回0.
      //如果是左障碍特殊处理，则左黑线函数不用执行，直接置为失败。
      if(
        (BrickSpeDealFlag == 1 && BrickSpeDealDir == 0)
      ||(BlackGetLeft() == 0)
        )
      {
        BlackLeftDone = 0;                  //左黑线提取失败
        BlackLeftHeadLine = MaxValUint8;    //左线头置无效值。
        BlackLeftEndLine = MaxValUint8;    //左线尾置无效值。
      }
      else
      {
        //先进行左黑线连续性补线
        BlackLeftContinueAdd();
        //再进行左黑线线头线尾查找
        BlackLeftHeadEndSearch();
  
      }
    }
     
    //左、右黑线均提取失败，则黑线提取失败。
    if((BlackLeftDone == 0) && (BlackRightDone == 0))
    {
      return 0;
    }
    else
    {
      //左右黑线畸变矫正
      BlackUdis();
      
      //只要有一边成功就可以进来。
      //由于回弯要用到斜率，所以提前计算。
      //曲率、斜率求取的标志位，1成功，0失败。
      CurveSlopeFlagL = 1;
      CurveSlopeFlagR = 1;
      //左黑线的曲率、斜率的求取。  
      if(CurveSlopeGetLeft() == 0)
      {
        CurveSlopeFlagL = 0;
        //uart_sendN(UART0, (uint8 *)"\nCurveSlopeGetLeft() Failed!", 28);
      }
      else
      {
      }
      
      //右黑线的曲率、斜率的求取。  
      if(CurveSlopeGetRight() == 0)
      {
        CurveSlopeFlagR = 0;
        //uart_sendN(UART0, (uint8 *)"\nCurveSlopeGetRight() Failed!", 29);
      }
      else
      {
      }
      
      //左线提取成功则进行左回弯检测
      if(BlackLeftDone == 1)
      {
        //左回弯特殊检测。
        if(BlackLeftRollCheck() == 0)
        {
          BlackLeftRollCheckFlag = 1;         //检测到回弯。
          BlackLeftDone = 0;                  //左黑线提取失败
          //清除所有值。
          for(i = BlackLeftEndLine; ; i--)
          {
            BlackLeftLoc[i][0] = MaxValUint8;
            if(i == BlackLeftHeadLine) break;
          }
          BlackLeftHeadLine = MaxValUint8;    //左线头置无效值。
          BlackLeftEndLine = MaxValUint8;     //左线尾置无效值。
        }
        else
        {
        }
      }
      //左线提取失败就不用进行左回弯检测了
      else
      {
      }
      
      //右线提取成功则进行右回弯检测
      if(BlackRightDone == 1)
      {
        //右回弯特殊检测。
        if(BlackRightRollCheck() == 0)
        {
          BlackRightRollCheckFlag = 1;         //检测到回弯。
          BlackRightDone = 0;                  //右黑线提取失败
          //清除所有值。
          for(i = BlackRightEndLine; ; i--)
          {
            BlackRightLoc[i][0] = MaxValUint8;
            if(i == BlackRightHeadLine) break;
          }
          BlackRightHeadLine = MaxValUint8;    //右线头置无效值。
          BlackRightEndLine = MaxValUint8;     //右线尾置无效值。
        }
        else
        {
        }
      }
      //右线提取失败就不用进行右回弯检测了
      else
      {
      }
      
    }
    
    //若处在特殊赛道锁存或者特殊控制阶段，则不用进行大跳变最终检测 和 单线错误检测
    if(AngleConfirmLockFlag == 1 || AngleSpeDealFlag == 1 || BrickConfirmLockFlag == 1 || BrickSpeDealFlag == 1)
    {
    }
    else
    {
      //斜入直角黑块检测
      tempBlackAngleZoneSpeCheck = 0;
//      if(BlackAngleZoneSpeCheckSwitch == 1)
//      {
//        //1表示检测到斜入直角黑块，0表示没有检测到。
//        tempBlackAngleZoneSpeCheck = BlackAngleZoneSpeCheck();
//      }
//      else
//      {
//      }

      //大跳变最终检测.
      tempBlackBigChangeFinalCheck = 0;
      if(tempBlackAngleZoneSpeCheck == 0 && BlackBigChangeFinalCheckSwitch == 1)
      {
        //1表示检测到，0表示没检测到。
        tempBlackBigChangeFinalCheck = BlackBigChangeFinalCheck();
        //大跳变检测成功，重新找线头线尾，作畸变矫正，计算整条线的斜率、曲率，
        if(tempBlackBigChangeFinalCheck == 1)
        {
          //左黑线线头线尾查找
          BlackLeftHeadEndSearch();
          //右黑线线头线尾查找
          BlackRightHeadEndSearch();
          
          //左右黑线畸变矫正
          BlackUdis();
          
          //只要有一边成功就可以进来。
          //由于回弯要用到斜率，所以提前计算。
          //曲率、斜率求取的标志位，1成功，0失败。
          CurveSlopeFlagL = 1;
          CurveSlopeFlagR = 1;
          //左黑线的曲率、斜率的求取。  
          if(CurveSlopeGetLeft() == 0)
          {
            CurveSlopeFlagL = 0;
            //uart_sendN(UART0, (uint8 *)"\nCurveSlopeGetLeft() Failed!", 28);
          }
          else
          {
          }
          
          //右黑线的曲率、斜率的求取。  
          if(CurveSlopeGetRight() == 0)
          {
            CurveSlopeFlagR = 0;
            //uart_sendN(UART0, (uint8 *)"\nCurveSlopeGetRight() Failed!", 29);
          }
          else
          {
          }          
        }
        else
        {
        }
      }
      else
      {        
      }
      
      if(tempBlackAngleZoneSpeCheck == 1 || tempBlackBigChangeFinalCheck == 1)
      {
        //前面的斜入直角黑块或大跳变已经检测成功了，就不用检测单线错误，两种针对的情况完全不同，关键在于线头线尾、畸变矫正、斜曲率计算最好不要重复，浪费时间。
      }
      /*
      else
      {
        //黑线检测最后一步，找单线错误。
        //单线错误比较差劲，出错了，这幅图就直接判定为错图。   
        tempBlackSingleSpeCheck = BlackSingleSpeCheck();
        if(tempBlackSingleSpeCheck == 0)
        {
          BlackLeftDone = 0;
          BlackLeftHeadLine = MaxValUint8;
          BlackLeftEndLine = MaxValUint8;
          BlackRightDone = 0;
          BlackRightHeadLine = MaxValUint8;
          BlackRightEndLine = MaxValUint8;          
          
          return 0;
        }
        else
        {
          //检测到了单线错误，重新找线头线尾，作畸变矫正，计算整条线的斜率、曲率，
          if(BlackSingleFlag == 1)
          {
            //左黑线线头线尾查找
            BlackLeftHeadEndSearch();
            //右黑线线头线尾查找
            BlackRightHeadEndSearch();
            
            //左右黑线畸变矫正
            BlackUdis();
            
            //只要有一边成功就可以进来。
            //由于回弯要用到斜率，所以提前计算。
            //曲率、斜率求取的标志位，1成功，0失败。
            CurveSlopeFlagL = 1;
            CurveSlopeFlagR = 1;
            //左黑线的曲率、斜率的求取。  
            if(CurveSlopeGetLeft() == 0)
            {
              CurveSlopeFlagL = 0;
              //uart_sendN(UART0, (uint8 *)"\nCurveSlopeGetLeft() Failed!", 28);
            }
            else
            {
            }
            
            //右黑线的曲率、斜率的求取。  
            if(CurveSlopeGetRight() == 0)
            {
              CurveSlopeFlagR = 0;
              //uart_sendN(UART0, (uint8 *)"\nCurveSlopeGetRight() Failed!", 29);
            }
            else
            {
            }
          }
          //没有检测到单线错误，不用作处理。
          else
          {
          }
        }
      }*/          
    }

    
    return 1;
}


//针对单线错误的中心线重新处理
uint8 CenterLineGetSingleWrongAdd(void)
{
  uint8 row, i;
  uint8 temp;
  uint8 flag1, flag2;
  uint8 tempflag1;
  uint8 tempway1;
  uint8 x[CenterForwardLineNum];   //中心线向前补线计算斜率时，x坐标，表示第几行。。
  int16 y[CenterForwardLineNum];   //中心线向前补线计算斜率时，y坐标，表示中心线位置。
  int32 a1, a0;    //中心线向前补线时，最小二乘法的斜率和纵截距。
  
  float slope;
  
  //中心线数组的重新初始化
  for(i = 0; ; i++)
  {
    CenterLineLoc[i] = MaxValInt16;
    if(i == CameraHight - 1) break;
  }
  
  //左线悬空，右线有效。
  if(RoadHalfWidthWrongDir == 0)
  {
    //若右线的线头线尾无效，则直接返回失败。
    if(BlackRightHeadLine == MaxValUint8 || BlackRightEndLine == MaxValUint8)
    {
      return 0;
    }
    else
    {
    }
       
    flag1 = 1;
    flag2 = 1;
    
    //中心线的线尾取左线的线尾即可。
    CenterHeadLine = BlackRightHeadLine;   //线头取左线的线头，注意这不是实际的线头。
    //CenterHeadLine = 0;                    //线头因为要固定补到第0行，所以直接置0.
    CenterEndLine = BlackRightEndLine;
    
    //首先看线头是否大于BlackControlLineLow行
    if(BlackRightHeadLine >= BlackControlLineLow)
    {
      flag1 = 0;     
    }
    else
    {
    }
    
    //右线尾小于CameraHight - 1行。
    if(BlackRightEndLine < CameraHight - 1)
    {
      //只有右线成功，而且右线尾还那么远，真心没法继续。直接返回失败。
      //if(BlackRightEndLine < CameraHight - 1 - BlackRightBackLineStart)
      if(BlackRightEndLine <= BlackControlLineLow)  
      {
        flag2 = 0;
        return 0;
      }
      //右线尾在一定范围内。可以允许把第59行往前移几行。直接取BlackRightEndLine。
      else
      {
      }
    }
    //右线尾等于CameraHight - 1，直接取BlackRightEndLine
    else
    {
    }
    
    //丢失另一根线的时候，先根据斜率估算出半宽，若没有上一次双线图像时的赛道半宽大，则直接用上一次双线的赛道半宽。
    //求5行的斜率。
    if(BlackRightEndLine - BlackRightHeadLine > 5)
    {
      slope = 1.0 * (BlackUdisRightLocation[BlackRightEndLine].x - BlackUdisRightLocation[BlackRightEndLine-4].x) / 
                    (BlackUdisRightLocation[BlackRightEndLine].y - BlackUdisRightLocation[BlackRightEndLine-4].y);
    }
    else
    {
      slope = 0;
    }
    //赛道半宽
    temp = (uint8)(1.0 * sqrt(slope * slope + 1) * RoadWidth / 2);
    //取最大值
    RoadHalfWidth = MaxRe(temp, RoadHalfWidth);
    
    tempflag1 = 0;
    tempway1 = 0;
    //根据赛道宽度估计出中心线的线头。
    CenterLineLoc[BlackRightEndLine] = BlackUdisRightLocation[BlackRightEndLine].x - RoadHalfWidth;
    //根据右线的走势估计出中心线。中心线是个int16型矩阵，有正负。
    for(row = BlackRightEndLine - 1; ;row--)
    {
      //右线有效时。根据右线的趋势补线。
      if(BlackRightLoc[row][0] != MaxValUint8)
      {
        tempflag1 = 0;
        CenterLineLoc[row] = CenterLineLoc[row+1] + BlackUdisRightLocation[row].x - BlackUdisRightLocation[row+1].x;
      }
      //右线无效时。根据最后两行的中心线趋势补线。
      else
      {
          //当右线无效时，可以采用最后两行趋势补线，也可以根据最后CenterForwardLineNum行最小二乘法趋势补线。
          if(tempflag1 == 0)
          {
            //每幅图只进来一次，用来确定补线方式，以后就不用再进来了。
            tempflag1 = 1;
            //最后几行行数够多，可以采用最小二乘法的方式。
            if(row + CenterForwardLineNum <= CameraHight - 1)
            {
              tempway1 = 0;
              //进行最小二乘法系数的计算。
              for(i = 0; i < CenterForwardLineNum; i++)
              {
                x[i] = row + i + 1;
                y[i] = CenterLineLoc[row + i + 1];
              }
              a1 = LeastSquareInt16a1(x, y, CenterForwardLineNum);         //斜率计算。
              a0 = LeastSquareInt16a0(x, y, a1, CenterForwardLineNum);     //纵截距计算。
            }
            //最后几行的行数不够多，采用最后两行趋势补线的方式。
            else
            {
              tempway1 = 1;
            }
          }
          else
          {
          }
        
          //确认补线方式后，开始补线。
          if(tempway1 == 0)
          {
            //最小二乘法的方式。
            CenterLineLoc[row] = (a0 + a1 * row) / LeastSquareMulti;
          }
          else
          {
            //最近2行。
            if(row == CameraHight - 1 || row == CameraHight - 2)
            {
              CenterLineLoc[row] = BlackUdisRightLocation[row].x - RoadHalfWidth;
            }
            //不是最近2行。
            else
            {
              //最后两行的方式。
              CenterLineLoc[row] = CenterLineLoc[row+1] + CenterLineLoc[row+1] - CenterLineLoc[row+2];
            }
          }
        }
      
        if(row == 0) break;
      }
    
      
    
    //对未处理的行的中心线置无效值。
    //底部置无效值。顶部不用处理。
    if(CenterEndLine != CameraHight - 1)
    {
      for(row = CameraHight - 1; ; row--)
      {
        if(row == CenterEndLine) break;       
        CenterLineLoc[row] = MaxValInt16;
      }
    }
    else
    {
    }
  }
  //右线悬空，左线有效。
  else
  {
    //若左线的线头线尾无效，则直接返回失败。
    if(BlackLeftHeadLine == MaxValUint8 || BlackLeftEndLine == MaxValUint8)
    {
      return 0;
    }
    else
    {
    }
    
    flag1 = 1;
    flag2 = 1;
    
    //中心线的线尾取左线的线尾即可。
    CenterHeadLine = BlackLeftHeadLine;     //线头取左线的线头，注意这不是实际的线头。
    //CenterHeadLine = 0;   //线头因为要固定补到第0行，所以直接置0.
    CenterEndLine = BlackLeftEndLine;
    
    //首先看线头是否大于BlackControlLineLow行
    if(BlackLeftHeadLine >= BlackControlLineLow)
    {
      flag1 = 0;
    }
    else
    {
    }
    
    //左线尾小于CameraHight - 1行。
    if(BlackLeftEndLine < CameraHight - 1)
    {
      //只有左线成功，而且左线尾还那么远，真心没法继续。直接返回失败。
      //if(BlackLeftEndLine < CameraHight - 1 - BlackLeftBackLineStart)
      if(BlackLeftEndLine <= BlackControlLineLow)
      {
        flag2 = 0;
        return 0;       
      }
      //左线尾在一定范围内。可以允许把第59行往前移几行。直接取BlackLeftEndLine。
      else
      {
      }
    }
    //左线尾等于CameraHight - 1，直接取BlackLeftEndLine
    else
    {
    }
    
    //丢失另一根线的时候，先根据斜率估算出半宽，若没有上一次双线图像时的赛道半宽大，则直接用上一次双线的赛道半宽。
    //求5行的斜率。
    if(BlackLeftEndLine - BlackLeftHeadLine > 5)
    {
      slope = 1.0 * (BlackUdisLeftLocation[BlackLeftEndLine].x - BlackUdisLeftLocation[BlackLeftEndLine-4].x) / 
                    (BlackUdisLeftLocation[BlackLeftEndLine].y - BlackUdisLeftLocation[BlackLeftEndLine-4].y);
    }
    else
    {
      slope = 0;
    }
    //赛道半宽
    temp = (uint8)(1.0 * sqrt(slope * slope + 1) * RoadWidth / 2);
    //取最大值
    RoadHalfWidth = MaxRe(temp, RoadHalfWidth);
    
    //根据赛道宽度估计出中心线的线头。
    CenterLineLoc[BlackLeftEndLine] = BlackUdisLeftLocation[BlackLeftEndLine].x + RoadHalfWidth;
    
    tempflag1 = 0;
    tempway1 = 0;
    //根据左线的走势估计出中心线。中心线是个int16型矩阵，有正负。
    for(row = BlackLeftEndLine - 1; ;row--)
    {
      //左线有效时。根据左线的趋势补线。
      if(BlackLeftLoc[row][0] != MaxValUint8)
      {
        tempflag1 = 0;
        CenterLineLoc[row] = CenterLineLoc[row+1] + BlackUdisLeftLocation[row].x - BlackUdisLeftLocation[row+1].x;
      }
      //左线无效时。根据最后两行的中心线趋势补线。
      else
      {
        //当左线无效时，可以采用最后两行趋势补线，也可以根据最后CenterForwardLineNum行最小二乘法趋势补线。
        if(tempflag1 == 0)
        {
          //每幅图只进来一次，用来确定补线方式，以后就不用再进来了。
          tempflag1 = 1;
          //最后几行行数够多，可以采用最小二乘法的方式。
          if(row + CenterForwardLineNum <= CameraHight - 1)
          {
            tempway1 = 0;
            //进行最小二乘法系数的计算。
            for(i = 0; i < CenterForwardLineNum; i++)
            {
              x[i] = row + i + 1;
              y[i] = CenterLineLoc[row + i + 1];
            }
            a1 = LeastSquareInt16a1(x, y, CenterForwardLineNum);         //斜率计算。
            a0 = LeastSquareInt16a0(x, y, a1, CenterForwardLineNum);     //纵截距计算。
          }
          //最后几行的行数不够多，采用最后两行趋势补线的方式。
          else
          {
            tempway1 = 1;
          }
        }
        else
        {
        }
        
        //确认补线方式后，开始补线。
        if(tempway1 == 0)
        {
          //最小二乘法的方式。
          CenterLineLoc[row] = (a0 + a1 * row) / LeastSquareMulti;
        }
        else
        {
          //如果是最近2行。
          if(row == CameraHight -1  || row == CameraHight - 2)
          {
            CenterLineLoc[row] = BlackUdisLeftLocation[row].x + RoadHalfWidth;
          }
          //不是最近2行。
          else
          {
            //最后两行的方式。
            CenterLineLoc[row] = CenterLineLoc[row+1] + CenterLineLoc[row+1] - CenterLineLoc[row+2];            
          }
        }
      }
      
      //一直补到最高行。
      if(row == 0) break;
    }
    
    //对未处理的行的中心线置无效值。
    //底部置无效值。顶部不用处理。
    if(CenterEndLine != CameraHight - 1)
    {
      for(row = CameraHight - 1; ; row--)
      {
        if(row == CenterEndLine) break;       
        CenterLineLoc[row] = MaxValInt16;
      }
    }
    else
    {
    }
  }
  
  return 1;
}



//非特殊赛道的中心线提取程序
//中心线提取，进来前左右黑线均连续。
//采用原始的左右黑线，不使用矫正后的左右黑线。
uint8 CenterLineGetNormal(void)
{
    int black_row, i, j;
            uint8 temp;
            uint8 flag1, //左线头标志（远处）
                flag2, //左线尾标志（近处）
                flag3, //右线头标志
                flag4;  //右线尾标志
            uint8 tempflag1=1;
            uint8 tempway1=1;
            uint8 x[CenterForwardLineNum];   //中心线向前补线计算斜率时，x坐标，表示第几行。。
            int16 y[CenterForwardLineNum];   //中心线向前补线计算斜率时，y坐标，表示中心线位置。
            int32 a1 = 0, a0 = 0;    //中心线向前补线时，最小二乘法的斜率和纵截距。
            uint8 index;
            uint8 a[BlackBackLineNum];   //取多少个点在一开始有定义。
            uint8 b[BlackBackLineNum];


            float slope;


            //左线提取成功，右线提取失败。
            if (BlackLeftDone == 1 && BlackRightDone == 0)
            {
                //若右线的线头线尾无效，则直接返回失败。
                if (BlackLeftHeadLine == MaxValUint8 || BlackLeftEndLine == MaxValUint8)
                {
                    return 0;
                }
                else
                {
                }

                flag1 = 1;
                flag2 = 1;

                //中心线的线尾取左线的线尾即可。
                CenterHeadLine = BlackLeftHeadLine;   //线头取左线的线头，注意这不是实际的线头。
                                                       //CenterHeadLine = 0;                    //线头因为要固定补到第0行，所以直接置0.
                CenterEndLine = BlackLeftEndLine;

                //首先看线头是否大于BlackControlLineLow行
                if (BlackLeftHeadLine >= 55)
                {
                    flag1 = 0;
                }
                else
                {
                }

                //右线尾小于CameraHight - 1行。
                if (BlackLeftEndLine < CameraHight - 1)
                {
                    //只有右线成功，而且右线尾还那么远，真心没法继续。直接返回失败。
                    //if(BlackRightEndLine < CameraHight - 1 - BlackRightBackLineStart)
                    if (BlackLeftEndLine <= 5)
                    {
                        flag2 = 0;
                        return 0;
                    }
                    //右线尾在一定范围内。可以允许把第59行往前移几行。直接取BlackRightEndLine。
                    else
                    {
                    }
                }
                //右线尾等于CameraHight - 1，直接取BlackRightEndLine
                else
                {
                }

                //丢失另一根线的时候，先根据斜率估算出半宽，若没有上一次双线图像时的赛道半宽大，则直接用上一次双线的赛道半宽。
                //求5行的斜率。
                for (black_row = BlackLeftHeadLine; ; black_row++)
                {
                    if (BlackLeftEndLine - BlackLeftHeadLine > 5)
                    {
                        if (BlackLeftLoc[black_row][ 0] != MaxValUint8)
                        {
                            slope = 1.0 * (BlackUdisLeftLocation[black_row + 4].x - BlackUdisLeftLocation[black_row].x) /
                                  (BlackUdisLeftLocation[black_row + 4].y - BlackUdisLeftLocation[black_row].y);
                            /* else
                             {
                               slope = 0;
                             }*/
                            //赛道半宽
                            temp = CenterLineHalfWidth[black_row];// (int)(1.0 * Math.Sqrt(slope * slope + 1) * CenterLineHalfWidth[black_row] / 2);
                            //取最大值

                            RoadHalfWidth = temp;// MaxRe(temp, RoadHalfWidth);

                            tempflag1 = 0;
                            tempway1 = 0;
                            buXianCount = 0;
                            //根据赛道宽度估计出中心线的线头。
                            CenterLineLoc[black_row] = BlackLeftLoc[black_row][ 0] + RoadHalfWidth;
                        }
                        //根据右线的走势估计出中心线。中心线是个int16型矩阵，有正负。

                        // CenterLineLoc[black_row] = CenterLineLoc[black_row + 1] + BlackRightLoc[black_row, 0] - BlackRightLoc[black_row + 1, 0];
                        //右线无效时。根据最后两行的中心线趋势补线。
                        else
                        {
                            //当右线无效时，可以采用最后两行趋势补线，也可以根据最后CenterForwardLineNum行最小二乘法趋势补线。
                            if (tempflag1 == 0)
                            {
                                //每幅图只进来一次，用来确定补线方式，以后就不用再进来了。
                                tempflag1 = 1;
                                //最后几行行数够多，可以采用最小二乘法的方式。
                                //最后一行或两行尽量不用，因为有可能是拐点。但也不能太往后取，斜率就不对了。
                                if (black_row + CenterForwardLineNum + 2 <= CameraHight - 1)
                                {
                                    tempway1 = 0;
                                    //进行最小二乘法系数的计算。
                                    for (i = 0; i < CenterForwardLineNum; i++)
                                    {
                                        x[i] = black_row - i - 3;
                                        y[i] = CenterLineLoc[black_row - i - 3];
                                    }
                                    a1 = LeastSquareInt16a1(x, y, CenterForwardLineNum);         //斜率计算。
                                    a0 = LeastSquareInt16a0(x, y, a1, CenterForwardLineNum);     //纵截距计算。            
                                }
                                else if (black_row + CenterForwardLineNum + 1 <= CameraHight - 1)
                                {
                                    tempway1 = 0;
                                    //进行最小二乘法系数的计算。
                                    for (i = 0; i < CenterForwardLineNum; i++)
                                    {
                                        x[i] = black_row - i - 2;
                                        y[i] = CenterLineLoc[black_row - i - 2];
                                    }
                                    a1 = LeastSquareInt16a1(x, y, CenterForwardLineNum);         //斜率计算。
                                    a0 = LeastSquareInt16a0(x, y, a1, CenterForwardLineNum);     //纵截距计算。                        
                                }
                                else if (black_row + CenterForwardLineNum <= CameraHight - 1)
                                {
                                    tempway1 = 0;
                                    //进行最小二乘法系数的计算。
                                    for (i = 0; i < CenterForwardLineNum; i++)
                                    {
                                        x[i] = black_row - i - 1;
                                        y[i] = CenterLineLoc[black_row - i - 1];
                                    }
                                    a1 = LeastSquareInt16a1(x, y, CenterForwardLineNum);         //斜率计算。
                                    a0 = LeastSquareInt16a0(x, y, a1, CenterForwardLineNum);     //纵截距计算。
                                }
                                //最后几行的行数不够多，采用最后两行趋势补线的方式。
                                else
                                {
                                    tempway1 = 1;
                                }
                            }
                            else
                            {
                            }

                            //确认补线方式后，开始补线。
                            if (tempway1 == 0)
                            {
                                //最小二乘法的方式。
                                CenterLineLoc[black_row] = (a0 + a1 * black_row) / LeastSquareMulti;
                            }
                            else
                            {
                                /*//最近2行。
                                if(black_row == CameraHight - 1 || black_row == CameraHight - 2)
                                {
                                  CenterLineLoc[black_row] = BlackRightLoc[black_row,0] - RoadHalfWidth;
                                }
                                //不是最近2行。
                                else
                                {
                                  //最后两行的方式。
                                  CenterLineLoc[black_row] = CenterLineLoc[black_row+1] + CenterLineLoc[black_row+1] - CenterLineLoc[black_row+2];
                                }
                                */
                            }
                        }
                    }
                    else
                    {
                        tempflag1 = 1;
                        tempway1 = 1;
                        temp = CenterLineHalfWidth[black_row];// (int)(1.0 * CenterLineHalfWidth[black_row] / 2);
                        RoadHalfWidth = temp;// MaxRe(temp, RoadHalfWidth);
                        CenterLineLoc[black_row] = BlackLeftLoc[black_row][ 0] + RoadHalfWidth;

                    }
                    if (CenterLineLoc[black_row] >= MaxValUint8)
                        CenterLineLoc[black_row] = MaxValUint8;
                    else if (CenterLineLoc[black_row] <= 0)
                        CenterLineLoc[black_row] = 0;
                    else
                    {
                    }
                    if (black_row >= 59) break;
                }



            }
            //右线提取成功，左线提取失败。
            else if (BlackLeftDone == 0 && BlackRightDone == 1)
            {
                //若右线的线头线尾无效，则直接返回失败。
                if (BlackRightHeadLine == MaxValUint8 || BlackRightEndLine == MaxValUint8)
                {
                    return 0;
                }
                else
                {
                }

                flag1 = 1;
                flag2 = 1;

                //中心线的线尾取左线的线尾即可。
                CenterHeadLine = BlackRightHeadLine;   //线头取左线的线头，注意这不是实际的线头。
                                                       //CenterHeadLine = 0;                    //线头因为要固定补到第0行，所以直接置0.
                CenterEndLine = BlackRightEndLine;

                //首先看线头是否大于BlackControlLineLow行
                if (BlackRightHeadLine >= 55)
                {
                    flag1 = 0;
                }
                else
                {
                }

                //右线尾小于CameraHight - 1行。
                if (BlackRightEndLine < CameraHight - 1)
                {
                    //只有右线成功，而且右线尾还那么远，真心没法继续。直接返回失败。
                    //if(BlackRightEndLine < CameraHight - 1 - BlackRightBackLineStart)
                    if (BlackRightEndLine <= 5)
                    {
                        flag2 = 0;
                        return 0;
                    }
                    //右线尾在一定范围内。可以允许把第59行往前移几行。直接取BlackRightEndLine。
                    else
                    {
                    }
                }
                //右线尾等于CameraHight - 1，直接取BlackRightEndLine
                else
                {
                }

                //丢失另一根线的时候，先根据斜率估算出半宽，若没有上一次双线图像时的赛道半宽大，则直接用上一次双线的赛道半宽。
                //求5行的斜率。
                for (black_row = BlackRightHeadLine; ; black_row++)
                {
                    if (BlackRightEndLine - BlackRightHeadLine > 5)
                    {
                        if (BlackRightLoc[black_row][ 0] != MaxValUint8)
                        {
                            slope = 1.0 * (BlackUdisRightLocation[black_row + 4].x - BlackUdisRightLocation[black_row].x) /
                                  (BlackUdisRightLocation[black_row + 4].y - BlackUdisRightLocation[black_row].y);
                            /* else
                             {
                               slope = 0;
                             }*/
                            //赛道半宽
                            temp = CenterLineHalfWidth[black_row];// (int)(1.0 * Math.Sqrt(slope * slope + 1) * CenterLineHalfWidth[black_row] / 2);
                            //取最大值

                            RoadHalfWidth = temp;// MaxRe(temp, RoadHalfWidth);

                            tempflag1 = 0;
                            tempway1 = 0;
                            buXianCount = 0;
                            //根据赛道宽度估计出中心线的线头。
                            CenterLineLoc[black_row] = BlackRightLoc[black_row][ 0] - RoadHalfWidth;
                        }
                        //根据右线的走势估计出中心线。中心线是个int16型矩阵，有正负。

                        // CenterLineLoc[black_row] = CenterLineLoc[black_row + 1] + BlackRightLoc[black_row, 0] - BlackRightLoc[black_row + 1, 0];
                        //右线无效时。根据最后两行的中心线趋势补线。
                        else
                        {
                            //当右线无效时，可以采用最后两行趋势补线，也可以根据最后CenterForwardLineNum行最小二乘法趋势补线。
                            if (tempflag1 == 0)
                            {
                                //每幅图只进来一次，用来确定补线方式，以后就不用再进来了。
                                tempflag1 = 1;
                                //最后几行行数够多，可以采用最小二乘法的方式。
                                //最后一行或两行尽量不用，因为有可能是拐点。但也不能太往后取，斜率就不对了。
                                if (black_row + CenterForwardLineNum + 2 <= CameraHight - 1)
                                {
                                    tempway1 = 0;
                                    //进行最小二乘法系数的计算。
                                    for (i = 0; i < CenterForwardLineNum; i++)
                                    {
                                        x[i] = black_row - i - 3;
                                        y[i] = CenterLineLoc[black_row - i - 3];
                                    }
                                    a1 = LeastSquareInt16a1(x, y, CenterForwardLineNum);         //斜率计算。
                                    a0 = LeastSquareInt16a0(x, y, a1, CenterForwardLineNum);     //纵截距计算。            
                                }
                                else if (black_row + CenterForwardLineNum + 1 <= CameraHight - 1)
                                {
                                    tempway1 = 0;
                                    //进行最小二乘法系数的计算。
                                    for (i = 0; i < CenterForwardLineNum; i++)
                                    {
                                        x[i] = black_row - i - 2;
                                        y[i] = CenterLineLoc[black_row - i - 2];
                                    }
                                    a1 = LeastSquareInt16a1(x, y, CenterForwardLineNum);         //斜率计算。
                                    a0 = LeastSquareInt16a0(x, y, a1, CenterForwardLineNum);     //纵截距计算。                        
                                }
                                else if (black_row + CenterForwardLineNum <= CameraHight - 1)
                                {
                                    tempway1 = 0;
                                    //进行最小二乘法系数的计算。
                                    for (i = 0; i < CenterForwardLineNum; i++)
                                    {
                                        x[i] = black_row - i - 1;
                                        y[i] = CenterLineLoc[black_row - i - 1];
                                    }
                                    a1 = LeastSquareInt16a1(x, y, CenterForwardLineNum);         //斜率计算。
                                    a0 = LeastSquareInt16a0(x, y, a1, CenterForwardLineNum);     //纵截距计算。
                                }
                                //最后几行的行数不够多，采用最后两行趋势补线的方式。
                                else
                                {
                                    tempway1 = 1;
                                }
                            }
                            else
                            {
                            }

                            //确认补线方式后，开始补线。
                            if (tempway1 == 0)
                            {
                                //最小二乘法的方式。
                                CenterLineLoc[black_row] = (a0 + a1 * black_row) / LeastSquareMulti;
                            }
                            else
                            {
                                /*//最近2行。
                                if(black_row == CameraHight - 1 || black_row == CameraHight - 2)
                                {
                                  CenterLineLoc[black_row] = BlackRightLoc[black_row,0] - RoadHalfWidth;
                                }
                                //不是最近2行。
                                else
                                {
                                  //最后两行的方式。
                                  CenterLineLoc[black_row] = CenterLineLoc[black_row+1] + CenterLineLoc[black_row+1] - CenterLineLoc[black_row+2];
                                }
                                */
                            }
                        }
                    }
                    else
                    {
                        tempflag1 = 1;
                        tempway1 = 1;
                        temp = CenterLineHalfWidth[black_row];// (int)(1.0 * CenterLineHalfWidth[black_row] / 2);
                        RoadHalfWidth = temp;// MaxRe(temp, RoadHalfWidth);
                        CenterLineLoc[black_row] = BlackRightLoc[black_row][ 0] - RoadHalfWidth;

                    }
                    if (CenterLineLoc[black_row] >= MaxValUint8)
                        CenterLineLoc[black_row] = MaxValUint8;
                    else if (CenterLineLoc[black_row] <= 0)
                        CenterLineLoc[black_row] = 0;
                    else
                    {
                    }
                    if (black_row >= 59) break;
                }


            }



            //对未处理的行的中心线置无效值。
            //底部置无效值。顶部不用处理。
            /* if(CenterEndLine != CameraHight - 1)
             {
               for(black_row = CameraHight - 1; ; black_row--)
               {
                 if(black_row == CenterEndLine) break;       
                 CenterLineLoc[black_row] = MaxValInt16;
               }
             }
             else
             {
             }*/

       

  //左右线均提取成功。
 else if(BlackLeftDone == 1 && BlackRightDone == 1)
  {
    CenterHeadLine = MinRe(BlackLeftHeadLine, BlackRightHeadLine);    //线头取两者较小者。     
    //CenterHeadLine = 0;    //因为固定要补到第0行，所以固定取0.
    //线尾取两者较大者。
    CenterEndLine = MinRe(BlackLeftEndLine, BlackRightEndLine);             
    
    //不是单线的情况。
    if(SingleBlackAllFlag == 0)
    {
    
      //左右线均提取成功，首选根据两线得中心线，实在不行再根据赛道宽度估计。
      //不根据趋势来预测，因为畸变矫正基本上已经把左右线差搞成定值了，所以不用那么麻烦。    
      
      //先检查左线，左线不满足条件再检测右线。右线不满足再返回失败。
      flag1 = 1;
      flag2 = 1;
      flag3 = 1;
      flag4 = 1;
      //取左右两线的第39到第59行的差，取最大值。
      //首先看线头是否大于39行，大于39行就直接返回失败。
      if(BlackLeftHeadLine >= 55)
      {
        flag1 = 0;
      }
      else
      {
      }
      //左线尾小于CameraHight - 1行。
      if(BlackLeftEndLine < CameraHight - 1)
      {
        //只有左线成功，而且左线尾还那么远，真心没法继续。看右线是不是一样的情况。
        //if(BlackLeftEndLine < CameraHight - 1 - BlackLeftBackLineStart)
        if(BlackLeftEndLine <= CameraHight - BlackStep1Size - 1)
        {
          flag2 = 0;
        }
        //左线尾在一定范围内。可以允许把第59行往前移几行。直接取BlackLeftEndLine。
        else
        {
        }
      }
      //左线尾等于CameraHight - 1，直接取BlackLeftEndLine
      else
      {
      }
  
      //处理右线，右线不满足条件，则直接返回失败。
      //首先看线头是否大于39行，大于39行就直接返回失败。
      if(BlackRightHeadLine >= 55)
      {
        flag3 = 0;
      }
      else
      {
      }    
      //右线尾小于CameraHight - 1行。
      if(BlackRightEndLine < CameraHight - 1)
      {
        //只有右线成功，而且右线尾还那么远，真心没法继续。直接返回失败。
        //if(BlackRightEndLine < CameraHight - 1 - BlackRightBackLineStart)
        if(BlackRightEndLine <= CameraHight - BlackStep1Size - 1)
        {
          flag4 = 0;
          //两次都线尾很远，直接返回失败。
          if(flag2 == 0) 
          {
            return 0;
          }
          else
          {
          }
        }
        //右线尾在一定范围内。可以允许把第59行往前移几行。直接取BlackRightEndLine。
        else
        {
        }
      }
      //右线尾等于CameraHight - 1，直接取BlackRightEndLine
      else
      {
      }
      
      //只有右线满足要求，则直接取右线的列差值绝对值。
      if(
        (flag3 == 1 && flag4 == 1)
      &&((flag1 == 0 && flag2 == 0) || (flag1 == 1 && flag2 == 0) || (flag1 == 0 && flag2 == 1))
        )
      {
        //斜率的方式估算出赛道半宽。
        //求5行的斜率。
        if(BlackRightEndLine - BlackRightHeadLine > 5)
        {
          slope = 1.0 * (BlackUdisRightLocation[BlackRightEndLine].x - BlackUdisRightLocation[BlackRightEndLine-4].x) / 
                    (BlackUdisRightLocation[BlackRightEndLine].y - BlackUdisRightLocation[BlackRightEndLine-4].y);
        }
        else
        {
          slope = 0;
        }
        //赛道半宽
        temp = (uint8)(1.0 * sqrt(slope * slope + 1) * RoadWidth / 2);
        RoadHalfWidthCompen = RoadHalfWidth / temp;
        if(RoadHalfWidthSwitch == 1)
        {
          RoadHalfWidth = MaxRe(temp, RoadHalfWidth);
        }
        else
        {
        }
      }
      //只有左线满足要求，则直接取左线的列差值绝对值。
      else if(
             (flag1 == 1 && flag2 == 1)
           &&((flag3 == 0 && flag4 == 0) || (flag3 == 1 && flag4 == 0) || (flag3 == 0 && flag4 == 1))
             )
      {
        //斜率的方式估算出赛道半宽。
        //求5行的斜率。
        if(BlackLeftEndLine - BlackLeftHeadLine > 5)
        {
          slope = 1.0 * (BlackUdisLeftLocation[BlackLeftEndLine].x- BlackUdisLeftLocation[BlackLeftEndLine-4].x) / 
                    (BlackUdisLeftLocation[BlackLeftEndLine].y - BlackUdisLeftLocation[BlackLeftEndLine-4].y);
        }
        else
        {
          slope = 0;
        }
        //赛道半宽
        temp = (uint8)(1.0 * sqrt(slope * slope + 1) * RoadWidth / 2);
        RoadHalfWidthCompen = RoadHalfWidth / temp;
        if(RoadHalfWidthSwitch == 1)
        {
          RoadHalfWidth = MaxRe(temp, RoadHalfWidth);
        }
        else
        {
        }
      }
      //左右线均满足条件。则取列差值绝对值较大的。
      else if(flag1 == 1 && flag2 == 1 && flag3 == 1 && flag4 == 1)
      {
        //斜率的方式估算出赛道半宽。
        //求5行的斜率。
        if(BlackLeftEndLine - BlackLeftHeadLine > 5)
        {
          slope = 1.0 * (BlackUdisLeftLocation[BlackLeftEndLine].x - BlackUdisLeftLocation[BlackLeftEndLine-4].x) / 
                    (BlackUdisLeftLocation[BlackLeftEndLine].y - BlackUdisLeftLocation[BlackLeftEndLine-4].y);
        }
        else
        {
          slope = 0;
        }
        //赛道半宽
        temp = (uint8)(1.0 * sqrt(slope * slope + 1) * RoadWidth / 2);
        
        //求5行的斜率。
        if(BlackRightEndLine - BlackRightHeadLine > 5)
        {
          slope = 1.0 * (BlackUdisRightLocation[BlackRightEndLine].x - BlackUdisRightLocation[BlackRightEndLine-4].x) / 
                    (BlackUdisRightLocation[BlackRightEndLine].y - BlackUdisRightLocation[BlackRightEndLine-4].y);
        }
        else
        {
          slope = 0;
        }
        //赛道半宽
        temp = MaxRe((uint8)(1.0 * sqrt(slope * slope + 1) * RoadWidth / 2), temp);
        RoadHalfWidthCompen = RoadHalfWidth / temp;
        if(RoadHalfWidthSwitch == 1)
        {
          RoadHalfWidth = MaxRe(temp, RoadHalfWidth);
        }
        else
        {
        }
      }
      //左右线均不满足要求。
      else
      {
        //左右线都在第BlackControlLineLow行的远方。直接返回失败。
        if(flag2 == 0 && flag4 == 0)
        {
          return 0;
        }
        else
        {
          //左线在第BlackControlLineLow行的近方，右线在第BlackControlLineLow行的远方，则用左线来求。
          if(flag1 == 1 && flag3 == 0)
          {
            //斜率的方式估算出赛道半宽。
            //求5行的斜率。
            if(BlackLeftEndLine - BlackLeftHeadLine > 5)
            {
              slope = 1.0 * (BlackUdisLeftLocation[BlackLeftEndLine].x - BlackUdisLeftLocation[BlackLeftEndLine-4].x) / 
                    (BlackUdisLeftLocation[BlackLeftEndLine].y - BlackUdisLeftLocation[BlackLeftEndLine-4].y);
            }
            else
            {
              slope = 0;
            }
            //赛道半宽
            temp = (uint8)(1.0 * sqrt(slope * slope + 1) * RoadWidth / 2);
            RoadHalfWidthCompen = RoadHalfWidth / temp;
            if(RoadHalfWidthSwitch == 1)
            {
              RoadHalfWidth = MaxRe(temp, RoadHalfWidth);
            }
            else
            {
            }
          }
          //右线在第BlackControlLineLow行的近方，左线在第BlackControlLineLow行的远方，则用右线来求。
          else if(flag1 == 0 && flag3 == 1)
          {
            //斜率的方式估算出赛道半宽。
            //求5行的斜率。
            if(BlackRightEndLine - BlackRightHeadLine > 5)
            {
              slope = 1.0 * (BlackUdisRightLocation[BlackRightEndLine].x - BlackUdisRightLocation[BlackRightEndLine-4].x) / 
                    (BlackUdisRightLocation[BlackRightEndLine].y - BlackUdisRightLocation[BlackRightEndLine-4].y);
            }
            else
            {
              slope = 0;
            }
            //赛道半宽
            temp = (uint8)(1.0 * sqrt(slope * slope + 1) * RoadWidth / 2);
            RoadHalfWidthCompen = RoadHalfWidth / temp;
            if(RoadHalfWidthSwitch == 1)
            {
              RoadHalfWidth = MaxRe(temp, RoadHalfWidth);
            }
            else
            {
            }
          }
          //左右线都在第行的近方，则取较大值。
          else if(flag1 == 0 && flag3 == 0)
          {
            //斜率的方式估算出赛道半宽。
            //求5行的斜率。
            if(BlackLeftEndLine - BlackLeftHeadLine > 5)
            {
              slope = 1.0 * (BlackUdisLeftLocation[BlackLeftEndLine].x - BlackUdisLeftLocation[BlackLeftEndLine-4].x) / 
                    (BlackUdisLeftLocation[BlackLeftEndLine].y - BlackUdisLeftLocation[BlackLeftEndLine-4].y);
            }
            else
            {
              slope = 0;
            }
            //赛道半宽
            temp = (uint8)(1.0 * sqrt(slope * slope + 1) * RoadWidth / 2);
            
            //求5行的斜率。
            if(BlackRightEndLine - BlackRightHeadLine > 5)
            {
              slope = 1.0 * (BlackUdisRightLocation[BlackRightEndLine].x - BlackUdisRightLocation[BlackRightEndLine-4].x) / 
                    (BlackUdisRightLocation[BlackRightEndLine].y - BlackUdisRightLocation[BlackRightEndLine-4].y);
            }
            else
            {
              slope = 0;
            }
            //赛道半宽
            temp = MaxRe((uint8)(1.0 * sqrt(slope * slope + 1) * RoadWidth / 2), temp);
            RoadHalfWidthCompen = RoadHalfWidth / temp;
            if(RoadHalfWidthSwitch == 1)
            {
              RoadHalfWidth = MaxRe(temp, RoadHalfWidth);
            }
            else
            {
            }
          }
          //理论上不可能走到这里。
          else
          {
            return 0;
          }
        }
      }
      
      int temproad;  //本幅图的最后一个双线有效路宽。给本幅图的单线部分用。
      temproad = RoadHalfWidth;
      
      tempflag1 = 0;
      tempway1 = 0;
    buXianCount = 0;

                    //赛道宽度已经求得，现在开始从高到低求中心线。
                        for (black_row = CenterHeadLine; black_row <= CameraHight - 1; black_row++)
                        {
                            //当前行左右黑线均有效。检测有无效不能用Udis数组，只能用原始的黑线储存数组。
                            if (BlackLeftLoc[black_row][ 0] != MaxValUint8 && BlackRightLoc[black_row][ 0] != MaxValUint8)
                            {
                                tempflag1 = 0;
                                //取左右黑线的平均值。
                                CenterLineLoc[black_row] = (BlackLeftLoc[black_row][ 0] + BlackRightLoc[black_row][ 0]) / 2;
                                //取最后一行两行有效的行的赛道半宽，作为最新的赛道半宽。
                                /*  

                                  //本幅图的最后一个双线有效路宽。给本幅图的单线部分用。
                                  temproad = RoadHalfWidthRecord[black_row];*/
                                //赛道半宽记录.存第CameraHight - 1行的赛道半宽，给下一幅图用。
                                //怕出意外，限个幅。
                                /* if(black_row == CameraHight - 1) 
                                 {  
                                   if(RoadHalfWidthRecord[black_row] > RoadHalfWidthLimitMax)
                                   {
                                     RoadHalfWidth = RoadHalfWidthLimitMax;
                                   }
                                   else if(RoadHalfWidthRecord[black_row] < RoadHalfWidthLimitMin)
                                   {
                                     RoadHalfWidth = RoadHalfWidthLimitMin;
                                   }
                                   else
                                   {
                                     RoadHalfWidth = RoadHalfWidthRecord[black_row];
                                   }            
                                 }
                                 else
                                 {
                                 } */
                            }
                            //左行或右行无效。则用上半段的中心线向后补线。
                            else if (BlackLeftLoc[black_row][ 0] == MaxValUint8 || BlackRightLoc[black_row][ 0] == MaxValUint8)
                            {

                                if (BlackLeftCrossDone == 2 || BlackRightCrossDone == 2)
                                {
                                    buXianCount--;
                                }
                                else if (BlackRightHeadLine >= 25 && BlackLeftHeadLine >= 25) { buXianCount = 0; }
                                else if (BlackLeftHeadLine <= 20 && BlackRightHeadLine >= 50) { buXianCount = 0; }
                                else if ((CURVEL < 0 && CURVER > 0) || (CURVEL > 0 && CURVER < 0)) { buXianCount = 0; }
                                else if (BlackLeftHeadLine <= 5) { buXianCount = 0; }
                                else if (BlackLeftHeadLine <= 20 && BlackRightHeadLine >= 45) { buXianCount = 0; }
                                else if (((AbsInt(CURVEL) >= 20) && (AbsInt(CURVEL) <= 50)) || ((AbsInt(CURVER) >= 20) && (AbsInt(CURVER) <= 50)))
                                {
                                    buXianCount = 0;
                                }
                                else if (((AbsInt(CURVEL) > 50) && (AbsInt(CURVEL) <= 2000)) || ((AbsInt(CURVER) > 50) && (AbsInt(CURVER) <= 2000)))
                                {
                                    buXianCount = 0;
                                }
                                else if ((AbsInt(CURVEL) > 2000) || (AbsInt(CURVER) > 2000))
                                {
                                    buXianCount = 0;
                                }


                                tempflag1 = 0;
                                index = 0;
                                for (i = black_row - 1;i>=0 ; i--)
                                {
                                    if (CenterLineLoc[i] != MaxValUint8)
                                    {
                                        b[index] = CenterLineLoc[i];
                                        a[index] = i;
                                        index++;
                                    }
                                    if (index == BlackBackLineNum)
                                    {

                                        break;
                                    }

                                }

                                //最小二乘法
                                //y = a0 + a1 * x
                                a1 = LeastSquarea1(a, b, BlackBackLineNum);

                                a0 = LeastSquarea0(a, b, a1, BlackBackLineNum);


                                //补线也有个度，向左向右都不能补出范围。
                                temp = (a0 + a1 * black_row) / LeastSquareMulti;
                                if (temp >= CameraRealWidth - 1)
                                {
                                    //BlackLeftLoc[black_row,0] = CameraRealWidth - 1;
                                    //超过左右界限时，原策略为置边界值，现在的策略是从该行起置无效值。

                                    temp = CameraRealWidth - 1;
                                }
                                else if (temp <= 0)
                                {
                                    //BlackLeftLoc[black_row,0] = 0;
                                    //超过左右界限时，原策略为置边界值，现在的策略是从该行起置无效值。
                                    temp = 0;
                                }
                                CenterLineLoc[black_row] = (int)(temp);
                                //  CenterLineLoc[black_row] = BlackLeftLoc[black_row,0] + temproad + buXianCount;
                            }
                            //左行无效，右行有效。则用右行减去一个赛道半宽。
                            /*else if(BlackLeftLoc[black_row,0] == MaxValUint8 && BlackRightLoc[black_row,0] != MaxValUint8)
                            {
                                if (BlackLeftCrossDone == 2 || BlackRightCrossDone == 2)
                                {
                                    buXianCount--;
                                }
                                else if (BlackRightHeadLine >= 25 && BlackLeftHeadLine >= 25) { buXianCount = 0; }
                                else if (BlackRightHeadLine <= 20 && BlackLeftHeadLine >= 50) { buXianCount = 0; }
                                else if ((CURVEL < 0 && CURVER > 0) || (CURVEL > 0 && CURVER < 0)) { buXianCount = 0; }
                                else if (BlackRightHeadLine <= 5) { buXianCount = 0; }
                                else if (BlackRightHeadLine <= 20 && BlackLeftHeadLine >= 45) { buXianCount = 0; }
                                else if (((AbsInt(CURVEL) >= 20) && (AbsInt(CURVEL) <= 50)) || ((AbsInt(CURVER) >= 20) && (AbsInt(CURVER) <= 50)))
                                {
                                    buXianCount=0;
                                }
                                else if (((AbsInt(CURVEL) > 50) && (AbsInt(CURVEL) <= 2000)) || ((AbsInt(CURVER) > 50) && (AbsInt(CURVER) <= 2000)))
                                {
                                    buXianCount =0;
                                }
                                else if ((AbsInt(CURVEL) > 2000) ||(AbsInt(CURVER) > 2000))
                                {
                                    buXianCount =0;
                                }


                                tempflag1 = 0;
                              CenterLineLoc[black_row] = BlackRightLoc[black_row,0] - temproad - buXianCount;
                            }
                            //左右行均无效。则用最后的中心线均势往前走。
                            else if(BlackLeftLoc[black_row,0] == MaxValUint8 && BlackRightLoc[black_row,0] == MaxValUint8)
                            {
                              //当左右线均无效时，可以采用最后两行趋势补线，也可以根据最后CenterForwardLineNum行最小二乘法趋势补线。
                              if(tempflag1 == 0)
                              {
                                //每幅图只进来一次，用来确定补线方式，以后就不用再进来了。
                                //除非又出现左或右黑线有效的情况，则再次刷新斜率。
                                tempflag1 = 1;
                                //最后几行行数够多，可以采用最小二乘法的方式。
                                //最后一行或两行尽量不用，因为有可能是拐点。但也不能太往后取，斜率就不对了。
                                if(black_row + CenterForwardLineNum + 2 <= CameraHight - 1)
                                {
                                  tempway1 = 0;
                                  //进行最小二乘法系数的计算。
                                  for(i = 0; i < CenterForwardLineNum; i++)
                                  {
                                    x[i] = black_row + i + 3;
                                    y[i] = CenterLineLoc[black_row + i + 3];
                                  }
                                  a1 = LeastSquareInt16a1(x, y, CenterForwardLineNum);         //斜率计算。
                                  a0 = LeastSquareInt16a0(x, y, a1, CenterForwardLineNum);     //纵截距计算。            
                                }
                                else if(black_row + CenterForwardLineNum + 1 <= CameraHight - 1)
                                {
                                  tempway1 = 0;
                                  //进行最小二乘法系数的计算。
                                  for(i = 0; i < CenterForwardLineNum; i++)
                                  {
                                    x[i] = black_row + i + 2;
                                    y[i] = CenterLineLoc[black_row + i + 2];
                                  }
                                  a1 = LeastSquareInt16a1(x, y, CenterForwardLineNum);         //斜率计算。
                                  a0 = LeastSquareInt16a0(x, y, a1, CenterForwardLineNum);     //纵截距计算。                        
                                }
                                else if(black_row + CenterForwardLineNum <= CameraHight - 1)
                                {
                                  tempway1 = 0;
                                  //进行最小二乘法系数的计算。
                                  for(i = 0; i < CenterForwardLineNum; i++)
                                  {
                                    x[i] = black_row + i + 1;
                                    y[i] = CenterLineLoc[black_row + i + 1];
                                  }
                                  a1 = LeastSquareInt16a1(x, y, CenterForwardLineNum);         //斜率计算。
                                  a0 = LeastSquareInt16a0(x, y, a1, CenterForwardLineNum);     //纵截距计算。
                                }
                                //最后几行的行数不够多，采用最后两行趋势补线的方式。
                                else
                                {
                                  tempway1 = 1;
                                }
                              }
                              else
                              {
                              }

                              //确认补线方式后，开始补线。
                              if(tempway1 == 0)
                              {
                                //最小二乘法的方式。
                                CenterLineLoc[black_row] = (a0 + a1 * black_row) / LeastSquareMulti;
                              }
                              else
                              {
                                //最后两行的方式。
                                CenterLineLoc[black_row] = CenterLineLoc[black_row+1] + CenterLineLoc[black_row+1] - CenterLineLoc[black_row+2];
                              }

                            }*/
                            //理论上不会走到这里。
                            else
                            {
                            }

                        }
                    
      //对未处理的行的中心线置无效值。对顶部的不作处理。
      //底部置无效值。
      /* if(CenterEndLine != CameraHight - 1)
       {
         for(black_row = CameraHight - 1; ; black_row--)
         {
           if(black_row == CenterEndLine) break;       
           CenterLineLoc[black_row] = MaxValInt16;
         }
       }
       else
       {
       }      */
    }
    //单线的情况，也就是SingleBlackAllFlag==1的情况。
    else
    {
      tempflag1 = 0;
      tempway1 = 0;
      
      //赛道宽度已经求得，现在开始从低到高求中心线。
      for(black_row = CenterEndLine; ; black_row--)
      {
        //当前行左右黑线均有效。检测有无效不能用Udis数组，只能用原始的黑线储存数组。
        if(BlackLeftLoc[black_row][0] != MaxValUint8 && BlackRightLoc[black_row][0] != MaxValUint8)
        {
          tempflag1 = 0;
          //取左右黑线的平均值。
          CenterLineLoc[black_row] = (BlackLeftLoc[black_row][0] + BlackRightLoc[black_row][0]) / 2;
          //取最后一行两行有效的行的赛道半宽，作为最新的赛道半宽。
          if(BlackLeftLoc[black_row,0] > BlackRightLoc[black_row,0])
          {
            RoadHalfWidthSingle = (BlackLeftLoc[black_row,0] - BlackRightLoc[black_row,0]) / 2;
          }
          else
          {
            RoadHalfWidthSingle = (BlackRightLoc[black_row,0] - BlackLeftLoc[black_row,0]) / 2;
          }        
        }
        //左行有效，右行无效。则用左行加上一个赛道半宽。
        else if(BlackLeftLoc[black_row][0] != MaxValUint8 && BlackRightLoc[black_row][0] == MaxValUint8)
        {
          tempflag1 = 0;
          CenterLineLoc[black_row] = BlackLeftLoc[black_row][0] - RoadHalfWidthSingle;    //因为是单线，所以是减号。
        }
        //左行无效，右行有效。则用右行减去一个赛道半宽。
        else if(BlackLeftLoc[black_row][0] == MaxValUint8 && BlackRightLoc[black_row][0] != MaxValUint8)
        {
          tempflag1 = 0;
          CenterLineLoc[black_row] = BlackRightLoc[black_row][0] + RoadHalfWidthSingle;   //因为是单线，所以是加号。
        }
        //左右行均无效。则用最后的中心线均势往前走。
        else if(BlackLeftLoc[black_row][0] == MaxValUint8 && BlackRightLoc[black_row][0] == MaxValUint8)
        {
          //当左右线均无效时，可以采用最后两行趋势补线，也可以根据最后CenterForwardLineNum行最小二乘法趋势补线。
          if(tempflag1 == 0)
          {
            //每幅图只进来一次，用来确定补线方式，以后就不用再进来了。
            //除非又出现左或右黑线有效的情况，则再次刷新斜率。
            tempflag1 = 1;
            //最后几行行数够多，可以采用最小二乘法的方式。
            //最后一行或两行尽量不用，因为有可能是拐点。但也不能太往后取，斜率就不对了。
            if(black_row + CenterForwardLineNum + 2 <= CameraHight - 1)
            {
              tempway1 = 0;
              //进行最小二乘法系数的计算。
              for(i = 0; i < CenterForwardLineNum; i++)
              {
                x[i] = black_row + i + 3;
                y[i] = CenterLineLoc[black_row + i + 3];
              }
              a1 = LeastSquareInt16a1(x, y, CenterForwardLineNum);         //斜率计算。
              a0 = LeastSquareInt16a0(x, y, a1, CenterForwardLineNum);     //纵截距计算。            
            }
            else if(black_row + CenterForwardLineNum + 1 <= CameraHight - 1)
            {
              tempway1 = 0;
              //进行最小二乘法系数的计算。
              for(i = 0; i < CenterForwardLineNum; i++)
              {
                x[i] = black_row + i + 2;
                y[i] = CenterLineLoc[black_row + i + 2];
              }
              a1 = LeastSquareInt16a1(x, y, CenterForwardLineNum);         //斜率计算。
              a0 = LeastSquareInt16a0(x, y, a1, CenterForwardLineNum);     //纵截距计算。                        
            }
            else if(black_row + CenterForwardLineNum <= CameraHight - 1)
            {
              tempway1 = 0;
              //进行最小二乘法系数的计算。
              for(i = 0; i < CenterForwardLineNum; i++)
              {
                x[i] = black_row + i + 1;
                y[i] = CenterLineLoc[black_row + i + 1];
              }
              a1 = LeastSquareInt16a1(x, y, CenterForwardLineNum);         //斜率计算。
              a0 = LeastSquareInt16a0(x, y, a1, CenterForwardLineNum);     //纵截距计算。
            }
            //最后几行的行数不够多，采用最后两行趋势补线的方式。
            else
            {
              tempway1 = 1;
            }
          }
          else
          {
          }
          
          //确认补线方式后，开始补线。
          if(tempway1 == 0)
          {
            //最小二乘法的方式。
            CenterLineLoc[black_row] = (a0 + a1 * black_row) / LeastSquareMulti;
          }
          else
          {
            //最后两行的方式。
            CenterLineLoc[black_row] = CenterLineLoc[black_row+1] + CenterLineLoc[black_row+1] - CenterLineLoc[black_row+2];
          }
          
        }
        //理论上不会走到这里。
        else
        {
        }
        
        if(black_row == 0) break;
      }
         
      //对未处理的行的中心线置无效值。对顶部的不作处理。
      //底部置无效值。
      if(CenterEndLine != CameraHight - 1)
      {
        for(black_row = CameraHight - 1; ; black_row--)
        {
          if(black_row == CenterEndLine) break;       
          CenterLineLoc[black_row] = MaxValInt16;
        }
      }
      else
      {
      }
    }
  }
  //理论上不会走到这里。
  else
  {
    //左右黑线均提取失败，返回0.
    return 0;
  }
       /*     if (BlackLeftLoc[CenterEndLine, 0] > BlackRightLoc[CenterEndLine, 0])
            {
                RoadHalfWidth = (BlackLeftLoc[CenterEndLine, 0] - BlackRightLoc[CenterEndLine, 0]) / 2;
            }
            else
            {
                RoadHalfWidth = (BlackRightLoc[CenterEndLine, 0] - BlackLeftLoc[CenterEndLine, 0]) / 2;
            }
            if (RoadHalfWidth > RoadHalfWidthLimitMax)
            {
                RoadHalfWidth = RoadHalfWidthLimitMax;
            }
            else if (RoadHalfWidth < RoadHalfWidthLimitMin)
            {
                RoadHalfWidth = RoadHalfWidthLimitMin;
            }
            else
            {
            }*/
            return 1;
}



//障碍，锁存后，用以确定是否开始特殊控制。返回1开始，0再等等
uint8 BrickSpeDealStartJudge(void)
{
  uint8 col, count1, i, index;
  int32 a0, a1;
  uint8 x[BrickSlopeRowNum];   //取多少个点在一开始有定义。
  uint8 y[BrickSlopeRowNum];
  
  return 1;   ///////////////不等待，直接特殊控制。
  
  //通过判断障碍是不是向近处越过了某一行，从而判断是否开始特殊控制。
  
  //左障碍
  if(BrickSpeDealDir == 0)
  {
    //最小二乘法计算扫描起始行和结束行的黑点所在列。
    //取BrickSlopeRowNum个点作为最小二乘法的基准点。
    //以左上角为原点，向下为x轴正方向，向右为y轴正方向
    index = 0;
    for(i = CameraHight - 1; ; i--)
    {
      if(BlackLeftLoc[i][0] != MaxValUint8)
      {
        y[index] = BlackLeftLoc[i][0];
        x[index] = i;
        index++;
      }
      if(index == BrickSlopeRowNum)
      {
        break;
      }
      if(i == BrickSpeDealStartJudgeRow) //CameraHight-BrickSpeDealStartJudgeRow必须比BrickSlopeRowNum大
      {
        return 0;
      }
    }
    //最小二乘法
    //y = a0 + a1 * x（整体扩大了LeastSquareMulti倍）
    a1 = LeastSquarea1(x, y, BrickSlopeRowNum);
    a0 = LeastSquarea0(x, y, a1, BrickSlopeRowNum);
   
    //对特定一行的处理。
    //该行黑点计数，左障碍从右往左扫描。
    count1 = 0;
    //当前行的扫描左边界。
    BrickSpeDealStartJudgeCol = (a0 + a1 * BrickSpeDealStartJudgeRow) / LeastSquareMulti;
    //向右BrickScanColNum列为右边界
    for(col = BrickSpeDealStartJudgeCol + BrickScanColNum; ; col--)
    {
      if(ImgNew[BrickSpeDealStartJudgeRow][col] < LimitLeftB)  //够黑
      {
        count1++;
      }
      else
      {
      }
      //向左扫描至左边界。
      if(col <= BrickSpeDealStartJudgeCol || col == 0) break;
    }
    
    //该行黑点个数大于BrickLineBlackCountNum个时，则行计数加1.
    if(count1 > BrickLineBlackCountNum) 
    {
      return 1;
    }
    else
    {
    }
  }
  //右障碍
  else
  {
    //最小二乘法计算扫描起始行和结束行的黑点所在列。
    //取BrickSlopeRowNum个点作为最小二乘法的基准点。
    //以左上角为原点，向下为x轴正方向，向右为y轴正方向
    index = 0;
    for(i = CameraHight - 1; ; i--)
    {
      if(BlackRightLoc[i][0] != MaxValUint8)
      {
        y[index] = BlackRightLoc[i][0];
        x[index] = i;
        index++;
      }
      if(index == BrickSlopeRowNum)
      {
        break;
      }
      if(i == BrickSpeDealStartJudgeRow) //CameraHight-BrickSpeDealStartJudgeRow必须比BrickSlopeRowNum大
      {
        return 0;
      }
    }
    //最小二乘法
    //y = a0 + a1 * x（整体扩大了LeastSquareMulti倍）
    a1 = LeastSquarea1(x, y, BrickSlopeRowNum);
    a0 = LeastSquarea0(x, y, a1, BrickSlopeRowNum);
   
    //对特定一行的处理。
    //该行黑点计数，右障碍从左往右扫描。
    count1 = 0;
    //当前行的扫描左边界。
    BrickSpeDealStartJudgeCol = (a0 + a1 * BrickSpeDealStartJudgeRow) / LeastSquareMulti;
    //向左BrickScanColNum列为左边界
    for(col = BrickSpeDealStartJudgeCol - BrickScanColNum; ; col++)
    {
      if(ImgNew[BrickSpeDealStartJudgeRow][col] < LimitRightB)  //够黑
      {
        count1++;
      }
      else
      {
      }
      //向右扫描至右边界。
      if(col >= BrickSpeDealStartJudgeCol || col >= CameraRealWidth - 1) break;
    }
    
    //该行黑点个数大于BrickLineBlackCountNum个时，则行计数加1.
    if(count1 > BrickLineBlackCountNum) 
    {
      return 1;
    }
    else
    {
    }
  }
  
  //常态为再等等。
  return 0;
}



//障碍，特殊控制时，用以确定是否退出特殊控制。
//改变BrickSpeDealExitJudgeFlag，1~2继续特殊控制，其中1表示初始态，2表示当前已经与障碍平行，0退出特殊控制，把控制权交还给普通控制。
uint8 BrickSpeDealExitJudge(void)
{
  return 1;   //////////////不改变退出标志位，只通过倒计时来控制特殊控制的次数。
  
  
  //根据第CameraHight - 1行的赛道半宽来，先必须检测到小于65的样子，然后必须检测到大于75（不是65，有返回系数的意思）的样子。
  //左右障碍不用区分，没有必要。

  //当前为初始态，需要判断是不是已经跟障碍物平行了。
  if(BrickSpeDealExitJudgeFlag == 1)
  {
    if(
       (BlackLeftLoc[CameraHight - 1][0] != MaxValUint8)
    && (BlackRightLoc[CameraHight - 1][0] != MaxValUint8)
    && (BlackRightLoc[CameraHight - 1][0] > BlackLeftLoc[CameraHight - 1][0]) //两线有效
    && (BlackRightLoc[CameraHight - 1][0] - BlackLeftLoc[CameraHight - 1][0] < BrickSpeDealExitState1Val * 2) //赛道半宽小于一定值
      )
    {
      //符合所有条件，过渡到状态2，表示已经与障碍物平行。
      BrickSpeDealExitJudgeFlag = 2;
    }
    else
    {
    }
  }
  //当前已经与障碍物平行了，需要判断是不是车前端已经越过了障碍物从而退出特殊控制。
  else if(BrickSpeDealExitJudgeFlag == 2)
  {
    if(
       (BlackLeftLoc[CameraHight - 1][0] != MaxValUint8)
    && (BlackRightLoc[CameraHight - 1][0] != MaxValUint8)
    && (BlackRightLoc[CameraHight - 1][0] > BlackLeftLoc[CameraHight - 1][0]) //两线有效
    && (BlackRightLoc[CameraHight - 1][0] - BlackLeftLoc[CameraHight - 1][0] > BrickSpeDealExitState2Val * 2) //赛道半宽大于一定值
      )
    {
      //符合所有条件，过渡到状态0，表示车前端已经一定程序越过障碍物，退出特殊控制。
      BrickSpeDealExitJudgeFlag = 0;
    }
    else
    {
    }
  }
  //理论上不会到这里。
  else
  {
  }

  return 1;
}



//障碍物对舵机的特殊处理。///////////////////////////不知道是不是鸡肋
uint8 BrickSpeDealServor(void)
{  
  //舵机打角限幅。
  //左障碍，不能往右打太多。
  if(BrickSpeDealFlag == 1 && BrickSpeDealCenFlag == 0 && BrickSpeDealDir == 0)
  {
    //强制左转，侧撞总比出线好。
    ServoPIDInputVal = ServoPIDCen - 100;
    //ServoPIDInputVal = ServoPIDCen - 0;
    //StopFlag = 1;
  }
  //右障碍，不能往左打太多。
  else if(BrickSpeDealFlag == 1 && BrickSpeDealCenFlag == 0 && BrickSpeDealDir == 1)
  {
    //强制右转，侧撞总比出线好。
    ServoPIDInputVal = ServoPIDCen + 100;  
    //ServoPIDInputVal = ServoPIDCen + 0;
    
    //StopFlag = 1;
  }
  //不是障碍，不用限制
  else
  {    
  }

  return 1;
}




//障碍，对中心线的特殊处理。不直接修改DeviFuse，而是某一条黑线给出一条修正后的中心线。然后交由ImgProSucceedDeal()函数里的DeviFuse更新函数去弄。
//会对CenterLineLoc[]数组产生影响。
//进来之前，黑线一定是连续的。所以直接从线尾补到线头即可。
//如果直接用BlackLoc数组不太理想，可以尝试使用畸变矫正后的数组，会更直。/////////////////////////
uint8 BrickSpeDealCen(void)
{
  uint8 row;
  
  //左障碍，按右线往左一小段来补线。
  if(BrickSpeDealDir == 0)
  {
    //图像失败或非障碍边失败时，不允许补线，返回失败。之后会根据上一幅图的DeviFuse来强制给。
    if(ImgBlackSucceedFlag == 0 || BlackRightDone == 0)
    {
      return 0;
    }
    else
    {
    }
    
    //线头线尾
    //CenterHeadLine = BlackRightHeadLine;
    CenterHeadLine = 0;
    CenterEndLine = BlackRightEndLine;
    
    //采用未矫正的数组构造中心线数组，来避障
    if(BrickSpeDealCenChoose == 0)
    {
      //逐行补线
      for(row = BlackRightEndLine; ; row--)
      {
        CenterLineLoc[row] = BlackRightLoc[row][0] - BrickSpeDealBias[BrickSpeDealBackCount] + (BrickConfirmFinalColCom - 50); //BrickConfirmFinalColCom用于OLED修正
        
        if(row == BlackRightHeadLine) break;
      }
    }
    //采用矫正后的数组构造中心线数组，来避障
    else
    {
      //逐行补线
      for(row = BlackRightEndLine; ; row--)
      {
        CenterLineLoc[row] = BlackUdisRightLocation[row].x - BrickSpeDealBias[BrickSpeDealBackCount] + (BrickConfirmFinalColCom - 50); //BrickConfirmFinalColCom用于OLED修正
        
        if(row == BlackRightHeadLine) break;
      }      
    }
    
    //一直补到最顶行。
    if(BlackRightHeadLine != 0)
    {
      for(row = BlackRightHeadLine - 1; ; row--)
      {
        CenterLineLoc[row] = CenterLineLoc[row+1] + CenterLineLoc[row+1] - CenterLineLoc[row+2];
        
        if(row == 0) break;
      }
    }
    else
    {     
    }
    
  }
  //右障碍，按左线往右一小段来补线。
  else
  {
    //图像失败或非障碍边失败时，不允许补线，返回失败。之后会根据上一幅图的DeviFuse来强制给。
    if(ImgBlackSucceedFlag == 0 || BlackLeftDone == 0)
    {
      return 0;
    }
    else
    {
    }
    
    //线头线尾
    //CenterHeadLine = BlackLeftHeadLine;
    CenterHeadLine = 0;
    CenterEndLine = BlackLeftEndLine;
    
    //采用未矫正的数组构造中心线数组，来避障
    if(BrickSpeDealCenChoose == 0)
    {
      //逐行补线
      for(row = BlackLeftEndLine; ; row--)
      {
        CenterLineLoc[row] = BlackLeftLoc[row][0] + BrickSpeDealBias[BrickSpeDealBackCount] - (BrickConfirmFinalColCom - 50); //BrickConfirmFinalColCom用于OLED修正
        
        if(row == BlackLeftHeadLine) break;
      }
    }
    //采用矫正后的数组构造中心线数组，来避障
    else
    {
      //逐行补线
      for(row = BlackLeftEndLine; ; row--)
      {
        CenterLineLoc[row] = BlackUdisLeftLocation[row].x + BrickSpeDealBias[BrickSpeDealBackCount] - (BrickConfirmFinalColCom - 50); //BrickConfirmFinalColCom用于OLED修正
        
        if(row == BlackLeftHeadLine) break;
      }      
    }
    
    //一直补到最顶行。
    if(BlackLeftHeadLine != 0)
    {
      for(row = BlackLeftHeadLine - 1; ; row--)
      {
        CenterLineLoc[row] = CenterLineLoc[row+1] + CenterLineLoc[row+1] - CenterLineLoc[row+2];
        
        if(row == 0) break;
      }
    }
    else
    {     
    }
    
  }
  
  return 1;
}



//避障程序
uint8 BrickAvoid(void)
{
  uint8 i;
  
  ////障碍锁存标志没有被置位的时候才进行障碍计数，若已被置位，则不用进行障碍计数。
  if(BrickConfirmLockFlag == 0)
  {
    //A次计数内有B次障碍成功，则判断为障碍。先入先出队列。
    for(i = 0; ; i++)
    {
      BrickConfirmMat0[i] = BrickConfirmMat0[i + 1];
      BrickConfirmMat1[i] = BrickConfirmMat1[i + 1];
      if(i == BrickConfirmMatNum - 2) break;
    }
    //左障碍成功
    if(PathType == PathBrickLeft)
    {
      //左障碍储存数组置1.
      BrickConfirmMat0[BrickConfirmMatNum - 1] = 1;
      //右障碍储存数组置0。
      BrickConfirmMat1[BrickConfirmMatNum - 1] = 0;
    }
    else if(PathType == PathBrickRight)
    {
      //右障碍储存数组置1.
      BrickConfirmMat1[BrickConfirmMatNum - 1] = 1;
      //左障碍储存数组置0。
      BrickConfirmMat0[BrickConfirmMatNum - 1] = 0;   
    }
    else
    {
      //判定为非障碍则新来的值置0.
      BrickConfirmMat0[BrickConfirmMatNum - 1] = 0;
      BrickConfirmMat1[BrickConfirmMatNum - 1] = 0;
    }
    
    //收集BrickConfirmMat数组里1的个数。
    BrickConfirmMatCollect0 = 0;
    for(i = 0; ; i++)
    {
      if(BrickConfirmMat0[i] == 1) BrickConfirmMatCollect0++;
      
      if(i == BrickConfirmMatNum - 1) break;
    }
    
    //收集BrickConfirmMat数组里1的个数。
    BrickConfirmMatCollect1 = 0;
    for(i = 0; ; i++)
    {
      if(BrickConfirmMat1[i] == 1) BrickConfirmMatCollect1++;
      
      if(i == BrickConfirmMatNum - 1) break;
    }
    
    //大于若干次，则锁存为左障碍。
    if(BrickConfirmMatCollect0 >= BrickConfirmMatLimit)
    {
      //锁存标志位置位。
      BrickConfirmLockFlag = 1;
      //数组清空，防止重复锁存。
      for(i = 0; ; i++)
      {
        BrickConfirmMat0[i] = 0;
        BrickConfirmMat1[i] = 0;
        if(i == BrickConfirmMatNum - 1) break;
      }
      //确认障碍的方向。
      BrickSpeDealDir = 0;             //左障碍
    }
    else if(BrickConfirmMatCollect1 >= BrickConfirmMatLimit)
    {
      //锁存标志位置位。
      BrickConfirmLockFlag = 1;
      //数组清空，防止重复锁存。
      for(i = 0; ; i++)
      {
        BrickConfirmMat0[i] = 0;
        BrickConfirmMat1[i] = 0;
        if(i == BrickConfirmMatNum - 1) break;
      }
      //确认障碍的方向。
      BrickSpeDealDir = 1;             //右障碍      
    }
    else
    {
    }
  }
  //障碍锁存已被置位，则不用进行障碍计数。
  else
  {
  }
  
  //若障碍锁存标志位置位，则等待矮线的线头够低后开始障碍处理。
  if(BrickConfirmLockFlag == 1)
  {
    light_control(PORTD, 15, Light_ON);
    //判断可否开始特殊处理。
    if(BrickSpeDealStartJudge() == 1)
    {
      BrickSpeDealBackCount = BrickSpeDealBackCountNum;
    }
    else
    {
      //还没满足条件，再等等。
    }
  }
  else
  {
  }

  //退出障碍特殊处理的判断。BrickSpeDealExitJudgeFlag会更新。
  BrickSpeDealExitJudge();
  
  //有了倒数次数后，就开始每次倒数，直到为0或判断已经出了障碍.
  if(BrickSpeDealBackCount != 0 && BrickSpeDealExitJudgeFlag != 0)
  {
    BrickConfirmLockFlag = 0;    //障碍特殊控制开启成功，则锁存标志位清零。
    
    //倒数值减1。
    BrickSpeDealBackCount--;
    //标志位说明进行了障碍的特殊处理。
    BrickSpeDealFlag = 1;
    
    //对中心线的特殊处理函数，还剩BrickSpeDealBackCount次。
    BrickSpeDealCenFlag = BrickSpeDealCen();
    //若处理已经出错了，则立即停止特殊控制，后面会将舵机强制打回来。
    if(BrickSpeDealCenFlag == 0)
    {
      BrickSpeDealBackCount = 0;
    }
    else
    {
    }
    //特殊控制期间，为了防止撞障碍引起的上下坡误判，将坡道禁止赛道类型判断标志复位，并将倒计数置0（不允许置下坡标志，会引起减速），保证复位。
    //会持续多个周期，所以应该是保险的。
    GyroResultForbidFlag = 0;
    GyroResultForbidBackCount = 0;
    
  }
  else
  {    
    //倒数次数清零，防止再次启动障碍特殊处理。
    BrickSpeDealBackCount = 0;
    
    //标志位说明没有进行障碍的特殊处理。
    BrickSpeDealFlag = 0;
    
    //判出标志置初始状态1.
    BrickSpeDealExitJudgeFlag = 1;
  }
  
  return 1;
}


//中心线处理函数，包括特殊和非特殊赛道
uint8 CenterLineGetRaw(void)
{
  uint8 flag;
  
  flag = 1;
  //直角弯 锁存 或 强制控制期间，就不用进行正常的中心线提取。
  //ImgProSucceedDeal()里面的DeviFuse更新函数是没有用的，因为AngleStep2Confirm()在ImgProSucceedDeal()的后面执行，会强制更改DeviFuse.
  if(AngleConfirmLockFlag == 1 || AngleSpeDealFlag == 1)
  {   
    //直角弯锁存或特殊控制时，CenterLineLoc[]数组一直是无效值。
  }
  //障碍物强制控制期间，不执行正常的中心线提取，而是采用单边线的特殊中心线提取函数。
  //这里只是给出一条修正后的中心线，用来给ImgProSucceedDeal()里面的DeviFuse更新函数去用。
  //但是障碍物锁存期间，仍然靠正常提取中心线。
  else if(BrickSpeDealFlag == 1)
  {
    //在BrickAvoid()中已经执行了中心线提取函数。
  }
  //不是特殊赛道，进行正常的中心线提取。有可能返回失败。
  else
  {
    flag = CenterLineGetNormal();
  }
  
  return flag;
}




//中心线提取，进来前左右黑线均连续。
uint8 CenterLineGet(void)
{
  uint8 row, i;
  uint8 temp;
  uint8 flag1, flag2, flag3, flag4;  
  uint8 tempflag1;
  uint8 tempway1;
  uint8 x[CenterForwardLineNum];   //中心线向前补线计算斜率时，x坐标，表示第几行。。
  int16 y[CenterForwardLineNum];   //中心线向前补线计算斜率时，y坐标，表示中心线位置。
  int32 a1, a0;    //中心线向前补线时，最小二乘法的斜率和纵截距。
  
  float slope;
  
  
  //左线提取成功，右线提取失败。
  if(BlackLeftDone == 1 && BlackRightDone == 0)
  {
    //若左线的线头线尾无效，则直接返回失败。
    if(BlackLeftHeadLine == MaxValUint8 || BlackLeftEndLine == MaxValUint8)
    {
      return 0;
    }
    else
    {
    }
    
    flag1 = 1;
    flag2 = 1;
    
    //中心线的线尾取左线的线尾即可。
    CenterHeadLine = BlackLeftHeadLine;     //线头取左线的线头，注意这不是实际的线头。
    //CenterHeadLine = 0;   //线头因为要固定补到第0行，所以直接置0.
    CenterEndLine = BlackLeftEndLine;
    
    //首先看线头是否大于BlackControlLineLow行
    if(BlackLeftHeadLine >= BlackControlLineLow)
    {
      flag1 = 0;
    }
    else
    {
    }
    
    //左线尾小于CameraHight - 1行。
    if(BlackLeftEndLine < CameraHight - 1)
    {
      //只有左线成功，而且左线尾还那么远，真心没法继续。直接返回失败。
      //if(BlackLeftEndLine < CameraHight - 1 - BlackLeftBackLineStart)
      if(BlackLeftEndLine <= BlackControlLineLow)
      {
        flag2 = 0;
        return 0;       
      }
      //左线尾在一定范围内。可以允许把第59行往前移几行。直接取BlackLeftEndLine。
      else
      {
      }
    }
    //左线尾等于CameraHight - 1，直接取BlackLeftEndLine
    else
    {
    }
    
    //丢失另一根线的时候，先根据斜率估算出半宽，若没有上一次双线图像时的赛道半宽大，则直接用上一次双线的赛道半宽。
    //求5行的斜率。
    if(BlackLeftEndLine - BlackLeftHeadLine > 5)
    {
      slope = 1.0 * (BlackUdisLeftLocation[BlackLeftEndLine].x - BlackUdisLeftLocation[BlackLeftEndLine-4].x) / 
                    (BlackUdisLeftLocation[BlackLeftEndLine].y - BlackUdisLeftLocation[BlackLeftEndLine-4].y);
    }
    else
    {
      slope = 0;
    }
    //赛道半宽
    temp = (uint8)(1.0 * sqrt(slope * slope + 1) * RoadWidth / 2);
    //取最大值
    RoadHalfWidth = MaxRe(temp, RoadHalfWidth);
    
    //根据赛道宽度估计出中心线的线头。
    CenterLineLoc[BlackLeftEndLine] = BlackUdisLeftLocation[BlackLeftEndLine].x + RoadHalfWidth;
    
    tempflag1 = 0;
    tempway1 = 0;
    //根据左线的走势估计出中心线。中心线是个int16型矩阵，有正负。
    for(row = BlackLeftEndLine - 1; ;row--)
    {
      //左线有效时。根据左线的趋势补线。
      if(BlackLeftLoc[row][0] != MaxValUint8)
      {
        tempflag1 = 0;
        CenterLineLoc[row] = CenterLineLoc[row+1] + BlackUdisLeftLocation[row].x - BlackUdisLeftLocation[row+1].x;
      }
      //左线无效时。根据最后两行的中心线趋势补线。
      else
      {
        //当左线无效时，可以采用最后两行趋势补线，也可以根据最后CenterForwardLineNum行最小二乘法趋势补线。
        if(tempflag1 == 0)
        {
          //每幅图只进来一次，用来确定补线方式，以后就不用再进来了。
          tempflag1 = 1;
          //最后几行行数够多，可以采用最小二乘法的方式。
          if(row + CenterForwardLineNum <= CameraHight - 1)
          {
            tempway1 = 0;
            //进行最小二乘法系数的计算。
            for(i = 0; i < CenterForwardLineNum; i++)
            {
              x[i] = row + i + 1;
              y[i] = CenterLineLoc[row + i + 1];
            }
            a1 = LeastSquareInt16a1(x, y, CenterForwardLineNum);         //斜率计算。
            a0 = LeastSquareInt16a0(x, y, a1, CenterForwardLineNum);     //纵截距计算。
          }
          //最后几行的行数不够多，采用最后两行趋势补线的方式。
          else
          {
            tempway1 = 1;
          }
        }
        else
        {
        }
        
        //确认补线方式后，开始补线。
        if(tempway1 == 0)
        {
          //最小二乘法的方式。
          CenterLineLoc[row] = (a0 + a1 * row) / LeastSquareMulti;
        }
        else
        {
          //如果是最近2行。
          if(row == CameraHight -1  || row == CameraHight - 2)
          {
            CenterLineLoc[row] = BlackUdisLeftLocation[row].x + RoadHalfWidth;
          }
          //不是最近2行。
          else
          {
            //最后两行的方式。
            CenterLineLoc[row] = CenterLineLoc[row+1] + CenterLineLoc[row+1] - CenterLineLoc[row+2];            
          }
        }
      }
      
      //一直补到最高行。
      if(row == 0) break;
    }
    
    //对未处理的行的中心线置无效值。
    //底部置无效值。顶部不用处理。
    if(CenterEndLine != CameraHight - 1)
    {
      for(row = CameraHight - 1; ; row--)
      {
        if(row == CenterEndLine) break;       
        CenterLineLoc[row] = MaxValInt16;
      }
    }
    else
    {
    }

    
  }
  //右线提取成功，左线提取失败。
  else if(BlackLeftDone == 0 && BlackRightDone == 1)
  {
    //若右线的线头线尾无效，则直接返回失败。
    if(BlackRightHeadLine == MaxValUint8 || BlackRightEndLine == MaxValUint8)
    {
      return 0;
    }
    else
    {
    }
       
    flag1 = 1;
    flag2 = 1;
    
    //中心线的线尾取左线的线尾即可。
    CenterHeadLine = BlackRightHeadLine;   //线头取左线的线头，注意这不是实际的线头。
    //CenterHeadLine = 0;                    //线头因为要固定补到第0行，所以直接置0.
    CenterEndLine = BlackRightEndLine;
    
    //首先看线头是否大于BlackControlLineLow行
    if(BlackRightHeadLine >= BlackControlLineLow)
    {
      flag1 = 0;     
    }
    else
    {
    }
    
    //右线尾小于CameraHight - 1行。
    if(BlackRightEndLine < CameraHight - 1)
    {
      //只有右线成功，而且右线尾还那么远，真心没法继续。直接返回失败。
      //if(BlackRightEndLine < CameraHight - 1 - BlackRightBackLineStart)
      if(BlackRightEndLine <= BlackControlLineLow)  
      {
        flag2 = 0;
        return 0;
      }
      //右线尾在一定范围内。可以允许把第59行往前移几行。直接取BlackRightEndLine。
      else
      {
      }
    }
    //右线尾等于CameraHight - 1，直接取BlackRightEndLine
    else
    {
    }
    
    //丢失另一根线的时候，先根据斜率估算出半宽，若没有上一次双线图像时的赛道半宽大，则直接用上一次双线的赛道半宽。
    //求5行的斜率。
    if(BlackRightEndLine - BlackRightHeadLine > 5)
    {
      slope = 1.0 * (BlackUdisRightLocation[BlackRightEndLine].x - BlackUdisRightLocation[BlackRightEndLine-4].x) / 
                    (BlackUdisRightLocation[BlackRightEndLine].y - BlackUdisRightLocation[BlackRightEndLine-4].y);
    }
    else
    {
      slope = 0;
    }
    //赛道半宽
    temp = (uint8)(1.0 * sqrt(slope * slope + 1) * RoadWidth / 2);
    //取最大值
    RoadHalfWidth = MaxRe(temp, RoadHalfWidth);
    
    tempflag1 = 0;
    tempway1 = 0;
    //根据赛道宽度估计出中心线的线头。
    CenterLineLoc[BlackRightEndLine] = BlackUdisRightLocation[BlackRightEndLine].x - RoadHalfWidth;
    //根据右线的走势估计出中心线。中心线是个int16型矩阵，有正负。
    for(row = BlackRightEndLine - 1; ;row--)
    {
      //右线有效时。根据右线的趋势补线。
      if(BlackRightLoc[row][0] != MaxValUint8)
      {
        tempflag1 = 0;
        CenterLineLoc[row] = CenterLineLoc[row+1] + BlackUdisRightLocation[row].x - BlackUdisRightLocation[row+1].x;
      }
      //右线无效时。根据最后两行的中心线趋势补线。
      else
      {
          //当右线无效时，可以采用最后两行趋势补线，也可以根据最后CenterForwardLineNum行最小二乘法趋势补线。
          if(tempflag1 == 0)
          {
            //每幅图只进来一次，用来确定补线方式，以后就不用再进来了。
            tempflag1 = 1;
            //最后几行行数够多，可以采用最小二乘法的方式。
            if(row + CenterForwardLineNum <= CameraHight - 1)
            {
              tempway1 = 0;
              //进行最小二乘法系数的计算。
              for(i = 0; i < CenterForwardLineNum; i++)
              {
                x[i] = row + i + 1;
                y[i] = CenterLineLoc[row + i + 1];
              }
              a1 = LeastSquareInt16a1(x, y, CenterForwardLineNum);         //斜率计算。
              a0 = LeastSquareInt16a0(x, y, a1, CenterForwardLineNum);     //纵截距计算。
            }
            //最后几行的行数不够多，采用最后两行趋势补线的方式。
            else
            {
              tempway1 = 1;
            }
          }
          else
          {
          }
        
          //确认补线方式后，开始补线。
          if(tempway1 == 0)
          {
            //最小二乘法的方式。
            CenterLineLoc[row] = (a0 + a1 * row) / LeastSquareMulti;
          }
          else
          {
            //最近2行。
            if(row == CameraHight - 1 || row == CameraHight - 2)
            {
              CenterLineLoc[row] = BlackUdisRightLocation[row].x - RoadHalfWidth;
            }
            //不是最近2行。
            else
            {
              //最后两行的方式。
              CenterLineLoc[row] = CenterLineLoc[row+1] + CenterLineLoc[row+1] - CenterLineLoc[row+2];
            }
          }
        }
      
        if(row == 0) break;
      }
    
      
    
    //对未处理的行的中心线置无效值。
    //底部置无效值。顶部不用处理。
    if(CenterEndLine != CameraHight - 1)
    {
      for(row = CameraHight - 1; ; row--)
      {
        if(row == CenterEndLine) break;       
        CenterLineLoc[row] = MaxValInt16;
      }
    }
    else
    {
    }
   
  }
  //左右线均提取成功。
  else if(BlackLeftDone == 1 && BlackRightDone == 1)
  {
    CenterHeadLine = MinRe(BlackLeftHeadLine, BlackRightHeadLine);    //线头取两者较小者。     
    //CenterHeadLine = 0;    //因为固定要补到第0行，所以固定取0.
    //线尾取两者较大者。
    CenterEndLine = MaxRe(BlackLeftEndLine, BlackRightEndLine);             
    
    //不是单线的情况。
    if(SingleBlackAllFlag == 0)
    {
    
      //左右线均提取成功，首选根据两线得中心线，实在不行再根据赛道宽度估计。
      //不根据趋势来预测，因为畸变矫正基本上已经把左右线差搞成定值了，所以不用那么麻烦。    
      
      //先检查左线，左线不满足条件再检测右线。右线不满足再返回失败。
      flag1 = 1;
      flag2 = 1;
      flag3 = 1;
      flag4 = 1;
      //取左右两线的第39到第59行的差，取最大值。
      //首先看线头是否大于39行，大于39行就直接返回失败。
      if(BlackLeftHeadLine >= BlackControlLineLow)
      {
        flag1 = 0;
      }
      else
      {
      }
      //左线尾小于CameraHight - 1行。
      if(BlackLeftEndLine < CameraHight - 1)
      {
        //只有左线成功，而且左线尾还那么远，真心没法继续。看右线是不是一样的情况。
        //if(BlackLeftEndLine < CameraHight - 1 - BlackLeftBackLineStart)
        if(BlackLeftEndLine <= BlackControlLineLow)
        {
          flag2 = 0;
        }
        //左线尾在一定范围内。可以允许把第59行往前移几行。直接取BlackLeftEndLine。
        else
        {
        }
      }
      //左线尾等于CameraHight - 1，直接取BlackLeftEndLine
      else
      {
      }
  
      //处理右线，右线不满足条件，则直接返回失败。
      //首先看线头是否大于39行，大于39行就直接返回失败。
      if(BlackRightHeadLine >= BlackControlLineLow)
      {
        flag3 = 0;
      }
      else
      {
      }    
      //右线尾小于CameraHight - 1行。
      if(BlackRightEndLine < CameraHight - 1)
      {
        //只有右线成功，而且右线尾还那么远，真心没法继续。直接返回失败。
        //if(BlackRightEndLine < CameraHight - 1 - BlackRightBackLineStart)
        if(BlackRightEndLine <= BlackControlLineLow)
        {
          flag4 = 0;
          //两次都线尾很远，直接返回失败。
          if(flag2 == 0) 
          {
            return 0;
          }
          else
          {
          }
        }
        //右线尾在一定范围内。可以允许把第59行往前移几行。直接取BlackRightEndLine。
        else
        {
        }
      }
      //右线尾等于CameraHight - 1，直接取BlackRightEndLine
      else
      {
      }
      
      //只有右线满足要求，则直接取右线的列差值绝对值。
      if(
        (flag3 == 1 && flag4 == 1)
      &&((flag1 == 0 && flag2 == 0) || (flag1 == 1 && flag2 == 0) || (flag1 == 0 && flag2 == 1))
        )
      {
        //斜率的方式估算出赛道半宽。
        //求5行的斜率。
        if(BlackRightEndLine - BlackRightHeadLine > 5)
        {
          slope = 1.0 * (BlackUdisRightLocation[BlackRightEndLine].x - BlackUdisRightLocation[BlackRightEndLine-4].x) / 
                        (BlackUdisRightLocation[BlackRightEndLine].y - BlackUdisRightLocation[BlackRightEndLine-4].y);
        }
        else
        {
          slope = 0;
        }
        //赛道半宽
        temp = (uint8)(1.0 * sqrt(slope * slope + 1) * RoadWidth / 2);
        RoadHalfWidthCompen = RoadHalfWidth / temp;
        RoadHalfWidth = MaxRe(temp, RoadHalfWidth);
      }
      //只有左线满足要求，则直接取左线的列差值绝对值。
      else if(
             (flag1 == 1 && flag2 == 1)
           &&((flag3 == 0 && flag4 == 0) || (flag3 == 1 && flag4 == 0) || (flag3 == 0 && flag4 == 1))
             )
      {
        //斜率的方式估算出赛道半宽。
        //求5行的斜率。
        if(BlackLeftEndLine - BlackLeftHeadLine > 5)
        {
          slope = 1.0 * (BlackUdisLeftLocation[BlackLeftEndLine].x - BlackUdisLeftLocation[BlackLeftEndLine-4].x) / 
                        (BlackUdisLeftLocation[BlackLeftEndLine].y - BlackUdisLeftLocation[BlackLeftEndLine-4].y);
        }
        else
        {
          slope = 0;
        }
        //赛道半宽
        temp = (uint8)(1.0 * sqrt(slope * slope + 1) * RoadWidth / 2);
        RoadHalfWidthCompen = RoadHalfWidth / temp;
        RoadHalfWidth = MaxRe(temp, RoadHalfWidth);
      }
      //左右线均满足条件。则取列差值绝对值较大的。
      else if(flag1 == 1 && flag2 == 1 && flag3 == 1 && flag4 == 1)
      {
        //斜率的方式估算出赛道半宽。
        //求5行的斜率。
        if(BlackLeftEndLine - BlackLeftHeadLine > 5)
        {
          slope = 1.0 * (BlackUdisLeftLocation[BlackLeftEndLine].x - BlackUdisLeftLocation[BlackLeftEndLine-4].x) / 
                        (BlackUdisLeftLocation[BlackLeftEndLine].y - BlackUdisLeftLocation[BlackLeftEndLine-4].y);
        }
        else
        {
          slope = 0;
        }
        //赛道半宽
        temp = (uint8)(1.0 * sqrt(slope * slope + 1) * RoadWidth / 2);
        
        //求5行的斜率。
        if(BlackRightEndLine - BlackRightHeadLine > 5)
        {
          slope = 1.0 * (BlackUdisRightLocation[BlackRightEndLine].x - BlackUdisRightLocation[BlackRightEndLine-4].x) / 
                        (BlackUdisRightLocation[BlackRightEndLine].y - BlackUdisRightLocation[BlackRightEndLine-4].y);
        }
        else
        {
          slope = 0;
        }
        //赛道半宽
        temp = MaxRe((uint8)(1.0 * sqrt(slope * slope + 1) * RoadWidth / 2), temp);
        RoadHalfWidthCompen = RoadHalfWidth / temp;
        RoadHalfWidth = MaxRe(temp, RoadHalfWidth);
      }
      //左右线均不满足要求。
      else
      {
        //左右线都在第BlackControlLineLow行的远方。直接返回失败。
        if(flag2 == 0 && flag4 == 0)
        {
          return 0;
        }
        else
        {
          //左线在第BlackControlLineLow行的近方，右线在第BlackControlLineLow行的远方，则用左线来求。
          if(flag1 == 1 && flag3 == 0)
          {
            //斜率的方式估算出赛道半宽。
            //求5行的斜率。
            if(BlackLeftEndLine - BlackLeftHeadLine > 5)
            {
              slope = 1.0 * (BlackUdisLeftLocation[BlackLeftEndLine].x - BlackUdisLeftLocation[BlackLeftEndLine-4].x) / 
                            (BlackUdisLeftLocation[BlackLeftEndLine].y - BlackUdisLeftLocation[BlackLeftEndLine-4].y);
            }
            else
            {
              slope = 0;
            }
            //赛道半宽
            temp = (uint8)(1.0 * sqrt(slope * slope + 1) * RoadWidth / 2);
            RoadHalfWidthCompen = RoadHalfWidth / temp;
            RoadHalfWidth = MaxRe(temp, RoadHalfWidth);
          }
          //右线在第BlackControlLineLow行的近方，左线在第BlackControlLineLow行的远方，则用右线来求。
          else if(flag1 == 0 && flag3 == 1)
          {
            //斜率的方式估算出赛道半宽。
            //求5行的斜率。
            if(BlackRightEndLine - BlackRightHeadLine > 5)
            {
              slope = 1.0 * (BlackUdisRightLocation[BlackRightEndLine].x - BlackUdisRightLocation[BlackRightEndLine-4].x) / 
                            (BlackUdisRightLocation[BlackRightEndLine].y - BlackUdisRightLocation[BlackRightEndLine-4].y);
            }
            else
            {
              slope = 0;
            }
            //赛道半宽
            temp = (uint8)(1.0 * sqrt(slope * slope + 1) * RoadWidth / 2);
            RoadHalfWidthCompen = RoadHalfWidth / temp;
            RoadHalfWidth = MaxRe(temp, RoadHalfWidth);
          }
          //左右线都在第行的近方，则取较大值。
          else if(flag1 == 0 && flag3 == 0)
          {
            //斜率的方式估算出赛道半宽。
            //求5行的斜率。
            if(BlackLeftEndLine - BlackLeftHeadLine > 5)
            {
              slope = 1.0 * (BlackUdisLeftLocation[BlackLeftEndLine].x - BlackUdisLeftLocation[BlackLeftEndLine-4].x) / 
                            (BlackUdisLeftLocation[BlackLeftEndLine].y - BlackUdisLeftLocation[BlackLeftEndLine-4].y);
            }
            else
            {
              slope = 0;
            }
            //赛道半宽
            temp = (uint8)(1.0 * sqrt(slope * slope + 1) * RoadWidth / 2);
            
            //求5行的斜率。
            if(BlackRightEndLine - BlackRightHeadLine > 5)
            {
              slope = 1.0 * (BlackUdisRightLocation[BlackRightEndLine].x - BlackUdisRightLocation[BlackRightEndLine-4].x) / 
                            (BlackUdisRightLocation[BlackRightEndLine].y - BlackUdisRightLocation[BlackRightEndLine-4].y);
            }
            else
            {
              slope = 0;
            }
            //赛道半宽
            temp = MaxRe((uint8)(1.0 * sqrt(slope * slope + 1) * RoadWidth / 2), temp);
            RoadHalfWidthCompen = RoadHalfWidth / temp;
            RoadHalfWidth = MaxRe(temp, RoadHalfWidth);
          }
          //理论上不可能走到这里。
          else
          {
            return 0;
          }
        }
      }
      
      uint8 temproad;  //本幅图的最后一个双线有效路宽。给本幅图的单线部分用。
      temproad = RoadHalfWidth;
      
      tempflag1 = 0;
      tempway1 = 0;      
      //赛道宽度已经求得，现在开始从低到高求中心线。
      for(row = CenterEndLine; ; row--)
      {
        //当前行左右黑线均有效。检测有无效不能用Udis数组，只能用原始的黑线储存数组。
        if(BlackLeftLoc[row][0] != MaxValUint8 && BlackRightLoc[row][0] != MaxValUint8)
        {
          tempflag1 = 0;
          //取左右黑线的平均值。
          CenterLineLoc[row] = (BlackUdisLeftLocation[row].x + BlackUdisRightLocation[row].x) / 2;
          //取最后一行两行有效的行的赛道半宽，作为最新的赛道半宽。
          if(BlackUdisLeftLocation[row].x > BlackUdisRightLocation[row].x)
          {
            RoadHalfWidthRecord[row] = (BlackUdisLeftLocation[row].x - BlackUdisRightLocation[row].x) / 2;           
          }
          else
          {
            RoadHalfWidthRecord[row] = (BlackUdisRightLocation[row].x - BlackUdisLeftLocation[row].x) / 2;
          }
          
          //本幅图的最后一个双线有效路宽。给本幅图的单线部分用。
          temproad = RoadHalfWidthRecord[row];
          //赛道半宽记录.存第CameraHight - 1行的赛道半宽，给下一幅图用。
          //怕出意外，限个幅。
          if(row == CameraHight - 1) 
          {  
            if(RoadHalfWidthRecord[row] > RoadHalfWidthLimitMax)
            {
              RoadHalfWidth = RoadHalfWidthLimitMax;
            }
            else if(RoadHalfWidthRecord[row] < RoadHalfWidthLimitMin)
            {
              RoadHalfWidth = RoadHalfWidthLimitMin;
            }
            else
            {
              RoadHalfWidth = RoadHalfWidthRecord[row];
            }            
          }
          else
          {
          }
        }
        //左行有效，右行无效。则用左行加上一个赛道半宽。
        else if(BlackLeftLoc[row][0] != MaxValUint8 && BlackRightLoc[row][0] == MaxValUint8)
        {
          tempflag1 = 0;
          CenterLineLoc[row] = BlackUdisLeftLocation[row].x + temproad;
        }
        //左行无效，右行有效。则用右行减去一个赛道半宽。
        else if(BlackLeftLoc[row][0] == MaxValUint8 && BlackRightLoc[row][0] != MaxValUint8)
        {
          tempflag1 = 0;
          CenterLineLoc[row] = BlackUdisRightLocation[row].x - temproad;
        }
        //左右行均无效。则用最后的中心线均势往前走。
        else if(BlackLeftLoc[row][0] == MaxValUint8 && BlackRightLoc[row][0] == MaxValUint8)
        {
          //当左右线均无效时，可以采用最后两行趋势补线，也可以根据最后CenterForwardLineNum行最小二乘法趋势补线。
          if(tempflag1 == 0)
          {
            //每幅图只进来一次，用来确定补线方式，以后就不用再进来了。
            //除非又出现左或右黑线有效的情况，则再次刷新斜率。
            tempflag1 = 1;
            //最后几行行数够多，可以采用最小二乘法的方式。
            if(row + CenterForwardLineNum <= CameraHight - 1)
            {
              tempway1 = 0;
              //进行最小二乘法系数的计算。
              for(i = 0; i < CenterForwardLineNum; i++)
              {
                x[i] = row + i + 1;
                y[i] = CenterLineLoc[row + i + 1];
              }
              a1 = LeastSquareInt16a1(x, y, CenterForwardLineNum);         //斜率计算。
              a0 = LeastSquareInt16a0(x, y, a1, CenterForwardLineNum);     //纵截距计算。
            }
            //最后几行的行数不够多，采用最后两行趋势补线的方式。
            else
            {
              tempway1 = 1;
            }
          }
          else
          {
          }
          
          //确认补线方式后，开始补线。
          if(tempway1 == 0)
          {
            //最小二乘法的方式。
            CenterLineLoc[row] = (a0 + a1 * row) / LeastSquareMulti;
          }
          else
          {
            //最后两行的方式。
            CenterLineLoc[row] = CenterLineLoc[row+1] + CenterLineLoc[row+1] - CenterLineLoc[row+2];
          }
          
        }
        //理论上不会走到这里。
        else
        {
        }
        
        if(row == 0) break;
      }
         
      //对未处理的行的中心线置无效值。对顶部的不作处理。
      //底部置无效值。
      if(CenterEndLine != CameraHight - 1)
      {
        for(row = CameraHight - 1; ; row--)
        {
          if(row == CenterEndLine) break;       
          CenterLineLoc[row] = MaxValInt16;
        }
      }
      else
      {
      }
      
      //单线错误暂时不检查
      /*
      uint8 temphead;
      
      //检查单线错误，先判断是不是一线初始行够高，一线初始行够低，若不是，直接跳过节省时间。
      if(BlackLeftStep1ScanRow <= CameraHight - 3 && BlackRightStep1ScanRow >= CameraHight - 3)
      {
        //双线都采集到的时候，最终需要检查一下赛道半宽是不是正确。
        //第一种情况，有效双行很短，而且范围内存在赛道半宽很小的点。
        for(i = 0; ; i++)
        {
          if(RoadHalfWidthRecord[i] != MaxValUint8)
          {
            temphead = i;
            break;
          }
          
          //理论上不可能走到这里
          if(i == CameraHight - 1) return 1;  //无法检测单线错误，返回1.
        }
        
        //长度是不是够小。
        if(BlackLeftStep1ScanRow > temphead && BlackLeftStep1ScanRow - temphead < RoadHalfWidthSingleGapLine)
        {
          for(i = BlackLeftStep1ScanRow; ; i--)
          {
            if(RoadHalfWidthRecord[i] < RoadHalfWidthSingleLimit)
            {
              //存在半宽很小的行，直接置单线错误标志位。 
              RoadHalfWidthWrongDir = 0;          //单线错误时左线悬空。
              RoadHalfWidthWrongFlag = 1;
              break;
            }            
            //不存在半宽很小的行，跳出，进行第二种情况的检测。
            if(i == temphead)
            {
              break;
            }
          }
          
        }
        
        //第一种情况没有检测出来的话，就检测第二种情况。
        if(RoadHalfWidthWrongFlag == 2)
        {
          for(i = CameraHight - 1; ; i--)
          {
            //只要有效的双行里有任何一个赛道半宽大于阈值，就判断为不是单线错误。
            if(RoadHalfWidthRecord[i] != MaxValUint8 && RoadHalfWidthRecord[i] > RoadHalfWidthSingleLimit)
            {
              RoadHalfWidthWrongFlag = 0;
              break;
            }
            else
            {
            }
            
            //直到最后一行仍然不满足条件，则判断为单线错误。
            if(i == 0)
            {
              RoadHalfWidthWrongDir = 0;          //单线错误时左线悬空。
              RoadHalfWidthWrongFlag = 1;
              break;
            }
            else
            {          
            }
          }
        }
        else
        {
        }        
      }
      else if(BlackRightStep1ScanRow <= CameraHight - 3 && BlackLeftStep1ScanRow >= CameraHight - 3)
      {
        //双线都采集到的时候，最终需要检查一下赛道半宽是不是正确。
        //第一种情况，有效双行很短，而且范围内存在赛道半宽很小的点。
        for(i = 0; ; i++)
        {
          if(RoadHalfWidthRecord[i] != MaxValUint8)
          {
            temphead = i;
            break;
          }
          
          //理论上不可能走到这里
          if(i == CameraHight - 1) return 1;  //无法检测单线错误，返回1.
        }
        
        //长度是不是够小。
        if(BlackRightStep1ScanRow > temphead && BlackRightStep1ScanRow - temphead < RoadHalfWidthSingleGapLine)
        {
          for(i = BlackRightStep1ScanRow; ; i--)
          {
            if(RoadHalfWidthRecord[i] < RoadHalfWidthSingleLimit)
            {
              //存在半宽很小的行，直接置单线错误标志位。 
              RoadHalfWidthWrongDir = 1;          //单线错误时右线悬空。
              RoadHalfWidthWrongFlag = 1;
              break;
            }            
            //不存在半宽很小的行，跳出，进行第二种情况的检测。
            if(i == temphead)
            {
              break;
            }
          }
          
        }
        
        //第一种情况没有检测出来的话，就检测第二种情况。
        if(RoadHalfWidthWrongFlag == 2)
        {
          for(i = CameraHight - 1; ; i--)
          {
            //只要有效的双行里有任何一个赛道半宽大于阈值，就判断为不是单线错误。
            if(RoadHalfWidthRecord[i] != MaxValUint8 && RoadHalfWidthRecord[i] > RoadHalfWidthSingleLimit)
            {
              RoadHalfWidthWrongFlag = 0;
              break;
            }
            else
            {
            }
            
            //直到最后一行仍然不满足条件，则判断为单线错误。
            if(i == 0)
            {
              RoadHalfWidthWrongDir = 1;          //单线错误时右线悬空。
              RoadHalfWidthWrongFlag = 1;
              break;
            }
            else
            {          
            }
          }
        }
        else
        {
        }
        
      }
      //初始行不满足条件，则不需要检测单线错误。
      else
      {      
      }
      
      //单线错误的处理，暂时简单处理为中心线错误，也就是图像错误。为了让扫描起点保持原状。
      if(RoadHalfWidthWrongFlag == 1)
      {        
        RoadHalfWidth = RoadHalfWidthLast;  //这次的赛道半宽无效，还原为上幅图像最终的赛道半宽。
        //一边的线是错误的。
        if(RoadHalfWidthWrongDir == 0)
        {
          BlackLeftDone = 0;
        }
        else
        {
          BlackRightDone = 0;
        }
        CenterLineGetSingleWrongAdd();      //针对单线错误的中心线重新处理。
      }
      else
      {        
      }
      */
      
    }
    //单线的情况，也就是SingleBlackAllFlag==1的情况。
    else
    {
      tempflag1 = 0;
      tempway1 = 0;
      
      //赛道宽度已经求得，现在开始从低到高求中心线。
      for(row = CenterEndLine; ; row--)
      {
        //当前行左右黑线均有效。检测有无效不能用Udis数组，只能用原始的黑线储存数组。
        if(BlackLeftLoc[row][0] != MaxValUint8 && BlackRightLoc[row][0] != MaxValUint8)
        {
          tempflag1 = 0;
          //取左右黑线的平均值。
          CenterLineLoc[row] = (BlackUdisLeftLocation[row].x + BlackUdisRightLocation[row].x) / 2;
          //取最后一行两行有效的行的赛道半宽，作为最新的赛道半宽。
          if(BlackUdisLeftLocation[row].x > BlackUdisRightLocation[row].x)
          {
            RoadHalfWidthSingle = (BlackUdisLeftLocation[row].x - BlackUdisRightLocation[row].x) / 2;
          }
          else
          {
            RoadHalfWidthSingle = (BlackUdisRightLocation[row].x - BlackUdisLeftLocation[row].x) / 2;
          }        
        }
        //左行有效，右行无效。则用左行加上一个赛道半宽。
        else if(BlackLeftLoc[row][0] != MaxValUint8 && BlackRightLoc[row][0] == MaxValUint8)
        {
          tempflag1 = 0;
          CenterLineLoc[row] = BlackUdisLeftLocation[row].x - RoadHalfWidthSingle;    //因为是单线，所以是减号。
        }
        //左行无效，右行有效。则用右行减去一个赛道半宽。
        else if(BlackLeftLoc[row][0] == MaxValUint8 && BlackRightLoc[row][0] != MaxValUint8)
        {
          tempflag1 = 0;
          CenterLineLoc[row] = BlackUdisRightLocation[row].x + RoadHalfWidthSingle;   //因为是单线，所以是加号。
        }
        //左右行均无效。则用最后的中心线均势往前走。
        else if(BlackLeftLoc[row][0] == MaxValUint8 && BlackRightLoc[row][0] == MaxValUint8)
        {
          //当左右线均无效时，可以采用最后两行趋势补线，也可以根据最后CenterForwardLineNum行最小二乘法趋势补线。
          if(tempflag1 == 0)
          {
            //每幅图只进来一次，用来确定补线方式，以后就不用再进来了。
            //除非又出现左或右黑线有效的情况，则再次刷新斜率。
            tempflag1 = 1;
            //最后几行行数够多，可以采用最小二乘法的方式。
            if(row + CenterForwardLineNum <= CameraHight - 1)
            {
              tempway1 = 0;
              //进行最小二乘法系数的计算。
              for(i = 0; i < CenterForwardLineNum; i++)
              {
                x[i] = row + i + 1;
                y[i] = CenterLineLoc[row + i + 1];
              }
              a1 = LeastSquareInt16a1(x, y, CenterForwardLineNum);         //斜率计算。
              a0 = LeastSquareInt16a0(x, y, a1, CenterForwardLineNum);     //纵截距计算。
            }
            //最后几行的行数不够多，采用最后两行趋势补线的方式。
            else
            {
              tempway1 = 1;
            }
          }
          else
          {
          }
          
          //确认补线方式后，开始补线。
          if(tempway1 == 0)
          {
            //最小二乘法的方式。
            CenterLineLoc[row] = (a0 + a1 * row) / LeastSquareMulti;
          }
          else
          {
            //最后两行的方式。
            CenterLineLoc[row] = CenterLineLoc[row+1] + CenterLineLoc[row+1] - CenterLineLoc[row+2];
          }
          
        }
        //理论上不会走到这里。
        else
        {
        }
        
        if(row == 0) break;
      }
         
      //对未处理的行的中心线置无效值。对顶部的不作处理。
      //底部置无效值。
      if(CenterEndLine != CameraHight - 1)
      {
        for(row = CameraHight - 1; ; row--)
        {
          if(row == CenterEndLine) break;       
          CenterLineLoc[row] = MaxValInt16;
        }
      }
      else
      {
      }
    }
  }
  //理论上不会走到这里。
  else
  {
    //左右黑线均提取失败，返回0.
    return 0;
  }
  
  return 1;
}





//中心线求取时，根据两列偏差绝对值来求取赛道半宽度的函数。
//val为第39行与第59的列偏差的绝对值。
//返回估计出来的赛道半宽度。
uint8 CenterLineHalfWidthGet(uint8 val)
{
  uint8 temphalfwidth;
  
  if(val <= 25)
  {
    temphalfwidth = 100;
  }
  else
  {
    if(val <= 30)
    {
      //temphalfwidth = 95;
      temphalfwidth = 110;
    }
    else
    {
      if(val <= 35)
      {
        //temphalfwidth = 105;
        temphalfwidth = 120;
      }
      else
      {
          //temphalfwidth = 110;
          temphalfwidth = 130;
      }
    }
  }
  
  return temphalfwidth;
}




//图像重新赋值，摄像头反转版本。
void ImgReversePut(void)
{

    uint16 i, j, k, temp1, temp2;   
    
    temp1 = CameraRealLeftCol;
    temp2 = CameraRealLeftCol + CameraRealWidth;
    //temp1 = CameraWidth/2 - CameraRealWidth/2 - 1;
    //temp2 = CameraWidth/2 + CameraRealWidth/2 - 1;
    
    if (ImgPresent == ImgNO1)    //如果当前是第1幅图像正在接收数据（即第2幅图像接收完成）
    {
      for(i = 0; i < CameraHight; i++)
      {
        k = 0;
        for(j = temp1; j < temp2; j++)
        {
          ImgNew[CameraHight-1-i][CameraRealWidth-1-k] = ImgStore2[i][j];
          k++;
        }
      }
    }
    else if (ImgPresent == ImgNO2)  //如果当前是第2幅图像正在接收数据（即第1幅图像接收完成）
    {
      for(i = 0; i < CameraHight; i++)
      {
        k = 0;
        for(j = temp1; j < temp2; j++)
        {
          ImgNew[CameraHight-1-i][CameraRealWidth-1-k] = ImgStore1[i][j];
          k++;
        }
      }
    }
    else 
    {
        //uart_sendN(UART0, (uint8 *)"\nError In FieldIsr()!", 21);   //错误警告
    }
  
}



//图像重新赋值,行不变（行中DMA中变化，选取需要的行），列无法在DMA中变化，就在这里变化。
///////////////////////////////////////////但这里需要用示波器测量一下时间，看是不是耗费了太多时间。
void ImgPut(void)
{
    uint16 i, j, k, temp1, temp2;   
    
    temp1 = CameraRealLeftCol;
    temp2 = CameraRealLeftCol + CameraRealWidth;
    //temp1 = CameraWidth/2 - CameraRealWidth/2 - 1;
    //temp2 = CameraWidth/2 + CameraRealWidth/2 - 1;
    
    if (ImgPresent == ImgNO1)    //如果当前是第1幅图像正在接收数据（即第2幅图像接收完成）
    {
      for(i = 0; i < CameraHight; i++)
      {
        k = 0;
        for(j = temp1; j < temp2; j++)
        {
          ImgNew[i][k] = ImgStore2[i][j];
          k++;
        }
      }
    }
    else if (ImgPresent == ImgNO2)  //如果当前是第2幅图像正在接收数据（即第1幅图像接收完成）
    {
      for(i = 0; i < CameraHight; i++)
      {
        k = 0;
        for(j = temp1; j < temp2; j++)
        {
          ImgNew[i][k] = ImgStore1[i][j];
          k++;
        }
      }
    }
    else 
    {
        //uart_sendN(UART0, (uint8 *)"\nError In FieldIsr()!", 21);   //错误警告
    }

}





//中心线离散度求取函数。自身的离散程度，和中心线的。
void ErrorGet(void)
{
  uint8 row;
  uint8 avg = 0;
  float temp;
  uint8 index;
  //求取平均值
  temp = 0;
  index = 0;
  for(row = CameraHight - 1; ; row--)
  {
    if(CenterLineLoc[row] != MaxValUint8)
    {
      temp += CenterLineLoc[row];
      index++;
    }
    if(row == 0) break;
  }
  avg = (uint8)(temp / index);
  /*
  //1. 先取线头
  for(row = CameraHight - 1; ; row--)
  {
    if(CenterLineLoc[row] != MaxValUint8)
    {
      avg = CenterLineLoc[row];
      break;
    }
    if(row == 0) break;
  }
  //2. 叠加求平均
  for(row--; ; row--)
  {
    if(CenterLineLoc[row] != MaxValUint8)
    {
      avg = (avg + CenterLineLoc[row]) / 2;
    }
    if(row == 0) break;    
  }
  */
  

  temp = 0;
  index = 0;
  for(row = CameraHight - 1; ; row--)
  {
    if(CenterLineLoc[row] != MaxValUint8)
    {
      temp += AbsRe(CenterLineLoc[row], avg);
      index++;
    }
    if(row == 0) break;
  }
  ErrorGetSelf = (uint8)(temp / index);
  ErrorGetCen = AbsRe(avg, PhyCenterCol);
  
}



//开根号运算
//本来传进来传出去都是uint16型的，但是我调用的时候只会用到uint8型的，且不会超出255.
uint16 MathSqrt(uint16 x1)
{
  uint8 ans = 0, p = 0x80;
  uint16 x = x1;
  
  while(p!=0)
  {
    ans += p;
    if(ans * ans > x) ans -=p;
    p = (uint8)(p / 2);
  }
  return ans;  
}

//曲线的曲率求取，曲线向右拐（三点为顺时针方向分布），曲率为负，曲线向左拐（三点为逆时钟方向分布），曲率为正
//K = 4*SABC/AB/BC/AC
int16 CurveGet(int16 AX, uint8 AY, int16 BX, uint8 BY, int16 CX, uint8 CY)
{
  int16 SABC_temp; //三角形的面积，有正负。
  int16 tempab, tempbc, tempac;
  int16 AB, BC, AC; //三角形的边长，均为正。
  int16 result;
  //int16 K;        //曲率结果，有正负。
  
  SABC_temp = ((BX - AX) * (CY - AY) - (CX - AX) * (BY - AY));
  
  tempab = (BX - AX) * (BX - AX) + (BY - AY) * (BY - AY);
  if(tempab > 100) 
  {
    AB = MathSqrt(tempab / 100) * 10; //失掉两位的精度，因为求根运算范围的限制。
  }
  else
  {
    AB = MathSqrt(tempab);
  }
  
  tempbc = (BX - CX) * (BX - CX) + (BY - CY) * (BY - CY);
  if(tempbc > 100) 
  {
    BC = MathSqrt(tempbc / 100) * 10; //失掉两位的精度，因为求根运算范围的限制。
  }
  else
  {
    BC = MathSqrt(tempbc);
  }
  
  tempac = (CX - AX) * (CX - AX) + (CY - AY) * (CY - AY);
  if(tempac > 100) 
  {
    AC = MathSqrt(tempac / 100) * 10; //失掉两位的精度，因为求根运算范围的限制。
  }
  else
  {
    AC = MathSqrt(tempac);
  }
  
  result = CurveGetCompen * SABC_temp / AB / BC / AC;
  if(result >= MaxValUint8) 
  {
    result = MaxValUint8 - 1;           //不能大过MaxValUint8
  }
  else if(result + MaxValUint8 < 0)
  {
    result = 1 - MaxValUint8;           //不能小过-MaxValUint8
  }
  else
  {
  }
  
  return result;
}



//SABC的面积求取，顺时针则返回负值，逆时针则返回正值。
//以左下角为原点，向右为x轴正方向(0~249)，向上为y轴正方向(0~49)。
int16 SABCGet(uint8 x1, uint8 y1, uint8 x2, uint8 y2, uint8 x3, uint8 y3)
{
  int16 SABC_temp;
  
  SABC_temp = ((x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1)) / 2;
  
  return SABC_temp;
}




//中心线的曲率和斜率求取
uint8 CurveSlopeGetCenter(void)
{
  uint8 row;
  uint8 temp;
  uint8 tempslopex[3];
  int16 tempslopey[3];
  
  //1. 
  //曲率符号预先清零。
  CurveLineChosenC1 = MaxValUint8;
  CurveLineChosenC2 = MaxValUint8;
  CurveLineChosenC3 = MaxValUint8;
  
  if(CenterHeadLine == MaxValUint8 || CenterEndLine == MaxValUint8)
  {
    CURVEC = MaxValUint8;  //return 0之前要将中心线曲率置无效值。
    SlopeC = MaxValInt16;  //return 0之前要将中心线斜率置无效值。
    return 0;
  }
  
  //曲率基于的行1 CurveLineChosen1，即中心线的线头。
  if(CenterHeadLine >= CameraHight - 2)
  {
    CurveLineChosenC1 = MaxValUint8;  
    CURVEC = MaxValUint8;  //return 0之前要将中心线曲率置无效值。
    SlopeC = MaxValInt16;  //return 0之前要将中心线斜率置无效值。
    return 0;    
  }
  else
  {
    CurveLineChosenC1 = CenterHeadLine;    
  }
  
  
  //把第1个有效值到CameraHight-1的距离分成2段。
  temp = (CenterEndLine - 1 - CenterHeadLine) / 2;
  //2 * 2 + 1 = 5, 即如果有效行只有3行以内的话，就放弃求取曲率和斜率。（因为会很不精确）
  if(temp <= 2) 
  {
    CurveLineChosenC1 = MaxValUint8;
    CURVEC = MaxValUint8;  //return 0之前要将中心线曲率置无效值。
    SlopeC = MaxValInt16;  //return 0之前要将中心线斜率置无效值。
    return 0;
  }
  
  //曲率基于的行2 CurveLineChosen2
  for(row = CurveLineChosenC1 + temp - 1; ; row++)
  {
    if(row > CenterEndLine - 1) 
    {
      CurveLineChosenC1 = MaxValUint8;
      CurveLineChosenC2 = MaxValUint8;
      CURVEC = MaxValUint8;  //return 0之前要将中心线曲率置无效值。
      SlopeC = MaxValInt16;  //return 0之前要将中心线斜率置无效值。
      return 0;
    }
    //不用判断该行是不是无效值。因为中心线连续。
    CurveLineChosenC2 = row;
    break;
    
  }
  
  //曲率基于的行3 CurveLineChosen3
  for(row = CurveLineChosenC2 + temp - 1; ; row++)
  {
    if(row > CenterEndLine) 
    {
      CurveLineChosenC1 = MaxValUint8;
      CurveLineChosenC2 = MaxValUint8;
      CurveLineChosenC3 = MaxValUint8;
      CURVEC = MaxValUint8;  //return 0之前要将中心线曲率置无效值。
      SlopeC = MaxValInt16;  //return 0之前要将中心线斜率置无效值。      
      return 0;
    }
    //不用判断该行是不是无效值。因为中心线连续。
    CurveLineChosenC3 = row;
    break;
  }
  //SABCC中心线曲率三角形面积的求取
  //SABCC = SABCGet(CenterLineLoc[CurveLineChosenC1], CameraHight - 1 - CurveLineChosenC1,
  //                CenterLineLoc[CurveLineChosenC2], CameraHight - 1 - CurveLineChosenC2,
  //                CenterLineLoc[CurveLineChosenC3], CameraHight - 1 - CurveLineChosenC3
  //                );
  //曲率求取，最大值不会超过MaxValUint8;
  CURVEC = CurveGet(CenterLineLoc[CurveLineChosenC1], CameraHight - 1 - CurveLineChosenC1,
                   CenterLineLoc[CurveLineChosenC2], CameraHight - 1 - CurveLineChosenC2,
                   CenterLineLoc[CurveLineChosenC3], CameraHight - 1 - CurveLineChosenC3
                   );
  //中心线斜率求取，左上角为原点，向下X，向右Y。应该不会超过MaxValUint8
  tempslopex[0] = CurveLineChosenC1;
  tempslopex[1] = CurveLineChosenC2;
  tempslopex[2] = CurveLineChosenC3;
  tempslopey[0] = CenterLineLoc[CurveLineChosenC1];
  tempslopey[1] = CenterLineLoc[CurveLineChosenC2];
  tempslopey[2] = CenterLineLoc[CurveLineChosenC3];
  SlopeC = LeastSquareInt16a1(tempslopex, tempslopey, 3);  //已经乘以了LeastSquareMulti倍。
  
  return 1;
  
}

//左黑线的曲率和斜率求取
uint8 CurveSlopeGetLeft(void)
{
  uint8 row;
  uint8 temp;
  uint8 tempslopex[3];
  int16 tempslopey[3];
  
  //左右黑线有可能只有一边提取成功，若不成功，就不用费时间了。
  if(BlackLeftDone == 1)
  {
    //2. 左黑线曲率符号求取
    //曲率符号预先清零。
    CurveLineChosenL1 = MaxValUint8;
    CurveLineChosenL2 = MaxValUint8;
    CurveLineChosenL3 = MaxValUint8;
    
    //曲率基于的行1 CurveLineChosen1，即左线线头BlackLeftHeadLine.
    if(BlackLeftHeadLine == MaxValUint8 || BlackLeftEndLine == MaxValUint8)
    {
      CURVEL = MaxValInt16;       //return 0之前，要把左黑线的曲率置无效值。
      SlopeL = MaxValInt16;       //return 0之前，要把左黑线的斜率置无效值。
      return 0;      
    }
    
    if(BlackLeftHeadLine >= CameraHight - 2)
    {
      CURVEL = MaxValInt16;       //return 0之前，要把左黑线的曲率置无效值。
      SlopeL = MaxValInt16;       //return 0之前，要把左黑线的斜率置无效值。
      return 0;      
    }
    else
    {
      CurveLineChosenL1 = BlackLeftHeadLine;
    }

    //把第1个有效值到CameraHight-1的距离分成2段。
    temp = (BlackLeftEndLine - 1 - BlackLeftHeadLine) / 2;
    //2 * 2 + 1 = 5, 即如果有效行只有3行以内的话，就放弃求取曲率和斜率。（因为会很不精确）
    if(temp <= 2) 
    {
      CurveLineChosenL1 = MaxValUint8;
      CURVEL = MaxValInt16;       //return 0之前，要把左黑线的曲率置无效值。
      SlopeL = MaxValInt16;       //return 0之前，要把左黑线的斜率置无效值。      
      return 0;
    }
    
    //曲率基于的行2 CurveLineChosen2
    for(row = CurveLineChosenL1 + temp - 1; ; row++)
    {
      if (row > BlackLeftEndLine - 1) 
      {
        CurveLineChosenL1 = MaxValUint8;
        CurveLineChosenL2 = MaxValUint8;
        CURVEL = MaxValInt16;       //return 0之前，要把左黑线的曲率置无效值。
        SlopeL = MaxValInt16;       //return 0之前，要把左黑线的斜率置无效值。     
        return 0;
      }
      //不用判断是不是无效值。因为左黑线连续。
      CurveLineChosenL2 = row;
      break;
    }
    
    //曲率基于的行3 CurveLineChosen3
    for(row = CurveLineChosenL2 + temp - 1; ; row++)
    {
      if (row > BlackLeftEndLine) 
      {
        CurveLineChosenL1 = MaxValUint8;
        CurveLineChosenL2 = MaxValUint8;
        CurveLineChosenL3 = MaxValUint8;
        CURVEL = MaxValInt16;       //return 0之前，要把左黑线的曲率置无效值。
        SlopeL = MaxValInt16;       //return 0之前，要把左黑线的斜率置无效值。        
        return 0;
      }
      //不用判断是不是无效值。因为左黑线连续。
      CurveLineChosenL3 = row;
      break;
    }
    //SABCL左黑线曲率三角形面积的求取
    //SABCL = SABCGet(BlackLeftLoc[CurveLineChosenL1,0], CameraHight - 1 - CurveLineChosenL1,
    //               BlackLeftLoc[CurveLineChosenL2,0], CameraHight - 1 - CurveLineChosenL2,
    //               BlackLeftLoc[CurveLineChosenL3,0], CameraHight - 1 - CurveLineChosenL3
    //               );
    
//    CURVEL = CurveGet(BlackUdisLeftLocation[CurveLineChosenL1].x, CameraHight - 1 - CurveLineChosenL1,
//                      BlackUdisLeftLocation[CurveLineChosenL2].x, CameraHight - 1 - CurveLineChosenL2,
//                      BlackUdisLeftLocation[CurveLineChosenL3].x, CameraHight - 1 - CurveLineChosenL3
//                      );
    
    CURVEL = CurveGet(BlackLeftLoc[CurveLineChosenL1][0], CameraHight - 1 - CurveLineChosenL1,
                      BlackLeftLoc[CurveLineChosenL2][0], CameraHight - 1 - CurveLineChosenL2,
                      BlackLeftLoc[CurveLineChosenL3][0], CameraHight - 1 - CurveLineChosenL3
                      );
    //左黑线斜率求取，左上角为原点，向下X，向右Y。
    tempslopex[0] = CurveLineChosenL1;
    tempslopex[1] = CurveLineChosenL2;
    tempslopex[2] = CurveLineChosenL3;
    tempslopey[0] = BlackUdisLeftLocation[CurveLineChosenL1].x;
    tempslopey[1] = BlackUdisLeftLocation[CurveLineChosenL2].x;
    tempslopey[2] = BlackUdisLeftLocation[CurveLineChosenL3].x;
    SlopeL = LeastSquareInt16a1(tempslopex, tempslopey, 3);   //已经乘以了LeastSquareMulti倍。
    
    return 1;
  }
  //左黑线提取失败，直接返回失败。
  else
  {
    CURVEL = MaxValInt16;       //return 0之前，要把左黑线的曲率置无效值。
    SlopeL = MaxValInt16;       //return 0之前，要把左黑线的斜率置无效值。
    return 0;
  }
}



//右黑线的曲率和斜率求取
uint8 CurveSlopeGetRight(void)
{
  uint8 row;
  uint8 temp;
  uint8 tempslopex[3];
  int16 tempslopey[3];

  //左右黑线有可能只有一边提取成功，若不成功，就不用费时间了。
  if(BlackRightDone == 1)
  {
    //3. 右黑线曲率符号求取
    //曲率符号预先清零。
    CurveLineChosenR1 = MaxValUint8;
    CurveLineChosenR2 = MaxValUint8;
    CurveLineChosenR3 = MaxValUint8;
    
    //曲率基于的行1 CurveLineChosen1，从上到下取第一个有效值。
    if(BlackRightHeadLine == MaxValUint8 || BlackRightEndLine == MaxValUint8)
    {
      CURVER = MaxValInt16;      //return 0前先把右黑线的曲率置无效值。   
      SlopeR = MaxValInt16;      //return 0前先把右黑线的斜率置无效值。        
      return 0;      
    }
    if(BlackRightHeadLine  >= CameraHight - 2)
    {
      CURVER = MaxValInt16;      //return 0前先把右黑线的曲率置无效值。   
      SlopeR = MaxValInt16;      //return 0前先把右黑线的斜率置无效值。        
      return 0;      
    }
    else
    {
      CurveLineChosenR1 = BlackRightHeadLine;
    }

    //把第1个有效值到CameraHight-1的距离分成2段。
    temp = (BlackRightEndLine - 1 - BlackRightHeadLine) / 2;
    //2 * 2 + 1 = 5, 即如果有效行只有3行以内的话，就放弃求取曲率和斜率。（因为会很不精确）
    if(temp <= 2)
    {
      CurveLineChosenR1 = MaxValUint8;
      CURVER = MaxValInt16;      //return 0前先把右黑线的曲率置无效值。   
      SlopeR = MaxValInt16;      //return 0前先把右黑线的斜率置无效值。      
      return 0;
    }
    
    //曲率基于的行2 CurveLineChosen2
    for(row = CurveLineChosenR1 + temp - 1; ; row++)
    {
      if (row > BlackRightEndLine - 1) 
      {
        CurveLineChosenR1 = MaxValUint8;
        CurveLineChosenR2 = MaxValUint8;
        CURVER = MaxValInt16;      //return 0前先把右黑线的曲率置无效值。   
        SlopeR = MaxValInt16;      //return 0前先把右黑线的斜率置无效值。      
        return 0;
      }
      //不用判断该行是不是无效值，因为右线连续。
      CurveLineChosenR2 = row;
      break;
    }
    
    //曲率基于的行3 CurveLineChosen3
    for(row = CurveLineChosenR2 + temp - 1; ; row++)
    {
      if (row > BlackRightEndLine) 
      {
        CurveLineChosenR1 = MaxValUint8;
        CurveLineChosenR2 = MaxValUint8;
        CurveLineChosenR3 = MaxValUint8;
        CURVER = MaxValInt16;      //return 0前先把右黑线的曲率置无效值。   
        SlopeR = MaxValInt16;      //return 0前先把右黑线的斜率置无效值。        
        return 0;
      }
      //不用判断该行是不是无效值，因为右线连续。
      CurveLineChosenR3 = row;
      break;
    }   
    //SABCR右黑线曲率三角形面积的求取
    //SABCR = SABCGet(BlackRightLoc[CurveLineChosenR1,0], CameraHight - 1 - CurveLineChosenR1, 
    //                BlackRightLoc[CurveLineChosenR2,0], CameraHight - 1 - CurveLineChosenR2, 
    //                BlackRightLoc[CurveLineChosenR3,0], CameraHight - 1 - CurveLineChosenR3
    //                );
    
//    CURVER = CurveGet(BlackUdisRightLocation[CurveLineChosenR1].x, CameraHight - 1 - CurveLineChosenR1,
//                      BlackUdisRightLocation[CurveLineChosenR2].x, CameraHight - 1 - CurveLineChosenR2,
//                      BlackUdisRightLocation[CurveLineChosenR3].x, CameraHight - 1 - CurveLineChosenR3
//                      );
    
    CURVER = CurveGet(BlackRightLoc[CurveLineChosenR1][0], CameraHight - 1 - CurveLineChosenR1,
                      BlackRightLoc[CurveLineChosenR2][0], CameraHight - 1 - CurveLineChosenR2,
                      BlackRightLoc[CurveLineChosenR3][0], CameraHight - 1 - CurveLineChosenR3
                      );
    
    //右黑线斜率求取，左上角为原点，向下X，向右Y。
    tempslopex[0] = CurveLineChosenR1;
    tempslopex[1] = CurveLineChosenR2;
    tempslopex[2] = CurveLineChosenR3;
    tempslopey[0] = BlackUdisRightLocation[CurveLineChosenR1].x;
    tempslopey[1] = BlackUdisRightLocation[CurveLineChosenR2].x;
    tempslopey[2] = BlackUdisRightLocation[CurveLineChosenR3].x;
    SlopeR = LeastSquareInt16a1(tempslopex, tempslopey, 3);         //已经乘以了LeastSquareMulti倍。
    
    return 1;
  }
  //右黑线提取失败，直接返回失败。
  else
  {
    CURVER = MaxValInt16;      //return 0前先把右黑线的曲率置无效值。   
    SlopeR = MaxValInt16;      //return 0前先把右黑线的斜率置无效值。
    return 0;
  }
  
}





//曲率符号求取
//影响：CurveSL, CurveSC, CurveSR, 0无效，1正，2负
//      SABCL, SABCC, SABCR
uint8 CurveSlopeGet(void)
{
  //左、右黑线的斜曲率求取在BlackGet函数中已经进行过了，相应标志位也已经更新。
  //曲率、斜率求取的标志位，1成功，0失败。默认置为成功。
  CurveSlopeFlagC = 1;  
  
  //中心线的曲率、斜率的求取。
  if(CurveSlopeGetCenter() == 0)
  {
    CurveSlopeFlagC = 0;
    //uart_sendN(UART0, (uint8 *)"\nCurveSlopeGetCenter() Failed!", 30);
  }  
  
  //只有当3条线的曲率、斜率求取均失败的时候，才返回0，意在提醒后面的赛道类型不用判断了。
  if(CurveSlopeFlagL == 0 && CurveSlopeFlagC == 0 && CurveSlopeFlagR == 0)
  {
    return 0;
  }
  else
  {
    return 1;
  }
  
}


//直角线头差判断第6步，非障碍确认。
//其实是效仿障碍判断中的非障碍边确认。
//输入：dir 0表示非左障碍确认，1表示非右障碍确认
//返回：1成功 0失败
uint8 AngleStep1JudgeNotBrick(uint8 dir)
{
  uint8 row;
  uint8 col;
  uint8 count3;
    
  int32 a0, a1;
  uint8 i, index, tempend;
  
  uint8 x[BrickSlopeRowNum];   //取多少个点在一开始有定义。
  uint8 y[BrickSlopeRowNum];
  
  uint8 tempheadrow;
  
  //非左障碍确认
  if(dir == 0)
  {
    //2. 1 非障碍边最小二乘法
    index = 0;    
    for(i = BrickEndRow + 1; ; i++)
    {
      if(BlackLeftLoc[i][0] != MaxValUint8)
      {
        y[index] = BlackLeftLoc[i][0];
        x[index] = i;
        index++;
      }
      if(index == BrickSlopeRowNum)
      {
        break;  
      }
      if(i == CameraHight - 1)
      {
        return 0;
      }
    }
    
    //最小二乘法
    //y = a0 + a1 * x（整体扩大了LeastSquareMulti倍）
    a1 = LeastSquarea1(x, y, BrickSlopeRowNum);
    //左边，斜率应该是-PathJudgeStraightSlopeLimit~0,否则斜率不对，返回失败。
    if(a1 < 0 && a1 + PathJudgeStraightSlopeLimit > 0)
    {
    }
    else
    {
      return 0;
    }
    a0 = LeastSquarea0(x, y, a1, BrickSlopeRowNum);
    //直角梯形的两个突出顶点所在列
    //起始行（近行）黑点所在列。
    AngleStep1NotBrickEndColL = (a0 + a1 * BrickEndRow) / LeastSquareMulti + BrickColCom;
    //直角梯形的直角边所在列。
    //不能比该行有效右黑线还大。    
    if(BlackRightLoc[BrickEndRow][0] != MaxValUint8)
    {
      AngleStep1NotBrickBorderColL = MinRe(AngleStep1NotBrickEndColL + BrickScanColNum, BlackRightLoc[BrickEndRow][0]); //这里与非障碍边不同，非障碍是BrickScanColNum / 2，这里为了保险，取BrickScanColNum。
    }
    else
    {
    }

    //2.2 非障碍边，开始扫描，找到黑点过多的行，则右障碍确认失败。
    //为了防止检测过头，检测的最远行需要跟左线头挂勾。
    //不能比BrickEndRow更近。
    tempheadrow = MinRe(BrickEndRow, BlackLeftHeadLine);
    //不能比BrickHeadRow更远。
    tempheadrow = MaxRe(BrickHeadRow, tempheadrow);    
    for(row = tempheadrow; ; row++)
    {
      //对某一行的处理。
      //该行黑点计数
      count3 = 0;
      //从右往左扫描。
      tempend = (a0 + a1 * row) / LeastSquareMulti + BrickColCom;   //记得除以增加的倍数
      //限幅。
      tempend = MinRe(tempend, AngleStep1NotBrickBorderColL);
      //黑点扫描。
      for(col = AngleStep1NotBrickBorderColL; ; col--)
      {
        if(ImgNew[row][col] < LimitLeftB)  //够黑
        {
          count3++;
        }
        else
        {
        }
        //向左扫描至边界
        if(col <= tempend || col == 0) break;
      }
      //该行黑点个数大于BrickLineBlackCountNum个时，则行计数加1.
      if(count3 > BrickLineBlackCountNum) 
      {
        //存在有行黑点过多，则直接返回失败
        return 0;
      }
      else
      {
      }
      
      if(row == BrickEndRow) break;
    }
    
    //3. 最终确认，返回成功。
    return 1;
       
  }
  //非右障碍确认
  else if(dir == 1)
  {
    //1. 最小二乘法
    index = 0;    
    for(i = BrickEndRow + 1; ; i++)
    {
      if(BlackRightLoc[i][0] != MaxValUint8)
      {
        y[index] = BlackRightLoc[i][0];
        x[index] = i;
        index++;
      }
      if(index == BrickSlopeRowNum)
      {
        break;  
      }
      if(i == CameraHight - 1)
      {
        return 0;
      }
    }
    
    //最小二乘法
    //y = a0 + a1 * x（整体扩大了LeastSquareMulti倍）
    a1 = LeastSquarea1(x, y, BrickSlopeRowNum);
    //右边，斜率应该是0~PathJudgeStraightSlopeLimit,否则斜率不对，返回失败。
    if(a1 > 0 && a1 < PathJudgeStraightSlopeLimit)
    {
    }
    else
    {
      return 0;
    }
    a0 = LeastSquarea0(x, y, a1, BrickSlopeRowNum);
    //直角梯形的两个突出顶点所在列
    //起始行（近行）黑点所在列。
    AngleStep1NotBrickEndColR = (a0 + a1 * BrickEndRow) / LeastSquareMulti - BrickColCom;   //变量有改，不会重叠。
    //直角梯形的直角边所在列。不能比当前行的有效左黑线还要小。
    if(BlackLeftLoc[BrickEndRow][0] != MaxValUint8)
    {
      AngleStep1NotBrickBorderColR = MaxRe(AngleStep1NotBrickEndColR - BrickScanColNum, BlackLeftLoc[BrickEndRow][0]);  //这里与非障碍边不同，非障碍是BrickScanColNum / 2，这里为了保险，取BrickScanColNum。
    }
    else
    {
    }
    
    //2.2 非障碍边，开始扫描，找到黑点过多的行，则左障碍确认失败。进行右障碍扫描。
    //为了防止检测过头，检测的最远行需要跟右线头挂勾。
    //不能比BrickEndRow更近。
    tempheadrow = MinRe(BrickEndRow, BlackRightHeadLine);
    //不能比BrickHeadRow更远。
    tempheadrow = MaxRe(BrickHeadRow, tempheadrow);    
    for(row = tempheadrow; ; row++)
    {
      //对某一行的处理。
      //该行黑点计数，从左往右扫描。
      count3 = 0;
      //当前行的扫描的右边界。
      tempend = (a0 + a1 * row) / LeastSquareMulti - BrickColCom;   //记得除以增加的倍数
      //限幅。
      tempend = MaxRe(tempend, AngleStep1NotBrickBorderColR);
      //黑点扫描。
      for(col = AngleStep1NotBrickBorderColR; ; col++)
      {
        if(ImgNew[row][col] < LimitRightB)  //够黑
        {
          count3++;
        }
        else
        {
        }
        //向右扫描至边界
        if(col >= tempend || col == CameraRealWidth - 1) break;
      }
      //该行黑点个数大于BrickLineBlackCountNum个时，则行计数加1.
      if(count3 > BrickLineBlackCountNum) 
      {
        //存在有行黑点过多，则直接返回失败
        return 0;
      }
      else
      {
      }
      
      if(row == BrickEndRow) break;
    }
    
    //3. 最终确认
    //走到这里说明没有非障碍检测成功，返回成功。
    return 1;
  }
  else
  {
  }

  
  //常态为返回失败。
  return 0;
}



//======================================================================
//函数名：AngleStep1Judge
//功  能：直角弯初步判断
//参  数：
//返  回：1成功，0失败
//影  响：
//说  明：1. 这只是初步判断，需确认多次方可确认为直角弯。
//        2. 其实还可以添加第6步，就是矮线的左右边有黑色区域。
//             
//======================================================================
uint8 AngleStep1Judge(void)
{
  uint8 i, j, col;
  uint8 errorcount;
  uint8 templ, tempr;
  uint8 check_row, leftborder, rightborder;
  uint8 leftbordercal, rightbordercal;
  //由于现在进来之前不需要判断直道，所以需要先做一遍直道做的事情。
  //若判断不准，可以尝试把errorcount的阈值减小。

  //1. 左、右黑线均提取成功
  if(BlackLeftDone == 1 && BlackRightDone == 1)
  {
  }
  else
  {
    return 0;
  }
  
  //进来后进一步需要判断的条件：
  //2. 线头有高度差
  //左线头比右线头高很多，则有可能是右直角弯。
  if (BlackRightHeadLine >= BlackLeftHeadLine + AngleHeadLineNum)
  {
      //3. 左线十字状态未进入或失败（反正不能成功），线尾很近
      //   右线十字状态未进入或失败（反正不能成功），线尾很近
      //BlackLeftCrossDone 0未进入，1失败，2成功
      //BlackLeftStep4Flag 0未进入，1失败，2成功
      if ((BlackLeftCrossDone != 2)
      //&& (BlackLeftEndLine > CameraHight - 5)
      && (BlackRightCrossDone != 2)
      //&& (BlackRightEndLine > CameraHight - 5)
      && (CURVEL <= 30 && CURVEL >= 0 - 30)      //左曲率应该是个绝对值较小的值
      && (CURVER <= 30 && CURVER >= 0 - 40)      //右直角弯 //右曲率应该是个绝对值较小的值
        )
      {
          //3.5 检查右线头处有没有小拐弯，如果有小拐弯，则返回失败。
          if(BlackRightLoc[BlackRightHeadLine][0] > BlackRightLoc[BlackRightHeadLine+1][0] && BlackRightLoc[BlackRightHeadLine+1][0] > BlackRightLoc[BlackRightHeadLine+2][0])
          {
            return 0;
          }
          else
          {
          }
          
          //4. 两线5Slope数组均很小
          //左线
          //考虑到了Black5Slope数组的个数与有效黑点不一定相同，大大减少了误判的机率。
          //直角弯长线头策略，1表示去掉长线线头。
          if(AngleLongHeadLineStrategy == 1)
          {
            if(BlackLeftEndLine - BlackLeftHeadLine - 4 - AngleHeadLineNum < 0 || BlackLeft5SlopeIndex - 1 - AngleHeadLineNum < 0)
            {
              return 0;
            }
            else
            {        
            }
            templ = MinRe(BlackLeftEndLine - BlackLeftHeadLine - 4, BlackLeft5SlopeIndex - 1);               
          }
          //0表示不去掉。
          else
          {
            if(BlackLeftEndLine - BlackLeftHeadLine - 4 < 0 || BlackLeft5SlopeIndex - 1 < 0)
            {
              return 0;
            }
            else
            {        
            }
            templ = MinRe(BlackLeftEndLine - BlackLeftHeadLine - 4, BlackLeft5SlopeIndex - 1);            
          }
          //错误计数清零
          errorcount = 0;
          //逐个判断小5点斜率是否满足条件
          for(i = 0; ; i++)
          {
            if(BlackLeft5Slope[i] > 0 || BlackLeft5Slope[i] < 0 - PathJudgeStraightSlopeLimit) //右上到左下的斜率是负的。 
            {
              errorcount++;
              if(errorcount > 3) break;   //允许有若干个点斜率超出范围
            }
      
            if(i == templ) break;
          }
          //不满足条件，返回失败。
          if(i != templ) return 0;
          
          //右线
          //考虑到了Black5Slope数组的个数与有效黑点不一定相同，大大减少了误判的机率。
          if(BlackRightEndLine - BlackRightHeadLine - 4 < 0 || BlackRight5SlopeIndex - 1 < 0)
          {
            return 0;
          }
          else
          {
          }
          tempr = MinRe(BlackRightEndLine - BlackRightHeadLine - 4, BlackRight5SlopeIndex - 1);
          //错误计数清零
          errorcount = 0;
          //逐个判断小5点斜率是否满足条件
          for(i = 0; ; i++)
          {
            if(BlackRight5Slope[i] < 0 || BlackRight5Slope[i] > PathJudgeStraightSlopeLimit) //左上到右下的斜率是正的。
            {
              errorcount++;
              if(errorcount > 3) break;   //允许有若干个点斜率超出范围
            }
      
            if(i == tempr) break;
          }
          //不满足条件，返回失败。
          if(i != tempr) return 0;
          
          //若左右两线都满足要求，会继续往下走。

          //5. 对右线头往上一些点进行白区域检测。
          //5.1. 一竖
          col = BlackRightLoc[BlackRightHeadLine][0];   //这里采用原始的黑点，不能采用矫正后的。
          for(i = BlackRightHeadLine-3; i > (BlackRightHeadLine-AngleHeadLineNum); i--)
          {
            //与右线白点阈值进行比较，必须大于白点阈值。
            if(ImgNew[i][col] < LimitRightW - AngleWhiteAreaLimitVal)
            {
              flag222 = 10;
              //只要有一个点不满足，即判定不是直角弯。
              break;
            }
            else
            {
              flag222 = 0;
            }
          }
          //从下到上一直满足白点，且最上与最下两个点像素差值够小。
          if(
            (i <= BlackRightHeadLine-AngleHeadLineNum)
          &&(AbsInt(ImgNew[BlackRightHeadLine-3][col] - ImgNew[BlackRightHeadLine-AngleHeadLineNum][col]) < 2 * AngleWhiteAreaLimitVal)
            )
          {
            //5.2. 一横
            check_row = BlackRightHeadLine - AngleHeadLineNum / 2;
            //一横的左边界
            //不能小于图像左边界
            leftborder = MaxRe(0, BlackRightLoc[BlackRightHeadLine][0] - 15);
            leftbordercal = MaxRe(0, BlackRightLoc[BlackRightHeadLine][0] - 15);
            //不能比该行有效左黑线小。
            if(BlackLeftLoc[check_row][0] != MaxValUint8)
            {
              leftborder = MaxRe(BlackLeftLoc[check_row][0], leftborder);
              leftbordercal = MaxRe(BlackLeftLoc[check_row][0], leftbordercal);
            }
            else
            {
            } 
            //一横的右边界。
            rightborder = MinRe(CameraRealWidth - 1, BlackRightLoc[BlackRightHeadLine][0] + 15);
            rightbordercal = MinRe(CameraRealWidth - 1, BlackRightLoc[BlackRightHeadLine][0] + 15);
            //从左到右进行白点检查
            for(j = leftborder; j < rightborder; j++)
            {
              if(ImgNew[check_row][j] < LimitRightW - AngleWhiteAreaLimitVal) break;              
            }
            //从左到右一直满足白点，且最左最右两点差值够小，进入第6步。
            if(
              (j >= rightborder) 
            &&(AbsInt(ImgNew[check_row][leftbordercal] - ImgNew[check_row][rightbordercal]) < 2 * AngleWhiteAreaLimitVal) //隔的点比较多，允许差值稍微大一些。
              )
            {
              //6. 非右障碍确认成功，则最终确认是右直角。
              if(AngleStep1JudgeNotBrick(1) == 1)
              {
                PathType = PathTypeAngleR;
              }
              else
              {
              }
            }
            else
            {
              
            }           
          }
          else
          {            
          }
      }
      //否则仍为直道
      else
      {
      }

  }
  //右线头比左线头高很多，则有可能是左直角弯。
  else if (BlackLeftHeadLine >= BlackRightHeadLine + AngleHeadLineNum)
  {
      //3. 左线十字状态未进入或失败（反正不能成功），线尾很近
      //   右线十字状态未进入或失败（反正不能成功），线尾很近
      //BlackLeftCrossDone 0未进入，1失败，2成功
      //BlackLeftStep4Flag 0未进入，1失败，2成功
      if ((BlackLeftCrossDone != 2)
      //&& (BlackLeftEndLine > CameraHight - 5)
      && (BlackRightCrossDone != 2)
      //&& (BlackRightEndLine > CameraHight - 5)
      && (CURVEL <= 40 && CURVEL >= 0 - 30)      //左直角弯//左曲率应该是个绝对值较小的值
      && (CURVER <= 30 && CURVER >= 0 - 30)      //右曲率应该是个绝对值较小的值
        )
      {
          //3.5 检查左线头处有没有小拐弯，如果有小拐弯，则返回失败。
          if(BlackLeftLoc[BlackLeftHeadLine][0] < BlackLeftLoc[BlackLeftHeadLine+1][0] && BlackLeftLoc[BlackLeftHeadLine+1][0] < BlackLeftLoc[BlackLeftHeadLine+2][0])
          {
            return 0;
          }
          else
          {
          }

          //4. 两线5Slope数组均很小
          //左线
          //考虑到了Black5Slope数组的个数与有效黑点不一定相同，大大减少了误判的机率。
          if(BlackLeftEndLine - BlackLeftHeadLine - 4 < 0 || BlackLeft5SlopeIndex - 1 < 0)
          {
            return 0;
          }
          else
          {        
          }
          templ = MinRe(BlackLeftEndLine - BlackLeftHeadLine - 4, BlackLeft5SlopeIndex - 1);
          //错误计数清零
          errorcount = 0;
          //逐个判断小5点斜率是否满足条件
          for(i = 0; ; i++)
          {
            if(BlackLeft5Slope[i] > 0 || BlackLeft5Slope[i] < 0 - PathJudgeStraightSlopeLimit) //右上到左下的斜率是负的。 
            {
              errorcount++;
              if(errorcount > 3) break;   //允许有若干个点斜率超出范围
            }
      
            if(i == templ) break;
          }
          //不满足条件，返回失败。
          if(i != templ) return 0;
          
          //右线
          //考虑到了Black5Slope数组的个数与有效黑点不一定相同，大大减少了误判的机率。
          //直角弯线头差策略，1表示去掉长线的线头。
          if(AngleLongHeadLineStrategy == 1)
          {
            if(BlackRightEndLine - BlackRightHeadLine - 4 - AngleHeadLineNum < 0 || BlackRight5SlopeIndex - 1 - AngleHeadLineNum < 0)
            {
              return 0;
            }
            else
            {
            }
            tempr = MinRe(BlackRightEndLine - BlackRightHeadLine - 4 - AngleHeadLineNum, BlackRight5SlopeIndex - 1 - AngleHeadLineNum);
          }
          //0表示不去掉长线的线头。
          else
          {
            if(BlackRightEndLine - BlackRightHeadLine - 4 < 0 || BlackRight5SlopeIndex - 1 < 0)
            {
              return 0;
            }
            else
            {
            }
            tempr = MinRe(BlackRightEndLine - BlackRightHeadLine - 4, BlackRight5SlopeIndex - 1);
            
          }
          //错误计数清零
          errorcount = 0;
          //逐个判断小5点斜率是否满足条件
          for(i = 0; ; i++)
          {
            if(BlackRight5Slope[i] < 0 || BlackRight5Slope[i] > PathJudgeStraightSlopeLimit) //左上到右下的斜率是正的。
            {
              errorcount++;
              if(errorcount > 3) break;   //允许有若干个点斜率超出范围
            }
      
            if(i == tempr) break;
          }
          //不满足条件，返回失败。
          if(i != tempr) return 0;
          
          //若左右两线都满足要求，会继续往下走。        

          //5. 对左线头往上一些点进行白区域检测。
          //5.1. 一竖
          col = BlackLeftLoc[BlackLeftHeadLine][0];   //这里采用原始的黑点，不能采用矫正后的。
          for(i = BlackLeftHeadLine-3; i > (BlackLeftHeadLine-AngleHeadLineNum); i--)
          {
            //与右线白点阈值进行比较，必须大于白点阈值。
            if(ImgNew[i][col] < LimitLeftW - AngleWhiteAreaLimitVal)
            {
              flag222 = 10;
              //只要有一个点不满足，即判定不是直角弯。
              break;
            }
            else
            {
              flag222 = 0;
            }
          }
          //从下到上一直满足白点，且最上与最下两个点像素差值够小。
          if(
            (i <= BlackLeftHeadLine-AngleHeadLineNum)
          &&(AbsInt(ImgNew[BlackLeftHeadLine-3][col] - ImgNew[BlackLeftHeadLine-AngleHeadLineNum][col]) < 2 * AngleWhiteAreaLimitVal) //隔的点比较多，允许差值稍微大一些。
            )
          {
            //5.2. 一横
            check_row = BlackLeftHeadLine - AngleHeadLineNum / 2;
            //一横的左边界
            //不能小于图像左边界
            leftborder = MaxRe(0, BlackLeftLoc[BlackLeftHeadLine][0] - 15);
            leftbordercal = MaxRe(0, BlackLeftLoc[BlackLeftHeadLine][0] - 15);
            //一横的右边界。
            rightborder = MinRe(CameraRealWidth - 1, BlackLeftLoc[BlackLeftHeadLine][0] + 15);
            rightbordercal = MinRe(CameraRealWidth - 1, BlackLeftLoc[BlackLeftHeadLine][0] + 15);
            //不能比该行有效右黑线还大
            if(BlackRightLoc[check_row][0] != MaxValUint8)
            {
              rightborder = MinRe(BlackRightLoc[check_row][0], rightborder);
              rightbordercal = MinRe(BlackRightLoc[check_row][0], rightbordercal);
            }
            else
            {
            }
            //从左到右进行白点检查
            for(j = leftborder; j < rightborder; j++)
            {
              if(ImgNew[check_row][j] < LimitLeftW - AngleWhiteAreaLimitVal) break;              
            }
            //从左到右一直满足白点，且最左最右两点差值够小，进入第6步。
            if(
              (j >= rightborder) 
            &&(AbsInt(ImgNew[check_row][leftbordercal] - ImgNew[check_row][rightbordercal]) < 2 * AngleWhiteAreaLimitVal)
              )
            {
              //6. 非左障碍确认成功，则最终确认是左直角。
              if(AngleStep1JudgeNotBrick(0) == 1)
              {
                PathType = PathTypeAngleL;
              }
              else
              {
              }
            }
            else
            {
            }            
          }
          else
          {
          }
      }
      //否则仍为直道
      else
      {
      }
  }
  //否则保持直道不变。
  else
  {
  }
  return 1;
  
}




//======================================================================
//函数名：PathLCRAngle1Judge
//功  能：直道、左弯、右弯判断，直角弯第1阶段预判。
//参  数：
//返  回：1成功，0失败。
//影  响：
//说  明：1. 只有在斜曲率求取失败时才会返回0，否则只会返回1. 
//        2. 在左右黑线都求取成功的时候，会进行直角弯第1阶段预判。
//             
//======================================================================
uint8 PathLCRAngle1Judge(void)
{
  
  //ErrorGet();
  int32 AbsSlopeC;
  uint8 curvetempLL, curvetempLR, curvetempRL, curvetempRR;
  uint8 slopetempLL, slopetempLR, slopetempRL, slopetempRR;
  
  //以下是一套以斜率为主导的赛道类型判断方法

  //赛道类型PathType: 0未知 1直道 2左内 3左中 4左外 5右内 6右中 7右外
  PathType = PathTypeUnknown;
  //曲率符号获取成功
  //如果CurveSlopeGet()返回0，则代表3条线的曲率、斜率均求取失败，不用进行下面的判断。
  if(CurveSlopeGet() == 1)
  {
    //先将绝对值求出来，后面就不用反复使用了。
    AbsSlopeC = AbsInt(SlopeC);
    //AbsSlopeL = AbsSelfFloat(SlopeL);
    //AbsSlopeR = AbsSelfFloat(SlopeR);
    //不用判断中心线成功，到这里了肯定是成功的。
    //此时中心线的曲率、斜率一定存在，左线或者右线不一定。
    //下面用来“或”的时候用,对于曲率来说，左负右正。对于斜率来说，左正右负。
    curvetempRL = (CurveSlopeFlagL == 1 && (CURVEL > CurveStraightLimitL));        //如果该线曲率无效，就为0，如果有效，就有可能为1.用以判断左线曲率是否朝右。
    curvetempRR = (CurveSlopeFlagR == 1 && (CURVER > CurveStraightLimitR));        //如果该线曲率无效，就为0，如果有效，就有可能为1.用以判断右线曲率是否朝右。
    curvetempLL = (CurveSlopeFlagL == 1 && (CURVEL + CurveStraightLimitL < 0));    //如果该线曲率无效，就为0，如果有效，就有可能为1.用以判断左线曲率是否朝左。          
    curvetempLR = (CurveSlopeFlagR == 1 && (CURVER + CurveStraightLimitR < 0));    //如果该线曲率无效，就为0，如果有效，就有可能为1.用以判断右线曲率是否朝左。  
    slopetempRL = (CurveSlopeFlagL == 1 && (SlopeL + SlopeLimitL < 0));            //如果该线斜率无效，就为0，如果有效，就有可能为1.用以判断左线斜率是否朝右。
    slopetempRR = (CurveSlopeFlagR == 1 && (SlopeR + SlopeLimitR < 0));            //如果该线斜率无效，就为0，如果有效，就有可能为1.用以判断右线斜率是否朝右。
    slopetempLL = (CurveSlopeFlagL == 1 && (SlopeL > SlopeLimitL));                //如果该线斜率无效，就为0，如果有效，就有可能为1.用以判断左线斜率是否朝左。
    slopetempLR = (CurveSlopeFlagR == 1 && (SlopeR > SlopeLimitR));                //如果该线斜率无效，就为0，如果有效，就有可能为1.用以判断右线斜率是否朝左。
    //下面用来“与”的时候用
    /*
    tempRL = ((CurveSlopeFlagL && (CURVEL > CurveStraightLimitL)) //如果该线曲率有效，则该逻辑就有可能是0。
          ||(!CurveSlopeFlagL)                                   //如果该线曲率无效，则该逻辑就一定是1.
            );
    tempRR = ((CurveSlopeFlagR && (CURVER > CurveStraightLimitR)) //如果该线曲率有效，则该逻辑就有可能是0。
          ||(!CurveSlopeFlagR)                                   //如果该线曲率无效，则该逻辑就一定是1.
            );
    tempLL = ((CurveSlopeFlagL && (CURVEL + CurveStraightLimitL < 0)) //如果该线曲率有效，则该逻辑就有可能是0。
          ||(!CurveSlopeFlagL)                                   //如果该线曲率无效，则该逻辑就一定是1.
            );
    tempLR = ((CurveSlopeFlagR && (CURVER + CurveStraightLimitR < 0)) //如果该线曲率有效，则该逻辑就有可能是0。
          ||(!CurveSlopeFlagR)                                   //如果该线曲率无效，则该逻辑就一定是1.
            );
    */
      //1. 左右黑线均提取成功的情况。
      if(BlackLeftDone == 1 && BlackRightDone == 1)
      {
      //1.1. 中心线斜率足够正。这一步避免了270度弯曲率小斜率大的判断失误问题。
      if(AbsSlopeC <= SlopeLimitC)
      {
        //1.1.1. 左、右线曲率里只要有一条比较大，且向右，那么判断为右弯。        
        if(
           //(CURVEC > CurveStraightLimitC)
            (curvetempRL)
         || (curvetempRR)
           )
        {//1.1.1.1. 左右黑线斜率里只要有一条比较大，且向右，那么判断为右弯。
          if((slopetempRL)
          || (slopetempRR)
              )
          {
            PathType = PathTypeRight;
          }
          //1.1.1.2. 左右黑线斜率都不大，就算曲率比较大，也判断为直道。
          else
          {
              PathType = PathTypeStraight;
          }
          
        }
        //1.1.2. 左、右线曲率里只要有一条比较大，且向左，那么判断为左弯。
        else if(
               //(CURVEC + CurveStraightLimitC < 0)
                 (curvetempLL)
              || (curvetempLR)
                )
        {   //1.1.2.1. 左右黑线斜率里只要有一条比较大，且向左，那么判断为左弯。
            if((slopetempLL)
            || (slopetempLR)
              )
            {
                PathType = PathTypeLeft;
            }
            //1.1.2.2. 左右黑线斜率都不大，就算斜率比较大，也判断为直道。
            else
            {
                PathType = PathTypeStraight;
            }         
        }
        //1.1.3. 左、右线曲率都不大，那么就判断为直道。
        else
        {
          PathType = PathTypeStraight;
         
        }
      }
      //1.2. 斜率向左偏，最有可能是左道。对于斜率来说，向左是正，向右是负。
      else if(SlopeC > SlopeLimitC)
      { //1.2.1. 三线只要有一个曲率比较大，且向左，就判断为左道。
        //左右黑线斜率里只要有一条比较大，且向左，那么判断为左弯。
        if(
           //(CURVEC + CurveStraightLimitC < 0)
           (curvetempLL)
        || (curvetempLR)
        || (slopetempLL)
        || (slopetempLR)
          )
        {
          PathType = PathTypeLeft;
        }
        //1.2.2. 否则就是直道。
        else
        {
          PathType = PathTypeStraight;
        }
      }
      //1.3. 斜率向右偏，最有可能是右道。
      else if(SlopeC + SlopeLimitC < 0)
      {
        //1.3.1. 三线只要有一个曲率比较大，且向右，就判断为右道。
        //左右黑线斜率里只要有一条比较大，且向右，那么判断为右弯。
        if(
           //(CURVEC > CurveStraightLimitC)
            (curvetempRL)
         || (curvetempRR)
         || (slopetempRL)
         || (slopetempRR)
           )
        {
          PathType = PathTypeRight;
        }
        //1.3.2. 否则就是直道。
        else
        {
          PathType = PathTypeStraight;
        }        
      }
      //1.4. 理论上不会走到这里。
      else
      {
      }
      
      //若判断为直道，则启动直角弯单独检测。
      if(PathType == PathTypeStraight)
      {
        AngleStep1Judge();
      }
      else
      {
      }   
      
    }
    //2. 左线提取成功，右线提取失败
    else if(BlackLeftDone == 1 && BlackRightDone == 0)
    {
      
      //2.1. 中心线斜率足够正。
      if(AbsSlopeC <= SlopeLimitC)
      {
        //2.1.1. 中左线曲率里只要有一条比较大，且向右，那么判断为右弯。        
        if((CURVEC > CurveStraightLimitC)
         || (curvetempRL)
           )
        {   //2.1.1.1. 若左线斜率较大，且向右，那么判断为右弯。
            if (slopetempRL)
            {
                PathType = PathTypeRight;
            }
            //2.1.1.2. 若左线斜率不大，就算曲率较大，也判断为直道。
            else
            {
                PathType = PathTypeStraight;
            }
          
        }
        //2.1.2. 中左线曲率里只要有一条比较大，且向左，那么判断为左弯。
        else if((CURVEC + CurveStraightLimitC < 0)
              || (curvetempLL)
                )
        {   //2.1.2.1. 若左线斜率较大，且向左，那么有判断为左弯。
            if (slopetempLL)
            {
                PathType = PathTypeLeft;
            }
            //2.1.2.2. 若在线斜率不大，就算曲率较大，也判断为直道。
            else
            {
                PathType = PathTypeStraight;
            }          
        }
        //2.1.3. 曲率都不大，最大值比较小。那么就判断为直道。
        else
        {
          PathType = PathTypeStraight;
        }
      }
      //2.2. 斜率向左偏，最有可能是左道。对于斜率来说，向左是正，向右是负。
      else if(SlopeC > SlopeLimitC)
      { //2.2.1. 中左线只要有一个曲率比较大，且向左，就判断为左道。
        //若左线斜率较大，且向左，就判断为左道。
        if((CURVEC + CurveStraightLimitC < 0)
        || (curvetempLL)
        || (slopetempLL)
          )
        {
          PathType = PathTypeLeft;
        }
        //2.2.2. 否则就是直道。
        else
        {
          PathType = PathTypeStraight;
        }
      }
      //2.3. 斜率向右偏，最有可能是右道。
      else if(SlopeC + SlopeLimitC < 0)
      {
        //2.3.1. 中左线只要有一个曲率比较大，且向右，就判断为右道。
        //若左线斜率较大，且向右，就判断为右道。
        if((CURVEC > CurveStraightLimitC)
         || (curvetempRL)
         || (slopetempRL)
           )
        {
          PathType = PathTypeRight;
        }
        //2.3.2. 否则就是直道。
        else
        {
          PathType = PathTypeStraight;
        }        
      }
      //2.4. 理论上不会走到这里。
      else
      {
      } 
    }
    //3. 左线提取失败，右线提取成功
    else if(BlackLeftDone == 0 && BlackRightDone == 1)
    {             
      //3.1. 中心线斜率足够正。
      if(AbsSlopeC <= SlopeLimitC)
      { 
        //3.1.1. 中右线曲率里只要有一条比较大，且向右，那就判断为右弯。        
        if((CURVEC > CurveStraightLimitC)
         || (curvetempRR)
           )
        {   //3.1.1.1. 如果右线斜率较大，且向右，那就判断为右弯。
            if (slopetempRR)
            {
                PathType = PathTypeRight;
            }
            //3.1.1.2. 如果右线斜率不大，就算曲率较大，仍判断为直道。
            else
            {
                PathType = PathTypeStraight;
            }
          
        }
        //3.1.2. 中右线曲率里只要有一条比较大，且向左，那么有可能是左弯。
        else if((CURVEC + CurveStraightLimitC < 0)
              || (curvetempLR)
                )
        {   //3.1.2.1. 如果右线斜率较大，且向左，那就判断为左弯。
            if (slopetempLR)
            {
                PathType = PathTypeLeft;
            }
            //3.1.2.2. 如果右线斜率不大，就算曲率较大，也判断为直道。
            else
            {
                PathType = PathTypeStraight;
            }          
        }
        //3.1.3. 曲率都不大，最大值比较小。那么就判断为直道。
        else
        {
          PathType = PathTypeStraight;
        }
      }
      //3.2. 斜率向左偏，最有可能是左道。对于斜率来说，向左是正，向右是负。
      else if(SlopeC > SlopeLimitC)
      { //3.2.1. 中右线只要有一个曲率比较大，且向左，就判断为左道。
        //如果右线斜率较大，且向左，那就判断为左弯。
        if((CURVEC + CurveStraightLimitC < 0)
        || (curvetempLR)
        || (slopetempLR)
          )
        {
          PathType = PathTypeLeft;
        }
        //3.2.2. 否则就是直道。
        else
        {
          PathType = PathTypeStraight;
        }
      }
      //3.3. 斜率向右偏，最有可能是右道。
      else if(SlopeC + SlopeLimitC < 0)
      {
        //3.3.1. 中右线只要有一个曲率比较大，且向右，就判断为右道。
        //如果右线斜率较大，且向右，那就判断为右弯。
        if((CURVEC > CurveStraightLimitC)
         || (curvetempRR)
         || (slopetempRR)
           )
        {
          PathType = PathTypeRight;
        }
        //3.3.2. 否则就是直道。
        else
        {
          PathType = PathTypeStraight;
        }        
      }
      //3.4. 理论上不会走到这里。
      else
      {
      }      
    }
    //4. 理论上不会出现这种情况。
    else 
    {
    }
    
    return 1;
    //直道、左弯、右弯、判断完毕，直角弯预判完毕。
  }
  //3条线的曲率、斜率均求取失败，直接返回失败。
  else
  {
    PathType = PathTypeUnknown;  
    return 0;
  }

}


//======================================================================
//函数名：PathSmallSJudgeStep2
//功  能：赛道类型判断时，小S弯第2个拐角的判断。
//参  数：row需要处理的前一行，sel 左线0 右线1，dir Z字形0 反Z字形1.
//返  回：1成功，0失败。
//影  响：
//说  明：1. 第2个拐角判断成功，则返回1。否则返回0.
//        2. 进入该函数前，黑线已经连续。
//             
//======================================================================
uint8 PathSmallSJudgeStep2(uint8 row, uint8 sel, uint8 dir)
{
  uint8 i;
  
  //左线
  if(sel == 0)
  {
    //Z字形
    if(dir == 0)
    {
      for(i = row + 1; i < BlackLeftEndLine - PathSmallSGapLine; i++)
      {
        //Z字形，需要检测从左上到右下的轨迹。
        if(BlackLeftLoc[i][0] < BlackLeftLoc[i + 1 + PathSmallSGapLine][0])
        {
          return 1;
        }
        else
        {
        }
      }
    }
    //反Z字形
    else
    {
      for(i = row + 1; i < BlackLeftEndLine - PathSmallSGapLine; i++)
      {
        //反Z字形，需要检测从右上到左下的轨迹。
        if(BlackLeftLoc[i][0] >= BlackLeftLoc[i + 1 + PathSmallSGapLine][0])
        {
          return 1;
        }
        else
        {
        }
      }
    }
    
    //一直到有效黑线末尾仍未检测到相应的轨迹，第2个拐角检测失败，返回0。
    return 0;
  }
  //右线
  else
  {
    //Z字形
    if(dir == 0)
    {
      for(i = row + 1; i < BlackRightEndLine - PathSmallSGapLine; i++)
      {
        //Z字形，需要检测从左上到右下的轨迹。
        if(BlackRightLoc[i][0] < BlackRightLoc[i + 1 + PathSmallSGapLine][0])
        {
          return 1;
        }
        else
        {
        }
      }
    }
    //反Z字形
    else
    {
      for(i = row + 1; i < BlackRightEndLine - PathSmallSGapLine; i++)
      {
        //反Z字形，需要检测从右上到左下的轨迹。
        if(BlackRightLoc[i][0] >= BlackRightLoc[i + 1 + PathSmallSGapLine][0])
        {
          return 1;
        }
        else
        {
        }
      }
    }
    
    //一直到有效黑线末尾仍未检测到相应的轨迹，第2个拐角检测失败，返回0。
    return 0;
  }

}


//======================================================================
//函数名：PathSmallSJudgeStep1
//功  能：赛道类型判断时，小S弯第1个拐角的判断。
//参  数：
//返  回：1成功，0失败。
//影  响：
//说  明：1. 返回1或0只是表示是否为小S弯。
//        2. 进入该函数前，黑线已经连续。
//             
//======================================================================
uint8 PathSmallSJudgeStep1(void)
{
    uint8 i;
    uint8 signflag1 = 2;    //初始为2，不为0也不为1.
    //1. 线头够高。2. 从下往下的差会出现变化。

    //1. 左右黑线均提取成功。
    if (BlackLeftDone == 1 && BlackRightDone == 1)
    {
        //1.1. 左线线头够高。
        if (
            (BlackLeftHeadLine < PathSmallSHeadLimit) 
         && (BlackLeftEndLine > BlackLeftHeadLine + PathSmallSGapLine + 1)
            )
        {
            //初始为2，不为0也不为1.
            signflag1 = 2;
            
            //1.2. 两线有正到负 或者 负到正 的变化。 
            //左线
            for (i = BlackLeftHeadLine; i < BlackLeftEndLine - PathSmallSGapLine; i++)
            {
                //赛道右向。若检测到跳变，中途会return.
                if (BlackLeftLoc[i][0] >= BlackLeftLoc[i + 1 + PathSmallSGapLine][0])
                {
                    //检测到跳变
                    if (signflag1 == 0)
                    {
                        //进行小S弯的第2个拐角的识别。左线，Z字形。
                        if(PathSmallSJudgeStep2(i, 0, 0) == 1)
                        {
                          PathSmallSDoneFlag = 1;   //小S弯判断成功。
                          PathSmallSDir = 1;    //小S弯先右向。
                          PathType = PathTypeSmallSR;   //右S
                          return 1;
                        }
                        else
                        {
                          //左线第2个拐角没有检测成功，没关系，下面会进行右线的检测。
                          //return 0;
                        }
                    }
                    else
                    {
                    }
                    signflag1 = 1;
                }
                //赛道左向。
                else
                {   
                    //检测到跳变。
                    if (signflag1 == 1)
                    { 
                        //进行小S弯的第2个拐角的识别。左线，反Z字形。
                        if(PathSmallSJudgeStep2(i, 0, 1) == 1)
                        {
                          PathSmallSDoneFlag = 1;   //小S弯判断成功。
                          PathSmallSDir = 0;    //小S弯先左向。
                          PathType = PathTypeSmallSL;  //左S
                          return 1;
                        }
                        else
                        {
                          //左线第2个拐角没有检测成功，没关系，下面会进行右线的检测。
                          //return 0;
                        }
                    }
                    else
                    {
                    }
                    signflag1 = 0;
                }
            }
            
            //到这里说明左线不成功，但没关系，继续分析右线。
        }
        //左线线头不够高，则直接跳过，分析右线。
        else
        {
        }
        
        //右线线头要够高。
        if(
            (BlackRightHeadLine < PathSmallSHeadLimit)
         && (BlackRightEndLine > BlackRightHeadLine + PathSmallSGapLine + 1)
           )
        {
          //初始为2，不为0也不为1.
          signflag1 = 2;

          //右线。左线若没有return，就继续找跳变。
          for (i = BlackRightHeadLine; i < BlackRightEndLine - PathSmallSGapLine; i++)
          {
              //赛道右向。
              if (BlackRightLoc[i][0] >= BlackRightLoc[i + 1 + PathSmallSGapLine][0])
              {
                  //检测到跳变
                  if (signflag1 == 0)
                  {
                      //进行小S弯的第2个拐角的识别。右线，Z字形。
                      if(PathSmallSJudgeStep2(i, 1, 0) == 1)
                      {
                        PathSmallSDoneFlag = 1;   //小S弯判断成功。
                        PathSmallSDir = 1;    //小S弯先右向。
                        PathType = PathTypeSmallSR;   //右S
                        return 1;
                      }
                      else
                      {
                        return 0;
                      }
                  }
                  else
                  {
                  }
                  signflag1 = 1;
              }
              //赛道左向。
              else
              {
                  //检测到跳变。
                  if (signflag1 == 1)
                  {
                      //进行小S弯的第2个拐角的识别。右线，反Z字形。
                      if(PathSmallSJudgeStep2(i, 1, 1) == 1)
                      {
                        PathSmallSDoneFlag = 1;   //小S弯判断成功。
                        PathSmallSDir = 0;    //小S弯先左向。
                        PathType = PathTypeSmallSL;   //左S
                        return 1;
                      }
                      else
                      {
                        return 0;
                      }
                  }
                  else
                  {
                  }
                  signflag1 = 0;
              }
          }

          //若左右线中途都没有return 1，则返回检测失败。
          return 0;
        }
        else
        {
          //右线线头过低，不符合小S弯。返回失败。
          return 0;
        }
    }
    //2. 左线提取成功，右线提取失败。
    else if (BlackLeftDone == 1 && BlackRightDone == 0)
    {
        //2.1. 左线线头要求够高。
        if (BlackLeftHeadLine < PathSmallSHeadLimit && BlackLeftEndLine > BlackLeftHeadLine + PathSmallSGapLine + 1)
        {
            //2.2. 检测跳变。
            for (i = BlackLeftHeadLine; i < BlackLeftEndLine - PathSmallSGapLine; i++)
            {

                //赛道右向。若检测到跳变，中途会return.
                if (BlackLeftLoc[i][0] >= BlackLeftLoc[i + 1 + PathSmallSGapLine][0])
                {
                    //检测到跳变
                    if (signflag1 == 0)
                    {                           
                        //进行小S弯的第2个拐角的识别。左线，Z字形。
                        if(PathSmallSJudgeStep2(i, 0, 0) == 1)
                        {
                          PathSmallSDoneFlag = 1;   //小S弯判断成功。
                          PathSmallSDir = 1;    //小S弯先右向。
                          PathType = PathTypeSmallSR;   //右S
                          return 1;
                        }
                        else
                        {
                          return 0;
                        }
                    }
                    else
                    {
                    }
                    signflag1 = 1;
                }
                //赛道左向。
                else
                {
                    //检测到跳变。
                    if (signflag1 == 1)
                    {                           
                        //进行小S弯的第2个拐角的识别。左线，反Z字形。
                        if(PathSmallSJudgeStep2(i, 0, 1) == 1)
                        {
                          PathSmallSDoneFlag = 1;   //小S弯判断成功。
                          PathSmallSDir = 0;    //小S弯先左向。
                          PathType = PathTypeSmallSL;  //左S
                          return 1;
                        }
                        else
                        {
                          return 0;
                        }
                    }
                    else
                    {
                    }
                    signflag1 = 0;
                }
            }

            //若左线中途没有return，则返回检测失败。
            return 0;
        }
        //线头过低，不符合小S弯。返回失败。
        else
        {
            return 0;
        }
    }
    //3. 右线提取成功，左线提取失败。
    else if (BlackLeftDone == 0 && BlackRightDone == 1)
    {
        //3.1. 两线线头都要够高。
        if (BlackRightHeadLine < PathSmallSHeadLimit && BlackRightEndLine > BlackRightHeadLine + PathSmallSGapLine + 1)
        {
            //右线。
            for (i = BlackRightHeadLine; i < BlackRightEndLine - PathSmallSGapLine; i++)
            {
                  //赛道右向。
                  if (BlackRightLoc[i][0] >= BlackRightLoc[i + 1 + PathSmallSGapLine][0])
                  {
                      //检测到跳变
                      if (signflag1 == 0)
                      {                           
                          //进行小S弯的第2个拐角的识别。右线，Z字形。
                          if(PathSmallSJudgeStep2(i, 1, 0) == 1)
                          {
                            PathSmallSDoneFlag = 1;   //小S弯判断成功。
                            PathSmallSDir = 1;    //小S弯先右向。
                            PathType = PathTypeSmallSR; //右S
                            return 1;
                          }
                          else
                          {
                            return 0;
                          }
                      }
                      else
                      {
                      }
                      signflag1 = 1;
                  }
                  //赛道左向。
                  else
                  {
                      //检测到跳变。
                      if (signflag1 == 1)
                      {                           
                          //进行小S弯的第2个拐角的识别。右线，反Z字形。
                          if(PathSmallSJudgeStep2(i, 1, 1) == 1)
                          {
                            PathSmallSDoneFlag = 1;   //小S弯判断成功。
                            PathSmallSDir = 0;    //小S弯先左向。
                            PathType = PathTypeSmallSL;  //左S
                            return 1;
                          }
                          else
                          {
                            return 0;
                          }
                      }
                      else
                      {
                      }
                      signflag1 = 0;
                  }
            }

            //若右线中途没有return，则返回检测失败。
            return 0;
        }
        //线头过低，不符合小S弯。返回失败。
        else
        {
            return 0;
        }

    }
    //4. 理论上不会走到这里。
    else
    {
        return 0;
    }

}



//赛道类型判断
//======================================================================
//函数名：PathJudge
//功  能：赛道类型判断
//参  数：无
//返  回：1成功 0失败
//影  响：PathType
//说  明：1. 根据曲率和斜率来判断。
//        2. PathType: 0未知 1直道 2左内 3左中 4左外 5右内 6右中 7右外
//             
//======================================================================
uint8 PathJudge(void)
{  
  uint8 flag1, flag2;
  
  
  //直道、左弯、右弯判断，直角弯第1阶段预判。
  //当斜曲率求取失败时，会返回0.
  flag1 = PathLCRAngle1Judge();

  //小S弯判断。非小S弯会返回0.
  //flag2 = PathSmallSJudgeStep1();
  flag2 = 1;
  
  if(flag1 == 0 && flag2 == 0)
  {
    return 0;
  }
  else
  {
    return 1;
  }
  
  //以下是一套以曲率为主导的赛道类型判断方法。
  /*
  //赛道类型PathType: 0未知 1直道 2左内 3左中 4左外 5右内 6右中 7右外
  PathType = PathTypeUnknown;
  //曲率符号获取成功
  //如果CurveSlopeGet()返回0，则代表3条线的曲率、斜率均求取失败，不用进行下面的判断。
  if(CurveSlopeGet())
  {
    //不用判断中心线成功，到这里了肯定是成功的。
    //左右黑线均提取成功，的情况。
    if(BlackLeftDone == 1 && BlackRightDone == 1)
    { //全部逆时针，且逆得比较多，则为左弯，而且是小车走在左弯道较为中心的左弯。
      if(
         (CURVEL + CurveStraightLimitL < 0) 
       &&(CURVEC + CurveStraightLimitC < 0)
       &&(CURVER + CurveStraightLimitR < 0)       
         )
      {
        PathType = PathTypeLeftMiddle;
      }
      //全部顺时针，且顺得比较多，则为右弯，而且是小车走在右弯道较为中心的右弯。
      else if(CURVEL > CurveStraightLimitL && CURVEC > CurveStraightLimitC && CURVER > CurveStraightLimitR)
      {        
        PathType = PathTypeRightMiddle;
      }
      else
      {
        if(
          (AbsSelf(CURVEL) < CurveStraightLimitL)
        &&(AbsSelf(CURVEC) < CurveStraightLimitC)
        &&(AbsSelf(CURVER) < CurveStraightLimitR) //三个值的绝对值够小。
        &&(SlopeL + SlopeLimitL < 0)              //左黑线斜率小于某负值
        &&(AbsSelfFloat(SlopeC) < SlopeLimitC)         //中心线斜率绝对值够小
        &&(SlopeR > SlopeLimitR)                  //右黑线斜率大于某正值
          )
        { //均小于阈值，为直道。
          PathType = PathTypeStraight;  
        }
        else
        {
          PathType = PathTypeUnknown; //未知赛道类型。  
        }
      }
    }
    
    //左黑线提取失败，右黑线提取成功，的情况。
    if(BlackLeftDone == 0 && BlackRightDone == 1)
    { //左弯道，外道。
      if((CURVEC + CurveStraightLimitC < 0) 
       &&(CURVER + CurveStraightLimitR < 0)
         )
      {
        PathType = PathTypeLeftOut;
      }
      //右弯道，内道。
      else if(CURVEC > CurveStraightLimitC && CURVER > CurveStraightLimitR)
      {
        PathType = PathTypeRightIn;
      }
      else
      {
        if(
          (AbsSelf(CURVEC) < CurveStraightLimitC)
        &&(AbsSelf(CURVER) < CurveStraightLimitR)
        &&(AbsSelfFloat(SlopeC) < SlopeLimitC)         //中心线斜率绝对值够小
        &&(SlopeR > SlopeLimitR)                  //右黑线斜率大于某正值
          )
        { //均小于阈值，为直道。
          PathType = PathTypeStraight;  
        }
        else
        {
          PathType = PathTypeUnknown; //未知赛道类型。  
        }
      }
    }
    
    //右黑线提取失败，左黑线提取成功，的情况。
    if(BlackLeftDone == 1 && BlackRightDone == 0)
    { //左弯道，内道。
      if((CURVEC + CurveStraightLimitC < 0)
       &&(CURVEL + CurveStraightLimitL < 0)
         )
      {
        PathType = PathTypeLeftIn;
      }
      //右弯道，外道。
      else if(CURVEC > CurveStraightLimitC && CURVEL > CurveStraightLimitL)
      {
        PathType = PathTypeRightOut;
      }
      else
      {
        if(
          (AbsSelf(CURVEC) < CurveStraightLimitC)
        &&(AbsSelf(CURVEL) < CurveStraightLimitL)
        &&(SlopeL + SlopeLimitL < 0)              //左黑线斜率小于某负值
        &&(AbsSelfFloat(SlopeC) < SlopeLimitC)         //中心线斜率绝对值够小          
          )
        { //均小于阈值，为直道。
          PathType = PathTypeStraight;  
        }
        else
        {
          PathType = PathTypeUnknown; //未知赛道类型。  
        }
      }
    }
    
    return 1;
  }
  //3条线的曲率、斜率均求取失败，直接返回失败。
  else
  {
    PathType = PathTypeUnknown;  
    return 0;
  }
  
  */   //end of 以曲率为主导的赛道类型判断方法。
  
}








//图像畸变矫正
uint8 BlackUdis(void)
{
  uint8 row;
  
  //矫正
  //对左黑线的畸变矫正
  if(BlackLeftDone == 1)
  {
    //从线尾到线头。
    for(row = BlackLeftEndLine; ; row--)
    {
      //近处的行
      if(row > BlackUdisMiddleLine)
      {
        //列
        BlackUdisLeftLocation[row].x  = (BlackUdisMatrixLow[0] * BlackLeftLoc[row][0] + BlackUdisMatrixLow[1] * (row + 1) + BlackUdisMatrixLow[2]) 
                                      / (BlackUdisMatrixLow[6] * BlackLeftLoc[row][0] + BlackUdisMatrixLow[7] * (row + 1) + 1000);
        //行
        BlackUdisLeftLocation[row].y  = (BlackUdisMatrixLow[3] * BlackLeftLoc[row][0] + BlackUdisMatrixLow[4] * (row + 1) + BlackUdisMatrixLow[5]) 
                                      / (BlackUdisMatrixLow[6] * BlackLeftLoc[row][0] + BlackUdisMatrixLow[7] * (row + 1) + 1000);
      }
      else
      {
        //中间的行
        if(row > BlackUdisFarLine)
        {
          //列
          BlackUdisLeftLocation[row].x  = (BlackUdisMatrixMiddle[0] * BlackLeftLoc[row][0] + BlackUdisMatrixMiddle[1] * (row + 1) + BlackUdisMatrixMiddle[2]) 
                                        / (BlackUdisMatrixMiddle[6] * BlackLeftLoc[row][0] + BlackUdisMatrixMiddle[7] * (row + 1) + 1000);
          //行
          BlackUdisLeftLocation[row].y  = (BlackUdisMatrixMiddle[3] * BlackLeftLoc[row][0] + BlackUdisMatrixMiddle[4] * (row + 1) + BlackUdisMatrixMiddle[5]) 
                                        / (BlackUdisMatrixMiddle[6] * BlackLeftLoc[row][0] + BlackUdisMatrixMiddle[7] * (row + 1) + 1000);     
        }
        //远处的行
        else
        {
          //列
          BlackUdisLeftLocation[row].x  = (BlackUdisMatrixFar[0] * BlackLeftLoc[row][0] + BlackUdisMatrixFar[1] * (row + 1) + BlackUdisMatrixFar[2]) 
                                        / (BlackUdisMatrixFar[6] * BlackLeftLoc[row][0] + BlackUdisMatrixFar[7] * (row + 1) + 1000);
          //行
          BlackUdisLeftLocation[row].y  = (BlackUdisMatrixFar[3] * BlackLeftLoc[row][0] + BlackUdisMatrixFar[4] * (row + 1) + BlackUdisMatrixFar[5]) 
                                        / (BlackUdisMatrixFar[6] * BlackLeftLoc[row][0] + BlackUdisMatrixFar[7] * (row + 1) + 1000);          
        }       
      }
      
      if(row == BlackLeftHeadLine) break;
    }
  }
  else
  {    
  }
  
  //对右黑线的畸变矫正
  if(BlackRightDone == 1)
  {
    //从线尾到线头。
    for(row = BlackRightEndLine; ; row--)
    {
      //近处的行
      if(row > BlackUdisMiddleLine)
      {
        //列
        BlackUdisRightLocation[row].x  = (BlackUdisMatrixLow[0] * BlackRightLoc[row][0] + BlackUdisMatrixLow[1] * (row + 1) + BlackUdisMatrixLow[2]) 
                                       / (BlackUdisMatrixLow[6] * BlackRightLoc[row][0] + BlackUdisMatrixLow[7] * (row + 1) + 1000);
        //行
        BlackUdisRightLocation[row].y  = (BlackUdisMatrixLow[3] * BlackRightLoc[row][0] + BlackUdisMatrixLow[4] * (row + 1) + BlackUdisMatrixLow[5]) 
                                       / (BlackUdisMatrixLow[6] * BlackRightLoc[row][0] + BlackUdisMatrixLow[7] * (row + 1) + 1000);
      }
      else
      {
        //中间的行
        if(row > BlackUdisFarLine)
        {
          //列
          BlackUdisRightLocation[row].x  = (BlackUdisMatrixMiddle[0] * BlackRightLoc[row][0] + BlackUdisMatrixMiddle[1] * (row + 1) + BlackUdisMatrixMiddle[2]) 
                                         / (BlackUdisMatrixMiddle[6] * BlackRightLoc[row][0] + BlackUdisMatrixMiddle[7] * (row + 1) + 1000);
          //行
          BlackUdisRightLocation[row].y  = (BlackUdisMatrixMiddle[3] * BlackRightLoc[row][0] + BlackUdisMatrixMiddle[4] * (row + 1) + BlackUdisMatrixMiddle[5]) 
                                         / (BlackUdisMatrixMiddle[6] * BlackRightLoc[row][0] + BlackUdisMatrixMiddle[7] * (row + 1) + 1000);     
        }
        //远处的行
        else
        {
          //列
          BlackUdisRightLocation[row].x  = (BlackUdisMatrixFar[0] * BlackRightLoc[row][0] + BlackUdisMatrixFar[1] * (row + 1) + BlackUdisMatrixFar[2]) 
                                         / (BlackUdisMatrixFar[6] * BlackRightLoc[row][0] + BlackUdisMatrixFar[7] * (row + 1) + 1000);
          //行
          BlackUdisRightLocation[row].y  = (BlackUdisMatrixFar[3] * BlackRightLoc[row][0] + BlackUdisMatrixFar[4] * (row + 1) + BlackUdisMatrixFar[5]) 
                                         / (BlackUdisMatrixFar[6] * BlackRightLoc[row][0] + BlackUdisMatrixFar[7] * (row + 1) + 1000);          
        }       
      }
      
      if(row == BlackRightHeadLine) break;
    }
  }
  else
  {
  }
  
  return 1;
}


//直角弯时，对中心线的特殊处理函数。
//放在ImgProSucceedDeal()函数里有DeviFuse更新函数的后面，强制修改DeviFuse的值，不影响CenterLineLoc[]数组。
//直角弯锁存或特殊控制时，CenterLineLoc[]数组一直是无效值。
uint8 AngleSpeDealCen(uint8 dir)
{
  //左直角弯
  if(dir == 0)
  {
    //对DeviNow的特殊赋值。影响舵机控制。
    DeviFuse = AngleSpeDealDeviNow[AngleSpeDealBackCountNum-AngleSpeDealBackCount-1];
    //对DeviNowLevel的特殊赋值。影响舵机和电机控制。
    //DeviNowLevel = AngleSpeDealDeviNowLevel[AngleSpeDealBackCountNum-AngleSpeDealBackCount-1];
    ////对SlopeC的特殊赋值。影响SlopeCLevel。影响电机控制。
    SlopeC = AngleSpeDealSlopeC[AngleSpeDealBackCountNum-AngleSpeDealBackCount-1];
  }
  //右直角弯
  else if(dir == 1)
  {
    //对DeviNow的特殊赋值。影响舵机控制。
    DeviFuse = 0 - AngleSpeDealDeviNow[AngleSpeDealBackCountNum-AngleSpeDealBackCount-1];
    //对DeviNowLevel的特殊赋值。影响舵机和电机控制。
    //DeviNowLevel = AngleSpeDealDeviNowLevel[AngleSpeDealBackCountNum-AngleSpeDealBackCount-1];
    //对SlopeC的特殊赋值。影响SlopeCLevel。影响电机控制。
    SlopeC = AngleSpeDealSlopeC[AngleSpeDealBackCountNum-AngleSpeDealBackCount-1];    
  }
  //错误
  else
  {    
  }
  
  return 1;
}


//出直角弯判断，1已出，0未出
uint8 AngleSpeDealExitJudge(void)
{
  //至少要进行AngleSpeDealBackCountMin次强制控制。
  if(AngleSpeDealBackCountNum - AngleSpeDealBackCount < AngleSpeDealBackCountMin)
  {
    return 0;
  }
  else
  {
  }
  
  //退出强制控制的条件，左右线均成功，均左右线斜率绝对值小于一定值。
  if(
     (BlackLeftDone == 1)
   &&(BlackRightDone == 1)
   &&(AbsInt(SlopeL) < AngelSpeDealExitSlope)
   &&(AbsInt(SlopeR) < AngelSpeDealExitSlope)
     )
  {
    return 1;
  }
  else
  {
    return 0;
  }
  
  /*
  //看到了2条线，可以判断已经出了直角弯。
  if(BlackLeftDone == 1 && BlackRightDone == 1)
  {
    return 1;
  }
  else
  {
  }
  
  //AngleSpeDealDir所在线的斜率超过了AngelSpeDealExitSlope，也可以把控制权交给常规控制了。
  if(AngleSpeDealDir == 0)
  {
    //左直角弯。
    if(SlopeR >= AngelSpeDealExitSlope)
    {
      return 1;
    }
    else
    {
    }
  }
  else if(AngleSpeDealDir == 1)
  {
    //右直角弯。
    if(SlopeL >= AngelSpeDealExitSlope)
    {
      return 1;
    }
    else
    {
    }    
  }
  else
  {
  }

  return 0;
  */
}

//直角弯确认函数。确认多次能才确认是直角弯，并作相应操作。
uint8 AngleStep2Confirm(void)
{          
  uint8 i;
  
  //用的是上一次的实时速度等级值，虽然没用这一次的，但也应该差别不大。
  //QuadSpeedLevel共10个等级，0低速9高速。目前确认次数只分2级。可以继续细分。
  /*
  if(QuadSpeedLevel >= 5)
  {
    //高速，确认次数相对较少 。
    realConfirmNum = 5;
  }
  else
  {
    //低速，确认次数相对较多。
    realConfirmNum = 10;
  }
  */
  ///////////////////////////////////暂时先不用。等测试值出来了再用。

  //A次计数内有B次直角成功，则判断为直角弯。先入先出队列。
  //直角锁存没被置位时，才直角计数，若已被置位，不用直角计数。
  if(AngleConfirmLockFlag == 0)
  {
    for(i = 0; ; i++)
    {
      AngleConfirmMat[i] = AngleConfirmMat[i + 1];
      if(i == AngleConfirmMatNum - 2) break;
    }
    //成功
    if(PathType == PathTypeAngleL || PathType == PathTypeAngleR)
    {
      //判定为直角则新来的值置1。
      AngleConfirmMat[AngleConfirmMatNum - 1] = 1;
    }
    else
    {
      //判定为非直角则新来的值置0.
      AngleConfirmMat[AngleConfirmMatNum - 1] = 0;
    }
    //收集AngleConfirmMat数组里1的个数。
    AngleConfirmMatCollect = 0;
    for(i = 0; ; i++)
    {
      if(AngleConfirmMat[i] == 1) AngleConfirmMatCollect++;
      
      if(i == AngleConfirmMatNum - 1) break;
    }
  }
  //直角锁存标志位已被置位，不用直角计数。
  else
  {
  }
  
  //大于若干次，则锁存为直角。
  if(AngleConfirmMatCollect >= AngleConfirmMatLimit)
  {
    //锁存标志位置位。
    AngleConfirmLockFlag = 1;
    //数组清空，防止重复锁存。
    for(i = 0; ; i++)
    {
      AngleConfirmMat[i] = 0;
      if(i == AngleConfirmMatNum - 1) break;
    }
    //确认直角弯的方向。
    if(PathType == PathTypeAngleL)
    {
      AngleSpeDealDir = 0;             //左直角弯
    }
    else if(PathType == PathTypeAngleR)
    {
      AngleSpeDealDir = 1;             //右直角弯
    }
    else
    {
      //AngleSpeDealDir = 2;             //错误
    }
  }
  else
  {
  }
  
  //若直角弯锁存标志位置位，则等待矮线的线头够低后开始直角弯处理。
  if(AngleConfirmLockFlag == 1)
  {
    //直角黑块锁存标志位清零。但因为直角锁存置位，所以直角黑块不会进行检测。
    AngleZoneConfirmLockFlag = 0;
    
    //左直角弯
    if(AngleSpeDealDir == 0)
    {
      if(BlackLeftHeadLine >= AngleSpeDealStartHeadLine || BlackLeftHeadLine == MaxValUint8)
      {
        //左线头够矮（快到直角弯）或者左线头无效（已经过了直角弯），则开始直角弯特殊处理。
        AngleSpeDealBackCount = AngleSpeDealBackCountNum;
      }
      else
      {
        //还没满足条件，再等等。
      }
    }
    //右直角弯
    else if(AngleSpeDealDir == 1)
    {
      if(BlackRightHeadLine >= AngleSpeDealStartHeadLine || BlackRightHeadLine == MaxValUint8)
      {
        //右线头够矮（快到直角弯）或者右线头无效（已经过了直角弯），则开始直角弯特殊处理。
        AngleSpeDealBackCount = AngleSpeDealBackCountNum;
      }
      else
      {
        //还没满足条件，再等等。
      }
    }
    else
    {
    }
  }
  else
  {
  }

  //退出直角特殊处理的判断。
  AngleSpeDealExitJudgeFlag = AngleSpeDealExitJudge();
  
  //有了倒数次数后，就开始每次倒数，直到为0或判断已经出了直角。并持续亮灯。
  if(AngleSpeDealBackCount != 0 && AngleSpeDealExitJudgeFlag == 0)
  {
    AngleConfirmLockFlag = 0;    //直角弯判断成功锁存标志位清零。
    
    //倒数值减1。
    AngleSpeDealBackCount--;
    //对中心线的特殊处理函数，还剩AngleSpeDealBackCount次。
    AngleSpeDealCen(AngleSpeDealDir);
    //标志位说明进行了直角弯的特殊处理。
    AngleSpeDealFlag = 1;
    
    //直角黑块锁存标志位清零。但因为直角锁存置位，所以直角黑块不会进行检测。
    AngleZoneConfirmLockFlag = 0;    
  }
  else
  {
    //倒数次数清零，防止再次启动直角弯特殊处理。
    AngleSpeDealBackCount = 0;
    
    //标志位说明没有进行直角弯的特殊处理。
    AngleSpeDealFlag = 0;
  }
  
  return 1;
}









//赛道类型判断中的直道判断
//1是直道，0不是直道
uint8 PathJudgeNewStraight(void)
{
  uint8 templ, tempr;
  uint8 i;
  uint8 errorcount;
  
  
  //直道判断
  //1. 直道的判断很严格，必须左右线均成功，且整个小5点斜率数组均很小。
  if(BlackLeftDone == 1 && BlackRightDone == 1)
  {
    //2. 直道还必须要求，线头为最低行。考虑到第56-59行会先找单线，不一定能成功，所以非起始扫描线放宽到第55行。
    if(BlackGetPreDir == 0 || BlackGetPreDir == 2)
    {
      if(BlackLeftStep1ScanRow == CameraHight - 1 && BlackRightStep1ScanRow >= CameraHight - 5)
      {
      }
      else
      {
        return 0;
      }
    }
    else
    {
      if (BlackLeftStep1ScanRow >= CameraHight - 5 && BlackRightStep1ScanRow == CameraHight - 1)
      {
      }
      else
      {
        return 0;
      }
    }
    //3. PathJudgeNewStraightChoose为0表示判断直道要用到所有的点，而不是掐头。
    //此种方法比较严格，会减小直道判断成功率，但不容易将其它赛道误判成直道。
    if(PathJudgeNewStraightChoose == 0)
    {
      errorcount = 0;
      //左线
      //搜索小5点斜率数组的末端
      templ = BlackLeft5SlopeIndex - 1;
      //if(templ <= 10) return 0;
      
      //逐个判断小5点斜率是否满足条件
      for(i = 0; ; i++)
      {
        if(BlackLeft5Slope[i] > 0 || BlackLeft5Slope[i] < 0 - PathJudgeStraightSlopeLimit) //右上到左下的斜率是负的。 
        {
          errorcount++;
          if(errorcount > 3) break;   //允许有若干个点斜率超出范围
        }
        
        //if(i == templ - 10) break;
        if(i == templ) break;
      }
      //if(i != templ - 10) 
      if(i != templ)
      {
        flag000 = 3;
        flag111 = BlackLeft5Slope[i];
        return 0;
      }
      
      errorcount = 0;
      //右线
      //搜索小5点斜率数组的末端
      tempr = BlackRight5SlopeIndex - 1;
      //if(tempr <= 10) return 0;
      
      //逐个判断小5点斜率是否满足条件
      for(i = 0; ; i++)
      {
        if(BlackRight5Slope[i] < 0 || BlackRight5Slope[i] > PathJudgeStraightSlopeLimit) //左上到右下的斜率是正的。
        {
          errorcount++;
          if(errorcount > 3) break;   //允许有若干个点斜率超出范围
        }
        
        //if(i == tempr - 10) break;
        if(i == tempr) break;
      }
      //if(i != tempr - 10)
      if(i != tempr)
      {
        flag111 = BlackRight5Slope[i];
        flag000 = 5;
        return 0;
      }
      
      //走到这里说明判断直道成功。
      flag000 = 0;
      PathType = PathTypeStraight;
      return 1;
    
    }
    //3. PathJudgeNewStraightChoose为1表示判断直道时掐掉线头开始往近行的10行，再判断。
    //此种方法能增加直道判断成功率，但容易将其它赛道误判成直道。
    else if(PathJudgeNewStraightChoose == 1)
    {
      errorcount = 0;
      //左线
      //搜索小5点斜率数组的末端
      templ = BlackLeft5SlopeIndex - 1;
      if(templ <= 10) return 0; //
      
      //逐个判断小5点斜率是否满足条件
      for(i = 0; ; i++)
      {
        if(BlackLeft5Slope[i] > 0 || BlackLeft5Slope[i] < 0 - PathJudgeStraightSlopeLimit) //右上到左下的斜率是负的。 
        {
          errorcount++;
          if(errorcount > 3) break;   //允许有若干个点斜率超出范围
        }
        
        if(i == templ - 10) break;
        //if(i == templ) break;
      }
      if(i != templ - 10) 
      //if(i != templ)
      {
        flag000 = 3;
        flag111 = BlackLeft5Slope[i];
        return 0;
      }
      
      errorcount = 0;
      //右线
      //搜索小5点斜率数组的末端
      tempr = BlackRight5SlopeIndex - 1;
      if(tempr <= 10) return 0;  //
      
      //逐个判断小5点斜率是否满足条件
      for(i = 0; ; i++)
      {
        if(BlackRight5Slope[i] < 0 || BlackRight5Slope[i] > PathJudgeStraightSlopeLimit) //左上到右下的斜率是正的。
        {
          errorcount++;
          if(errorcount > 3) break;   //允许有若干个点斜率超出范围
        }
        
        if(i == tempr - 10) break;
        //if(i == tempr) break;
      }
      if(i != tempr - 10) 
      //if(i != tempr)
      {
        flag111 = BlackRight5Slope[i];
        flag000 = 5;
        return 0;
      }
      
      //走到这里说明判断直道成功。
      flag000 = 0;
      PathType = PathTypeStraight;
      return 1;      
    }
    //3. PathJudgeNewStraightChoose为2表示判断直道要用到所有的点，而不是掐头。
    //此种方法比较严格，会减小直道判断成功率，但不容易将其它赛道误判成直道。
    //这种方法的厉害之处在于考虑到了Black5Slope数组的个数与有效黑点不一定相同，大大减少了误判的机率。
    else
    {
      //不是单线的情况
      if(SingleBlackAllFlag == 0)
      {
        errorcount = 0;
        //左线
        //考虑到了Black5Slope数组的个数与有效黑点不一定相同，大大减少了误判的机率。
        if(BlackLeftEndLine - BlackLeftHeadLine - 4 < 0 || BlackLeft5SlopeIndex - 1 < 0)
        {
          return 0;
        }
        else
        {        
        }
        templ = MinRe(BlackLeftEndLine - BlackLeftHeadLine - 4, BlackLeft5SlopeIndex - 1);
        //if(templ <= 10) return 0;
        
        //逐个判断小5点斜率是否满足条件
        for(i = 0; ; i++)
        {
          if(BlackLeft5Slope[i] > 0 || BlackLeft5Slope[i] < 0 - PathJudgeStraightSlopeLimit) //右上到左下的斜率是负的。 
          {
            errorcount++;
            if(errorcount > 3) break;   //允许有若干个点斜率超出范围
          }
          
          //if(i == templ - 10) break;
          if(i == templ) break;
        }
        //if(i != templ - 10) 
        if(i != templ)
        {
          flag000 = 3;
          flag111 = BlackLeft5Slope[i];
          return 0;
        }
        
        errorcount = 0;
        
        //右线
        //考虑到了Black5Slope数组的个数与有效黑点不一定相同，大大减少了误判的机率。
        if(BlackRightEndLine - BlackRightHeadLine - 4 < 0 || BlackRight5SlopeIndex - 1 < 0)
        {
          return 0;
        }
        else
        {
        }
        tempr = MinRe(BlackRightEndLine - BlackRightHeadLine - 4, BlackRight5SlopeIndex - 1);
        //if(tempr <= 10) return 0;
        
        //逐个判断小5点斜率是否满足条件
        for(i = 0; ; i++)
        {
          if(BlackRight5Slope[i] < 0 || BlackRight5Slope[i] > PathJudgeStraightSlopeLimit) //左上到右下的斜率是正的。
          {
            errorcount++;
            if(errorcount > 3) break;   //允许有若干个点斜率超出范围
          }
          
          //if(i == tempr - 10) break;
          if(i == tempr) break;
        }
        //if(i != tempr - 10)
        if(i != tempr)
        {
          flag111 = BlackRight5Slope[i];
          flag000 = 5;
          return 0;
        }
        
        //走到这里说明判断直道成功。
        flag000 = 0;
        PathType = PathTypeStraight;
        return 1;   
      }
      //单线的情况
      else
      {
        errorcount = 0;
        //左线
        //考虑到了Black5Slope数组的个数与有效黑点不一定相同，大大减少了误判的机率。
        if(BlackLeftEndLine - BlackLeftHeadLine - 4 < 0 || BlackLeft5SlopeIndex - 1 < 0)
        {
          return 0;
        }
        else
        {        
        }
        templ = MinRe(BlackLeftEndLine - BlackLeftHeadLine - 4, BlackLeft5SlopeIndex - 1);
        //if(templ <= 10) return 0;
        
        //逐个判断小5点斜率是否满足条件
        for(i = 0; ; i++)
        {
          if(BlackLeft5Slope[i] > 1001 || BlackLeft5Slope[i] < 0 - 1000) //右上到左下的斜率是负的。 
          {
            errorcount++;
            if(errorcount > 3) break;   //允许有若干个点斜率超出范围
          }
          
          //if(i == templ - 10) break;
          if(i == templ) break;
        }
        //if(i != templ - 10) 
        if(i != templ)
        {
          flag000 = 3;
          flag111 = BlackLeft5Slope[i];
          return 0;
        }
        
        errorcount = 0;
        
        //右线
        //考虑到了Black5Slope数组的个数与有效黑点不一定相同，大大减少了误判的机率。
        if(BlackRightEndLine - BlackRightHeadLine - 4 < 0 || BlackRight5SlopeIndex - 1 < 0)
        {
          return 0;
        }
        else
        {
        }
        tempr = MinRe(BlackRightEndLine - BlackRightHeadLine - 4, BlackRight5SlopeIndex - 1);
        //if(tempr <= 10) return 0;
        
        //逐个判断小5点斜率是否满足条件
        for(i = 0; ; i++)
        {
          if(BlackRight5Slope[i] < 0 - 1001 || BlackRight5Slope[i] > 1000) //左上到右下的斜率是正的。
          {
            errorcount++;
            if(errorcount > 3) break;   //允许有若干个点斜率超出范围
          }
          
          //if(i == tempr - 10) break;
          if(i == tempr) break;
        }
        //if(i != tempr - 10)
        if(i != tempr)
        {
          flag111 = BlackRight5Slope[i];
          flag000 = 5;
          return 0;
        }
        
        //走到这里说明判断直道成功。
        flag000 = 0;
        PathType = PathTypeStraight;
        return 1;           
      }
    }
  }
  else
  {
    flag000 = 1;
    //两线若有什么任何一线不成功，则不可能是直道，直接返回0.
    return 0;
  }
  
}

//障碍判断
//进来的前提是赛道类型为直道
uint8 BrickJudge(void)
{
  uint8 row;
  uint8 col;
  uint8 count1, count2, count3;
    
  int32 a0, a1;
  uint8 i, index, tempend;
  
  uint8 x[BrickSlopeRowNum];   //取多少个点在一开始有定义。
  uint8 y[BrickSlopeRowNum];
  
  if(BlackLeftDone == 1 && BlackRightDone == 1)
  {
  }
  else
  {
    return 0;
  }
  
  
  //左障碍查找，先看看满不满足基本条件。
  if(
    (BlackRightHeadLine > BrickHeadRow)                                         //右线头太近
  ||(BlackRightCrossDone != 0 && BlackRightRow1RealLastLine  > BrickHeadRow)    //右线开启了十字再检测，且第1段黑线起始行太近。
    )
  {
    //不满足基本条件，更有可能是右障碍，直接跳过左障碍去检测右障碍。
  }
  else
  {
    //1.1 障碍边，最小二乘法计算扫描起始行和结束行的黑点所在列。
    //取BrickSlopeRowNum个点作为最小二乘法的基准点。
    //以左上角为原点，向下为x轴正方向，向右为y轴正方向
    index = 0;    
    for(i = BrickEndRow + 1; ; i++)
    {
      if(BlackLeftLoc[i][0] != MaxValUint8)
      {
        y[index] = BlackLeftLoc[i][0];
        x[index] = i;
        index++;
      }
      if(index == BrickSlopeRowNum)
      {
        break;  
      }
      if(i == CameraHight - 1)
      {
        return 0;
      }
    }
    
    //最小二乘法
    //y = a0 + a1 * x（整体扩大了LeastSquareMulti倍）
    a1 = LeastSquarea1(x, y, BrickSlopeRowNum);
    //左边，斜率应该是-PathJudgeStraightSlopeLimit~0,否则斜率不对，返回失败。
    if(a1 < 0 && a1 + PathJudgeStraightSlopeLimit > 0)
    {
    }
    else
    {
      return 0;
    }
    a0 = LeastSquarea0(x, y, a1, BrickSlopeRowNum);
    //直角梯形的两个突出顶点所在列
    //起始行（近行）黑点所在列。
    BrickEndColL = (a0 + a1 * BrickEndRow) / LeastSquareMulti + BrickColCom;
    //结束行（远行）黑点所在列
    BrickHeadColL = (a0 + a1 * BrickHeadRow) / LeastSquareMulti + BrickColCom;
    //直角梯形的直角边所在列。左障碍扫描的右边界。稍微限幅一下。
    //不能超过图像右边界
    BrickBorderColL = MinRe(CameraRealWidth - 1, BrickEndColL + BrickScanColNum);
    //不能超过该行右线黑点。
    if(BlackRightLoc[BrickEndRow][0] != MaxValUint8)
    {
      BrickBorderColL = MinRe(BrickBorderColL, BlackRightLoc[BrickEndRow][0]);
    }
    else
    {
    }
    
    //1.2 障碍边，开始扫描
    count2 = 0;
    for(row = BrickHeadRow; ; row++)
    {
      //对某一行的处理。
      //该行黑点计数，从右往左扫描。
      count1 = 0;
      //当前行的扫描左边界。
      tempend = (a0 + a1 * row) / LeastSquareMulti + BrickColCom;   //记得除以增加的倍数
      for(col = BrickBorderColL; ; col--)
      {
        if(ImgNew[row][col] < LimitLeftB)  //够黑
        {
          count1++;
        }
        else
        {
        }
        //向左扫描至边界。
        if(col <= tempend || col == 0) break;
      }
      //该行黑点个数大于BrickLineBlackCountNum个时，则行计数加1.
      if(count1 > BrickLineBlackCountNum) 
      {
        count2++;
        BrickLineCountNumRecord = count2;
        //满足黑点个数的行有BrickLineCountNum个时，则初步判断为左障碍。
        if(count2 > BrickLineCountNum) 
        {
          //不能直接返回成功，还需要对右边进行检测。
          break;
        }
        else
        {      
        }
      }
      else
      {
      }
      if(row == BrickEndRow) break;
    }
    
    //2. 非障碍边确认
    if(row != BrickEndRow)
    {
      //2. 1 非障碍边最小二乘法
      index = 0;    
      for(i = BrickEndRow + 1; ; i++)
      {
        if(BlackRightLoc[i][0] != MaxValUint8)
        {
          y[index] = BlackRightLoc[i][0];
          x[index] = i;
          index++;
        }
        if(index == BrickSlopeRowNum)
        {
          break;  
        }
        if(i == CameraHight - 1)
        {
          return 0;
        }
      }
      
      //最小二乘法
      //y = a0 + a1 * x（整体扩大了LeastSquareMulti倍）
      a1 = LeastSquarea1(x, y, BrickSlopeRowNum);
      //右边，斜率应该是0~PathJudgeStraightSlopeLimit,否则斜率不对，返回失败。
      if(a1 > 0 && a1 < PathJudgeStraightSlopeLimit)
      {
      }
      else
      {
        return 0;
      }
      a0 = LeastSquarea0(x, y, a1, BrickSlopeRowNum);
      //直角梯形的两个突出顶点所在列
      //起始行（近行）黑点所在列。
      BrickEndColR = (a0 + a1 * BrickEndRow) / LeastSquareMulti - BrickColCom;
      //结束行（远行）黑点所在列
      BrickHeadColR = (a0 + a1 * BrickHeadRow) / LeastSquareMulti - BrickColCom;
      //直角梯形的直角边所在列。左障碍的非障碍边，也就是右边直角梯形的直角边。不能比左直角梯形的直角边还小。
      BrickBorderColR = MaxRe(BrickEndColR - BrickScanColNum / 2, BrickBorderColL);
   
      //2.2 非障碍边，开始扫描，找到黑点过多的行，则左障碍确认失败。进行右障碍扫描。
      for(row = BrickHeadRow; ; row++)
      {
        //对某一行的处理。
        //该行黑点计数，从左往右扫描。
        count3 = 0;
        //当前行的扫描的右边界。
        tempend = (a0 + a1 * row) / LeastSquareMulti - BrickColCom;   //记得除以增加的倍数
        //限幅。
        tempend = MaxRe(tempend, BrickBorderColR);
        //黑点扫描。
        for(col = BrickBorderColR; ; col++)
        {
          if(ImgNew[row][col] < LimitRightB)  //够黑
          {
            count3++;
          }
          else
          {
          }
          //向右扫描至边界
          if(col >= tempend || col == CameraRealWidth - 1) break;
        }
        //该行黑点个数大于BrickLineBlackCountNum个时，则行计数加1.
        if(count3 > BrickLineBlackCountNum) 
        {
          //存在有行黑点过多，则直接跳出。
          break;
        }
        else
        {
        }
        
        if(row == BrickEndRow) break;
      }
      
      //3. 最终确认
      if(row == BrickEndRow)
      {
        //置左障碍标志
        PathType = PathBrickLeft;
        return 1;
      }
      else
      {
      }
    }
    //有效行不够，不用进行右边的确认，直接进入右障碍检测。
    else
    {
    }

  }
  
  
  //左障碍查找不成功或不需要查找，则开始右障碍查找。先看看是否满足基本条件。
  if(
    (BlackLeftHeadLine > BrickHeadRow)                                         //左线头太近
  ||(BlackLeftCrossDone != 0 && BlackLeftRow1RealLastLine  > BrickHeadRow)    //左线开启了十字再检测，且第1段黑线起始行太近。
    )
  {
    //不满足基本条件，左右障碍都不可能，不用任何检测。直接返回失败。
    return 0;
  }
  else
  {
    //1.1 障碍边，右边。最小二乘法计算扫描起始行和结束行的黑点所在列。
    //取BrickSlopeRowNum个点作为最小二乘法的基准点。
    //以左上角为原点，向下为x轴正方向，向右为y轴正方向
    index = 0;    
    for(i = BrickEndRow + 1; ; i++)
    {
      if(BlackRightLoc[i][0] != MaxValUint8)
      {
        y[index] = BlackRightLoc[i][0];
        x[index] = i;
        index++;
      }
      if(index == BrickSlopeRowNum)
      {
        break;  
      }
      if(i == CameraHight - 1)
      {
        return 0;
      }
    }
    
    //最小二乘法
    //y = a0 + a1 * x（整体扩大了LeastSquareMulti倍）
    a1 = LeastSquarea1(x, y, BrickSlopeRowNum);
    //右边，斜率应该是0~PathJudgeStraightSlopeLimit,否则斜率不对，返回失败。
    if(a1 > 0 && a1 < PathJudgeStraightSlopeLimit)
    {
    }
    else
    {
      return 0;
    }
    a0 = LeastSquarea0(x, y, a1, BrickSlopeRowNum);
    //直角梯形的两个突出顶点所在列
    //起始行（近行）黑点所在列。
    BrickEndColR = (a0 + a1 * BrickEndRow) / LeastSquareMulti - BrickColCom;
    //结束行（远行）黑点所在列
    BrickHeadColR = (a0 + a1 * BrickHeadRow) / LeastSquareMulti - BrickColCom;
    //直角梯形的直角边所在列。右障碍扫描的左边界。稍微限幅一下。
    //不能超过图像左边界
    BrickBorderColR = MaxRe(0, BrickEndColR - BrickScanColNum);
    //不能超过该行左线黑点。
    if(BlackLeftLoc[BrickEndRow][0] != MaxValUint8)
    {
      BrickBorderColR = MaxRe(BrickBorderColR, BlackLeftLoc[BrickEndRow][0]);
    }
    else
    {
    }
    
    //1.2 障碍边，开始扫描
    count2 = 0;
    for(row = BrickHeadRow; ; row++)
    {
      //对某一行的处理。
      //该行黑点计数
      count1 = 0;
      //从左往右扫描。
      tempend = (a0 + a1 * row) / LeastSquareMulti - BrickColCom;   //记得除以增加的倍数
      for(col = BrickBorderColR; ; col++)
      {
        if(ImgNew[row][col] < LimitRightB)  //够黑
        {
          count1++;
        }
        else
        {
        }
        //向右扫描至边界。
        if(col >= tempend || col == CameraRealWidth - 1) break;
      }
      //该行黑点个数大于BrickLineBlackCountNum个时，则行计数加1.
      if(count1 > BrickLineBlackCountNum) 
      {
        count2++;
        BrickLineCountNumRecord = count2;
        //满足黑点个数的行有BrickLineCountNum个时，则初步判断为右障碍。
        if(count2 > BrickLineCountNum) 
        {
          //不能直接返回成功，还需要对左边进行检测。
          break;
        }
        else
        {      
        }
      }
      else
      {
      }
      if(row == BrickEndRow) break;
    }
    
    //2. 非障碍边确认，左边。
    if(row != BrickEndRow)
    {
      //2. 1 非障碍边最小二乘法
      index = 0;    
      for(i = BrickEndRow + 1; ; i++)
      {
        if(BlackLeftLoc[i][0] != MaxValUint8)
        {
          y[index] = BlackLeftLoc[i][0];
          x[index] = i;
          index++;
        }
        if(index == BrickSlopeRowNum)
        {
          break;  
        }
        if(i == CameraHight - 1)
        {
          return 0;
        }
      }
      
      //最小二乘法
      //y = a0 + a1 * x（整体扩大了LeastSquareMulti倍）
      a1 = LeastSquarea1(x, y, BrickSlopeRowNum);
      //左边，斜率应该是-PathJudgeStraightSlopeLimit~0,否则斜率不对，返回失败。
      if(a1 < 0 && a1 + PathJudgeStraightSlopeLimit > 0)
      {
      }
      else
      {
        return 0;
      }
      a0 = LeastSquarea0(x, y, a1, BrickSlopeRowNum);
      //直角梯形的两个突出顶点所在列
      //起始行（近行）黑点所在列。
      BrickEndColL = (a0 + a1 * BrickEndRow) / LeastSquareMulti + BrickColCom;
      //结束行（远行）黑点所在列
      BrickHeadColL = (a0 + a1 * BrickHeadRow) / LeastSquareMulti + BrickColCom;
      //直角梯形的直角边所在列。右障碍的非障碍边，也就是左边直角梯形的直角边。不能比右直角梯形的直角边还大。
      BrickBorderColL = MinRe(BrickEndColL + BrickScanColNum / 2, BrickBorderColR);
   
      //2.2 非障碍边，开始扫描，找到黑点过多的行，则右障碍确认失败。
      for(row = BrickHeadRow; ; row++)
      {
        //对某一行的处理。
        //该行黑点计数
        count3 = 0;
        //从右往左扫描。
        tempend = (a0 + a1 * row) / LeastSquareMulti + BrickColCom;   //记得除以增加的倍数
        //限幅。
        tempend = MinRe(tempend, BrickBorderColL);
        //黑点扫描。
        for(col = BrickBorderColL; ; col--)
        {
          if(ImgNew[row][col] < LimitLeftB)  //够黑
          {
            count3++;
          }
          else
          {
          }
          //向左扫描至边界
          if(col <= tempend || col == 0) break;
        }
        //该行黑点个数大于BrickLineBlackCountNum个时，则行计数加1.
        if(count3 > BrickLineBlackCountNum) 
        {
          //存在有行黑点过多，则直接跳出。
          break;
        }
        else
        {
        }
        
        if(row == BrickEndRow) break;
      }
      
      //3. 最终确认
      if(row == BrickEndRow)
      {
        //置右障碍标志
        PathType = PathBrickRight;
        return 1;
      }
      else
      {
        return 0;
      }
    }
    //有效行不够，不用进行右边的确认，直接返回失败。
    else
    {
      return 0;
    }
  }
  
}



//直角黑块检测。0未检测到，1检测到。
//直角黑块有可能被误判成障碍，但障碍不可能被误判成直角黑块，所以先检测直角黑块。
//判断条件为，两线头差在4以内，且都在第25行到第40行，则找最远线头再往远4行、5行的这两行，最小二乘法估出来每行两个点，
//往内减去5列左右的裕量，然后看两点之间是不是都小于黑点阈值，若是，则判断成功。
uint8 AngleZoneJudge(void)
{
  uint8 countcol, count1, count2;
  
  int32 a0, a1;
  uint8 i, index;
  
  uint8 x[BrickSlopeRowNum];   //取多少个点在一开始有定义。
  uint8 y[BrickSlopeRowNum];
  uint8 col;
  
  //线尾不用作判断，不科学。
  
  if(
    (BlackLeftDone == 1 && BlackRightDone == 1)
  &&(AbsInt(BlackLeftHeadLine - BlackRightHeadLine) <= AngleZoneHeadLineNum)      //线头差必须在8以内
  &&(BlackLeftHeadLine < AngleZoneJudgeRowNear && BlackLeftHeadLine > AngleZoneJudgeRowFar)  //线头必须在某两行之间的区域，大概是25-40行之间
  &&(BlackRightHeadLine < AngleZoneJudgeRowNear && BlackRightHeadLine > AngleZoneJudgeRowFar)  //线头必须在某两行之间的区域，大概是25-40行之间  
    )
  {
  }
  else
  {
    return 0;
  }
  
  //最终大跳变检测，发现左线有大跳变。或右线头比左线头高。
  //则右线检测多一些黑块，左线检测少一些黑块。
  if(BlackBigChangeFinalCheckFlag == 1 || BlackRightHeadLine <= BlackLeftHeadLine)
  {
    //1. 右线。检测多一些黑块。
    //1.1 取右线头往远处5行左右。
    for(i = 0; ; i++)
    {
      AngleZoneJudgeRowR[i] = BlackRightHeadLine - i - 1;
        
      if(i == AngleZoneJudgeRowNum - 1) break;
    }
    //1.2 最小二乘法
    //取AngleZoneSlopeRowNum个点作为最小二乘法的基准点。
    //以左上角为原点，向下为x轴正方向，向右为y轴正方向
    //考虑到直角黑块大跳变现在基本可以滤除掉了，所以直角黑块定位的时候，采用线头左右开始往近处的8行，而不是最近的8行。
    index = 0;
    for(i = BlackRightHeadLine + 2; ; i++)
    {
      if(BlackRightLoc[i][0] != MaxValUint8)
      {
        y[index] = BlackRightLoc[i][0];
        x[index] = i;
        index++;
      }
      if(index == AngleJudgeSlopeRowNum)
      {
        break;  
      }
      
      if(i == CameraHight - 1) return 0;
    }
    //最小二乘法
    //y = a0 + a1 * x（整体扩大了LeastSquareMulti倍）
    a1 = LeastSquarea1(x, y, AngleJudgeSlopeRowNum);
    //右边，斜率应该是0~PathJudgeStraightSlopeLimit,否则斜率不对，返回失败。
    if(a1 > 0 && a1 < PathJudgeStraightSlopeLimit)
    {
    }
    else
    {
      return 0;
    }
    a0 = LeastSquarea0(x, y, a1, AngleJudgeSlopeRowNum);
    //1.3 计算给定5行的右边界。标志位清零。
    //要往中间留AngleZoneCheckRowCom列的裕量。
    for(i = 0; ; i++)
    {
      //根据线头几行来最小二乘法还是比较准的，不用修正太多列。
      AngleZoneJudgeColR[i] = (a0 + a1 * AngleZoneJudgeRowR[i]) / LeastSquareMulti - AngleZoneJudgeRowCom;  
      AngleZoneJudgeColRFlag[i] = 0;
      
      if(i == AngleZoneJudgeRowNum - 1) break;
    }
    //1.4 右边界确定后，往左一个赛道宽度内，只有有若干列都是黑点，则该行直角黑块判断成功。2行左右即可最终确认右线OK。
    for(i = 0; ; i++)
    {
      //每行的列黑点计数清零。
      countcol = 0;
      //从左往右扫一个赛道宽度。赛道宽度预定为70列左右。
      if(AngleZoneJudgeColR[i] - AngleZoneJudgeRoadWidth < 0)
      {
        return 0;
      }
      else
      {
      }      
      for(col = AngleZoneJudgeColR[i] - AngleZoneJudgeRoadWidth; ; col++)
      {
        //检测到有效黑点，计数加1.
        if(ImgNew[AngleZoneJudgeRowR[i]][col] < LimitRightB) countcol++;
        //该行右黑点够多。置标志位。跳出。
        if(countcol >= AngleZoneJudgeBlackNumP)  //优势边取45列左右，比障碍多
        {
          AngleZoneJudgeColRFlag[i] = 1;
          break;
        }
        //直到第70列也没有凑够黑点数，跳出。
        if(col == AngleZoneJudgeColR[i]) break;
      }
      //记录该行的有效黑点数，以供确定合适的阈值。
      AngleZoneJudgeCountColR[i] = countcol;
        
      //到最后一行了，退出。
      if(i == AngleZoneJudgeRowNum - 1) break;
    }
    //1.5 检测是不是至少有2行满足条件。若有，则继续检测左线，若无，则直接返回失败。
    count1 = 0;
    for(i = 0; ; i++)
    {
      if(AngleZoneJudgeColRFlag[i] == 1) count1++;
      
      if(i == AngleZoneJudgeRowNum - 1) break;
    }
    //反时限特性。两线头差值较大时，优势边的45列只要求一行。否则要求2行。
    if(AbsInt(BlackLeftHeadLine - BlackRightHeadLine) > 5)
    {
      if(count1 >= 1)
      {
      }
      else
      {
        return 0;
      }
    }
    else
    {
      if(count1 >= 2)
      {
      }
      else
      {
        return 0;
      }
    }
    
    //2. 左线。检测少一些黑块。
    //2.1 取左线头往远处5行左右。劣势边线头过去的第一行一般是不可能有那么多黑点的，所以要再过去一行。
    for(i = 0; ; i++)
    {
      AngleZoneJudgeRowL[i] = BlackLeftHeadLine - i - 1 - 1;
        
      if(i == AngleZoneJudgeRowNum - 1) break;
    }
    //2.2 最小二乘法
    //取AngleZoneSlopeRowNum个点作为最小二乘法的基准点。
    //以左上角为原点，向下为x轴正方向，向右为y轴正方向
    //考虑到直角黑块大跳变现在基本可以滤除掉了，所以直角黑块定位的时候，采用线头左右开始往近处的8行，而不是最近的8行。
    index = 0;
    for(i = BlackLeftHeadLine + 2; ; i++)
    {
      if(BlackLeftLoc[i][0] != MaxValUint8)
      {
        y[index] = BlackLeftLoc[i][0];
        x[index] = i;
        index++;
      }
      if(index == AngleJudgeSlopeRowNum)
      {
        break;  
      }
      
      if(i == CameraHight - 1) return 0;
    }
    //最小二乘法
    //y = a0 + a1 * x（整体扩大了LeastSquareMulti倍）
    a1 = LeastSquarea1(x, y, AngleJudgeSlopeRowNum);
    //左边，斜率应该是-PathJudgeStraightSlopeLimit~0,否则斜率不对，返回失败。
    if(a1 < 0 && a1 + PathJudgeStraightSlopeLimit > 0)
    {
    }
    else
    {
      return 0;
    }
    a0 = LeastSquarea0(x, y, a1, AngleJudgeSlopeRowNum);
    //2.3 计算给定5行的左边界。标志位清零。
    //要往中间留AngleZoneCheckRowCom列的裕量。
    for(i = 0; ; i++)
    {
      //根据线头几行来最小二乘法还是比较准的，不用修正太多列。
      AngleZoneJudgeColL[i] = (a0 + a1 * AngleZoneJudgeRowL[i]) / LeastSquareMulti + AngleZoneJudgeRowCom;  
      AngleZoneJudgeColLFlag[i] = 0;
      
      if(i == AngleZoneJudgeRowNum - 1) break;
    }
    //2.4 左边界确定后，往右一个赛道宽度内，只有有若干列都是黑点，则该行直角黑块判断成功。2行左右即可最终确认右线OK。
    for(i = 0; ; i++)
    {
      //每行的列黑点计数清零。
      countcol = 0;
      //从右往左扫半个赛道宽度。赛道宽度预定为70列左右。
      if(AngleZoneJudgeColL[i] + AngleZoneJudgeRoadWidth / 2 > CameraRealWidth - 1) //劣势边只允许扫描半个赛道宽度。防止障碍被误判成直角黑块。
      {
        return 0;
      }
      else
      {
      }
      for(col = AngleZoneJudgeColL[i] + AngleZoneJudgeRoadWidth /2 ; ; col--) //劣势边只允许扫描半个赛道宽度。防止障碍被误判成直角黑块。
      {
        //检测到有效黑点，计数加1.
        if(ImgNew[AngleZoneJudgeRowL[i]][col] < LimitLeftB) countcol++;
        //该行右黑点够多。置标志位。跳出。
        if(countcol >= AngleZoneJudgeBlackNumN)  //劣势边取20列左右，比单线宽度多。
        {
          AngleZoneJudgeColLFlag[i] = 1;
          break;
        }
        //直到第70列也没有凑够黑点数，跳出。
        if(col == AngleZoneJudgeColL[i]) break;
      }
      //记录该行的有效黑点数，以供确定合适的阈值。
      AngleZoneJudgeCountColL[i] = countcol;
        
      //到最后一行了，退出。
      if(i == AngleZoneJudgeRowNum - 1) break;
    }
    //2.5 检测是不是至少有2行满足条件。若有，则确认这幅图像是直角黑块，更新赛道类型、线头、返回成功。若无，则返回失败。
    count2 = 0;
    for(i = 0; ; i++)
    {
      if(AngleZoneJudgeColLFlag[i] == 1) count2++;
      
      if(i == AngleZoneJudgeRowNum - 1) break;
    }
    if(count2 >= 2)
    {
      //赛道类型，直角黑色区域。
      PathType = PathAngleZone; 
      
      //对线头作处理，远线头拉到近线头处来。使远处的行不至于太偏。
      //线头相同，不用处理。
      if(BlackLeftHeadLine == BlackRightHeadLine)
      {
      }
      //左线头更远
      else if(BlackLeftHeadLine < BlackRightHeadLine)
      {
        //删掉左线头超出右线头的部分。
        for(i = BlackLeftHeadLine; ; i++)
        {
          if(i == BlackRightHeadLine) break;
          
          BlackLeftLoc[i][0] = MaxValUint8;
        }
        //左线头的值更新。
        BlackLeftHeadLine = BlackRightHeadLine;
      }
      //右线头更远
      else
      {
        //删掉右线头超出左线头的部分。
        for(i = BlackRightHeadLine; ; i++)
        {
          if(i == BlackLeftHeadLine) break;
          
          BlackRightLoc[i][0] = MaxValUint8;
        }
        //右线头的值更新。
        BlackRightHeadLine = BlackLeftHeadLine;        
      }
            
      return 1;
    }
    else
    {
      return 0;
    }

  }
  //最终大跳变检测，发现右线有大跳变。或左线头比右线头高。
  //则左线检测多一些黑块，右线检测少一些黑块。
  else if(BlackBigChangeFinalCheckFlag == 2  || BlackLeftHeadLine < BlackRightHeadLine)
  {
    //1. 左线。检测多一些黑块。
    //2.1 取左线头往远处5行左右。
    for(i = 0; ; i++)
    {
      AngleZoneJudgeRowL[i] = BlackLeftHeadLine - i - 1;
        
      if(i == AngleZoneJudgeRowNum - 1) break;
    }
    //1.2 最小二乘法
    //取AngleZoneSlopeRowNum个点作为最小二乘法的基准点。
    //以左上角为原点，向下为x轴正方向，向右为y轴正方向
    //考虑到直角黑块大跳变现在基本可以滤除掉了，所以直角黑块定位的时候，采用线头左右开始往近处的8行，而不是最近的8行。
    index = 0;
    for(i = BlackLeftHeadLine + 2; ; i++)
    {
      if(BlackLeftLoc[i][0] != MaxValUint8)
      {
        y[index] = BlackLeftLoc[i][0];
        x[index] = i;
        index++;
      }
      if(index == AngleJudgeSlopeRowNum)
      {
        break;  
      }
      
      if(i == CameraHight - 1) return 0;
    }
    //最小二乘法
    //y = a0 + a1 * x（整体扩大了LeastSquareMulti倍）
    a1 = LeastSquarea1(x, y, AngleJudgeSlopeRowNum);
    //左边，斜率应该是-PathJudgeStraightSlopeLimit~0,否则斜率不对，返回失败。
    if(a1 < 0 && a1 + PathJudgeStraightSlopeLimit > 0)
    {
    }
    else
    {
      return 0;
    }
    a0 = LeastSquarea0(x, y, a1, AngleJudgeSlopeRowNum);
    //1.3 计算给定5行的左边界。标志位清零。
    //要往中间留AngleZoneCheckRowCom列的裕量。
    for(i = 0; ; i++)
    {
      //根据线头几行来最小二乘法还是比较准的，不用修正太多列。
      AngleZoneJudgeColL[i] = (a0 + a1 * AngleZoneJudgeRowL[i]) / LeastSquareMulti + AngleZoneJudgeRowCom;  
      AngleZoneJudgeColLFlag[i] = 0;
      
      if(i == AngleZoneJudgeRowNum - 1) break;
    }
    //1.4 左边界确定后，往右一个赛道宽度内，只有有若干列都是黑点，则该行直角黑块判断成功。2行左右即可最终确认右线OK。
    for(i = 0; ; i++)
    {
      //每行的列黑点计数清零。
      countcol = 0;
      //从右往左扫一个赛道宽度。赛道宽度预定为70列左右。
      if(AngleZoneJudgeColL[i] + AngleZoneJudgeRoadWidth > CameraRealWidth - 1)
      {
        return 0;
      }
      else
      {
      }
      for(col = AngleZoneJudgeColL[i] + AngleZoneJudgeRoadWidth; ; col--)
      {
        //检测到有效黑点，计数加1.
        if(ImgNew[AngleZoneJudgeRowL[i]][col] < LimitLeftB) countcol++;
        //该行右黑点够多。置标志位。跳出。
        if(countcol >= AngleZoneJudgeBlackNumP)  //优势边取45列左右，比障碍多。
        {
          AngleZoneJudgeColLFlag[i] = 1;
          break;
        }
        //直到第70列也没有凑够黑点数，跳出。
        if(col == AngleZoneJudgeColL[i]) break;
      }
      //记录该行的有效黑点数，以供确定合适的阈值。
      AngleZoneJudgeCountColL[i] = countcol;
        
      //到最后一行了，退出。
      if(i == AngleZoneJudgeRowNum - 1) break;
    }
    //1.5 检测是不是至少有2行满足条件。若有，则继续检测左线，若无，则直接返回失败。
    count2 = 0;
    for(i = 0; ; i++)
    {
      if(AngleZoneJudgeColLFlag[i] == 1) count2++;
      
      if(i == AngleZoneJudgeRowNum - 1) break;
    }
    //反时限特性。两线头差值较大时，优势边的45列只要求一行。否则要求2行。
    if(AbsInt(BlackLeftHeadLine - BlackRightHeadLine) > 5)
    {
      if(count2 >= 1)
      {
      }
      else
      {
        return 0;
      }
    }
    else
    {
      if(count2 >= 2)
      {
      }
      else
      {
        return 0;
      }
    }
       
    //2. 右线。检测少一些黑块。
    //2.1 取右线头往远处5行左右。劣势边线头过去的第一行一般是不可能有那么多黑点的，所以要再过去一行。
    for(i = 0; ; i++)
    {
      AngleZoneJudgeRowR[i] = BlackRightHeadLine - i - 1 - 1;
        
      if(i == AngleZoneJudgeRowNum - 1) break;
    }
    //2.2 最小二乘法
    //取AngleZoneSlopeRowNum个点作为最小二乘法的基准点。
    //以左上角为原点，向下为x轴正方向，向右为y轴正方向
    //考虑到直角黑块大跳变现在基本可以滤除掉了，所以直角黑块定位的时候，采用线头左右开始往近处的8行，而不是最近的8行。
    index = 0;
    for(i = BlackRightHeadLine + 2; ; i++)
    {
      if(BlackRightLoc[i][0] != MaxValUint8)
      {
        y[index] = BlackRightLoc[i][0];
        x[index] = i;
        index++;
      }
      if(index == AngleJudgeSlopeRowNum)
      {
        break;  
      }
      
      if(i == CameraHight - 1) return 0;
    }
    //最小二乘法
    //y = a0 + a1 * x（整体扩大了LeastSquareMulti倍）
    a1 = LeastSquarea1(x, y, AngleJudgeSlopeRowNum);
    //右边，斜率应该是0~PathJudgeStraightSlopeLimit,否则斜率不对，返回失败。
    if(a1 > 0 && a1 < PathJudgeStraightSlopeLimit)
    {
    }
    else
    {
      return 0;
    }
    a0 = LeastSquarea0(x, y, a1, AngleJudgeSlopeRowNum);
    //2.3 计算给定5行的右边界。标志位清零。
    //要往中间留AngleZoneCheckRowCom列的裕量。
    for(i = 0; ; i++)
    {
      //根据线头几行来最小二乘法还是比较准的，不用修正太多列。
      AngleZoneJudgeColR[i] = (a0 + a1 * AngleZoneJudgeRowR[i]) / LeastSquareMulti - AngleZoneJudgeRowCom;  
      AngleZoneJudgeColRFlag[i] = 0;
      
      if(i == AngleZoneJudgeRowNum - 1) break;
    }
    //2.4 右边界确定后，往左一个赛道宽度内，只有有若干列都是黑点，则该行直角黑块判断成功。2行左右即可最终确认右线OK。
    for(i = 0; ; i++)
    {
      //每行的列黑点计数清零。
      countcol = 0;
      //从左往右扫半个赛道宽度。赛道宽度预定为70列左右。
      if(AngleZoneJudgeColR[i] - AngleZoneJudgeRoadWidth / 2 < 0) //劣势边只允许扫描半个赛道宽度。防止障碍被误判成直角黑块。
      {
        return 0;
      }
      else
      {
      }      
      for(col = AngleZoneJudgeColR[i] - AngleZoneJudgeRoadWidth / 2; ; col++) //劣势边只允许扫描半个赛道宽度。防止障碍被误判成直角黑块。
      {
        //检测到有效黑点，计数加1.
        if(ImgNew[AngleZoneJudgeRowR[i]][col] < LimitRightB) countcol++;
        //该行右黑点够多。置标志位。跳出。
        if(countcol >= AngleZoneJudgeBlackNumN)  //劣势边取20列左右，比单线宽度多
        {
          AngleZoneJudgeColRFlag[i] = 1;
          break;
        }
        //直到第70列也没有凑够黑点数，跳出。
        if(col == AngleZoneJudgeColR[i]) break;
      }
      //记录该行的有效黑点数，以供确定合适的阈值。
      AngleZoneJudgeCountColR[i] = countcol;      
        
      //到最后一行了，退出。
      if(i == AngleZoneJudgeRowNum - 1) break;
    }
    //2.5 检测是不是至少有2行满足条件。若有，则确认这幅图像是直角黑块，更新赛道类型、线头、返回成功。若无，则返回失败。
    count1 = 0;
    for(i = 0; ; i++)
    {
      if(AngleZoneJudgeColRFlag[i] == 1) count1++;
      
      if(i == AngleZoneJudgeRowNum - 1) break;
    }
    if(count1 >= 2)
    {
      //赛道类型，直角黑色区域。
      PathType = PathAngleZone; 
      
      //对线头作处理，远线头拉到近线头处来。使远处的行不至于太偏。
      //线头相同，不用处理。
      if(BlackLeftHeadLine == BlackRightHeadLine)
      {
      }
      //左线头更远
      else if(BlackLeftHeadLine < BlackRightHeadLine)
      {
        //删掉左线头超出右线头的部分。
        for(i = BlackLeftHeadLine; ; i++)
        {
          if(i == BlackRightHeadLine) break;
          
          BlackLeftLoc[i][0] = MaxValUint8;
        }
        //左线头的值更新。
        BlackLeftHeadLine = BlackRightHeadLine;
      }
      //右线头更远
      else
      {
        //删掉右线头超出左线头的部分。
        for(i = BlackRightHeadLine; ; i++)
        {
          if(i == BlackLeftHeadLine) break;
          
          BlackRightLoc[i][0] = MaxValUint8;
        }
        //右线头的值更新。
        BlackRightHeadLine = BlackLeftHeadLine;        
      }
            
      return 1;      
    }
    else
    {
      return 0;
    }
    
  }
  //理论上不会走到这里，左右线头一定有个高低。
  else
  {
  }
  
  //常态是检测失败。
  return 0;
}


//直角黑块确认函数
uint8 AngleZoneConfirm(void)
{
  
  uint8 i;
  
  //直角黑块锁存标志没有被置位的时候才进行直角黑块计数，若已被置位，则不用进行直角黑块计数。
  if(AngleZoneConfirmLockFlag == 0)
  {
    //A次计数内有B次直角黑块成功，则判断为直角黑块。先入先出队列。
    for(i = 0; ; i++)
    {
      AngleZoneConfirmMat[i] = AngleZoneConfirmMat[i + 1];
      if(i == AngleZoneConfirmMatNum - 2) break;
    }
    //成功
    if(PathType == PathAngleZone)
    {
      //判定为直角黑块则新来的值置1。
      AngleZoneConfirmMat[AngleZoneConfirmMatNum - 1] = 1;
    }
    else
    {
      //判定为非直角黑块则新来的值置0.
      AngleZoneConfirmMat[AngleZoneConfirmMatNum - 1] = 0;
    }
    //收集AngleZoneConfirmMat数组里1的个数。
    AngleZoneConfirmMatCollect = 0;
    for(i = 0; ; i++)
    {
      if(AngleZoneConfirmMat[i] == 1) AngleZoneConfirmMatCollect++;
      
      if(i == AngleZoneConfirmMatNum - 1) break;
    }
    //大于若干次，则锁存为直角黑块。
    if(AngleZoneConfirmMatCollect >= AngleZoneConfirmMatLimit)
    {
      //锁存标志位置位。
      AngleZoneConfirmLockFlag = 1;
      //锁存标志位置位时，同时把1米距离内的计数值清零。
      AngleZoneConfirmLockMeterCount = 0;
      //数组清空，防止重复锁存。
      for(i = 0; ; i++)
      {
        AngleZoneConfirmMat[i] = 0;
        if(i == AngleZoneConfirmMatNum - 1) break;
      }

      //PatyType在AngleZoneJudge中已经更新。
    }
    else
    {
    }
  }
  //直角黑块锁存已被置位，则不用进行直角黑块计数。
  else
  {
  }
  
  //若直角黑块锁存标志位置位，则对1米的距离内保持锁存，若检测到不到线头高度差，则释放。
  //按2 m/s速度，跑1米大概需要0.5s，也就是30个16ms，3m/s对应20个16ms.所以应取30个以上。
  if(AngleZoneConfirmLockFlag == 1)
  {
    //直角黑块锁存期间，若线头差小于4，则远处线头删至近处线头。
    if(AbsInt(BlackLeftHeadLine - BlackRightHeadLine) < 4)
    {
      //线头相同，不用处理。
      if(BlackLeftHeadLine == BlackRightHeadLine)
      {
      }
      //左线头更远
      else if(BlackLeftHeadLine < BlackRightHeadLine)
      {   
        //删掉左线头超出右线头的部分。
        for(i = BlackLeftHeadLine; ; i++)
        {
          if(i == BlackRightHeadLine) break;
          
          BlackLeftLoc[i][0] = MaxValUint8;
        }
        //左线头的值更新。
        BlackLeftHeadLine = BlackRightHeadLine;
      }
      //右线头更远
      else
      {
        //删掉右线头超出左线头的部分。
        for(i = BlackRightHeadLine; ; i++)
        {
          if(i == BlackLeftHeadLine) break;
          
          BlackRightLoc[i][0] = MaxValUint8;
        }
        //右线头的值更新。
        BlackRightHeadLine = BlackLeftHeadLine;        
      }
    }
    else
    {
    }
    
    //Count的意义：
    //1. 直角黑块锁存确认后，Count清零。
    //2. 直角黑块锁存期间：2.1. 若没有检测到 障碍 或者 直角线头差，则计数加1。若连续计数直至达到阈值，则觉得应该是出直角的直角黑块，则计数清零，允许再次检测直角。
    //                     2.2. 若检测到了，则计数清零
    //3. 直角黑块非锁存期间：不做对计数做任何处理。
    if(AngleZoneConfirmLockFlag == 1)
    {
      if(PathType == PathTypeAngleL || PathType == PathTypeAngleR || PathType == PathBrickLeft || PathType == PathBrickRight)
      {
        //计数清零
        AngleZoneConfirmLockMeterCount = 0;
      }
      else
      {
        AngleZoneConfirmLockMeterCount++;
        if(AngleZoneConfirmLockMeterCount > AngleZoneConfirmLockMeterCountLimit)
        {
          //1米左右的距离仍没有检测到高度差，释放锁存标志位，再次检测直角黑块。
          AngleZoneConfirmLockFlag = 0;
        }
        else
        {      
        }
      }
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


//赛道类型判断
//返回0出错，1正常。
uint8 PathJudgeNew(void)
{
  
  //0. 默认赛道类型未知。
  PathType = PathTypeUnknown;
  

  //1. 直角线头差 、 障碍 的 锁存 或 特殊控制 期间，不进行直角、直角黑块、障碍的检测 以及 直道 的检测。
  if(
    (AngleConfirmLockFlag == 1)   //直角锁存期间
  ||(AngleSpeDealFlag == 1)       //直角强制控制期间
  ||(BrickConfirmLockFlag == 1)   //障碍锁存期间
  ||(BrickSpeDealFlag == 1)       //障碍特殊控制期间
  ||(GyroResultForbidFlag == 1)   //坡道禁止赛道类型判断期间
    )
  {
    //连续长直道计数清零。
    PathRealStraightCount = 0;
    //锁存或特殊控制期间，说明障碍或直角已经判断成功，可以将直角黑块锁存标志复位，允许判断直角黑块。
    AngleZoneConfirmLockFlag = 0;
  }
  else
  {
    //判断停车线
    //if(StopLineCheck()){StopLineCheckFlag = 1;}
    //十字的二重判定
    //if(ShiZhiCheck()) { ShiZhiFlag = 1; } else {ShiZhiFlag = 0;}
    //2.1 直角黑块未锁存期间，检测直角黑块 或 障碍物 或 直道， 不检测直角线头差。
    if(AngleZoneConfirmLockFlag == 0)
    {
      //2.1.1. 判断直道。
      if(PathJudgeNewStraight() == 1)
      {
      }
      else
      {
      }

      //2.1.2. 先检测直角黑块检测    //这届不检测了
      //AngleZoneJudge();
      //2.1.3. 若直角黑块检测不成功，再检测障碍物。
      if(PathType != PathAngleZone)
      {
        BrickJudge();
      }
      //直角黑块检测成功，就不用检测障碍物了。
      else
      {

      }    
    }
    //2.2 直角黑块锁存期间，检测障碍物 和 直角线头差。不检测直角黑块 和 直道。
    else
    {
      //2.2.1锁存期间，先检测障碍物。
      BrickJudge();
      //2.2.2 障碍物未检测成功，则检测直角线头差。 
      if(PathType != PathBrickLeft && PathType != PathBrickRight)
      {
        //AngleStep1Judge();
        
      }
      //确认为障碍，不进行直角线头差的检测。为了避免错误，直角线头差中还必须包括对黑块的扫描。
      else
      {
      }      
      
    }
  }

  //3. 长直道判断
  if(PathType == PathTypeStraight)
  {
    //新加入的停车线的检测，在直角和障碍均检测不到时检测
    //if(StopLineCheck()){StopLineCheckFlag = 1;light_control(PORTE, 26, Light_ON);}
    
    //连续长直道计数加1。
    PathRealStraightCount++;
    //连续计数达到阈值
    if(PathRealStraightCount > PathRealStraightCountLimit)
    {
      //长直道标志置1.
      PathRealStraightFlag = 1;
      //计数清零
      PathRealStraightCount = 0;
    }
    else if(PathRealStraightCount > 5)
    {
        xiaoZhiDaoFlag = 1;
    }
    else
    {
    }
    
  }
  else
  {
    //一旦检测到非直道，则计数清零。
    PathRealStraightCount = 0;
    //一旦检测到非直道，则长直道标志复位。
    PathRealStraightFlag = 0;
    xiaoZhiDaoFlag = 0;
  }
  
  
  
  
  
//  //1. 判断直道。
//  if(PathJudgeNewStraight() == 1)
//  {
//    //2. 直角锁存或特殊控制、障碍锁存或特殊控制期间，不进行直角、直角黑块、障碍的检测。
//    if(AngleConfirmLockFlag == 1 || AngleSpeDealFlag == 1 || BrickConfirmLockFlag == 1 || BrickSpeDealFlag == 1)
//    {
//    }
//    else
//    {
//      //3. 直角黑块未锁存期间，检测直角黑块 或 障碍物，不检测直角线头差。
//      if(AngleZoneConfirmLockFlag == 0)
//      {
//        //4. 先检测直角黑块检测
//        AngleZoneJudge();
//        //5. 若直角黑块检测不成功，再检测障碍物。
//        if(PathType != PathAngleZone)
//        {
//          BrickJudge();
//        }
//        //直角黑块检测成功，就不用检测障碍物了。
//        else
//        {
//        }    
//      }
//      //直角黑块锁存期间，检测直角线头差。不检测直角黑块 和 障碍物。
//      else
//      {
//        AngleStep1Judge();
//      }
//    }
//  }
//  //不是直道，直接退出。
//  else
//  {
//  }      
      
      
      
      
//  //判断直道。
//  if(PathJudgeNewStraight() == 1)
//  {
//    //障碍或直角的锁存和强制控制期间，不需要进行障碍或直角检测，以免误判 或 重复锁存。
//    if(AngleConfirmLockFlag == 0 && AngleSpeDealFlag == 0
//    && BrickConfirmLockFlag == 0 && BrickSpeDealFlag == 0)
//    {
//      //障碍判断
//      BrickJudge();
//      if(PathType != PathBrickLeft && PathType != PathBrickRight)
//      {
//        //如果不是障碍，则可以进一步判断是不是直角弯。
//        //直角先去掉，专门测砖块。   //AngleStep1Judge();
//      }
//      else
//      {
//      }
//    }
//    else
//    {
//    }
//  }
//  else
//  {
//  }
  
  return 1;
}


//各种变量的初始化赋值函数
uint8 VariablesInit(void)
{
  uint8 i;
  
  LimitLeftWB = LimitLeftWBMin;    //黑白像素点的差值，两个点的差必须要大于这个值，才能认为有跳变。
  LimitLeftW  = LimitLeftWMin;      //白点必须要大于此值
  LimitLeftB  = LimitLeftBMax;      //黑点必须要小于此值

  //每幅有效图像的左线线头对应的3个阈值，单独储存，每幅有效图像刷新一次。
  LimitLeftHeadWB = LimitLeftWBMin;    
  LimitLeftHeadW  = LimitLeftWMin;      
  LimitLeftHeadB  = LimitLeftBMax;
  
  LimitRightWB = LimitRightWBMin;    //黑白像素点的差值，两个点的差必须要大于这个值，才能认为有跳变。
  LimitRightW  = LimitRightWMin;      //白点必须要大于此值
  LimitRightB  = LimitRightBMax;      //黑点必须要小于此值

  //每幅有效图像的左线线头对应的3个阈值，单独储存，每幅有效图像刷新一次。
  LimitRightHeadWB = LimitRightWBMin;    
  LimitRightHeadW = LimitRightWMin;      
  LimitRightHeadB = LimitRightBMax;  
  
  //左右黑线跳变沿检测的起始列。需要考虑中间单线的宽度。
  BlackLeftLastEdgeStartCol = CameraRealWidth/2 + SingleBlackWidthMax[CameraHight-1];
  BlackLeftEdgeStartCol = CameraRealWidth/2 + SingleBlackWidthMax[CameraHight-1];
  
  BlackRightLastEdgeStartCol = CameraRealWidth/2 - SingleBlackWidthMax[CameraHight-1]; 
  BlackRightEdgeStartCol = CameraRealWidth/2 - SingleBlackWidthMax[CameraHight-1];
  
  //中心线获取时的赛道半宽，针对单线的情况。初始化为一个最大黑线宽度。
  RoadHalfWidthSingle = SingleBlackWidthMax[CameraHight - 1];
  
  //直角弯标志存储数组，初始化为均不是直角。
  for(i = 0; ; i++)
  {
    AngleConfirmMat[i] = 0;
    if(i == AngleConfirmMatNum - 1) break;
  }
  
  //双线半宽
  RoadHalfWidth = 100;
  RoadHalfWidthLast = RoadHalfWidth;
  
  return 1;
}









//======================================================================
//函数名：ImageProParaInit
//功  能：每幅图像处理之前的参数初始化函数
//参  数：
//返  回：
//影  响：
//说  明：
//      
//             
//======================================================================
uint8 ImageProParaInit(void)
{
  uint8 i;
  
  //图像处理成功标志位，默认置1.
  ImgBlackSucceedFlag = 1;
  ImgCenterSucceedFlag = 1;
  //小S弯判断标志初始化为失败。
  PathSmallSDoneFlag = 0;
   
  //中心线数组赋无效值。
  for(i = 0; ; i++)
  {
    CenterLineLoc[i] = MaxValInt16;
    if(i == CameraHight - 1) break;
  }

  //赛道半宽记录值
  for(i = 0; ; i++)
  {
    RoadHalfWidthRecord[i] = MaxValUint8;
    if(i == CameraHight - 1) break;
  }
  
  //通过赛道半宽判断出来的单线错误标志，初始化为未进入。
  RoadHalfWidthWrongFlag = 2;
  
  //单线地板变量的处理。
  SingleDoneFlag = 0;       
  SingleFloorFlag = 0;            
  SingleFloorConfirmCount = 0;     
  SingleFloorConfirmCountMax = 0;

  //每行的单线确认标志初始化为失败。
  for(i = 0; ; i++)
  {
    SingleBlackFlag[i] = 0;
    if(i == CameraHight - 1) break;
  }
  //总幅图像的单线标志，先存上一次的标志，再初始化为0.
  SingleBlackLastAllFlag = SingleBlackAllFlag;
  SingleBlackAllFlag = 0;
  
  //单线错误标志。0没错误，1有错误。每幅图初始化为0.
  BlackSingleFlag = 0;
  
  flag222 = 0;
  
  BrickLineCountNumRecord = 0;
  
  //最终大跳变检测标志位
  BlackBigChangeFinalCheckFlag = 0;
  
  //直角黑块
  for(i = 0; ; i++)
  {
    AngleZoneJudgeRowL[i] = 0;  //直角黑块，根据线头选中的要检测的左线行。
    AngleZoneJudgeRowR[i] = 0;  //直角黑块，根据线头选中的要检测的右线行。
    AngleZoneJudgeColL[i] = 0;  //直角黑块，左线定位的所在列。
    AngleZoneJudgeCountColL[i] = 0;
    AngleZoneJudgeColLFlag[i] = 0; //直角黑块，左线该行的有效黑点满足要求时置位。
    AngleZoneJudgeColR[i] = 0;  //直角黑块，右线定位的所在列。
    AngleZoneJudgeColRFlag[i] = 0; //直角黑块，右线该行的有效黑点满足要求时置位。 
    AngleZoneJudgeCountColR[i] = 0;
    
    if(i == AngleZoneJudgeRowNum - 1) break;   
  }
  
  //障碍
  BrickEndColL = 0;    //障碍物，左直角梯形，起始行（近行）黑点所在列。
  BrickHeadColL = 0;   //障碍物，左直角梯形，结束行（远行）黑点所在列。
  BrickBorderColL = 0; //障碍物，左直角梯形，直角边所在列。
  BrickEndColR = 0;    //障碍物，右直角梯形，起始行（近行）黑点所在列。
  BrickHeadColR = 0;   //障碍物，右直角梯形，结束行（远行）黑点所在列。
  BrickBorderColR = 0; //障碍物，右直角梯形，直角边所在列。  
  BrickSpeDealCenFlag = 1;
  
  return 1;
  
}

//图像成功后的处理。
//只有图像处理成功了，才会进行赛道类型判断，才会进行偏差获取，舵机电机控制值才会更新。
uint8 ImgProSucceedDeal(void)
{
  uint8 startline;
  
  //上一幅图最终的赛道半宽，图像成功才会切换。
  RoadHalfWidthLast = RoadHalfWidth;
  
  /*
  //赛道类型判断
  //到这里时，中心线提取成功，左右黑线至少有一个提取成功。
  PathJudgeDone = 1; //赛道类型判断标志位，默认成功
  if(!PathJudge())
  {
    PathJudgeDone = 0;  //赛道类型判断标志位，判断失败。
    PathType = 0;       //赛道类型置为未知。
    //uart_sendN(UART0, (uint8 *)"\nPathJudge Failed!", 18);
    //return 0;         //赛道类型判断失败了还能继续，不用返回0.
  }
  else
  {
  }
  */
  
  //发送中心线数组，不用发送float型的数据，同样的处理可以由上位机完成。
  //SendCenterLineLoc(CenterLineLoc);
  
  //SendImage(ImgNew);  
  
  //因为采用多行控制，所以把小S弯对偏差的特殊处理去掉了。
  /*
  //对小S弯的特殊处理。
  if(PathType == PathTypeSmallSL || PathType == PathTypeSmallSR)
  {
    //若为小S弯，则特殊处理。
    PathTypeSmallDeal();
  }
  else
  {
    //若不为小S弯，则正常求取偏差
    DeviationGet();        
  }
  */
       
   //多行偏差算法。
  //BiasFuse();
  DeviationFuse();
  MtrGet();//计算DeviFuse,DeviFuse_dot的隶属度
  
  EmergencyStopCount = 0;
  
  //下一幅图像的黑线起始扫描列更新，
  ////////////必须保证第CameraHight - 1行有效。
  BlackLeftLastEdgeStartCol = BlackLeftEdgeStartCol;
  BlackRightLastEdgeStartCol = BlackRightEdgeStartCol;

//  //障碍特殊控制期间，不需要更新扫描起点。
//  if(BrickSpeDealFlag == 1)
//  {
//    startline = BlackCenEdgeStartCol;
//  }
//  //不是障碍特殊控制，则正常取扫描起点。
//  else
//  {
    //若第CameraHight-1行的中心线数组有效，则取它。
    if(CenterLineLoc[CameraHight-1] != MaxValInt16)
    {
      startline = CenterLineLoc[CameraHight-1];
    }
    //若无效，则取第CameraHight-7(大于5行才能根据斜率来算)行对应的矫正行的BiasCal的计算值（根据斜率估算）
    else
    {
      //startline = CameraRealWidth / 2 - BiasCal(BlackUdisLeftLocation[CameraHight-7].y);
      //这种情况一般是十字，这时不用根据BiasCal来计算，直接用上一幅图的就好。否则会算得很乱。 
      startline = BlackCenEdgeStartCol;
    }
//  }
  
  //扫描起点的定位点
  BlackCenEdgeStartCol = BoundaryLimitRe(startline,
                                         BlackCenEdgeStartColLimitL,
                                         BlackCenEdgeStartColLimitR
                                         );
  //若上一幅图是单线，则左右两边放宽，2个单线宽度。
  if(SingleBlackLastAllFlag == 1)
  {
    //BlackLeftEdgeStartCol = BoundaryLimitRe(CenterLineLoc[CameraHight-1] + SingleBlackWidthMax[CameraHight-1],
    //左扫描起点等于定位点向右2个单线宽度。
    BlackLeftEdgeStartCol = BoundaryLimitRe(MinRe(CameraRealWidth - 1, BlackCenEdgeStartCol + 2 * SingleBlackWidthMax[CameraHight-1]),
                                            0,
                                            CameraRealWidth-1
                                            );
    //BlackRightEdgeStartCol = BoundaryLimitRe(CenterLineLoc[CameraHight-1] - SingleBlackWidthMax[CameraHight-1],
    //右扫描起点等于定位点向左2个单线宽度。
    BlackRightEdgeStartCol = BoundaryLimitRe(MaxRe(0, BlackCenEdgeStartCol - 2 * SingleBlackWidthMax[CameraHight-1]),
                                             0,
                                             CameraRealWidth-1
                                            );        
  }
  //上一幅图不是单线，双线正常处理。
  else
  {
    //不是障碍特殊处理阶段。左右各取1个单线宽度。
    if(BrickSpeDealFlag == 0)
    {
      //BlackLeftEdgeStartCol = BoundaryLimitRe(CenterLineLoc[CameraHight-1] + SingleBlackWidthMax[CameraHight-1],
      //左扫描起点等于定位点向右1个单线宽度。
      BlackLeftEdgeStartCol = BoundaryLimitRe(MinRe(CameraRealWidth - 1, BlackCenEdgeStartCol + SingleBlackWidthMax[CameraHight-1]),
                                              0,
                                              CameraRealWidth-1
                                              );
      //BlackRightEdgeStartCol = BoundaryLimitRe(CenterLineLoc[CameraHight-1] - SingleBlackWidthMax[CameraHight-1],
      //右扫描起点等于定位点向左1个单线宽度。
      BlackRightEdgeStartCol = BoundaryLimitRe(MaxRe(0, BlackCenEdgeStartCol - SingleBlackWidthMax[CameraHight-1]),
                                               0,
                                               CameraRealWidth-1
                                              );    
    }
    //障碍特殊处理阶段
    else
    {
      //左障碍，右线扫描起点向左一个单线宽度，左线扫描起点就取中心线处。
      if(BrickSpeDealDir == 0)
      {
        //BlackLeftEdgeStartCol = BoundaryLimitRe(CenterLineLoc[CameraHight-1] + SingleBlackWidthMax[CameraHight-1],
        //左扫描起点取定位点。
        BlackLeftEdgeStartCol = BoundaryLimitRe(MinRe(CameraRealWidth - 1, BlackCenEdgeStartCol + 1),
                                                0,
                                                CameraRealWidth-1
                                                );
        //BlackRightEdgeStartCol = BoundaryLimitRe(CenterLineLoc[CameraHight-1] - SingleBlackWidthMax[CameraHight-1],
        //右扫描起点等于定位点向左3个单线宽度。
        BlackRightEdgeStartCol = BoundaryLimitRe(MaxRe(0, BlackCenEdgeStartCol - 3 * SingleBlackWidthMax[CameraHight-1]),
                                                 0,
                                                 CameraRealWidth-1
                                                );
        //右扫描起点必须至少在右线左边3个单线宽度。
        if(BlackRightLoc[CameraHight - 1][0] != MaxValUint8)
        {
          BlackRightEdgeStartCol = BoundaryLimitRe(MinRe(BlackRightEdgeStartCol, BlackRightLoc[CameraHight - 1][0] - 3 * SingleBlackWidthMax[CameraHight-1]),
                                                  0,
                                                  CameraRealWidth-1
                                                  );
        }
        else
        {
        }        
      }
      //右障碍，左线扫描起点向右一个单线宽度，右线扫描起点就取中心线处。
      else
      {
        //BlackLeftEdgeStartCol = BoundaryLimitRe(CenterLineLoc[CameraHight-1] + SingleBlackWidthMax[CameraHight-1],
        //左扫描起点等于定位点向右3个单线宽度
        BlackLeftEdgeStartCol = BoundaryLimitRe(MinRe(CameraRealWidth - 1, BlackCenEdgeStartCol + 3 * SingleBlackWidthMax[CameraHight-1]),
                                                0,
                                                CameraRealWidth-1
                                                );
        //BlackRightEdgeStartCol = BoundaryLimitRe(CenterLineLoc[CameraHight-1] - SingleBlackWidthMax[CameraHight-1],
        //右扫描起点取定位点。
        BlackRightEdgeStartCol = BoundaryLimitRe(MaxRe(0, BlackCenEdgeStartCol - 1),
                                                 0,
                                                 CameraRealWidth-1
                                                );
        
        //左扫描起点必须至少在左线右边3个单线宽度。
        if(BlackLeftLoc[CameraHight - 1][0] != MaxValUint8)
        {
          BlackLeftEdgeStartCol = BoundaryLimitRe(MaxRe(BlackLeftEdgeStartCol, BlackLeftLoc[CameraHight - 1][0] + 3 * SingleBlackWidthMax[CameraHight-1]),
                                                  0,
                                                  CameraRealWidth-1
                                                  );
        }
        else
        {
        }
      }
    }
  }
  

  
  //右线扫描起点必须在左线扫描起点左边
  BlackRightEdgeStartCol = MinRe(BlackRightEdgeStartCol, BlackLeftEdgeStartCol);

  return 1;
}


//图像失败后的处理。
uint8 ImgProFailDeal(void)
{
  uint8 startline;
  int tempB, tempW, tempWB;
  
  
  //图像失败，偏差仍更新
  /*
  DeviPrePrePre=DeviPrePre;
  DeviPrePre=DeviPre;
  DeviPre=DeviFuse;
  DeviFuse=DeviPre+((DeviPre-DeviPrePre)*7+(DeviPrePre-DeviPrePrePre)*3)/10;
  MtrGet();//计算DeviFuse,DeviFuse_dot的隶属度
  */
  
  //紧急停车很危险，容易造成程序死循环，所以要慎用。用开关控制用不用。
  if(EmergencyStopSwitch == 1)
  {
    //图像处理失败计数，用作紧急停车。
    EmergencyStopCount++;
    if(EmergencyStopCount > EmergencyStopCountNum)
    {
      //计数器清零。
      EmergencyStopCount = 0;
      //紧急停车标志位。
      EmergencyStopFlag = 1;
      
      //紧急停车处理，电机输出0，并死循环，等待掉电。
      FTM0_C4V = 0;
      FTM0_C5V = 0;
      FTM0_C6V = 0;
      FTM0_C7V = 0;
      //死循环
      while(1) ;
    }
    else
    {
      
    }
  }
  else
  {
    
  }
  
  //赛道半宽判断出的单线错误标志置位时，扫描起点复位。直角弯不允许更新。
  if(RoadHalfWidthWrongFlag == 1 && AngleSpeDealFlag == 0)
  {
    //左线悬空
    if(RoadHalfWidthWrongDir == 0)
    {
      //左线最有可能是单线。
      if(BlackLeftLoc[BlackLeftStep1ScanRow][0] != MaxValUint8)
      {
        startline = BlackLeftLoc[BlackLeftStep1ScanRow][0];
      }
      else
      {
        startline = CameraRealWidth / 2;
      }
    }
    //右线悬空
    else if(RoadHalfWidthWrongDir == 1)
    {
      //右线最有可能是单线。
      if(BlackRightLoc[BlackRightStep1ScanRow][0] != MaxValUint8)
      {
        startline = BlackRightLoc[BlackRightStep1ScanRow][0];
      }
      else
      {
        startline = CameraRealWidth / 2;
      }            
    }
    else 
    {    
      //理论上不可能走到这里。
      startline = CameraRealWidth / 2;
    }
    
    //扫描起点的定位点
    BlackCenEdgeStartCol = BoundaryLimitRe(startline,
                                           BlackCenEdgeStartColLimitL,
                                           BlackCenEdgeStartColLimitR
                                           );
    
    
    //BlackLeftEdgeStartCol = BoundaryLimitRe(CenterLineLoc[CameraHight-1] + SingleBlackWidthMax[CameraHight-1],
    //左扫描起点等于定位点向左1个单线宽度。
    BlackLeftEdgeStartCol = BoundaryLimitRe(BlackCenEdgeStartCol + SingleBlackWidthMax[CameraHight-1],
                                            0,
                                            CameraRealWidth-1
                                            );
    //BlackRightEdgeStartCol = BoundaryLimitRe(CenterLineLoc[CameraHight-1] - SingleBlackWidthMax[CameraHight-1],
    //右扫描起点等于定位点向右1个单线宽度。
    BlackRightEdgeStartCol = BoundaryLimitRe(BlackCenEdgeStartCol - SingleBlackWidthMax[CameraHight-1],
                                             0,
                                             CameraRealWidth-1
                                            );
    
    //右线扫描起点必须在左线扫描起点左边
    BlackRightEdgeStartCol = MinRe(BlackRightEdgeStartCol, BlackLeftEdgeStartCol);
  }
  else
  {    
  }
  
  //只要不是直角弯导致的图像失败，则猜测可能是图像线头阈值太严格。线头阈值放宽一点点。 
  if(AngleSpeDealFlag == 0)
  {
    //左线线头阈值放宽一点点。
    //差值的处理，留有裕度，且有上下界。放宽一半裕度。
    tempWB = LimitLeftHeadWB - LimitLeftWBMargin / 2; 
    if(tempWB < LimitLeftWBMin) tempWB = LimitLeftWBMin;
    else if(tempWB > LimitLeftWBMax) tempWB = LimitLeftWBMax;
    else ;
    LimitLeftHeadWB = tempWB;
    
    //白点的处理，留有裕度，且有上下界。放宽一半裕度。
    tempW = LimitLeftHeadW - LimitLeftWMargin / 2;
    if(tempW < LimitLeftWMin) tempW = LimitLeftWMin;
    else if(tempW > LimitLeftWMax) tempW = LimitLeftWMax;
    else ;
    LimitLeftHeadW = tempW;
    
    //黑点的处理，留有裕度，且有上下界。放宽一半裕度。
    tempB = LimitLeftHeadB + LimitLeftBMargin / 2;
    if(tempB < LimitLeftBMin) tempB = LimitLeftBMin;
    else if(tempB > LimitLeftBMax) tempB = LimitLeftBMax;
    else ;
    //黑色阈值不能超过白色
    if(tempB >= LimitLeftHeadW)
    {
      tempB = LimitLeftHeadW - 1;
    }
    else
    {
    }             
    LimitLeftHeadB = tempB;  
    
    
    //右线线头阈值放宽一点点。
    //差值的处理，留有裕度，且有上下界。放宽一半裕度。
    tempWB = LimitRightHeadWB - LimitRightWBMargin / 2; 
    if(tempWB < LimitRightWBMin) tempWB = LimitRightWBMin;
    else if(tempWB > LimitRightWBMax) tempWB = LimitRightWBMax;
    else ;
    LimitRightHeadWB = tempWB;
    
    //白点的处理，留有裕度，且有上下界。放宽一半裕度。
    tempW = LimitRightHeadW - LimitRightWMargin / 2;
    if(tempW < LimitRightWMin) tempW = LimitRightWMin;
    else if(tempW > LimitRightWMax) tempW = LimitRightWMax;
    else ;
    LimitRightHeadW = tempW;
    
    //黑点的处理，留有裕度，且有上下界。放宽一半裕度。
    tempB = LimitRightHeadB + LimitRightBMargin / 2;
    if(tempB < LimitRightBMin) tempB = LimitRightBMin;
    else if(tempB > LimitRightBMax) tempB = LimitRightBMax;
    else ;
    //黑色阈值不能超过白色
    if(tempB >= LimitRightHeadW)
    {
      tempB = LimitRightHeadW - 1;
    }
    else
    {
    }             
    LimitRightHeadB = tempB;  
  }
  else
  {
  }
  
  
  
  //下一幅图像的黑线起始扫描列更新，暂时保持不变。
  BlackLeftLastEdgeStartCol = BlackLeftEdgeStartCol;
  BlackRightLastEdgeStartCol = BlackRightEdgeStartCol;
//  BlackLeftEdgeStartCol = BoundaryLimitRe(CenterLineLoc[CameraHight-1] + SingleBlackWidthMax[CameraHight-1],
//                                          0,
//                                          CameraRealWidth-1
//                                          );
//  BlackRightEdgeStartCol = BoundaryLimitRe(CenterLineLoc[CameraHight-1] - SingleBlackWidthMax[CameraHight-1],
//                                           0,
//                                           CameraRealWidth-1
//                                          );
  
  return 1;
}



//坡道禁止赛道类型判断期间的线头平齐
uint8 GyroResultForbidSpeDeal(void)
{
  uint8 row;
  
  //标志置位期间，将线头平齐。
  if(GyroResultForbidFlag == 1 && BlackLeftDone == 1 && BlackRightDone == 1 && BlackLeftHeadLine != MaxValUint8 && BlackRightHeadLine != MaxValUint8)
  {
    //右线头高，且两线头差别不大时。
    if(BlackLeftHeadLine > BlackRightHeadLine && BlackLeftHeadLine <= BlackRightHeadLine + 5)
    {
      //对右线头削顶。
      for(row = BlackRightHeadLine; ; row++)
      {
        BlackRightLoc[row][0] = MaxValUint8;
        
        if(row == BlackLeftHeadLine) break;
      }
      //更新右线头
      BlackRightHeadLine = BlackLeftHeadLine;
    }
    //左线头高，且两线头差别不大时。
    else if(BlackRightHeadLine > BlackLeftHeadLine && BlackRightHeadLine <= BlackLeftHeadLine + 5)
    {
      //对左线头削顶。
      for(row = BlackLeftHeadLine; ; row++)
      {
        BlackLeftLoc[row][0] = MaxValUint8;
        
        if(row == BlackRightHeadLine) break;
      }
      //更新左线头
      BlackLeftHeadLine = BlackRightHeadLine;
    }
    //否则不做线头平齐处理。
    else
    {
    }
  }
  else
  {
  }
  
  
  return 1;
}


//======================================================================
//函数名：ImagePro
//功  能：图像处理函数。图像尺寸缩减，黑线提取，中心线提取。
//参  数：无
//返  回：1成功 0失败
//影  响：
//说  明：1. 黑线提取失败后，不会执行中心线提取函数。返回失败。
//        2. 黑线提取失败后，后面的运动控制的参数不用更改，延用上一次的参数。
//             
//======================================================================
uint8 ImagePro(void)
{
    ErrorCountNow5++;
  
    //图像取需要的列，重新赋值是为了后面的编程方便。
    ImgPut();

    //图像处理之前的参数初始化。
    ImageProParaInit();
   
    //黑线提取函数
    if(BlackGet() == 0)
    {
        //uart_sendN(UART0, (uint8 *)"\nBlackGet Failed!", 17);  
        CenterLineResult = 0;
        PathJudgeDone = 0;             //赛道类型判断失败。
        PathType = PathTypeUnknown;    //赛道类型置为未知。
        ImgBlackSucceedFlag = 0;
    }
    //黑线提取成功后的处理。
    else
    {
    }

    //赛道类型判断
    //到这里时，左右黑线至少有一个提取成功，中心线还没有提取。
    PathJudgeNew();
    
    //直角黑块确认。//这届不使用
    //AngleZoneConfirm();
    
    //障碍物确认 与 避障处理，会改变CenterLineLoc[]数组的值。
    BrickAvoid();
    
    //坡道禁止赛道类型判断期间的线头平齐
    GyroResultForbidSpeDeal();
 
    //中心线提取函数。里面有中心线的线头线尾查找。
    //除非图像太差，否则该函数基本上不会返回0.
    //黑线提取失败后，不允许中心线提取。
    CenterLineResult = 1;
    if(ImgBlackSucceedFlag == 0 || CenterLineGetRaw() == 0)
    {
      //uart_sendN(UART0, (uint8 *)"\nCenterLineGet Failed!", 22); 
      CenterHeadLine = MaxValUint8;
      CenterEndLine = MaxValUint8;
      CenterLineResult = 0; //
      PathJudgeDone = 0;   //赛道类型判断失败。
      PathType = 0;        //赛道类型置为未知。
      ImgCenterSucceedFlag = 0;
    }
    else
    {
    }
    
    //必须黑线和中心线都提取成功后才能视为图像处理成功。
    if(ImgBlackSucceedFlag == 1 && ImgCenterSucceedFlag == 1)
    {
      GPIO_PDOR_REG(PORTA) |= (1<<17);
      ImgProSucceedFlag = 1;
    }
    else
    {
      GPIO_PDOR_REG(PORTA) &= ~(1<<17);
      ImgProSucceedFlag = 0;
    }
   
    
    if(ImgProSucceedFlag == 1)
    {
      //图像处理成功后的处理。
      ImgProSucceedDeal();
    }
    else
    {
      //图像处理失败后的处理。
      ImgProFailDeal();
    }
    
    //判断是否跑出赛道，10次图像失败就认为跑出赛道（刚开始做车的阶段先注释掉）
    /*if(CenterHeadLine==MaxValUint8 && CenterEndLine==MaxValUint8)
    {
      if(++ImgProFailCount > 10)
      {
        RunOutFlag = 1;
      }
    }
    else
    {
        ImgProFailCount = 0;
    }*/
    
    //偏差滤波。
    //DeviNowFilter();
    
    //直角弯确认函数。在偏差求取后面，因为此时是绝对瞎眼。
    //若干次直角弯确认后，才会最终确认为直角弯。
    //最终确认后，会进行若干次的特殊处理。
    //特殊处理时，不受图像成功标志位的影响。
    //特殊处理时，会对几个Level和DeviNow进行特殊赋值。
    //有想过针对最后一次有效的Devi进行分类的特殊处理，但似乎没有必要。
    
    //这届不需要确认直角弯
    //AngleStep2Confirm();
    
    return ImgProSucceedFlag;
}



//发送图像时专用的图像数组更新。
void ImgSendPut(void)
{
    uint16 i, j, k, temp1, temp2;   
    
    temp1 = CameraRealLeftCol;
    temp2 = CameraRealLeftCol + CameraRealWidth;
    //temp1 = CameraWidth/2 - CameraRealWidth/2 - 1;
    //temp2 = CameraWidth/2 + CameraRealWidth/2 - 1;
    
    for(i = 0; i < CameraHight; i++)
    {
      k = 0;
      for(j = temp1; j < temp2; j++)
      {
        ImgNew[i][k] = ImgRaw[i][j];
        k++;
      }
    }
}

//图像发送模式专用函数
uint8 ImageSend(void)
{
    //发送图像时专用的图像数组更新。
    ImgPut();
    
    //发送图像存储数组到上位机
    SendImage(ImgNew);      
    
    return 1;
}

