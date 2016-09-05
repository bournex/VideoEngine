#ifndef SC_PRE_CONFIGS_WINDOWS
#define SC_PRE_CONFIGS_WINDOWS

#if defined(WIN32) || defined(_WIN32)|| defined(_WIN32_)
#define SC_OS_WIN32 1
#endif

#if defined(WIN64) || defined(_WIN64)|| defined(_WIN64_)
#define SC_OS_WIN64  1
#endif  


#define SC_OS_WINDOWS  SC_OS_WIN64||SC_OS_WIN32
	
#define SC_MEM_POOL  1

#endif  