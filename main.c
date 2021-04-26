#include <reg52.h>
#include<stdlib.h>
#define uchar unsigned char
#define DIV 0x0d
#define EQU 0x0e
#define CLR 0x0f
xdata unsigned char OUTBIT _at_	0x9002;	
xdata unsigned char OUTSEG	_at_	0x9004;   // 段控制口
xdata unsigned char IN	_at_	0x9001;	
unsigned char	LEDBuf[6];	
sbit led0=P1^0;
sbit led1=P1^1;
sbit led2=P1^2;
sbit led3=P1^3;
sbit led4=P1^4;
sbit led5=P1^5;
sbit led6=P1^6;
sbit led7=P1^7;	
sbit beep=P2^0;
sbit clock1=P3^2;
unsigned char sum=0,temp;
unsigned int close=10;
unsigned int keys[8]={0};
unsigned int flags[] = {0,0,0,0,0,0,0,0};
unsigned char	LEDMAP[] = {		//8段显示码	
	0x3f,	0x06,	0x5b, 0x4f,	0x66,	0x6d, 0x7d,	0x07,
	0x7f,	0x6f,	0x77, 0x7c,	0x39,	0x5e, 0x79,	0x71,
	0x76,   0x38,   0x37, 0x3e, 0x73,   0x5c, 0x40
};
void init_tx();
void Timer0Init();  //定时器0初始化	
void light_sound(unsigned int i);
void Delay(unsigned char CNT)  //延时1ms
{

	unsigned char i;
	
	while (CNT-- !=0)
	
	for (i=100; i !=0; i--);
			
}					
void ket(unsigned int x)	 //设置随机密码
{	
    unsigned int sum=0,i;
    srand(temp);
	for(i=0;i<4;i++){

	sum=sum*10+rand()%10;
	} 
	keys[x]=sum;
}
void DisplayLED()			 //数码管显示函数
{	
	unsigned char i;	
	unsigned char Pos;	
	unsigned char LED;	
	Pos = 0x20;	
	for (i = 0; i < 6; i++) {
			OUTBIT = 0;	// trun off all LED
			LED	= LEDBuf[i];	
			OUTSEG = LED;	
			OUTBIT = Pos;	// trun on one LED
			Delay(5);		
			Pos	>>= 1;		//显示下一位
	}			
}			
unsigned char	KeyTable[] = {					 //键盘段码
		0x16,	0x15,	0x14, 0xff,
		0x13,	0x12,	0x11, 0x10,
		0x0d,	0x0c,	0x0b, 0x0a,
		0x0e,	0x03,	0x06, 0x09,
		0x0f,	0x02,	0x05, 0x08,
		0x00,	0x01,	0x04, 0x07
};			
unsigned char TestKey()		   
{
	
	OUTBIT = 0;  //输出线置零
	
	return (~IN & 0x0f); //读入键状态

}
unsigned char GetKey()				   //获取键盘值
{
	unsigned char Pos;	
	unsigned char i;	
	unsigned char k;	
	i = 6;	
	Pos = 0x20;	//找出键所在列
	do {	
		OUTBIT = ~ Pos;	
		Pos >>= 1;	
		k = ~IN & 0x0f;	
    } while ((--i != 0) && (k == 0));	
	if (k != 0) {	//键值在列*4+行
		i *= 4;	
		if (k & 2)	
		i += 1;	
		else if (k & 4)	
		i += 2;	
		else if (k & 8)	
		i += 3;	
		OUTBIT = 0;	
		do Delay(10); while (TestKey());//等键释放	
		return(KeyTable[i]);	//读取键码
     } else 
	 return(0xff);	
}
/* =====================================================================================*/
void DisplayResult(unsigned long int Result,signed int local)	  //将键盘值输送到数码管
{
	unsigned char i;			
			LEDBuf[0] = 0;			
			LEDBuf[1] = 0;			
			LEDBuf[2] = 0;						
			LEDBuf[3]	= 0;				
			LEDBuf[4]	= 0;
			LEDBuf[5]	= 0;							
			i = 5;				
			for(;local>0;local--)
			 {
			LEDBuf[i--] = LEDMAP[Result % 10]; Result = Result / 10;			
			}	  
}
void main()
{
	unsigned long int  Result;		//输入、显示的数值
	unsigned char Key=0;			//键盘值
	unsigned int local=0 ;			//数值长度
    unsigned int i = 0;
	beep=0;						 //蜂鸣器置为0
	Timer0Init();  //定时器0初始化	 	
	init_tx();		  //外部中断初始化
	Result = 0;
	               led0=flags[0];	  //led灯初始化
				   led1=flags[1];
				   led2=flags[2];
				   led3=flags[3];
				   led4=flags[4];
				   led5=flags[5];
				   led6=flags[6];
				   led7=flags[7];
	while (1) {
		while (!TestKey()) DisplayLED();	
		// 读取键码		
		Key = GetKey();		
		if ( (Key >= 0) && (Key <= 9) ) { // 是数字键		
		// 输入数值
		  Result = Result*10 + Key;
        local++;
		DisplayResult(Result,local);
		} else if (Key == CLR) {	// 是清零键	
			Result = 0;	 local=0;
			DisplayResult(Result,local);	
		} else if (Key==EQU)  { // 是取键		   
			 for(i=0;i<8;i++)
			 {
			  if(Result==keys[i]&&local==4&&flags[i]==1)
			  {
			      LEDBuf[0] = 0x5c;
				   LEDBuf[1] = 0x73;
				   LEDBuf[2] = 0x79;
				   LEDBuf[3] = 0x37;
				   LEDBuf[4] = 0x00;
				   LEDBuf[5] = 0x00;
				   flags[i]=0;
				   keys[i]=0;
				   light_sound(i);
				   break;
			  } 
			 }  
			  if(i==8)
			  {
				   LEDBuf[0] = 0x79;
				   LEDBuf[1] = 0x50;   //大点儿的r为;0x31
				   LEDBuf[2] = 0x50;
				   LEDBuf[3] = 0x00;
				   LEDBuf[4] = 0x00;
				   LEDBuf[5] = 0x00; 
			  }
			  Result = 0;	 local=0;		 
			}
			if (Key==DIV)		   //存键
			{
				for(i=0;i<8;i++)
				{
				     if(flags[i]==0)
				  	 {
					 ket(i);
					 
					 Result=keys[i];
					 DisplayResult(Result,4);
					 close=i;
					 TR0=0;
					 
					  DisplayLED();
					  TR0=1;
					 break;
					 }
				}
				if(i==8)
				{
				   LEDBuf[0] = 0x71;
				   LEDBuf[1] = 0x3e;
				   LEDBuf[2] = 0x38;
				   LEDBuf[3] = 0x38;
				   LEDBuf[4] = 0x00;
				   LEDBuf[5] = 0x00;
				}		
					Result = 0;
					local=0;
			}
			       led0=flags[0];
				   led1=flags[1];
				   led2=flags[2];
				   led3=flags[3];
				   led4=flags[4];
				   led5=flags[5];
				   led6=flags[6];
				   led7=flags[7];				   
				DisplayLED();				
		}	
}
void Timer0Init()
{
	TMOD|=0X01;//选择为定时器0模式，工作方式1，仅用TR0打开启动。
	TH0=0XFC;	//给定时器赋初值，定时1ms
	TL0=0X18;	
	ET0=1;//打开定时器0中断允许
	EA=1;//打开总中断
	TR0=1;
	ET0=1;			
}

