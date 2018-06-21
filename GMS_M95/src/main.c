
/*
 *Turn on GSM_M95
 * Vbat mức 1 trong khi pin GPS_PWRKEY cấp 1 clock( 0 1 0) // nên 2 lần để chắc chắn turn on module m95
 *UART2: PA2_TX, PA3_RX
 *
 *
 */
#include <stddef.h>
#include "stm32f10x.h"
#include <string.h>
char Signal_quality[50] ={};
char Server_Provider_Name[50] = {};
char Phone_number[50] = {};
char Data_packet[50]= {};
uint8_t i = 0;
uint8_t check_line = 0;
GPIO_InitTypeDef GPIO;
USART_InitTypeDef USART;
NVIC_InitTypeDef NVIC_InitStructure;
void Check_phone_number(void);
void Check_Server_Provider_Name(void);
void Check_Signal_Quality(void);
void TurnOnPower(void);
void send_byte(char b);
void Send_string(char *a);
void Send_string_USART2(char *a);
void send_byte_USART2(char b);
void USART2_IRQHandler(void);
void clear_buffer(char *buff);
int main(void)
{
	 //setup GPIOA turn on GMS
	 //Power key - PA00
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
//	GPIO.GPIO_Pin = GPIO_Pin_0;
//	GPIO.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO.GPIO_Mode=GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOA, &GPIO);
//	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
//	GPIO_SetBits(GPIOA,GPIO_Pin_0);
//	GPIO_SetBits(GPIOA,GPIO_Pin_0);
//	GPIO_SetBits(GPIOA,GPIO_Pin_0);
//	GPIO_ResetBits(GPIOA,GPIO_Pin_0);
//	GPIO_SetBits(GPIOA,GPIO_Pin_0);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO.GPIO_Pin = GPIO_Pin_8;
	GPIO.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO);
	GPIO_SetBits(GPIOB,GPIO_Pin_8 );

	//Setup UART1  UART_debug_COM
	RCC_APB2PeriphClockCmd(RCC_APB2ENR_USART1EN,ENABLE);
	USART.USART_BaudRate = 115200;
	USART.USART_WordLength = USART_WordLength_8b;
	USART.USART_StopBits = USART_StopBits_1;
	USART.USART_Parity = USART_Parity_No;
	USART.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //the Receive or Transmit mode is enabled or disabled
	USART_Init(USART1,&USART);
	USART_Cmd(USART1,ENABLE);
	//	  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //Enable the USART1 Receive interrupt: this interrupt is generated when the USART1 receive data register is not empty
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO.GPIO_Pin = GPIO_Pin_9;
	GPIO.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO);
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO.GPIO_Pin = GPIO_Pin_10;
	GPIO.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO);


															// Setup USART2
	RCC_APB1PeriphClockCmd(RCC_APB1ENR_USART2EN,ENABLE);
	USART.USART_BaudRate = 115200;
	USART.USART_WordLength = USART_WordLength_8b;
	USART.USART_StopBits = USART_StopBits_1;
	USART.USART_Parity = USART_Parity_No;
	USART.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //the Receive or Transmit mode is enabled or disabled
	USART_Init(USART2,&USART);
	USART_Cmd(USART2,ENABLE);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //Enable the USART1 Receive interrupt: this interrupt is generated when the USART1 receive data register is not empty
	/* Configure USART2 Tx (PA2) as alternate function push-pull */
	GPIO.GPIO_Pin = GPIO_Pin_2;
	GPIO.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO);

	/* Configure USART2 Rx (PA3) as input floating */
	GPIO.GPIO_Pin = GPIO_Pin_3;
	GPIO.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO);
	// NVIC _ interrupt Rx_USART2
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	Check_Signal_Quality();
	Check_Server_Provider_Name();
	Check_phone_number();
  while (1)
  {

	  GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	  for(int i = 0; i < 5000000; i ++)
	  {

	  }
	  GPIO_SetBits(GPIOB,GPIO_Pin_8);
	  for(int i = 0; i < 5000000; i++)
	  {

	  }
  }
}


