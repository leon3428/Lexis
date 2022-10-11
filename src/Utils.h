#ifdef ENABLE_DEBUG_MACRO
#  define DEBUG_PRINT(msg, ...) fprintf(stdout, "[debug] " msg "\n", ##__VA_ARGS__);
#else
#  define DEBUG_PRINT(msg, ...) 
#endif