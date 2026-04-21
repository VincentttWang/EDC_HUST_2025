#include "app_usart.h"

#define Package_size 100


u8 rx_buff[Package_size];
u8 new_package[Package_size];
u8 g_new_package_flag = 0;//接收到新的一包数据标志 Flag indicating new package received

u8 IR_Data_number[IR_Num];//数字值 // Digital values
u16 IR_Data_Anglo[IR_Num];//模拟值 // Analog values
u8 g_Amode_Data = 0;//模拟型标志 // Analog mode flag
u8 g_Dmode_Data = 0;//数字型标志 // Digital mode flag

//寻找字符型数据   Find string pattern
int StringFind(const char *pSrc, const char *pDst)  
{  
    int i, j;  
    for (i=0; pSrc[i]!='\0'; i++)  
    {  
        if(pSrc[i]!=pDst[0])  
            continue;         
        j = 0;  
        while(pDst[j]!='\0' && pSrc[i+j]!='\0')  
        {  
            j++;  
            if(pDst[j]!=pSrc[i+j])  
            break;  
        }  
        if(pDst[j]=='\0')  
            return i;  
    }  
    return -1;  
}  

void Deal_IR_Usart(u8 rxtemp)
{
	static u8 g_start = 0;
  static u8 step = 0;
  if(rxtemp == '$')
  {  
    g_start = 1;//开始接收标志    Start receiving flag
    rx_buff[step] = rxtemp;
    step++;
  }
  else
  {
    if(g_start == 0)
    {
      return;
    }
    else
    {
      rx_buff[step] = rxtemp;
      step ++;
      if(rxtemp == '#')//结束 End
      {
        g_start = 0;
        step = 0;
        memcpy(new_package,rx_buff,Package_size);//赋值   Copy data
        g_new_package_flag = 1;
				Deal_Usart_Data();
        memset(rx_buff,0,Package_size);//清空数据   Clear buffer

      }
      
      if(step >= Package_size)//数据异常    Data exception
      {
        g_start = 0;
        step = 0;
        memset(rx_buff,0,Package_size);//清空数据   Clear buffer
      }
    }
  }
}


void send_control_data(u8 adjust,u8 aData,u8 dData)
{
	u8 send_buf[8] = "$0,0,0#";
	if(adjust == 1)//校准命令   Calibration command
	{
		send_buf[1] = '1';
	}
	else
	{
		send_buf[1] = '0';
	}
	if(aData == 1)//模拟值数据   Analog data
	{
		send_buf[3] = '1';
		g_Amode_Data = 1;
	}
	else
	{
		send_buf[3] = '0';
		g_Amode_Data = 0;
	}
	if(dData == 1)//数字值数据   Digital data
	{
		send_buf[5] = '1';
		g_Dmode_Data = 1;
	}
	else
	{
		send_buf[5] = '0';
		g_Dmode_Data = 0;
	}
	
	USART2_Send_ArrayU8(send_buf,strlen((char*)send_buf));
	
}

//数据例子 Data example：$D,x1:0,x2:0,x3:0,x4:0,x5:0,x6:0,x7:0,x8:0#
void Deal_Usart_Data(void) //处理数字型数据    Process digital data
{
	if(new_package[1]!='D')
	{
		return; //此包数据不是数字型数据   Not digital data
	}
	for(u8 i = 0;i<IR_Num;i++)
	{
		IR_Data_number[i] = (new_package[6+i*5]-'0');//把字符转成数字  Convert char to number
	}
	
	memset(new_package,0,Package_size);//清除旧数据  Clear old data
  
}




//---------------------------模拟值处理相关的   Analog value processing related---------------------------


//传入参数：保留的字符串(指针数组)  原始字符串  分隔符号    Incoming parameters: reserved string (array of pointers) original string separator sign
void splitString(char* mystrArray[],char *str, const char *delimiter) 
{
    char *token = strtok(str, delimiter); //这是第一次分割,不会保留第一个字符值  This is the first split, the first character value is not retained.
    int i =0;
    while (token != NULL) 
    {
        token = strtok(NULL, delimiter);
        mystrArray[i] = token;
        i++;
    }
}

void Deal_Usart_AData(void) //处理模拟值数据   Processing analog value data
{
	//$A,x1:1000,x2:3450,x3:40,x4:450,x5:110,x6:4096,x7:780,x8:80#
	
	char* strArray[10];//指针数组 长度根据分割号定义  char 1字节   char* 4字节   Array of pointers Length defined by split number char 1 byte char* 4 bytes
	char* strArraytemp[2];
	char str_temp[Package_size] = {'\0'};
	char mystr_temp[8][10] = {'\0'}; //临时备份 Interim Backup
	
	
	if(new_package[1]!='A')
	{
		return; //此包数据不是模拟值数据   This packet data is not analog value data
	}
	strncpy(str_temp,(char*)new_package,strlen((char*)new_package)-1);
	
	splitString(strArray,str_temp, ", ");//以逗号切割    comma-delimited
	
	 //遍历数组 Iterate over the array
    for (int i = 0; i < IR_Num; i++)
    {
    	  strcpy(mystr_temp[i],strArray[i]);
    	  splitString(strArraytemp,mystr_temp[i], ": ");
				IR_Data_Anglo[i] = atoi(strArraytemp[0]);
    }
		memset(new_package,0,Package_size);//清除旧数据  Clearing old data
		
		memset(strArray,0,sizeof(strArray)); //数组指针清0   Array pointer clear 0
		memset(strArraytemp,0,sizeof(strArraytemp));//数组指针清0    Array pointer clear 0
  
}



