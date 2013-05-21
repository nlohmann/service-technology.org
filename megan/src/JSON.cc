#include <sstream>
#include <config.h>
#include "JSON.h"

JSON::JSON() : type(JSON_NULL) {
}

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

JSON::JSON(double f) : type(JSON_NUMBER_FLOAT) {
    payload.f = f;
}

JSON::JSON(const JSON& other) : type(other.type) {
    switch (other.type) {
        case (JSON_ARRAY): {
            payload.a = new std::vector<JSON>(*(other.payload.a));
            break;
        }

        case (JSON_OBJECT): {
            payload.o = new std::map<std::string, JSON>(*(other.payload.o));
            break;
        }

        case (JSON_STRING): {
            payload.s = new std::string(*(other.payload.s));
            break;
        }

        default: {
            // payloads without constructor can be copied straightforardly
            payload = other.payload;
            break;
        }
    }
}

JSON::operator const char* () const {
    return toString().c_str();
}

JSON::operator const std::string() const {
    return toString();
}


JSON::operator const int() const {
    switch (type) {
        case (JSON_NUMBER_INT):
            return payload.i;
        case (JSON_NUMBER_FLOAT):
            return static_cast<int>(payload.f);
        default:
            throw std::runtime_error("cannot cast to JSON number");
    }
}

JSON::operator const double() const {
    switch (type) {
        case (JSON_NUMBER_INT):
            return static_cast<double>(payload.i);
        case (JSON_NUMBER_FLOAT):
            return payload.f;
        default:
            throw std::runtime_error("cannot cast to JSON number");
    }
}

JSON::operator const bool() const {
    switch (type) {
        case (JSON_BOOLEAN):
            return payload.b;
        default:
            throw std::runtime_error("cannot cast to JSON Boolean");
    }
}

const std::string JSON::toString() const {
    switch (type) {
        case (JSON_NULL): {
            return "null";
        }

        case (JSON_STRING): {
            return std::string("\"") + *(payload.s) + "\"";
        }

        case (JSON_BOOLEAN): {
            return payload.b ? "true" : "false";
        }

        case (JSON_NUMBER_INT): {
            std::stringstream ss;
            ss << payload.i;
            return ss.str();
        }

        case (JSON_NUMBER_FLOAT): {
            std::stringstream ss;
            ss << payload.f;
            return ss.str();
        }

        case (JSON_ARRAY): {
            std::string result;

            for (std::vector<JSON>::const_iterator i = payload.a->begin(); i != payload.a->end(); ++i) {
                if (i != payload.a->begin()) {
                    result += ", ";
                }
                result += (*i).toString();
            }

            return "[" + result + "]";
        }

        case (JSON_OBJECT): {
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
    if (not(type == JSON_NULL or type == JSON_ARRAY)) {
        throw std::runtime_error("cannot add element to primitive type");
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

void JSON::add(double f) {
    JSON tmp(f);
    add(tmp);
}


void JSON::add(std::string n, JSON o) {
    if (not(type == JSON_NULL or type == JSON_OBJECT)) {
        throw std::runtime_error("cannot add element to primitive type");
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

void JSON::add(std::string n, double f) {
    JSON tmp(f);
    add(n, tmp);
}


/// operator to set an element in an object
JSON & JSON::operator[](const std::string & key) {
    if (type == JSON_NULL) {
        type = JSON_OBJECT;
        payload.o = new std::map<std::string, JSON>;
    }

    if (payload.o->find(key) == payload.o->end()) {
        (*(payload.o))[key] = JSON();
    }

    return (*(payload.o))[key];
}

/// operator to set an element in an object
JSON & JSON::operator[](const char *key) {
    if (type == JSON_NULL) {
        type = JSON_OBJECT;
        payload.o = new std::map<std::string, JSON>;
    }

    if (payload.o->find(key) == payload.o->end()) {
        (*(payload.o))[key] = JSON();
    }

    return (*(payload.o))[key];
}

/// operator to get an element in an object
const JSON & JSON::operator[](const std::string & key) const {
    if (payload.o->find(key) == payload.o->end()) {
        throw std::runtime_error("key " + key + " not found");
    } else {
        return payload.o->find(key)->second;
    }
}
