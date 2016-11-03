#include "main.h"
#include "math.h"

//====================================================================//
// 语法格式: static double Str_To_Double(char *buf)
// 实现功能： 把一个字符串转化成浮点数
// 参    数：字符串
// 返 回 值：转化后双精度值
//====================================================================//
double Str_To_Double(char *buf)
{
	double rev = 0;
	double dat;
	int integer = 1;
	char *str = buf;
	int i;
	
	while(*str != '\0')
	{
		switch(*str)
		{
			case '0':
				dat = 0;
				break;
			case '1':
				dat = 1;
				break;
			case '2':
				dat = 2;
				break;		
			case '3':
				dat = 3;
				break;
			case '4':
				dat = 4;
				break;
			case '5':
				dat = 5;
				break;
			case '6':
				dat = 6;
				break;
			case '7':
				dat = 7;
				break;
			case '8':
				dat = 8;
				break;
			case '9':
				dat = 9;
				break;
			case '.':
				dat = '.';
				break;
		}
		if(dat == '.')
		{
			integer = 0;
			i = 1;
			str++;
			continue;
		}
		if( integer == 1 )
		{
			rev = rev * 10 + dat;
		}
		else
		{
			rev = rev + dat / (10 * i);
			i = i * 10 ;
		}
		str++;
	}
	
	return rev;
}
												
//====================================================================//
// 语法格式: static double Get_Double_Number(char *s)
// 实现功能：把给定字符串第一个逗号之前的字符转化成双精度型[有修改为返回值为long型整数]
// 参    数：字符串
// 返 回 值：转化后双精度值
//====================================================================//
double Get_Double_Number(char *s)
{
	// --modified by Wangsi, 20151107
	char buf[15];
	u8 i,j;
	double rev;
	long a,temp;
	
	i = GetComma(1, s);	
	// 如果s中没有找到逗号，则数据错误，返回负数
	if(i<=0){ 
		printf("Get_Double_Number:: s中没有找到逗号\n");
		return -9999.0;
	}
	
	// 将数据段拷贝到buf中
	i = i - 1;
	strncpy(buf, s, i);
	if(i<15){
		buf[i] = 0;
	}else{
		printf("Get_Double_Number:: buf[i]访问越界\n");
		return -9999.0;
	}
	
	// 如果数据段含有非法字符（不是0~9或者'.'），则认为数据错误，返回负数
	j = 0;
	while(buf[j]!='\0'){		
		if( (buf[j]!='.') && (buf[j]>'9'||buf[j]<'0') ){			
			printf("Get_Double_Number:: buf中存在非法字符\n");
			printf("Get_Double_Number:: buf: %s\n", buf);
			return -9999.0;
		}
		j++;
	}
	rev = Str_To_Double(buf);
	
	temp = rev/100;
	a = temp*600000 + (rev - temp*100)*10000;
	
	return a;	
}

double Get_Double_Number_sog(char *s)  //for sog
{
	// --modified by Wangsi, 20151107
	char buf[10];
	u8 i;
	double rev;
	unsigned int a;
	
	i = GetComma(1, s);
	// 如果s中没有找到逗号，则数据错误，返回负数
	if(i<=0){ 
		return -9999.0;
	}
	
	i = i - 1;
	strncpy(buf, s, i);
	buf[i] = 0;
	rev = Str_To_Double(buf);
	
	a = rev*10/1;
	
	return a;	
}

double Get_Double_Number_direction(char *s)  //for direction
{
	char buf[10];
	u8 i;
	double rev;
	unsigned int a;
	
	i = GetComma(1, s);
	i = i - 1;
	strncpy(buf, s, i);
	buf[i] = 0;
	rev = Str_To_Double(buf);
	
	a = rev*10/1; //a = rev/1 *10;
	
	return a;	
}

double Get_Double_Number_time(char *s)  //for direction
{
	char buf[10];
	u8 i;
	double rev;
	unsigned long a;
	
	i = GetComma(1, s);
	i = i - 1;
	strncpy(buf, s, i);
	buf[i] = 0;
	rev = Str_To_Double(buf);
	
	a = rev/1;
	
	return a;	
}
//====================================================================//
// 语法格式：static uchar GetComma(uchar num,char *str)
// 实现功能：计算字符串中各个逗号的位置
// 参    数：查找的逗号是第几个的个数，需要查找的字符串
// 返 回 值：0
//====================================================================//
u8 GetComma(u8 num, char *str)
{
	u8 i,j = 0;
	int len = strlen(str);

	for(i = 0; i < len; i++)
	{
		if(str[i] == ',')
			j++;
		if(j == num)
			return i + 1;	
	}

	return 0;	 
}

