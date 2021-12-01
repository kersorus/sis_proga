#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define CHECKR(condition,message,errcode)\
        if (condition)\
        {\
            errno = 0;\
            errno += errcode;\
            perror(message);\
            errno = 0;\
        }\
