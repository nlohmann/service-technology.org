#include <iostream>
#include <fstream>
#include <cstdio>
#include <cassert>
#include <JSON.h>

void test_null() {
    /* a null object */

    // construct
    JSON a, b;

    // copy assign
    b = JSON();

    // copy construct
    JSON c(a);
    
    // copy construct
    JSON d = a;

    // assign operator
    JSON e = JSON();

    // compare
    assert (a == b);

    // type
    assert (a.type() == JSON::null);

    // empty and size
    assert (a.size() == 0);
    assert (a.empty() == true);

    // output
    std::cout << a << '\n';

    // string represetations
    assert(a.toString() == std::string("null"));

    // invalid conversion to int
    try {
        int i = 0;
        i = a;
        assert(false);
    } catch(const std::exception& ex) {
        assert (ex.what() == std::string("cannot cast null to JSON number"));
    }

    // invalid conversion to double
    try {
        double f = 0;
        f = a;
        assert(false);
    } catch(const std::exception& ex) {
        assert (ex.what() == std::string("cannot cast null to JSON number"));
    }

    // invalid conversion to bool
    try {
        bool b = false;
        b = a;
        assert(false);
    } catch(const std::exception& ex) {
        assert (ex.what() == std::string("cannot cast null to JSON Boolean"));
    }

    // invalid conversion to string
    try {
        std::string s = a;
        assert(false);
    } catch(const std::exception& ex) {
        assert (ex.what() == std::string("cannot cast null to JSON string"));
    }
}

void test_string() {
    /* a string object */

    // construct
    JSON a = "object a";
    JSON b;

    // copy assign
    b = JSON("object a");

    // copy construct
    JSON c(a);
    
    // copy construct
    JSON d = a;

    // assign operator
    JSON e = JSON("");

    // compare
    assert (a == b);

    // type
    assert (a.type() == JSON::string);

    // empty and size
    assert (a.size() == 1);
    assert (a.empty() == false);

    // output
    std::cout << a << '\n';

    // string represetations
    assert(a.toString() == std::string("\"object a\""));

    // invalid conversion to int
    try {
        int i = 0;
        i = a;
        assert(false);
    } catch(const std::exception& ex) {
        assert (ex.what() == std::string("cannot cast string to JSON number"));
    }

    // invalid conversion to double
    try {
        double f = 0;
        f = a;
        assert(false);
    } catch(const std::exception& ex) {
        assert (ex.what() == std::string("cannot cast string to JSON number"));
    }

    // invalid conversion to bool
    try {
        bool b = false;
        b = a;
        assert(false);
    } catch(const std::exception& ex) {
        assert (ex.what() == std::string("cannot cast string to JSON Boolean"));
    }
}

void test_array() {
    JSON a;
    a += JSON();
    a += 1;
    a += 1.0;
    a += true;
    a += "string";

    // type
    assert (a.type() == JSON::array);

    // empty and size
    assert (a.size() == 5);
    assert (a.empty() == false);

    // output
    std::cout << a << '\n';
}

int main() {
    test_null();
    test_string();

    test_array();

    /*
    
    JSON J_int = 1;
    JSON J_float = 1.0;
    JSON J_string = "Hello, world!";
    JSON J_null;
    JSON J_bool = false;

#ifdef __cplusplus11
    JSON J_array = {1, 1.0, "Hello, world", false};

    J_array += JSON("additional element");

    J_array[0] = 2;
    std::cout << J_array[2] << std::endl;
    std::cout << J_array.size() << std::endl;

    std::cout << J_array << '\n';
#endif

    JSON J_object;
    
    J_object["a"] = "b";
    J_object["b"] = 1.0;
    J_object["c"]["nested"] = true;

#ifdef __cplusplus11
    JSON::object t = {"hello", 1};
    JSON J_object2 = t;

    std::cout << J_object << '\n';
    std::cout << J_object2 << '\n';
#endif

    std::cout << J_int << " == " << J_float << ": " << ((J_int == J_float) ? "true" : "false") << '\n';

    std::cout << J_int << '\n';
    std::cout << J_float << '\n';
    std::cout << J_string << '\n';
    std::cout << J_null << '\n';
    std::cout << J_bool << '\n';
*/
    /*
    for(auto e : J_array) {
        std::cout << e << '\n';
    }*/
  /*  
    {
    JSON a;
    a["foo"]["bar"] = "foo";
    a["foo"]["baz"] = false;
    
    std::cout << " a = " << a << '\n';
    
    JSON b;
    b["test"] = a["foo"];

    std::cout << " b = " << b << '\n';
    }
*/    
    /*
    JSON foo;
    std::ifstream myFile;
    myFile.open("test.json");
    myFile >> foo;
    std::cout << foo << '\n';
*/
    return 0;
}
