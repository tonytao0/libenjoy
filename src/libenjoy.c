#include <stdlib.h>
#include <string.h>

#include "libenjoy.h"
#include "libenjoy_p.h"

struct libenjoy_joy_info_list joy_info;
struct libenjoy_joystick_list joy_list;

void libenjoy_init(void)
{
    joy_info.count = 0;
    joy_info.list = NULL;

    joy_list.count = 0;
    joy_list.list = NULL;

    libenjoy_init_private();
}

void libenjoy_close(void)
{
    libenjoy_close_private();
}

uint32_t libenjoy_get_new_joyid(void)
{
    static uint32_t counter = 0;
    return counter++;
}

int libenjoy_joy_info_created(uint32_t id)
{
    uint32_t i = 0;
    for(; i < joy_info.count; ++i)
        if(id == joy_info.list[i]->id)
            return 0;
    return -1;
}

libenjoy_joy_info* libenjoy_get_joy_info(uint32_t id)
{
    uint32_t i = 0;
    for(; i < joy_info.count; ++i)
        if(id == joy_info.list[i]->id)
            return joy_info.list[i];
    return NULL;
}

void libenjoy_add_joy_info(libenjoy_joy_info *inf)
{
    joy_info.list = (libenjoy_joy_info**)realloc(joy_info.list, ++joy_info.count*sizeof(libenjoy_joy_info*));
    joy_info.list[joy_info.count-1] = inf;

    // try to reopen old joystick
    libenjoy_joystick *joy = libenjoy_get_joystick(inf->id);
    if(joy && joy->valid == 0)
    {
        joy->os = libenjoy_open_os_specific(inf->id);
        if(joy->os)
        {
            inf->opened = 1;
            joy->valid = 1;
        }
    }
}

void libenjoy_destroy_joy_info(uint32_t id)
{
    uint32_t i = 0;
    for(; i < joy_info.count; ++i)
    {
        if(joy_info.list[i]->id != id)
            continue;

        libenjoy_joy_set_valid(id, 0);
        free(joy_info.list[i]);

        --joy_info.count;
        if(joy_info.count == 0)
        {
            free(joy_info.list);
            joy_info.list = NULL;
            return;
        }

        if(i != joy_info.count)
            joy_info.list[i] = joy_info.list[joy_info.count];

        joy_info.list = (libenjoy_joy_info**)realloc(joy_info.list, joy_info.count*sizeof(libenjoy_joy_info*));
        return;
    }
}

libenjoy_joy_info_list *libenjoy_get_info_list(void)
{
    libenjoy_joy_info_list *res = (libenjoy_joy_info_list*)malloc(sizeof(libenjoy_joy_info_list));
    res->count = joy_info.count;
    res->list = (libenjoy_joy_info**)calloc(res->count, sizeof(libenjoy_joy_info*));

    uint32_t i = 0;
    for(; i < joy_info.count; ++i)
    {
        res->list[i] = (libenjoy_joy_info*)malloc(sizeof(libenjoy_joy_info));
        res->list[i]->id = joy_info.list[i]->id;
        res->list[i]->name = (char*)malloc(strlen(joy_info.list[i]->name)+1);
        strcpy(res->list[i]->name, joy_info.list[i]->name);
    }
    return res;
}

void libenjoy_free_info_list(libenjoy_joy_info_list *list)
{
    uint32_t i = 0;
    for(; i < list->count; ++i)
    {
        free(list->list[i]->name);
        free(list->list[i]);
    }
    free(list->list);
    free(list);
}

libenjoy_joystick *libenjoy_open_joystick(uint32_t id)
{
    libenjoy_joy_info* info = libenjoy_get_joy_info(id);
    if(!info || info->opened != 0)
        return NULL;

    libenjoy_os_specific *os = libenjoy_open_os_specific(id);
    if(!os)
        return NULL;

    libenjoy_joystick *joy = (libenjoy_joystick*)malloc(sizeof(libenjoy_joystick));
    joy->id = id;
    joy->os = os;
    joy->valid = 1;
    libenjoy_add_joy_to_list(joy);

    info->opened = 1;
    return joy;
}

void libenjoy_close_joystick(libenjoy_joystick *joy)
{
    if(joy->os)
        libenjoy_close_os_specific(joy->os);

    if(joy->valid == 1)
    {
        libenjoy_joy_info* info = libenjoy_get_joy_info(joy->id);
        if(info)
            info->opened = 0;
    }

    libenjoy_rm_joy_from_list(joy);
    free(joy);
}

void libenjoy_add_joy_to_list(libenjoy_joystick *joy)
{
    joy_list.list = (libenjoy_joystick**)realloc(joy_list.list, ++joy_list.count*sizeof(libenjoy_joystick*));
    joy_list.list[joy_list.count-1] = joy;
}

void libenjoy_rm_joy_from_list(libenjoy_joystick *joy)
{
    uint32_t i = 0;
    for(;i < joy_list.count; ++i)
    {
        if(joy_list.list[i] != joy)
            continue;

        --joy_list.count;

        if(i != joy_list.count)
            joy_list.list[i] = joy_list.list[joy_list.count];

        joy_list.list = (libenjoy_joystick**)realloc(joy_list.list, joy_list.count*sizeof(libenjoy_joystick*));
        return;
    }
}

void libenjoy_joy_set_valid(uint32_t id, char valid)
{
    libenjoy_joystick *joy = libenjoy_get_joystick(id);
    if(joy)
    {
        if(joy->valid == 1 && valid == 0)
        {
            libenjoy_close_os_specific(joy->os);
            joy->os = NULL;
        }
        joy->valid = valid;
    }
}

libenjoy_joystick *libenjoy_get_joystick(uint32_t id)
{
    uint32_t i = 0;
    for(;i < joy_list.count; ++i)
    {
        if(joy_list.list[i]->id == id)
            return joy_list.list[i];
    }
    return NULL;
}

