#include <iostream>
#include "Joystick.h"

int main()
{
    Joystick joy;

    if (joy.strat())
    {
        while (true)
        {
            JoyEvent ev = joy.getJoyEvent();
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

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    }

    std::cout << "Hello World!\n";
}