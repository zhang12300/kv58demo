//==============================================================================
//文件名称：Flash.c
//功能概要：K60 Flash擦除/写入底层驱动程序源文件
//==============================================================================
#include "Flash.h" 

//=======================内部调用函数声明=======================================
//==============================================================================
//函数名称：hw_flash_sign_off
//函数返回：无
//参数说明：无
//功能概要：配置Flash存储控制器，清除Flash预读取缓冲
//==============================================================================
static void hw_flash_sign_off(void);

//==============================================================================
//函数名称：hw_flash_cmd_launch
//函数返回：0-成功 1-失败
//参数说明：无
//功能概要：启动Flash命令
//==============================================================================
static uint32 hw_flash_cmd_launch(void);

//==============================================================================

//=================外部接口函数=================================================
//==============================================================================
//函数名称：FlashInit
//函数返回：无
//参数说明：无
//功能概要：初始化Flash模块
//==============================================================================
void FlashInit(void)
{
  //清除FMC缓冲区
  hw_flash_sign_off();
    
  //禁止看门狗
  WDOG_UNLOCK = 0xC520;//Writing the unlock sequence values to this register to make the watchdog write-once registers writable 
                       //again. The required unlock squence is 0xc520 followed by 0xd928 within 20 bus clock cycles. A valid 
                       //unlock sequence opens a window equal in length to the WCT within which you can update the registers.
                       // writing a value other than the above mentioned sequence or if the ssequence is longer than 20 bus cycles,
                       //reset the system or if IRQRSTEN is set,it interrupts and then resets the system. The unlock sequence is effective 
                       // only if ALLOWUPDATE is set.
  WDOG_UNLOCK = 0xD928;
  WDOG_STCTRLH = 0;
    
  //等待命令完成
  while(!(FTFL_FSTAT & CCIF));
    
  //清除访问出错标志位
  FTFL_FSTAT = ACCERR | FPVIOL;
}

//==============================================================================
//函数名称：FlashEraseSector
//函数返回：函数执行执行状态：0=正常；非0=异常。
//参数说明：sectorNo：扇区号（K60N512实际使用0~255）
//功能概要：擦除指定flash扇区
//==============================================================================
uint8 FlashEraseSector(uint16 sectorNo)
{
  union
  {
    uint32  word;
    uint8   byte[4];
  }dest;
    
  dest.word = (uint32)(sectorNo*(1<<11));

  //设置擦除命令
  FTFL_FCCOB0 = ERSSCR;   //lyx: ERSSCR: 0x09 擦除Flash扇区
    
  //设置目标地址
  FTFL_FCCOB1 = dest.byte[2];
  FTFL_FCCOB2 = dest.byte[1];
  FTFL_FCCOB3 = dest.byte[0];
    
  //执行命令序列
  if(1 == hw_flash_cmd_launch())    //若执行命令出现错误
    return 1;     //擦除命令错误
   
  //若擦除sector0时，则解锁设备
  if(dest.word <= 0x800)
  {
    //写入4字节
    FTFL_FCCOB0 = PGM4; 
    //设置目标地址
    FTFL_FCCOB1 = 0x00;
    FTFL_FCCOB2 = 0x04;
    FTFL_FCCOB3 = 0x0C;
    //数据
    FTFL_FCCOB4 = 0xFF;
    FTFL_FCCOB5 = 0xFF;
    FTFL_FCCOB6 = 0xFF;
    FTFL_FCCOB7 = 0xFE;
    //执行命令序列
    if(1 == hw_flash_cmd_launch())  //若执行命令出现错误
      return 2;   //解锁命令错误
    }  
    
    return 0;  //成功返回
}

//==============================================================================
//函数名称：FlashWrite
//函数返回：函数执行状态：0=正常；非0=异常。
//参数说明：sectNo：目标扇区号 （K60N512实际使用0~255）
//          offset:写入扇区内部偏移地址（0~2043）
//          cnt：写入字节数目（0~2043）
//          buf：源数据缓冲区首地址
//功能概要：flash写入操作
//==============================================================================
uint8 FlashWrite(uint16 sectNo,uint16 offset,uint16 cnt,uint8 buf[])
{
  uint32 size;
  uint32 destaddr;
    
  union
  {
    uint32   word;
    uint8_t  byte[4];
  }dest;
    
  if(offset%4 != 0)
    return 1;   //参数设定错误，偏移量未对齐（4字节对齐）
    
  //设置写入命令
  FTFL_FCCOB0 = PGM4;
  destaddr = (uint32)(sectNo*(1<<11) + offset);//计算地址
  dest.word = destaddr;
  for(size=0; size<cnt; size+=4, dest.word+=4, buf+=4)
  {
    //设置目标地址
    FTFL_FCCOB1 = dest.byte[2];
    FTFL_FCCOB2 = dest.byte[1];
    FTFL_FCCOB3 = dest.byte[0];
 
    //拷贝数据
    FTFL_FCCOB4 = buf[3];
    FTFL_FCCOB5 = buf[2];
    FTFL_FCCOB6 = buf[1];
    FTFL_FCCOB7 = buf[0];
        
    if(1 == hw_flash_cmd_launch()) 
      return 2;  //写入命令错误
  }
  return 0;  //成功执行
}

//========================内部函数实现==========================================
//==============================================================================
//函数名称：hw_flash_sign_off
//函数返回：无
//参数说明：无
//功能概要：配置Flash存储控制器，清除Flash预读取缓冲
//==============================================================================
void hw_flash_sign_off(void)
{  
  //清除缓冲
  FMC_PFB0CR |= FMC_PFB0CR_S_INV_MASK;//p724 S_INV:invalidate Prefetch Speculation Buffer
                                              //S_INV bit determines if the FMC's prefetch speculation buffer is to be invalidated(cleared).
                                              //When S_INV bit is written ,the prefetch speculation buffer is immediately cleared. Whenever
                                              // flash memory is modified, the prefetch speculation buffer should be cleared. S_INV bit 
                                              // always reads as zero.
  
}

//==============================================================================
//函数名称：hw_flash_cmd_launch
//函数返回：0-成功 1-失败
//参数说明：无
//功能概要：启动Flash命令
//==============================================================================
static uint32 hw_flash_cmd_launch(void)
{
  //清除访问错误标志位和非法访问标志位
  FTFL_FSTAT = ACCERR | FPVIOL;           //lyx: ACCERR:flash访问错误标志位
                                          //lyx: FPVIOL:flash非法访问保护标志位
  //启动命令
  FTFL_FSTAT = CCIF;                      //lyx: CCIF:指令完成中断标志位

  //等待命令结束
  while(!(FTFL_FSTAT & CCIF));

  //检查错误标志
  if(FTFL_FSTAT & (ACCERR | FPVIOL | MGSTAT0))
    return 1 ; //执行命令出错
  
  return 0; //执行命令成功
}
//==============================================================================

