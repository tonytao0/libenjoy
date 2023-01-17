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