#ifndef MYNEW_H_ 
#define MYNEW_H_ 

#include <new>
#include <string>

void * operator new (size_t size, const std::string &file, int line);
void * operator new[] (size_t size, const std::string &file, int line);

#define new new(__FILE__, __LINE__)

#endif
