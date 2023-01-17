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