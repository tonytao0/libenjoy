#ifdef __linux__
#include "../src/linux/udp_client.h"
#elif _WIN32
#include "../src/windows/udp_client.h"
#endif // __linux__