/*****************************************
           定时器0中断函数
*****************************************/
void Timer0() interrupt 1
{
	int i;
	TH0=0XFC;	//给定时器赋初值，定时1ms
	TL0=0X18;
	i++;
	if(i==6000&&close>=0&&close<8)
	{
		i=0;
		LEDBuf[0] = 0x00;
				   LEDBuf[1] = 0x00;
				   LEDBuf[2] = 0x00;
				   LEDBuf[3] = 0x00;
				   LEDBuf[4] = 0x00;
				   LEDBuf[5] = 0x00;
				   LEDBuf[6] = 0x00;
				   LEDBuf[0] = 0x00;				   
		DisplayLED();close=10;
	}
	temp=i;	
}
void init_tx()
{
    EA=1;
	EX0=1;
	IT0=1;
	PX0=1;
}
void tx0()  interrupt 0
{ 
    flags[close]=1;
    led0=flags[0];
	led1=flags[1];
    led2=flags[2];
	led3=flags[3];
	led4=flags[4];
    led5=flags[5];
	led6=flags[6];
	led7=flags[7];
}
void light_sound(unsigned int i)			//蜂鸣器及led灯提示函数
{
    switch(i){
	case 0:
    for(sum=0;sum<2;sum++){               
		  led0=0;
         Delay(500);   
			  led0=!led0;               
          Delay(500);
		    } 
		led0=1;
		for(sum=0;sum<50;sum++){
                  //蜂鸣器响  
		  beep=~beep;
         Delay(5);   
		    } 
			break;
	case 1:
    for(sum=0;sum<2;sum++){                
		  led1=0;
         Delay(500);   
			  led1=!led1;
               
          Delay(500);
		    } 
		led1=1;
		for(sum=0;sum<50;sum++){
                  //蜂鸣器响  
		  beep=~beep;
         Delay(5);               
		    } 
			break;
case 2:
    for(sum=0;sum<2;sum++){                 
		  led2=0;
         Delay(500);   
			  led2=!led2;
               
          Delay(500);
		    } 
		led2=1;
		for(sum=0;sum<50;sum++){
                  //蜂鸣器响  
		  beep=~beep;
         Delay(5);   			  
		    } 
			break;
case 3:
    for(sum=0;sum<2;sum++){                
		  led3=0;
         Delay(500);   
			  led3=!led3;
               
          Delay(500);
		    } 
		led3=1;
		for(sum=0;sum<50;sum++){
                  //蜂鸣器响  
		  beep=~beep;        
          Delay(5);
		    } 
			break;
case 4:
    for(sum=0;sum<2;sum++){                 
		  led0=0;
         Delay(500);   
			  led4=!led4;
               
          Delay(500);
		    } 
		led4=1;
		for(sum=0;sum<50;sum++){
                  //蜂鸣器响  
		  beep=~beep;         
          Delay(5);
		    } 
			break;
case 5:
    for(sum=0;sum<2;sum++){               
		  led5=0;
         Delay(500);   
			  led5=!led5;
               
          Delay(500);
		    } 
		led5=1;
		for(sum=0;sum<50;sum++){
                  //蜂鸣器响  
		  beep=~beep;
         Delay(5);   			  
		    } 
			break;
case 6:
    for(sum=0;sum<2;sum++){                  
		  led6=0;
         Delay(500);   
			  led6=!led6;
               
          Delay(500);
		    } 
		led6=1;
		for(sum=0;sum<50;sum++){
                  //蜂鸣器响  
		  beep=~beep;
         Delay(5);   			  
		    } 
			break;
case 7:
    for(sum=0;sum<2;sum++){                 
		  led7=0;
         Delay(500);   
			  led7=!led7;
               
          Delay(500);
		    } 
		led7=1;
		for(sum=0;sum<50;sum++){
                  //蜂鸣器响  
		  beep=~beep;
         Delay(5);   
		    } 
			break;
   	  }
}
