#include <iostream>

#include "test.h"


void begin_test(const std::string & msg)
{
  std::cout << "Testing " << msg << " ... " << std::flush;
}

void end_test()
{
  std::cout << "Ok" << std::endl;
}
