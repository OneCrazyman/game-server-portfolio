//#define PROFILE
#ifdef PROFILE
#define CREATE_PROFILE Profiler pf(__FUNCTION__)
#include "profile.h"
#else
#define CREATE_PROFILE
#endif // !1