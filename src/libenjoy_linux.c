#include <sys/stat.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "libenjoy.h"
#include "libenjoy_p.h"
#include "libenjoy_linux.h"

static libenjoy_known_info **known_devs = NULL;

void libenjoy_init_private(void)
{
    known_devs = (libenjoy_known_info **)calloc(1, sizeof(libenjoy_known_info*));
}

void libenjoy_close_private(void)
{
    libenjoy_known_info *i = known_devs[0];
    for(; i != NULL; ++i)
    {
        free(i->path);
        free(i);
    }
    free(known_devs);
}

void libenjoy_enumerate(void)
{
    static const char* joydev_paths[] = {
        "/dev/input/js%u",
        "/dev/js%u"
    };

    char path[PATH_MAX];
    struct stat jstat;
    uint32_t j, i;

    uint32_t *existing_ids = libenjoy_create_existing_ids();
    uint32_t existing_size = joy_info.count;

    for(j = 0;j < LIBENJOY_MAX_JOYSTICK; ++j)
    {
        for(i = 0; i < sizeof(joydev_paths)/sizeof(joydev_paths[0]); ++i)
        {
            sprintf(path, joydev_paths[i], j);

            if(stat(path, &jstat) != 0)
                continue;

            int fd = open(path, O_RDONLY);
            if(fd == -1)
                continue;

            libenjoy_known_info *inf = libenjoy_get_known_devid(jstat.st_rdev);
            if(inf == NULL)
                inf = libenjoy_add_known_id(jstat.st_rdev, path);
            else
            {
                libenjoy_set_id_exists(inf->id, existing_ids);

                // Update path if needed
                if(strcmp(inf->path, path) != 0)
                {
                    inf->path = (char*)realloc(inf->path, strlen(path)+1);
                    strcpy(inf->path, path);
                }
            }

            if(libenjoy_joy_info_created(inf->id) != 0)
            {
                libenjoy_joy_info *joy_inf = (libenjoy_joy_info*)malloc(sizeof(libenjoy_joy_info));
                joy_inf->id = inf->id;
                joy_inf->opened = 0;

                char name[128];
                if (ioctl(fd, JSIOCGNAME(sizeof(name)), &name) < 0)
                    strncpy(name, "Unknown", sizeof(name));

                joy_inf->name = (char*)malloc(strlen(name)+1);
                strcpy(joy_inf->name, name);

                libenjoy_add_joy_info(joy_inf);
                printf("path %s name %s\n", inf->path, joy_inf->name);
            }

            close(fd);
        }
    }

    // remove no longer existing joysticks
    for(i = 0; i < existing_size; ++i)
    {
        if(existing_ids[i] == UINT_MAX)
            continue;
        libenjoy_destroy_joy_info(existing_ids[i]);
    }
    free(existing_ids);
}

libenjoy_known_info *libenjoy_get_known_devid(dev_t devid)
{
    libenjoy_known_info *i = known_devs[0];
    for(; i != NULL; ++i)
    {
        if(i->devid == devid)
            return i;
    }
    return NULL;
}

libenjoy_known_info *libenjoy_get_known_id(uint32_t id)
{
    libenjoy_known_info *i = known_devs[0];
    for(; i != NULL; ++i)
    {
        if(i->id == id)
            return i;
    }
    return NULL;
}

libenjoy_known_info *libenjoy_add_known_id(dev_t devid, char *path)
{
    uint32_t count = 2;
    libenjoy_known_info *i = known_devs[0];
    for(; i != NULL; ++i)
        ++count;

    known_devs = (libenjoy_known_info**)realloc(known_devs, sizeof(libenjoy_known_info*)*count);
    count -= 2;

    i = (libenjoy_known_info*)malloc(sizeof(libenjoy_known_info));
    i->devid = devid;
    i->id = libenjoy_get_new_joyid();
    i->path = (char*)malloc(strlen(path)+1);
    strcpy(i->path, path);
    printf("create path %s\n", path);

    known_devs[count] = i;
    return i;
}

uint32_t *libenjoy_create_existing_ids()
{
    uint32_t *res = (uint32_t*)malloc(joy_info.count);
    uint32_t i = 0;
    for(; i < joy_info.count; ++i)
        res[i] = joy_info.list[i]->id;
    return res;
}

void libenjoy_set_id_exists(uint32_t id, uint32_t *list)
{
    uint32_t i = 0;
    for(; i < joy_info.count; ++i)
    {
        if(list[i] == id)
        {
            list[i] = UINT_MAX;
            break;
        }
    }
}

libenjoy_os_specific *libenjoy_open_os_specific(uint32_t id)
{
    libenjoy_known_info *inf = libenjoy_get_known_id(id);
    if(!inf)
        return NULL;

    int fd = open(inf->path, O_RDONLY);
    if(fd == -1)
        return NULL;

    libenjoy_os_specific *res = (libenjoy_os_specific*)malloc(sizeof(libenjoy_os_specific));
    res->fd = fd;
    return res;
}

void libenjoy_close_os_specific(libenjoy_os_specific *os)
{
    close(os->fd);
    free(os);
}

int libenjoy_get_axes_num(libenjoy_joystick *joy)
{
    char num = 0;
    if(joy->os)
        ioctl(joy->os->fd, JSIOCGAXES, &num);
    return num;
}

int libenjoy_get_buttons_num(libenjoy_joystick *joy)
{
    char num = 0;
    if(joy->os)
        ioctl(joy->os->fd, JSIOCGBUTTONS, &num);
    return num;
}
