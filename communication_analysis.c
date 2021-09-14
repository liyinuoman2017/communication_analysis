/**
*********************************************************************************************************
*                                                touchpad
*                                      (c) Copyright 2011-2031
*                                           All Rights Reserved
*
* @File    : 
* @By      : liwei
* @Version : V0.01
*
*********************************************************************************************************
**/

/*
*********************************************************************************************************
Includes 
*********************************************************************************************************
*/
#include "include_all.h"
/*
*********************************************************************************************************
Define
*********************************************************************************************************
*/

#define CACHE_BUFF_LONGTH 		10
#define CACHE_BUFF_NUM			50
#define IDLE_NUM				5
/*
*********************************************************************************************************
Typedef
*********************************************************************************************************
*/
typedef struct commnuication_frame
{
	u8	data_pointer;	   									//ͨѶ����ָ��
	u8	data_cache[CACHE_BUFF_LONGTH][CACHE_BUFF_NUM];     	//ͨѶ����
	u16	data_cache_long[CACHE_BUFF_LONGTH]; 				//ͨѶ���泤��

									
	u16 refresh_num;
	u16 current_num;
	u8	receive_buff[CACHE_BUFF_NUM];
	u8 	analysis_buff[CACHE_BUFF_NUM];
	u16 analysis_long;
	u16 null;
	u16 null2;

}COM_FRAME;

/*
*********************************************************************************************************
Variables
*********************************************************************************************************
*/

COM_FRAME   rs485_commnuication_frame;
/*
*********************************************************************************************************
Function 
*********************************************************************************************************
*/

/******************************************
�������ƣ�
��    �ܣ�ˢ�´�����ʱ,��Ž������� ,�ڴ����жϺ�����ʹ��
void UART4_RX_ISR_PC(void)    interrupt 11
{
	res=SBUF2_RX;
	Common_Receive_Refresh(res,&rs485_commnuication_frame); 
}
��    ����
����ֵ  ��
*******************************************/
void Common_Receive_Refresh(u8 receive_data, COM_FRAME *com_pointer)
{
	//ˢ�¼�����
	com_pointer->refresh_num=0;
	//��������
	if(com_pointer->current_num<(CACHE_BUFF_NUM-1))
		com_pointer->receive_buff[com_pointer->current_num++]=receive_data;		
}
/******************************************
�������ƣ�
��    �ܣ�����5MS���ڿ����жϣ�������������1ms��ʱ����
void T0_ISR_PC(void)    interrupt 1
{
	//���ڿ����ж�
	Common_Idle_Judge(&rs485_commnuication_frame);
}
��    ����
����ֵ  ��
*******************************************/
void Common_Idle_Judge(COM_FRAME *com_pointer)
{
	com_pointer->refresh_num++;
	if(com_pointer->refresh_num>=IDLE_NUM)
	{
		com_pointer->refresh_num=IDLE_NUM;
		if(com_pointer->current_num>0)
		{	
			//��������
			Common_Cache_Data(com_pointer->current_num,com_pointer);
			com_pointer->current_num=0;
		}		
	}
}
/******************************************
�������ƣ�
��    �ܣ����洮�����ݣ����ڿ��к󻺴�����
��    ����
����ֵ  ��
*******************************************/
void Common_Cache_Data(u16 buff_long,COM_FRAME *com_pointer)
{
	//�жϻ�������Ƿ���
	if(com_pointer->data_pointer<(CACHE_BUFF_LONGTH-1))
	{
		//��������
		memcpy(com_pointer->data_cache[com_pointer->data_pointer],com_pointer->receive_buff,sizeof(com_pointer->data_cache[com_pointer->data_pointer]));		
		//�������ݳ���
		com_pointer->data_cache_long[com_pointer->data_pointer]=buff_long;
		//������ż�һ
		com_pointer->data_pointer++;		
	}
}
/******************************************
�������ƣ�
��    �ܣ� ͨ�ý������ݽ�����ִ�����β��޷��غ���
��    ����
����ֵ  ��
*******************************************/
void Common_Data_Analysis_Handle(COM_FRAME  *com_pointer,void (*Common_Analysis_Handle)(u8 * receive_buff , u8 length))
{
	if(com_pointer->data_pointer>0)
	{			
		//��ս���BUFF
		memset(com_pointer->analysis_buff,0,sizeof(com_pointer->analysis_buff));
		//�ӻ����п������ݵ�����BUFF
		memcpy(com_pointer->analysis_buff,com_pointer->data_cache[com_pointer->data_pointer-1],sizeof(com_pointer->analysis_buff));
		//�������ݳ���
		com_pointer->analysis_long= com_pointer->data_cache_long[com_pointer->data_pointer-1];
		// ��ջ���
		memset(com_pointer->data_cache[com_pointer->data_pointer-1],0,sizeof(com_pointer->data_cache[com_pointer->data_pointer-1]));	
		com_pointer->data_cache_long[com_pointer->data_pointer-1]=0;
 		//���ݽ���		
		Common_Analysis_Handle(com_pointer->analysis_buff , com_pointer->analysis_long);
		//ָ���Լ�
		com_pointer->data_pointer--;					
	}		
}
/******************************************
�������ƣ�
��    �ܣ� 
��    ����
����ֵ  ��
*******************************************/
void User_Communication_Analysis(void)
{
	/*�����û�Э�鴦��*/
	
}
/******************************************
�������ƣ�
��    �ܣ� 
��    ����
����ֵ  ��
*******************************************/
void Communication_Analysis_Handle(void)
{
	/*���ݽ���*/
	Common_Data_Analysis_Handle( &rs485_commnuication_frame, User_Communication_Analysis );
}
/***********************************************END*****************************************************/
