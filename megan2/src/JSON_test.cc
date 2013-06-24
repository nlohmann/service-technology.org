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

    J_array[0] = 2;
    std::cout << J_array[2] << std::endl;
    std::cout << J_array.size() << std::endl;

    JSON J_object;
    
    J_object["a"] = "b";
    J_object["b"] = 1.0;
    J_object["c"]["nested"] = true;

    JSON::object t = {"hello", 1};
    JSON J_object2 = t;

    std::cout << J_int << " == " << J_float << ": " << ((J_int == J_float) ? "true" : "false") << '\n';

    std::cout << J_int << '\n';
    std::cout << J_float << '\n';
    std::cout << J_string << '\n';
    std::cout << J_null << '\n';
    std::cout << J_bool << '\n';
    std::cout << J_array << '\n';
    std::cout << J_object << '\n';
    std::cout << J_object2 << '\n';

    JSON foo;
    std::ifstream myFile;
    myFile.open("test.json");
    myFile >> foo;
    std::cout << foo << '\n';

    return 0;
}
