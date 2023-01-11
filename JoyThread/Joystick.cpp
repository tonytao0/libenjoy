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
	, joy(nullptr)
	, ev{ 0 }
	, runFlag(false)
{
	//set device name
	joystickName = "Joystick";
	throttleName = "Throttle";
}

Joystick::~Joystick()
{
	// Joystick is really closed in libenjoy_poll or libenjoy_close,
	// because closing it while libenjoy_poll is in process in another thread
	// could cause crash. Be sure to call libenjoy_poll(ctx, NULL); (yes,
	// you can use NULL as event) if you will not poll nor libenjoy_close
	// anytime soon.
	libenjoy_close_joystick(joy);

	// Frees memory allocated by that joystick list.
	libenjoy_free_info_list(info);

	// deallocates all memory used by lib.
	libenjoy_close(ctx);
}

bool Joystick::strat()
{
	// initialize the library
	ctx = libenjoy_init();

	// Updates internal list of joysticks.
	libenjoy_enumerate(ctx);

	// get list with available joysticks.
	info = libenjoy_get_info_list(ctx);
	if (info->count <= 0)
	{
		stop();

		return false;
	}

	openJoysticks();

	runFlag = true;
	
	threadPtr = std::make_shared<std::thread>(&Joystick::run, this);

	//what is the worst situation
	//std::thread joyThread(&Joystick::run,this);
	//joyThread.detach();

	return true;
}

void Joystick::stop()
{
	//close thread
	runFlag = false;

	libenjoy_close_joystick(joy);

	// Frees memory allocated by that joystick list.
	libenjoy_free_info_list(info);

	// deallocates all memory used by lib.
	libenjoy_close(ctx);

	joy = nullptr;
	info = nullptr;
	ctx = nullptr;
}

void Joystick::getJoyEvent(libenjoy_event& event)
{
	event.data = ev.data;
	event.joy_id = ev.joy_id;
	event.part_id = ev.part_id;
	event.type = ev.type;
}

JoyEvent Joystick::getJoyEvent()
{
	//std::unique_lock<std::mutex> loker(m_lock);
	return ev;
}

void Joystick::openJoysticks()
{
	if (info != nullptr)
	{
		for (uint32_t i = 0; i < info->count; i++)
		{
			std::string devName(info->list[i]->name);
			if (std::string::npos != devName.find(joystickName) || std::string::npos != devName.find(throttleName))
			{
				libenjoy_open_joystick(ctx, info->list[i]->id);
			}
		}
	}
}

void Joystick::run()
{
	int counter = 0;

	while (runFlag)
	{
		// Value data are not stored in library. if you want to use
		// them, you have to store them

		// That's right, only polling available
		while (libenjoy_poll(ctx, &ev))
		{
			switch (ev.type)
			{
			case LIBENJOY_EV_AXIS:
				printf("%u: axis %d val %d\n", ev.joy_id, ev.part_id, ev.data);
				break;
			case LIBENJOY_EV_BUTTON:
				printf("%u: button %d val %d\n", ev.joy_id, ev.part_id, ev.data);
				break;
			case LIBENJOY_EV_CONNECTED:
				printf("%u: status changed: %d\n", ev.joy_id, ev.data);
				break;
			}
		}
#ifdef __linux
		usleep(50000);
#else
		Sleep(50);
#endif

		counter += 50;
		if (counter >= 2000)
		{
			libenjoy_enumerate(ctx);
			counter = 0;
		}
	}
}
