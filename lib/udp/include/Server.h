#ifdef __linux__
#include "../linux/udp_server.h"
#elif _WIN32
#include "../windows/udp_server.h"
#endif // __linux__
