#ifdef ENABLE_DEBUG_MACRO
#  define LogError(msg, ...) fprintf(stdout, "\x1B[31m[Error]\033[0m \x1B[90m" __FILE__ ":\033[0m " msg "\n", ##__VA_ARGS__);
#  define LogInfo(msg, ...) fprintf(stdout, "\x1B[33m[Info]\033[0m \x1B[90m" __FILE__ ":\033[0m " msg "\n", ##__VA_ARGS__);
#else
#  define LogError(msg, ...) 
#  define LogInfo(msg, ...) 
#endif