void send_byte(char b)
{
  /* Send one byte */
  USART_SendData(USART1, b);

  /* Loop until USART2 DR register is empty */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
  USART_ClearFlag(USART1,USART_FLAG_TXE);
}
void Send_string(char *a)
{
	 while(*a)
	 {
		 send_byte(*a);
		 a++;
	 }
}

void Send_string_USART2(char *a)
{
	 while(*a)
	 {
		 send_byte_USART2(*a);
		 a++;
	 }
}
void send_byte_USART2(char b)
{
  /* Send one byte */
  USART_SendData(USART2, b);

  /* Loop until USART2 DR register is empty */
  while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
  USART_ClearFlag(USART2,USART_FLAG_TXE);
}
void USART2_IRQHandler(void)
{
	 char c;
	 if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	 {
		 USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		 c = USART_ReceiveData(USART2);
		 if(check_line == 0)
		 {
			 if(c == '\n')
			 {
				 check_line = 1;
			 }
		 }
		 else if(check_line == 1)
		 {
			 if(c == '\r')
			 {
				 check_line = 0;
				 i = 0;
				 if(strncmp(Data_packet,"+CSQ",4) == 0)
				 {
					 char *temp = strstr(Data_packet," ");
					 char *temp1 = strstr(temp,",");
					 strncpy(Signal_quality,temp,strlen(temp) - strlen(temp1));
					 Send_string("\r\nSignal quality:");
					 Send_string(Signal_quality);
					 clear_buffer(Data_packet);
				 }
				 else if(strncmp(Data_packet,"+QSPN",5) == 0)
				 {
					 char * temp = strstr(Data_packet," ");
					 char *temp1 = strstr(temp,",");
					 strncpy(Server_Provider_Name,temp,strlen(temp) - strlen(temp1));
					 Send_string("\r\nProvider_Name: ");
					 Send_string(Server_Provider_Name);
					 clear_buffer(Data_packet);
				 }
				 else if(strncmp(Data_packet,"+CNUM",5) == 0)
				 {
					 char * temp = strstr(Data_packet,",");temp++;
					 char *temp1 = strstr(temp,",");
					 strncpy(Phone_number,temp,strlen(temp) - strlen(temp1));
					 Send_string("\r\nPhone number: ");
					 Send_string(Phone_number);
					 clear_buffer(Data_packet);
				 }
			 }
			 else
			 {
				 Data_packet[i] = c;
				 i++;

			 }
		 }

	 }
}
void clear_buffer(char *buff)
{
	while(*buff != 0x00)
	{
		*buff = 0x00;
		buff++;
	}
}
void Check_phone_number(void)
{
	for(int i = 0; i < 5000000; i ++)
	{

	}
	Send_string_USART2("AT+CNUM\r\n");

}
void Check_Server_Provider_Name(void)
{
	for(int i = 0; i < 5000000; i ++)
	{

	}
	Send_string_USART2("AT+QSPN?\r\n");

}
void Check_Signal_Quality(void)
{
	for(int i = 0; i < 5000000; i ++)
	{

	}
	Send_string_USART2("AT+CSQ\r\n");
	for(int i = 0; i < 5000000; i ++)
	{

	}
}
void TurnOnPower(void)
{
//	Send_string_USART2("AT+IPR=0");
//	for(int i = 0; i < 10000000; i ++)
//	{
//
//	}
	Send_string_USART2("AT\r\n");
	for(int i = 0; i < 5000000; i ++)
	{

	}
	Send_string_USART2("AT+IPR=115200\r\n");
	for(int i = 0; i < 5000000; i ++)
	{

	}
	Send_string_USART2("AT&W\r\n");
	for(int i = 0; i < 5000000; i ++)
	{

	}
}
