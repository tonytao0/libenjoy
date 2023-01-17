/**************************************************************************
*	Copyright 2020-2030 OnePeaceAllBlue Co.,Ltd
*	All right reserved. See COPYRIGHT for detailed Information
*
*	@file		WinSocketInit.cpp
*	@brief		winsocket��������ͷ���ʵ���ļ�
*	@note		
*
		* @author		tonytao
		* @date			2022-06-15
		* @history		�޸���ʷ
			@author��	tonytao
			@date:		2022-06-15
			@brief:		����
***************************************************************************/


#include "WinScoketInit.h"

WinScoketInit::WinScoketInit()
{
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (NO_ERROR != iResult)
	{
		std::cout << "��ʼ��windows socket��ʧ��\n";

		//exit if init failed(maybe too violent)
		exit(-1);
	}
}

WinScoketInit::~WinScoketInit()
{
	WSACleanup();
}
