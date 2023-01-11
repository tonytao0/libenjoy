#ifndef JOYDTICK_H
#define JOYDTICK_H

#include <string>
#include <stdio.h>
#include  <thread>


#include "../src/libenjoy.h"


#ifdef __cplusplus

//typedef struct joy_event
//{
//	uint32_t deviceID;
//	int16_t  keyVal;
//	uint8_t  part_id;
//	uint8_t  type;
//}JoyEvent;

typedef libenjoy_event JoyEvent;


class Joystick
{
public:
	Joystick();
	~Joystick();

	bool strat();

	void stop();

	void getJoyEvent(libenjoy_event & event);

	JoyEvent getJoyEvent();

private:
	void openJoysticks();

	void run();

	

private:
	libenjoy_context* ctx;
	libenjoy_joy_info_list* info;
	libenjoy_joystick* joy;
	JoyEvent ev;

	std::string joystickName;
	std::string throttleName;

	bool runFlag;

	std::shared_ptr<std::thread> threadPtr;
};
#endif //__cplusplus

#endif // !JOYDTICK_H