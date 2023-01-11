#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>  

#include "../src/libenjoy.h"
#pragma comment(lib, "winmm.lib")

int main()
{
    int sockfd;
    struct sockaddr_in server_addr;
    char send_data[1024];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8888);
    server_addr.sin_addr.s_addr = inet_addr("192.168.40.86");

    strcpy(send_data, "Hello, UDP Server!");
    sendto(sockfd, send_data, strlen(send_data), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));


    // initialize the library
	libenjoy_context *ctx = libenjoy_init(); 
    libenjoy_joy_info_list *info;

    // Updates internal list of joysticks. 
    libenjoy_enumerate(ctx);

    // get list with available joysticks. 
    info = libenjoy_get_info_list(ctx);

    if(info->count != 0) // just get the first joystick
    {
        libenjoy_joystick *joy;
        // printf("Opening joystick %s...", info->list[0]->name);
        joy = libenjoy_open_joystick(ctx, info->list[0]->id);
        joy = libenjoy_open_joystick(ctx, info->list[1]->id);
        if(joy)
        {
            int counter = 0;
            libenjoy_event ev;

            printf("Success!\n");
            printf("Axes: %d btns: %d\n", libenjoy_get_axes_num(joy),libenjoy_get_buttons_num(joy));

			char buf[9] = {0};

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

					memcpy(buf,&ev.joy_id,4);
					memcpy(buf+4,&ev.data,2);
					memcpy(buf+6,&ev.part_id,1);
					memcpy(buf+7,&ev.type,1);
					buf[8] = '\0';

					sendto(sockfd, buf, 8, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

                    printf("send successful\n");
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

	// int ret = 0;
	// do
	// {
	// 	// 2. 可以sendto发送和recvfrom接收了
	// 	ret = sendto(sockfd, "I am UDP Client!", strlen("I am UDP Client!"), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

	// 	char buf[100] = { 0 };
	// 	recvfrom(sockfd, buf, 100, 0, NULL, NULL); //不关注服务端地址

	// 	std::cout << "recvfrom " << ret << ":" << buf << std::endl;

	// 	usleep(500);
	// } while (ret >0);

    close(sockfd);
    return 0;
}

// #include <stdio.h>   
// #include <string.h>   
// #include <errno.h>   
// #include <stdlib.h>   
// #include <unistd.h>   
// #include <sys/types.h>   
// #include <sys/socket.h>   
// #include <netinet/in.h>   
// #include <arpa/inet.h>   
   
  
// #define DEST_PORT 8000   
// #define DSET_IP_ADDRESS  "127.0.0.1"   
   
  
// int main()  
// {  
//   /* socket文件描述符 */  
//   int sock_fd;  
  
//   /* 建立udp socket */  
//   sock_fd = socket(AF_INET, SOCK_DGRAM, 0);  
//   if(sock_fd < 0)  
//   {  
//     perror("socket");  
//     exit(1);  
//   }  
    
//   /* 设置address */  
//   struct sockaddr_in addr_serv;  
//   int len;  
//   memset(&addr_serv, 0, sizeof(addr_serv));  
//   addr_serv.sin_family = AF_INET;  
//   addr_serv.sin_addr.s_addr = inet_addr(DSET_IP_ADDRESS);  
//   addr_serv.sin_port = htons(DEST_PORT);  
//   len = sizeof(addr_serv);  
  
    
//   int send_num;  
//   int recv_num;  
//   char send_buf[20] = "hey, who are you?";  
//   char recv_buf[20];  
      
//   printf("client send: %s\n", send_buf);  
    
//   send_num = sendto(sock_fd, send_buf, strlen(send_buf), 0, (struct sockaddr *)&addr_serv, len);  
    
//   if(send_num < 0)  
//   {  
//     perror("sendto error:");  
//     exit(1);  
//   }  
    
//   recv_num = recvfrom(sock_fd, recv_buf, sizeof(recv_buf), 0, (struct sockaddr *)&addr_serv, (socklen_t *)&len);  
    
//   if(recv_num < 0)  
//   {  
//     perror("recvfrom error:");  
//     exit(1);  
//   }  
    
//   recv_buf[recv_num] = '\0';  
//   printf("client receive %d bytes: %s\n", recv_num, recv_buf);  
    
//   close(sock_fd);  
    
//   return 0;  
// }