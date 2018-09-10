#ifndef __ImagePro_H__
#define __ImagePro_H__
/******************************************************************************/
/*******************************************************************************
  文件名：图像处理程序头文件IamgePro.h
  功  能：图像处理
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
#include "ImageGet.h"
#include "RunControl.h"
#include "math.h"

//2. 宏定义
#define CameraRealWidth 250         //一行实际取的像素点数
#define MaxValUint8 255             //自定义的无效值
#define MaxValInt16 32767           //自定义的无效值。
#define PhyCenterCol 125           //物理中心所在列（最好跟CameraRealWidth/2相等）
//#define RoadWidth 166              //畸变矫正后的赛道宽度（直道上每行都是这个值）
#define RoadWidth 200

//#define TempHalfWidthVal 90           //暂定的赛道半宽。
#define TempHalfWidthVal 95           

#define BlackCenEdgeStartColLimitL 30   //扫描起点定位点的左边界限制
#define BlackCenEdgeStartColLimitR 220   //扫描起点定位点的左边界限制

//赛道类型
#define PathTypeUnknown     0            //赛道类型，未知
#define PathTypeStraight    1            //赛道类型，直道
#define PathTypeLeft        2            //赛道类型，左内
#define PathTypeRight       3            //赛道类型，右内
#define PathTypeAngleL      4            //赛道类型，左直角
#define PathTypeAngleR      5            //赛道类型，右直角
#define PathTypeSmallSL     6            //赛道类型，左S,反Z字形。
#define PathTypeSmallSR     7            //赛道类型，右S,Z字形。
#define PathBrickLeft       8            //赛道类型，左障碍
#define PathBrickRight      9            //赛道类型，右障碍
#define PathAngleZone       10           //赛道类型，直角黑色区域

#define PathJudgeStraightSlopeLimit 2100   //赛道类型判断时，直道小5点斜率的限制，必须在2000以内。
#define PathSmallSHeadLimit 10            //赛道类型判断时，小S弯判断要求的线头高度要求值
#define PathSmallSGapLine    3            //赛道类型判断时，小S弯判断时跳过的行数。
#define PathJudgeNewStraightChoose 2     //赛道类型，直道判断时。
                                         //0表示判断直道要用到所有的点，而不是掐头。比较严格，会减小直道判断成功率，但不容易将其它赛道误判成直道。
                                         //1表示判断直道时掐掉线头开始往近行的10行，再判断。能增加直道判断成功率，但容易将其它赛道误判成直道。
                                         //2是1的改进，考虑到了Black5Slope数组的个数与有效黑点不一定相同，大大减少了误判的机率。
//中心线
#define CenterForwardLineNum 4           //中心线最后几行向前补线时，计算斜率基于的行数。

#define BlackStep1Size 48                 //黑线提取第1阶段的搜索行数
#define BlackMiddleLine 50                //小于该行号的行，周围黑点要求数由3点减为2点。
#define BlackFarLine 40                   //小于该行号的行，周围黑点要求数由2点减为1点。
#define BlackFarfarLine 20                //小于该行号的行，只要求跳变和白点，不要求周围黑点。
#define BlackCrossGapLine 12              //十字路口时，第一段黑线到第二段黑线跳过的行数的估计值。原来是12
#define BlackBackLineNum 4                //十字向后补线时，最小二乘法基于的点数。
#define BlackCrossConfirmNum 4        //十字向前确认窗口时，最小二乘法基于的点数
#define BlackCrossConfirmWin 40     //十字向前确认窗口时，允许开窗的大小。原来是40
#define LeastSquareMulti 1000        //最小二乘法乘以的倍数
#define BlackLeftCrossStep3Multi 1000      //左线十字检测第3阶段脑补线处理时，乘以的倍数
#define BlackRightCrossStep3Multi 1000      //右线十字检测第3阶段脑补线处理时，乘以的倍数
#define BlackAgainForCrossFar10CutSwitch 1  //左右线十字再检测的最远10行删除开关，1表示最远10行的十字再检测就不要了，0表示不作特殊处理
#define BlackCrossConfirmSlopeLimit 3000   //十字补线确认时，上一幅图的斜率限制，不能太大。
#define BlackCrossConfirmHeadLineLimit 40  //十字补线确认时，上一幅图的线头限制，不能太近。

//十字外尾
#define BlackLeftCrossTailRowLimit 50      //十字外尾时，第1阶段检测的线头所在行小于该值时，在向后补线里面开始十字外尾检测。
#define BlackLeftCrossTailSlopeLimit 3400  //十字外尾时，第1个小五点斜率大于该值时，则确认为十字外尾。左线斜率是负值。
#define BlackRightCrossTailRowLimit 50      //十字外尾时，第1阶段检测的线头所在行小于该值时，在向后补线里面开始十字外尾检测。
#define BlackRightCrossTailSlopeLimit 3400  //十字外尾时，第1个小五点斜率大于该值时，则确认为十字外尾。右线斜率是正值。




//左


//左
//更新3个关键值的时候留的裕度
#define LimitLeftWBMargin 20         //黑白像素点差值的裕度
#define LimitLeftWMargin 40          //白点的裕度，白点对光线的变化很敏感，可以适当放宽。
#define LimitLeftBMargin 35           //黑点的裕度

#define BlackLeftWinVal 15           //窗口内跳变检测函数的窗的大小
#define BlackLeftStep1ScanMinus 8    //第1阶段线头查找时的连续两行黑线的差的允许的最大值
//#define BlackLeftBackLineStart 12    //向后补线时，线头最少要经过多少行。不允许比CameraHight-1-BlackControlLineBase还大。
#define BlackLeftBackLineStart 48
#define BlackLeftEdgeNum 3           //向左检测跳变沿的时候，跳变垮过的点数
#define BlackLeftBackLineSlope 100   //左黑线向后补线时，所补线的斜率检测确认。左上角为原点，向下为x,向右为y.
#define BlackLeftCrossConfirmSlopeLimit 1100  //左十字中间补线确认时，对斜率的限制
#define BlackLeftCrossConfirmGapLine 35      //左十字中间补线时，若近处的行太小，则必须大于的行间隔数。太小了会270误补十字，太大了会正常十字出不来。

//右
//更新3个关键值的时候留的裕度
#define LimitRightWBMargin 20         //黑白像素点差值的裕度
#define LimitRightWMargin 40          //白点的裕度，白点对光线的变化很敏感，可以适当放宽。
#define LimitRightBMargin 35           //黑点的裕度


#define BlackRightWinVal 15            //窗口内跳变检测函数的窗的大小
#define BlackRightStep1ScanMinus 8    //第1阶段线头查找时的连续两行黑线的差的允许的最大值
//#define BlackRightBackLineStart 12    //向后补线时，线头最少要经过多少行。不允许比CameraHight-1-BlackControlLineBase还大。
#define BlackRightBackLineStart 48
#define BlackRightEdgeNum 3           //向右检测跳变沿的时候，跳变垮过的点数
#define BlackRightBackLineSlope 100   //右黑线向后补线时，所补线的斜率检测确认。左上角为原点，向下为x,向右为y.
#define BlackRightCrossConfirmSlopeLimit 1100  //右十字中间补线确认时，对斜率的限制
#define BlackRightCrossConfirmGapLine 35      //右十字中间补线时，若近处的行太小，则必须大于的行间隔数。太小了会270误补十字，太大了会正常十字出不来。

#define DeviSelRowStart 30                //偏差求取时，选择的起始行。
#define DeviSelRowEnd 55                //偏差求取时，选择的终止行。
//DeviNow低通滤波时的数组
///////////////////修改此值时需要修改DeviNowArrayWight、DeviNowArray数组
#define DeviNowArrayNum 3

//紧急停车
#define EmergencyStopCountNum 15       //图像处理失败连续计数大于该值时，则停车等待掉电。125个16ms约为2s.
#define EmergencyStopSwitch 0           //紧急停车开关，1开，0关。因为紧急停车的会出现死循环，所以不太敢用。

//直角黑色区域
//判断
#define AngleZoneHeadLineNum 15     //直角黑块，线头差小于该值，才开始判断直角黑块。
#define AngleZoneJudgeRowNear 50   //直角黑块，两线头必须小于的行
#define AngleZoneJudgeRowFar 14   //直角黑块，两线头必须大于的行
#define AngleJudgeSlopeRowNum 8        //直角黑块，斜率计算基于的行数。
#define AngleZoneJudgeRowCom 2   //直角黑块，最小二乘法(因桶形畸变所以定位不够准)计算目标检测行的起始列和终止列时，向中间靠拢的修正列数。
#define AngleZoneJudgeRowNum 5   //直角黑块，根据线头选中的要检测的行的行总数
#define AngleZoneJudgeRoadWidth 70 //直角黑块，扫描有效黑时默认的赛道宽度。
#define AngleZoneJudgeBlackNumP 45  //直角黑块，优势边要求的有效黑点最少个数，取45列左右，比障碍多
#define AngleZoneJudgeBlackNumN 20  //直角黑块，劣势边要求的有效黑点最少个数，取20列左右，比单线宽度多
//确认
#define AngleZoneConfirmLockMeterCountLimit 40 //直角黑块，锁存后进行的累加计数阈值，用以直角黑块误判后的恢复正常。
                                             //按2 m/s速度，跑1米大概需要0.5s，也就是30个16ms，3m/s对应20个16ms.所以应取30个以上。速度更低时应该取更大。
                                             //加上在直角黑块前一段距离就很有可能被锁存了，所以距离保护值更应该加长。2m/s时，取2m的距离值60.
#define AngleZoneConfirmMatNum 6          //直角黑块，标志存储数组的个数。
//#define AngleZoneCheckColGapMin 48      //起始列和终止列之间必须大于的宽度。这里有几个讲究。
                                        //1. 必须比障碍物的列数（45列）要多。
                                        //2. 要考虑到正常直道时该行的赛道宽度，比如24、25行是100列。
                                        //3. 要考虑到向中心线修正量AngleZoneCheckRowCom。
                                        //4. 要考虑到出弯即直角黑块时，两列定位的偏移误差。
                                        //也就是说，要大于45列，小于100-10*2=80列，60列左右。

//直角弯处理
#define AngleConfirmMatNum 6          //直角弯标志存储数组的个数。

///////修改此值的时候注意修改AngleSpeDealDeviNow、AngleSpeDealDeviNowLevel、AngleSpeDealSlopeC的数组个数和内容
#define AngleSpeDealBackCountNum 15      //最终确认为直角弯后，最多倒数这么多次连续进行直角弯的特殊处理
#define AngleWhiteAreaLimitVal 20       //直角弯判定第5步白色区域检测时，允许的比白色阈值少的值。
#define AngleSpeDealStartHeadLine 40    //直角弯确认后，矮线线头大于该值才开始特殊处理。
#define AngleSpeDealBackCountMin 8     //至少要进行AngleSpeDealBackCountMin次强制控制。
#define AngelSpeDealExitSlope 3000     //斜率绝对值小于AngelSpeDealExitSlope，也可以把控制权交给常规控制了。

//障碍
#define BrickSpeDealBackCountNum 20      //最终确认为障碍后，最多倒数这么多次连续进行障碍的特殊处理
#define BrickConfirmMatNum 6          //障碍标志存储数组的个数。

#define BrickSpeDealStartJudgeRow 25   //障碍，当障碍最近处越过该行，则特殊控制开始。取值范围大概为30-50，即[BrickEndRow, CameraHight-BrickSlopeRowNum-1]
#define BrickSpeDealExitState1Val 65   //障碍，判出特殊控制时，第59行赛道半宽小于该值则表示小车当前与障碍平行。
#define BrickSpeDealExitState2Val 75   //障碍，判出特殊控制时，第59行赛道半宽大于该值则表示小车前端已经一定程序越过障碍。
//#define BrickSpeDealBias 8            //障碍，特殊控制时，偏移的列数
#define BrickSpeDealCenChoose 0        //障碍，避障时参考的黑线种类选择，0未矫正的数组（是斜的），1矫正后的数组（更竖直）。

#define BrickEndRow 35            //障碍，扫描起始行，近行。不能太大，以保证计算斜率的点足够。
#define BrickHeadRow 15              //障碍，扫描结束行，远行。也是非障碍边线头必须小于的行。
#define BrickScanColNum 60          //障碍，扫描的列数。要比BrickLineBlackCountNum的两倍大。EndRow=30时，对应45，EndRow=35时，对应60。
#define BrickColCom 5               //障碍，直角梯形的非直角边向中心的修正列，防止黑线上黑点对有效黑点造成影响。
#define BrickLineBlackCountNum 20   //障碍，该行有效必须大于的黑点数。要比BrickScanColNum的一半小。
#define BrickLineCountNum 3        //障碍，有效行大于该数则判断为障碍。
#define BrickSlopeRowNum 8            //障碍，斜率计算基于的行数。
#define BrickSpeDealServorLimit 30    //障碍，特殊控制期间，对舵机打角的限制。


//单线地板处理
#define SingleWhiteAvgNum 5            //单线左右白点取像素平均值时，基于的最多的点数。若太靠左右边界则有可能小于该值。
#define SingleAvgComVal 20             //单线左右白点取像素平均值时，两者差若在此范围内，则为单线，否则有可能是地板。
#define SingleFloorConfirmNum 2        //单线验证地板时，验证的次数。
                                    
//斜率曲率
#define CurveStraightLimitL 30          //赛道类型判断时，通过左黑线曲率判断赛道为直道的阈值。很敏感，任何硬软件改变后需手动改动此值。
#define CurveStraightLimitC 30          //赛道类型判断时，通过中心线曲率判断赛道为直道的阈值。很敏感，任何硬软件改变后需手动改动此值。
#define CurveStraightLimitR 30          //赛道类型判断时，通过右黑线曲率判断赛道为直道的阈值。很敏感，任何硬软件改变后需手动改动此值。
#define CurveGetCompen 2000             //曲率求取时，为了不使数太小，乘的补偿量。
#define SlopeLimitL 1500                //赛道类型判断时，左黑线的斜率的阈值。
#define SlopeLimitC 1500                //赛道类型判断时，中心线的斜率的阈值。
#define SlopeLimitR 1500                //赛道类型判断时，右黑线的斜率的阈值。

//畸变矫正
#define BlackUdisFarLine 18            //黑线畸变矫正时，远处行与中间行的分界行。
#define BlackUdisMiddleLine 38         //黑线畸变矫正时，中间行和近处行的分界行。

//大斜率检测
//关于斜率跳变的阈值，分析了多幅斜入十字的图像，发现跳变在5500-10000之间，尤其是7000-8000.所以采用5200.实践发现，这个值对小S弯等图像误判较少。
#define BlackLeftBigChangeSlopeLimit 4000   //大斜率检测时，前后斜率差的阈值。    
#define BlackRightBigChangeSlopeLimit 4000   //大斜率检测时，前后斜率差的阈值。


//回弯检测。
#define BlackLeftRollSlopeLimit 2000         //左回弯检测时，左线斜率的阈值，不能小于-BlackLeftRollSlopeLimit
#define BlackLeftRollLengthLimit 12          //左回弯检测时，左线长度的阈值，不能小于BlackLeftRollLengthLimit
#define BlackLeftRollSlopeRLimit 2000        //左回弯检测时，右线斜率的阈值。要大于该值。看了多幅270弯图像，发现实际在3500-4500之间。
#define BlackRightRollSlopeLimit 2000        //右回弯检测时，右线斜率的阈值，不能大于BlackRightRollSlopeLimit
#define BlackRightRollLengthLimit 8         //右回弯检测时，右线长度的阈值，不能小于BlackRightRollLengthLimit
#define BlackRightRollSlopeLLimit 2000       //右回弯检测时，左线斜率的阈值。绝对值要大于该值。看了多幅270弯图像，发现实际在3500-4500之间。


//赛道半宽
#define RoadHalfWidthSingleLimit 75       //通过赛道半宽判断单线错误时，每个有效双行必须大于的赛道半宽值。
#define RoadHalfWidthSingleGapLine 10     //通过赛道半宽判断单线错误时，第一种情况里，长度的阈值。这个必须设得够小，不然会影响正常采线。
#define RoadHalfWidthLimitMin 70          //赛道半宽更新时的最小值。
#define RoadHalfWidthLimitMax 100          //赛道半宽更新时的最大值。


//大跳变最终检测（针对直角黑块）
#define BlackBigChangeFinalCheckSwitch 1            //大跳变最终检测开启
#define BlackBigChangeFinalCheckSlopeLimit 2500 //5Slope数组前后两个同号且差值大于等于这个值时，开启删除操作

//3. 变量类型定义
typedef struct
{
  int32 x;  //列
  int32 y;  //行
}LocationUdis;  //畸变矫正点的位置

//4. 变量声明
//extern uint8 ImgNew[CameraHight][CameraRealWidth];      //重新处理后的数据储存数组   
extern uint8 LimitLeftWMin;
extern uint8 LimitLeftWMax;
extern uint8 LimitLeftBMin;
extern uint8 LimitLeftBMax;
extern uint8 LimitLeftWBMin;
extern uint8 LimitLeftWBMax;

extern uint8 LimitRightWMin;
extern uint8 LimitRightWMax;
extern uint8 LimitRightBMin;
extern uint8 LimitRightBMax;
extern uint8 LimitRightWBMin;
extern uint8 LimitRightWBMax;

extern uint8 BlackLeftLoc[CameraHight][3];
extern uint8 BlackRightLoc[CameraHight][3];
extern int16 CenterLineLoc[CameraHight];      //中心线位置存储数组, 255为无效值
extern int16 CURVEL;                          //曲率求取时，左黑线的曲率，顺时钟为负，逆时针为正。
extern int16 CURVEC;                          //曲率求取时，中心线的曲率，顺时钟为负，逆时针为正。
extern int16 CURVER;                          //曲率求取时，右黑线的曲率，顺时钟为负，逆时针为正。
extern uint8 PathType;                        //赛道类型
extern uint8 BlackLeftCrossDone;              //左黑线十字完成标志0未进入 1失败 2成功
extern uint8 BlackRightCrossDone;             //右黑线十字完成标志0未进入 1失败 2成功
extern uint8 BlackLeftStep4Flag;              //左黑线需要执行第4阶段的标志位，0未进入，1失败，2成功
extern uint8 BlackRightStep4Flag;             //右黑线需要执行第4阶段的标志位，0未进入，1失败，2成功
extern uint8 BlackLeftHeadLine;
extern uint8 BlackRightHeadLine;
extern uint8 BlackLeftEndLine;
extern uint8 BlackRightEndLine;
//extern float SlopeC;                           //斜率求取时，中心线的斜率。
//extern float SlopeLimitC;                      //赛道类型判断时，中心线的斜率的阈值。

extern uint8 DeviNowLevel;                    //当前偏差的绝对值的等级，10个等级，0偏差最小，9偏差最大。
extern uint8 DeviRowChosen;
extern int32 SlopeC;                  
extern uint8 ImgProSucceedFlag;
extern uint8 ImgBlackSucceedFlag;                 //黑线提取成功标志位，默认为1，若黑线提取失败，则置0.
extern uint8 ImgCenterSucceedFlag;                //中心线提取成功标志位，默认为1，若中心线提取失败，则置0.
extern uint8 SingleDoneFlag;              //单线处理时，单线成功标志位。
extern uint8 SingleFloorFlag;             //单线处理时，地板判断成功标志位。1该幅图画左线是地板，0不是或没有判断。
extern uint8 SingleFloorConfirmCount;       //单线处理时，地板判断确认计数值。每幅图归零一次。
extern uint8 SingleFloorConfirmCountMax;   //单线处理时，地板判断确认计数值的最大值，用于确认最佳确认阈值。每幅图归零一次。
extern uint8 PathSmallSDoneFlag;              //赛道类型判断时，小S弯判断成功标志位。1成功，0失败。每幅图赛道类型判断时初始化为0。
extern uint8 PathSmallSDir;                   //赛道类型判断时，小S弯起始方向。1
extern int32 DeviFuse;            //多行融合后的偏差。
extern int32 Devi_near,Devi_far;      //近远行偏移量
extern int32 DeviPre;                  //上一次偏差
extern int32 DeviPrePre;              //上上次偏差
extern int32 DeviPrePrePre;              //上上上次偏差
extern uint8 FuzzyDegree;              //隶属度计算辅助变量


//直角黑色区域
extern uint8 AngleZoneCheckRow0;     //直角黑块，根据线头选中的要检测的行之一。
extern uint8 AngleZoneCheckCol0L;    //直角黑块，根据线头选中的要检测的行之一的检测左边界。
extern uint8 AngleZoneCheckCol0R;    //直角黑块，根据线头选中的要检测的行之一的检测左边界。
extern uint8 AngleZoneCheckRow1;     //直角黑块，根据线头选中的要检测的行之一。
extern uint8 AngleZoneCheckCol1L;    //直角黑块，根据线头选中的要检测的行之一的检测左边界。
extern uint8 AngleZoneCheckCol1R;    //直角黑块，根据线头选中的要检测的行之一的检测左边界。
extern uint8 AngleZoneConfirmLockMeterCount; //直角黑块，锁存后进行的累加计数，用以直角黑块误判后的恢复正常。

extern uint8 AngleZoneConfirmCount;   //对直角黑块确认次数的计数。
extern uint8 AngleZoneConfirmLockFlag;    //直角黑块判断成功锁存标志位,确认是直角黑块后置位。
                                       //有两种情况清零，一是之后的线头差检测成功后，二是一定距离后仍没有检测到线头差。
extern uint8 AngleZoneConfirmMat[AngleZoneConfirmMatNum];  //直角黑块确认标志存储数组，1表示本次为直角黑块，0表示不是直角黑块。
extern uint8 AngleZoneConfirmMatCollect;      //直角黑块，AngleZoneConfirmMat数组中1的个数。

//直角弯处理
extern uint8 AngleConfirmCount;   //对直角弯确认次数的计数。
extern uint8 AngleSpeDealBackCount;  //直角弯最终确认后，特殊情况次数的倒数计数。
extern uint8 AngleSpeDealDir;        //直角弯方向标志，0左直角弯，1右直角弯，2错误。
extern uint8 AngleSpeDealFlag;       //直角弯特殊处理标志，1表示该周期进行了直角弯特殊处理，0表示该周期没有进行直角弯特殊处理。
extern uint16 AngleConfirmCountMax;   //记录实际直角弯确认次数的最大值，方便调节阈值。
extern uint8 AngleConfirmLockFlag;    //直角弯判断成功锁存标志位,确认是直角弯后置位，用以确认直角弯特殊处理开始的位置。
                                   //直角弯锁存期间，不进行特殊赛道判断。
extern uint8 AngleSpeDealExitJudgeFlag;  //出直角弯判断标志，1已出，0未出
extern uint8 AngleConfirmMat0[AngleConfirmMatNum];  //直角弯确认标志存储数组，1表示本次为直角，0表示不是直角。
extern uint8 AngleConfirmMatCollect0;      //直角弯 ，AngleConfirmMat数组中1的个数。
extern uint8 AngleConfirmMat1[AngleConfirmMatNum];  //直角弯确认标志存储数组，1表示本次为直角，0表示不是直角。
extern uint8 AngleConfirmMatCollect1;      //直角弯 ，AngleConfirmMat数组中1的个数。

//障碍物
extern uint8 BrickConfirmCount;   //对障碍物确认次数的计数。
extern uint8 BrickSpeDealBackCount;  //障碍物最终确认后，特殊情况次数的倒数计数。
extern uint8 BrickSpeDealDir;        //障碍物位置，0左障碍物，1右障碍物，2错误。
extern uint8 BrickSpeDealFlag;       //障碍物特殊处理标志，1表示该周期进行了障碍物特殊处理，0表示该周期没有进行障碍物特殊处理。
extern uint16 BrickConfirmCountMax;   //记录实际障碍物确认次数的最大值，方便调节阈值。
extern uint8 BrickConfirmLockFlag;    //障碍物判断成功锁存标志位,确认是障碍物后置位，用以确认障碍物特殊处理开始的位置。
                                   //障碍物锁存期间，不进行特殊赛道判断。
extern uint8 BrickSpeDealExitJudgeFlag;  //出障碍物判断标志，1表示初始态，2表示当前已经与障碍平行，0退出特殊控制，把控制权交还给普通控制。
                                      //不用每幅图更新，每次过完障碍，会手动写成1.
extern uint8 BrickConfirmMat[BrickConfirmMatNum];  //障碍物确认标志存储数组，1表示本次为障碍物，0表示不是障碍物。
extern uint8 BrickConfirmMatCollect;      //障碍，BrickConfirmMat数组中1的个数。

extern uint8 BrickSpeDealStartJudgeCol;     //障碍，判定是否开始特殊控制时，特定行对应的最小二乘法算出来的列。
extern uint8 BrickEndCol;    //障碍物的直角梯形，起始行（近行）黑点所在列。
extern uint8 BrickHeadCol;   //障碍物的直角梯形，结束行（远行）黑点所在列。
extern uint8 BrickLeftCol;   //障碍物的直角梯形，右障碍物的左边界。
extern uint8 BrickRightCol;   //障碍物的直角梯形，左障碍物的右边界。

extern uint8 BrickLineCountNumRecord;

extern int16 ErrorCountNow5;
extern LocationUdis BlackUdisLeftLocation[CameraHight];  //畸变矫正后，左黑线的位置储存数组。有正负，包括了行和列。
extern LocationUdis BlackUdisRightLocation[CameraHight]; //畸变矫正后，左黑线的位置储存数组。有正负，包括了行和列。
extern uint8 BlackLeftDone;
extern uint8 BlackRightDone;
extern uint8 BlackLeftLastEdgeStartCol;
extern uint8 BlackRightLastEdgeStartCol;
extern uint8 BlackLeftEdgeStartCol;
extern uint8 BlackRightEdgeStartCol;
extern uint8 BlackCenEdgeStartCol;             //扫描起点定位点
extern uint8 flag000;
extern int32 flag111;
extern int flag222;

extern uint8 BlackLeftContinueFlag;
extern uint8 BlackRightContinueFlag;
extern uint8 LimitLeftHeadWB;    
extern uint8 LimitLeftHeadW;      
extern uint8 LimitLeftHeadB;   
extern uint8 LimitRightHeadWB;    
extern uint8 LimitRightHeadW;      
extern uint8 LimitRightHeadB;   
extern uint8 countcount;
extern uint8 BrickSpeDealCenFlag;

//直角黑块
extern uint8 AngleZoneConfirmMatLimit;         //直角黑块，标志存储数组中直角黑块的个数大于该值时，则锁存为直角黑块。
//直角
extern uint8 AngleConfirmMatLimit;         //直角弯标志存储数组中直角的个数大于该值时，则锁存为直角。
extern uint8 AngleLongHeadLineStrategy;                //直角弯长线头策略，1表示去掉长线头，0表示不去长线头。
extern uint8 AngleHeadLineNum;              //左右线头之差大于该值，则有可能为直角弯。
//障碍
extern uint8 BrickConfirmMatLimit;         //障碍标志存储数组中障碍的个数大于该值时，则锁存为障碍。
extern uint8 BrickConfirmFinalColCom;     //障碍躲避时的列修正量（以15为零，左减右加。）
//长直道
extern uint8 PathRealStraightFlag;        //长直道标志，1是长直道，0不是。
extern uint8 PathRealStraightCountLimit;  //连续直道计数阈值，大于该值则确认为长直道。

extern uint8 xiaoZhiDaoFlag;
extern uint8 ShiZhiFlag;
extern uint8 limitStopLineBlackXiu;


//extern uint16 SW0;
//extern uint16 SW1;
//extern uint16 SW2;
//extern uint16 SW3;
//extern uint16 SW4;
//extern uint16 SW5;
//extern uint16 SW6;
//extern uint16 SW7;
//extern uint16 SW8;


extern int32 SlopeL;                           //斜率求取时，左黑线的斜率。
extern int32 SlopeC;                           //斜率求取时，中心线的斜率。
extern int32 SlopeR;                           //斜率求取时，右黑线的斜率。

extern uint8 StopLineCheckFlag;             //停车线
extern uint8 RunOutFlag;                    //跑出赛道标志
extern int32 DeviFuse_old;

//5. 函数声明
extern uint8 ImgNew[CameraHight][CameraRealWidth];
extern void ImaegExtract(uint8 *dst, uint8 *src, uint32 srclen);
extern uint8 ImagePro(void);
extern uint8 ImageSend(void);
extern uint8 VariablesInit(void);
extern int32 AbsInt(int32 val);
uint8 CurveSlopeGetLeft(void);
uint8 CurveSlopeGetRight(void);
uint8 BlackUdis(void);
extern uint8 BrickSpeDealServor(void);

//OV7725图像数据发送函数
//extern void SendImage(uint8 *imgaddr, uint32 imgsize);
//OV7620图像数据发送函数
//extern void SendImage(uint8 imgaddr[CameraHight][CameraWidth]);





#endif //end of __ImagePro_H__
