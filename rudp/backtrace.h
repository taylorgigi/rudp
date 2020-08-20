#ifndef LIBJUPITER_BACK_TRACE_INCLUDE_H
#define LIBJUPITER_BACK_TRACE_INCLUDE_H

#include <execinfo.h>

#define FRAME_BUFFER_SIZE 512

#define BACKTRACE() \
{\
        void *frame_buffer[FRAME_BUFFER_SIZE];\
        int sz = backtrace(frame_buffer, FRAME_BUFFER_SIZE);\
        char **frame_str = backtrace_symbols(frame_buffer, sz);\
        if(frame_str) {\
                for(int i = 0; i < sz; ++i) {\
                        printf("%s\n", frame_str[i]);\
                }\
        }\
}

#endif

