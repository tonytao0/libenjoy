#include "Joystick.h"


#ifdef __linux
#include <unistd.h>
#else
#include <windows.h>
#endif

// This tels msvc to link agains winmm.lib. Pretty nasty though.
#pragma comment(lib, "winmm.lib")


Joystick::Joystick()
	: ctx(nullptr)
	, info(nullptr)
	, joystickPtr(nullptr)
	, throttlePtr(nullptr)
{
	//set device name
	joystickName = "Joystick";
	throttleName = "Throttle";

	// initialize the library
	ctx = libenjoy_init();

	// Updates internal list of joysticks.
	libenjoy_enumerate(ctx);

	// get list with available joysticks.
	info = libenjoy_get_info_list(ctx);
	if (info->count > 0)
	{
		openJoysticks();
	}
}

Joystick::~Joystick()
{
	// Joystick is really closed in libenjoy_poll or libenjoy_close,
	// because closing it while libenjoy_poll is in process in another thread
	// could cause crash. Be sure to call libenjoy_poll(ctx, NULL); (yes,
	// you can use NULL as event) if you will not poll nor libenjoy_close
	// anytime soon.
	if (joystickPtr != nullptr)
	{
		libenjoy_close_joystick(joystickPtr);
	}

	if (nullptr != throttlePtr)
	{
		libenjoy_close_joystick(throttlePtr);
	}
	

	// Frees memory allocated by that joystick list.
	libenjoy_free_info_list(info);

	// deallocates all memory used by lib.
	libenjoy_close(ctx);
}

bool Joystick::getJoyEvent(JoyEvent& ev)
{
	int eventCount = 0;
	while (libenjoy_poll(ctx, &ev)) //read all event in the queue
	{
		eventCount++;
	}

	if (eventCount > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Joystick::updateJoysticks()
{
	// Updates internal list of joysticks.
	libenjoy_enumerate(ctx);

	// get list with available joysticks.
	info = libenjoy_get_info_list(ctx);

	openJoysticks();
}

void Joystick::openJoysticks()
{
	if (info != nullptr)
	{
		for (uint32_t i = 0; i < info->count; i++)
		{
			std::string devName(info->list[i]->name);
			if (std::string::npos != devName.find(joystickName) && joystickPtr == nullptr)
			{
				joystickPtr = libenjoy_open_joystick(ctx, info->list[i]->id);
			}
			else if (std::string::npos != devName.find(throttleName) && throttlePtr == nullptr)
			{
				throttlePtr = libenjoy_open_joystick(ctx, info->list[i]->id);
			}
		}
	}
}

uint32_t Joystick::getJoystickID()
{
	if (joystickPtr != nullptr)
	{
		return joystickPtr->id;
	}

	return 9999;
}

uint32_t Joystick::getThrottleID()
{
	if (throttlePtr != nullptr)
	{
		return throttlePtr->id;
	}

	return 9999;
}
