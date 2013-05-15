#include <cassert>
#include <sstream>
#include "JSON.h"

JSON::JSON() : type(JSON_NULL) {}
JSON::JSON(std::string s) : type(JSON_STRING) {
    payload.s = new std::string(s);
}
JSON::JSON(const char* s) : type(JSON_STRING) {
    payload.s = new std::string(s);
}
JSON::JSON(bool b) : type(JSON_BOOLEAN) {
    payload.b = b;
}
JSON::JSON(int i) : type(JSON_NUMBER_INT) {
    payload.i = i;
}
JSON::JSON(float f) : type(JSON_NUMBER_FLOAT) {
    payload.f = f;
}

JSON::operator const char *() const {
    return toString().c_str();
}

std::string JSON::toString() const {
    switch (type) {
        case(JSON_NULL): {
            return "null";
        }

        case(JSON_STRING): {
            assert(payload.s);
            return std::string("\"") + *(payload.s) + "\"";
        }

        case(JSON_BOOLEAN): {
            return payload.b ? "true" : "false";
        }

        case(JSON_NUMBER_INT): {
            std::stringstream ss;
            ss << payload.i;
            return ss.str();
        }

        case(JSON_NUMBER_FLOAT): {
            std::stringstream ss;
            ss << payload.f;
            return ss.str();
        }

        case(JSON_ARRAY): {
            std::string result;

            for (std::vector<JSON>::const_iterator i = payload.a->begin(); i != payload.a->end(); ++i) {
                if (i != payload.a->begin()) {
                    result += ", ";
                }
                result += (*i).toString();
            }

            return "[" + result + "]";
        }

        case(JSON_OBJECT): {
            std::string result;

            for (std::map<std::string, JSON>::const_iterator i = payload.o->begin(); i != payload.o->end(); ++i) {
                if (i != payload.o->begin()) {
                    result += ", ";
                }
                result += "\"" + i->first + "\": " + (i->second).toString();
            }

            return "{" + result + "}";
        }
    }
}

void JSON::add(JSON o) {
    if (not (type == JSON_NULL or type == JSON_ARRAY)) {
        assert(false);
    }

    if (type == JSON_NULL) {
        type = JSON_ARRAY;
        payload.a = new std::vector<JSON>;
    }
    
    payload.a->push_back(o);
}

void JSON::add() {
    JSON tmp;
    add(tmp);
}

void JSON::add(std::string s) {
    JSON tmp(s);
    add(tmp);
}

void JSON::add(const char* s) {
    JSON tmp(s);
    add(tmp);
}

void JSON::add(bool b) {
    JSON tmp(b);
    add(tmp);
}

void JSON::add(int i) {
    JSON tmp(i);
    add(tmp);
}

void JSON::add(float f) {
    JSON tmp(f);
    add(tmp);
}


void JSON::add(std::string n, JSON o) {
    if (not (type == JSON_NULL or type == JSON_OBJECT)) {
        assert(false);
    }

    if (type == JSON_NULL) {
        type = JSON_OBJECT;
        payload.o = new std::map<std::string, JSON>;
    }
    
    (*(payload.o))[n] = o;
}

void JSON::add(std::string n, std::string s) {
    JSON tmp(s);
    add(n, tmp);
}

void JSON::add(std::string n, const char* s) {
    JSON tmp(s);
    add(n, tmp);
}

void JSON::add(std::string n, bool b) {
    JSON tmp(b);
    add(n, tmp);
}

void JSON::add(std::string n, int i) {
    JSON tmp(i);
    add(n, tmp);
}

void JSON::add(std::string n, float f) {
    JSON tmp(f);
    add(n, tmp);
}
