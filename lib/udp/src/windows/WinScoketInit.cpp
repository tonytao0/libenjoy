/**************************************************************************
*	Copyright 2020-2030 OnePeaceAllBlue Co.,Ltd
*	All right reserved. See COPYRIGHT for detailed Information
*
*	@file		WinSocketInit.cpp
*	@brief		winsocket库加载与释放类实现文件
*	@note		
*
		* @author		tonytao
		* @date			2022-06-15
		* @history		修改历史
			@author：	tonytao
			@date:		2022-06-15
			@brief:		创建
***************************************************************************/


#include "WinScoketInit.h"

WinScoketInit::WinScoketInit()
{
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (NO_ERROR != iResult)
	{
		std::cout << "初始化windows socket库失败\n";

		//exit if init failed(maybe too violent)
		exit(-1);
	}
}

WinScoketInit::~WinScoketInit()
{
	WSACleanup();
}
