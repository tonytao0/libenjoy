/**************************************************************************
*	Copyright 2020-2030 OnePeaceAllBlue Co.,Ltd
*	All right reserved. See COPYRIGHT for detailed Information
*
*	@file		WinSocketInit.h
*	@brief		winsocket库加载与释放类声明文件
*	@note		
*
		* @author		tonytao
		* @date			2022-06-15
		* @history		修改历史
			@author：	tonytao
			@date:		2022-06-15
			@brief:		创建
***************************************************************************/
#ifndef _WINSOCKETINIT_H_
#define _WINSOCKETINIT_H_


#include <WinSock2.h>
#include <iostream>

#pragma comment(lib,"ws2_32.lib")


class WinScoketInit
{
public:
	WinScoketInit();

	~WinScoketInit();

private:
	WSADATA wsaData;
};

#endif // !_WINSOCKETINIT_H_