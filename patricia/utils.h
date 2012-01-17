#ifndef UTILS_H
#define UTILS_H

// comment the next line out to disable debugging
#define DEBUG

#ifdef DEBUG
#define debug_print(...) do{ fprintf( stderr, __VA_ARGS__ ); } while(0)
#else
#define debug_print(...) do { } while(0)
#endif

char* append(char* s1, char* s2);
void killProcess(void* arg);
void killThread(void* arg);
void killThreads(void* arg);

#endif
