# libenjoy
[![Build Status](https://travis-ci.org/Tasssadar/libenjoy.png?branch=master)](https://travis-ci.org/Tasssadar/libenjoy)

this library fork from [Tasssadar/libenjoy](https://github.com/Tasssadar/libenjoy).

Lighweight (I mean simple) joystick library for Linux and Windows. It is desinged
for use in [Lorris](https://github.com/Tasssadar/Lorris), so it is
the easiest to use with Qt - just  `include(libenjoy.pri)` in your .pro file.
It can be however used with any C/C++ app, just `#include "libenjoy.h"` and make
sure that `libenjoy.c` and `libenjoy_linux.c`/`libenjoy_win32.c` are compiled.

On Windows, you have to link with winmm.lib!

### Highlights
* Small. All files combined (both Linux and Windows) have about 1000 lines.
* Almost no additional dependencies.
  * On Linux, it is nothing other than GCC and kernel 2.2+
  * winmm.lib on Windows - nothing special
* Remembers joysticks. Joystick ID is unique, and libenjoy can automatically
  reconnect re-plugged joysticks. This works flawlessly on Linux, Windows
  on the other hand does not like it very much. Be sure to re-plug joysticks
  to the same USB port when using multiple joysticks at once.
* __Thread-safety note__: all functions (for one context) should be called
  from the same thread. The only exception is `libenjoy_poll`, which is made
  thread safe, so that you can make another thread to poll joystick events.

### WARNING!
The fact that **libenjoy** can handle re-plugged joystick means it
**cannot handle two or more exactly same joysticks at once**. It will pick
the one which was plugged-in first and ignore the other ones. This is because
without libusb, I can't get really unique device id, so there is no way for me
to identify more than one joystick of same type. But the situation "I just
wanna to fix the joystick's cable without restarting the app" is more frequent
than "Hey, let's buy twelve exactly same joysticks!", at least for me - that is
why I went on with this solution.

### Usage:
#### libenjoy example
```C
#include <stdio.h>
#ifdef __linux
  #include <unistd.h>
#else
  #include <windows.h>
#endif

#include "libenjoy.h"

// This tels msvc to link agains winmm.lib. Pretty nasty though.
#pragma comment(lib, "winmm.lib")

int main()
{
    libenjoy_context *ctx = libenjoy_init(); // initialize the library
    libenjoy_joy_info_list *info;

    // Updates internal list of joysticks. If you want auto-reconnect
    // after re-plugging the joystick, you should call this every 1s or so
    libenjoy_enumerate(ctx);

    // get list with available joysticks. structs are
    // typedef struct libenjoy_joy_info_list {
    //     uint32_t count;
    //     libenjoy_joy_info **list;
    // } libenjoy_joy_info_list;
    //
    // typedef struct libenjoy_joy_info {
    //     char *name;
    //     uint32_t id;
    //     char opened;
    // } libenjoy_joy_info;
    //
    // id is not linear (eg. you should not use vector or array), 
    // and if you disconnect joystick and then plug it in again,
    // it should have the same ID
    info = libenjoy_get_info_list(ctx);

    if(info->count != 0) // just get the first joystick
    {
        libenjoy_joystick *joy;
        printf("Opening joystick %s...", info->list[0]->name);
        joy = libenjoy_open_joystick(ctx, info->list[0]->id);
        if(joy)
        {
            int counter = 0;
            libenjoy_event ev;

            printf("Success!\n");
            printf("Axes: %d btns: %d\n", libenjoy_get_axes_num(joy),libenjoy_get_buttons_num(joy));

            while(1)
            {
                // Value data are not stored in library. if you want to use
                // them, you have to store them

                // That's right, only polling available
                while(libenjoy_poll(ctx, &ev))
                {
                    switch(ev.type)
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
                if(counter >= 1000)
                {
                    libenjoy_enumerate(ctx);
                    counter = 0;
                }
            }

            // Joystick is really closed in libenjoy_poll or libenjoy_close,
            // because closing it while libenjoy_poll is in process in another thread
            // could cause crash. Be sure to call libenjoy_poll(ctx, NULL); (yes,
            // you can use NULL as event) if you will not poll nor libenjoy_close
            // anytime soon.
            libenjoy_close_joystick(joy);
        }
        else
            printf("Failed!\n");
    }
    else
        printf("No joystick available\n");

    // Frees memory allocated by that joystick list. Do not forget it!
    libenjoy_free_info_list(info);

    // deallocates all memory used by lib. Do not forget this!
    // libenjoy_poll must not be called during or after this call
    libenjoy_close(ctx); 
    return 0;
}
```

#### JoyMannager example（secondary development of libenjoy)
```CPP
#include <iostream>

//you need include "Joystick.h"
#include "../../src/Joystick.h"

int main()
{
    Joystick joy;

    int count = 0;
    JoyEvent ev{ 0 };
    uint32_t joy_id = joy.getJoystickID();
    uint32_t thro_id = joy.getThrottleID();
    while (true)
    {
        if (joy.getJoyEvent(ev))
        {
            if (ev.joy_id == joy_id)
            {
                switch (ev.type)
                {
                case LIBENJOY_EV_AXIS:
                    printf("joystick: axis %d val %d\n", ev.part_id, ev.data);
                    break;
                case LIBENJOY_EV_BUTTON:
                    printf("joystick: button %d val %d\n", ev.part_id, ev.data);
                    break;
                case LIBENJOY_EV_CONNECTED:
                    printf("joystick: status changed: %d\n", ev.data);
                    break;
                }
            }
            else
            {
                switch (ev.type)
                {
                case LIBENJOY_EV_AXIS:
                    printf("throttle: axis %d val %d\n", ev.part_id, ev.data);
                    break;
                case LIBENJOY_EV_BUTTON:
                    printf("throttle: button %d val %d\n", ev.part_id, ev.data);
                    break;
                case LIBENJOY_EV_CONNECTED:
                    printf("throttle: status changed: %d\n", ev.data);
                    break;
                }
            }

        }

        //delay 100 ms
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        count += 10;
        if (count == 100)
        {
            joy.updateJoysticks();
            count = 0;
            joy_id = joy.getJoystickID();
            thro_id = joy.getThrottleID();
        }
    }

    std::cout << "Hello World!\n";
}
```
#### RemoteRocker example（secondary development of libenjoy)
```CPP
#include <iostream>
#include <thread>
#include "../../JoyClass/Joystick.h"
#include "../../lib/udp/include/Client.h"


int main()
{
    UDPClient client;

    client.resetIP("127.0.0.1");
    client.resetPort(9999);

    if (!client.connect())
    {
        std::cout << "connect failed!\n";

        return -1;
    }

    //client.send("this is udp clinet");

    //int ret = client.recv(buf, 1024);

    Joystick joy;

    int count = 0;
    JoyEvent ev{ 0 };
    uint32_t joy_id = joy.getJoystickID();
    uint32_t thro_id = joy.getThrottleID();

    char joy_buf[5] = { 0 };
    char thro_buf[5] = { 0 };
    while (true)
    {
        //摇杆的udp传输协议
        /*
        {
            uint8_t joy_id; //摇杆类型：自定义joystick值为0，throttle的值为2
            int16_t data;    //按键的值
            uint8_t part_id; //按键编号
            uint8_t type;    //事件类型
        }
        */
        //远程传输代码如下：
        if (joy.getJoyEvent(ev))
        {
            if (ev.joy_id == joy_id)
            {
                /* switch (ev.type)
                 {
                 case LIBENJOY_EV_AXIS:
                     printf("joystick: axis %d val %d\n", ev.part_id, ev.data);
                     break;
                 case LIBENJOY_EV_BUTTON:
                     printf("joystick: button %d val %d\n", ev.part_id, ev.data);
                     break;
                 case LIBENJOY_EV_CONNECTED:
                     printf("joystick: status changed: %d\n", ev.data);
                     break;
                 }*/
                joy_buf[0] = 0x00;
                //memcpy(joy_buf, &ev.data, 2);
                joy_buf[1] = (ev.data & 0xff);
                joy_buf[2] = ((ev.data & 0xff00) >> 8);
                joy_buf[3] = ev.part_id;
                joy_buf[4] = ev.type;

                client.send(joy_buf, 5);
            }
            else
            {
                switch (ev.type)
                {
                case LIBENJOY_EV_AXIS:
                    printf("throttle: axis %d val %d\n", ev.part_id, ev.data);
                    break;
                case LIBENJOY_EV_BUTTON:
                    printf("throttle: button %d val %d\n", ev.part_id, ev.data);
                    break;
                case LIBENJOY_EV_CONNECTED:
                    printf("throttle: status changed: %d\n", ev.data);
                    break;
                }

                thro_buf[0] = 0x00;
                //memcpy(thro_buf, &ev.data, 2);
                thro_buf[1] = (ev.data & 0xff);
                thro_buf[2] = ((ev.data & 0xff00) >> 8);
                thro_buf[3] = ev.part_id;
                thro_buf[4] = ev.type;

                client.send(thro_buf, 5);
            }
        }

        //delay 100 ms
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        count += 10;
        if (count == 100)
        {
            joy.updateJoysticks();
            count = 0;
            joy_id = joy.getJoystickID();
            thro_id = joy.getThrottleID();
        }
    }

    std::cout << "Hello World!\n";
}
```

### License
LGPLv2.1, see COPYING. Most of libenjoy_win32.c is from libSDL - thanks!
