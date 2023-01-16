#ifndef JOYDTICK_H
#define JOYDTICK_H

#include <string>
#include <stdio.h>
#include  <thread>


#include "../src/libenjoy.h"


#ifdef __cplusplus

typedef libenjoy_event JoyEvent;


class Joystick
{
public:
	Joystick();
	~Joystick();

	//get joystick events.return true when obtain  change events,otherwise return false
	bool  getJoyEvent(JoyEvent& ev);

	//update joystick list.if joysick throttle re-plugging (or plugging),this function will try to open it.
	void updateJoysticks();

	//return joystick device-id if it's opened,otherwise return 9999.
	uint32_t getJoystickID();

	//return throttle device-id if it's opened.
	uint32_t getThrottleID();

private:
	//open joystick and throttle devices.
	void openJoysticks();

	

private:
	libenjoy_context* ctx;
	libenjoy_joy_info_list* info;
	libenjoy_joystick* joystickPtr;
	libenjoy_joystick* throttlePtr;

	std::string joystickName;
	std::string throttleName;
};
#endif //__cplusplus

#endif // !JOYDTICK_H