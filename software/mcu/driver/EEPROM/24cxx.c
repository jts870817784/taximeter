/*

                        FLASH�ڴ�ӳ��

��׼��ַ��(FLASH_BASE_ADDRESS) 0x800FC00
��־������Ĭ��ֵ
--------------------------------------------------------------------------------------------------
|  ƫ�Ƶ�ַ  | 00 | 01 |  02  |  03  | 04 | 05 | 06 | 07 | 08 | 09 | 0A | 0B | 0C | 0D | 0E | 0F | 
|------------+-----------------------+-------------------+-------------------+-------------------|
|   ������   |  ��־   |  in_max_out |        in_kp      |       in_kd       |       in_ki       |
|------------+-----------------------+-------------------+-------------------+-------------------|
|  ƫ�Ƶ�ַ  |  10  |  11  | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 1A | 1B | 1C | 1D | 1E | 1F | 
|------------+-----------------------+-------------------+-------------------+-------------------|
|   ������   | out_max_out |z_max_out|       out_kp      |      out_kd       |      out_ki       |
|------------+-----------------------+-------------------+-------------------+-------------------|
|  ƫ�Ƶ�ַ  | 20 | 21 |  22  |  23  | 24 | 25 | 26 | 27 | 28 | 29 | 2A | 2B | 2C | 2D | 2E | 2F | 
|------------+-----------------------+-------------------+-------------------+-------------------|
|   ������   |in_max_ki|  out_max_ki |       z_kd        |       z_ki        |        z_kp       |
|------------+-----------------------+-------------------+-------------------+-------------------|
|  ƫ�Ƶ�ַ  | 30 | 31 |  32  |  33  | 34 | 35 | 36 | 37 | 38 | 39 | 3A | 3B | 3C | 3D | 3E | 3F | 
|------------+-----------------------+-------------------+-------------------+-------------------|
|   ������   | z_max_ki|   reserve   |     in_apart      |     out_apart     |      z_apart      |
--------------------------------------------------------------------------------------------------

*/

#include "flash.h"
#include "stdio.h"


//��ȡָ����ַ�İ���(16λ����)
uint16_t FLASH_ReadHalfWord(uint32_t address)
{
	return *(__IO uint16_t*)address; 
}



//��ָ����ַ��ʼ��ȡ�������
void FLASH_ReadMoreData(uint32_t startAddress,uint16_t *readData,uint16_t countToRead)
{
	uint16_t dataIndex;
	for(dataIndex=0;dataIndex<countToRead;dataIndex++)
	{
		readData[dataIndex]=FLASH_ReadHalfWord(startAddress+dataIndex*2);
	}
}


//��ȡָ����ַ��ȫ��(32λ����)
uint32_t FLASH_ReadWord(uint32_t address)
{
	uint32_t temp1,temp2;
	temp1=*(__IO uint16_t*)address; 
	temp2=*(__IO uint16_t*)(address+2); 
	return (temp2<<16)+temp1;
}


//��ָ����ַ��ʼд��������
void FLASH_WriteMoreData(uint32_t startAddress,uint16_t *writeData,uint16_t countToWrite)
{
	if((startAddress<FLASH_BASE)||((startAddress+countToWrite*2)>=(FLASH_BASE+1024*FLASH_SIZE)))
	{
		return;//�Ƿ���ַ
	}
	FLASH_Unlock();         //����д����
	uint32_t offsetAddress=startAddress-FLASH_BASE;               //����ȥ��0X08000000���ʵ��ƫ�Ƶ�ַ
	uint32_t sectorPosition=offsetAddress/SECTOR_SIZE;            //����������ַ������STM32F103VET6Ϊ0~255

	uint32_t sectorStartAddress=sectorPosition*SECTOR_SIZE+FLASH_BASE;    //��Ӧ�������׵�ַ

	FLASH_ErasePage(sectorStartAddress);//�����������

	uint16_t dataIndex;
	for(dataIndex=0;dataIndex<countToWrite;dataIndex++)
	{
		FLASH_ProgramHalfWord(startAddress+dataIndex*2,writeData[dataIndex]);
	}

	FLASH_Lock();//����д����
}

uint8_t getFlashInitStatus  ()
{
    uint16_t flag = 0;
    
    FLASH_ReadMoreData(FLASH_START - 2, &flag, 1);
    if (flag == FLASH_VERIFY) {
        return 0;
    } else {
        flag = FLASH_VERIFY;
        FLASH_WriteMoreData(FLASH_START - 2, &flag, 1);
        memset((void *)g_orderData, 0, sizeof(g_orderData));
        FLASH_WriteMoreData(FLASH_START, g_orderData, FLASH_MAX_BYTE >> 1);
        return 1;
    }
}

extern void *memset(void *s, int ch, size_t n);
void bspflashInit()
{
    if (!getFlashInitStatus()) {
        FLASH_ReadMoreData(FLASH_START, (uint16_t *)g_orderData, FLASH_MAX_BYTE >> 1);
        return;
    }

    memset((void *)g_orderData, 0, sizeof(g_orderData));
    FLASH_WriteMoreData(FLASH_START, g_orderData, FLASH_MAX_BYTE >> 1);
}

