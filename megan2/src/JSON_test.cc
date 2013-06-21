#include <iostream>
#include <fstream>
#include <cstdio>
#include "JSON.h"

int main() {
    JSON J_int = 1;
    JSON J_float = 1.0;
    JSON J_string = "Hello, world!";
    JSON J_null;
    JSON J_bool = false;
    JSON J_array = {1, 1.0, "Hello, world", false};

    J_array += JSON("additional element");

    JSON J_object;
    
    J_object["a"] = "b";
    J_object["b"] = 1.0;
    J_object["c"]["nested"] = true;

    //printf("%s", J_array);

    std::cout << J_int << '\n';
    std::cout << J_float << '\n';
    std::cout << J_string << '\n';
    std::cout << J_null << '\n';
    std::cout << J_bool << '\n';
    std::cout << J_array << '\n';
    std::cout << J_object << '\n';

    JSON foo;
    std::ifstream myFile;
    myFile.open("test.json");
    myFile >> foo;
    std::cout << foo << '\n';

    return 0;
}
