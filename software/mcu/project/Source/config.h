/*
*******************************************************************************
*                《ARM嵌入式编程与实战应用--STM32F1系列(C语言版)》
*                       配套 KST-32 单片机开发板 示例源代码
*
*          (c) 版权所有 2018 金沙滩工作室/清华大学出版社 保留所有权利
*              获取更多资料请访问：
*				  官方网站：http://www.qdkingst.com
*				  淘宝网店：https://kstmcu.taobao.com
*				  技术QQ  ：415942827 			
*			   关注微信公众号"金沙滩工作室",在线学习51，STM32单片机教程																
* 
*  文件名：config.h
*  描  述：
*  版本号：v1.0.0
*  备  注：
*******************************************************************************
*/
#ifndef _CONFIG_H
#define _CONFIG_H
#include "stm32f10x.h"

/**************************************位带操作*********************************************/
//位带宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //GPIOA输出数据寄存器地址0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //GPIOB输出数据寄存器地址0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //GPIOC输出数据寄存器地址0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //GPIOD输出数据寄存器地址0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //GPIOE输出数据寄存器地址0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //GPIOF输出数据寄存器地址0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //GPIOG输出数据寄存器地址0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //GPIOA输入数据寄存器地址0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //GPIOB输入数据寄存器地址0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //GPIOC输入数据寄存器地址0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //GPIOD输入数据寄存器地址0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //GPIOE输入数据寄存器地址0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //GPIOF输入数据寄存器地址0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //GPIOG输入数据寄存器地址0x40011E08 
 
//单个IO口操作
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //PAx输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //PAx输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //PBx输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //PBx输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //PCx输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //PCx输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //PDx输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //PDx输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //PEx输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //PEx输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //PFx输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //PFx输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //PGx输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //PGx输入
/********************************************************************************/

void delay_us(uint16_t us);
void delay_ms(uint16_t nms);







#endif


