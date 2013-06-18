#include "JSON.h"

// a mutex to ensure thread safety
std::mutex JSON::token;

/*******************************
 * CONSTRUCTORS AND DESTRUCTOR *
 *******************************/

JSON::JSON() : type(json_t::null) {
}

JSON::JSON(const std::string& s) : type(json_t::string) {
    payload.s = new std::string(s);
}

JSON::JSON(const char* s) : type(json_t::string) {
    payload.s = new std::string(s);
}

JSON::JSON(const bool b) : type(json_t::boolean) {
    payload.b = b;
}

JSON::JSON(const int i) : type(json_t::number_int) {
    payload.i = i;
}

JSON::JSON(const double f) : type(json_t::number_float) {
    payload.f = f;
}

JSON::~JSON() {
    switch (type) {
        case (json_t::array): {
            delete payload.a;
            break;
        }
        case (json_t::object): {
            delete payload.o;
            break;
        }
        case (json_t::string): {
            //delete payload.s;
            break;
        }
        default: {
            break;
        }
    }
}


/*****************************
 * OPERATORS AND CONVERSIONS *
 *****************************/

JSON::operator const char* () const {
    return toString().c_str();
}

JSON::operator const std::string() const {
    return toString();
}


JSON::operator const int() const {
    switch (type) {
        case (json_t::number_int):
            return payload.i;
        case (json_t::number_float):
            return static_cast<int>(payload.f);
        default:
            throw std::runtime_error("cannot cast to JSON number");
    }
}

JSON::operator const double() const {
    switch (type) {
        case (json_t::number_int):
            return static_cast<double>(payload.i);
        case (json_t::number_float):
            return payload.f;
        default:
            throw std::runtime_error("cannot cast to JSON number");
    }
}

JSON::operator const bool() const {
    switch (type) {
        case (json_t::boolean):
            return payload.b;
        default:
            throw std::runtime_error("cannot cast to JSON Boolean");
    }
}

const std::string JSON::toString() const {
    switch (type) {
        case (json_t::null): {
            return "null";
        }

        case (json_t::string): {
            return std::string("\"") + *(payload.s) + "\"";
        }

        case (json_t::boolean): {
            return payload.b ? "true" : "false";
        }

        case (json_t::number_int): {
            return std::to_string(payload.i);
        }

        case (json_t::number_float): {
            return std::to_string(payload.f);
        }

        case (json_t::array): {
            std::string result;

            for (auto i = payload.a->begin(); i != payload.a->end(); ++i) {
                if (i != payload.a->begin()) {
                    result += ", ";
                }
                result += (*i).toString();
            }

            return "[" + result + "]";
        }

        case (json_t::object): {
            std::string result;

            for (auto i = payload.o->begin(); i != payload.o->end(); ++i) {
                if (i != payload.o->begin()) {
                    result += ", ";
                }
                result += "\"" + i->first + "\": " + (i->second).toString();
            }

            return "{" + result + "}";
        }
    }
}

const char* JSON::c_str() const {
    return toString().c_str();
}


/*****************************************
 * ADDING ELEMENTS TO OBJECTS AND ARRAYS *
 *****************************************/

void JSON::add(JSON &o) {
    std::lock_guard<std::mutex> lg(token);

    if (not(type == json_t::null or type == json_t::array)) {
        throw std::runtime_error("cannot add element to primitive type");
    }

    if (type == json_t::null) {
        type = json_t::array;
        payload.a = new std::vector<JSON>;
    }

    payload.a->push_back(o);
}

void JSON::add(const std::string& s) {
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


void JSON::add(std::string n, JSON &o) {
    std::lock_guard<std::mutex> lg(token);

    if (not(type == json_t::null or type == json_t::object)) {
        throw std::runtime_error("cannot add element to primitive type");
    }

    if (type == json_t::null) {
        type = json_t::object;
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
JSON& JSON::operator[](const std::string& key) {
    std::lock_guard<std::mutex> lg(token);

    if (type == json_t::null) {
        type = json_t::object;
        payload.o = new std::map<std::string, JSON>;
    }

    if (payload.o->find(key) == payload.o->end()) {
        (*(payload.o))[key] = JSON();
    }

    return (*(payload.o))[key];
}

/// operator to set an element in an object
JSON& JSON::operator[](const char* key) {
    std::lock_guard<std::mutex> lg(token);

    if (type == json_t::null) {
        type = json_t::object;
        payload.o = new std::map<std::string, JSON>;
    }

    if (payload.o->find(key) == payload.o->end()) {
        (*(payload.o))[key] = JSON();
    }

    return (*(payload.o))[key];
}

/// operator to get an element in an object
const JSON& JSON::operator[](const std::string& key) const {
    if (payload.o->find(key) == payload.o->end()) {
        throw std::runtime_error("key " + key + " not found");
    } else {
        return payload.o->find(key)->second;
    }
}
