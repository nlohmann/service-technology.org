#include "JSON.h"
#include <utility>
#include <stdexcept>

// a mutex to ensure thread safety
std::mutex JSON::token;

/*******************************
 * CONSTRUCTORS AND DESTRUCTOR *
 *******************************/

JSON::JSON(const std::string& s) : type(json_t::string), payload(new std::string(s)) {
}

JSON::JSON(const char* s) : type(json_t::string), payload(new std::string(s)) {
}

JSON::JSON(const bool b) : type(json_t::boolean), payload(new bool(b)) {
}

JSON::JSON(const int i) : type(json_t::number_int), payload(new int(i)) {
}

JSON::JSON(const double f) : type(json_t::number_float), payload(new float(f)) {
}

/// copy constructor
JSON::JSON(const JSON &o) : type(o.type) {
    switch (type) {
        case (json_t::array): payload = new std::vector<JSON>(*(o.getArray())); break;
        case (json_t::object): payload = new std::map<std::string, JSON>(*(o.getObject())); break;
        case (json_t::string): payload = new std::string(*(o.getString())); break;
        case (json_t::boolean): payload = new bool(*(o.getBoolean())); break;
        case (json_t::number_int): payload = new int(*(o.getInteger())); break;
        case (json_t::number_float): payload = new float(*(o.getFloat())); break;
        case (json_t::null): break;
    }
}

/// move constructor
JSON::JSON(JSON &&o) : type(std::move(o.type)), payload(std::move(o.payload)) {
}

/// copy assignment
JSON & JSON::operator=(JSON o) {
    std::swap(type, o.type);
    std::swap(payload, o.payload);
    return *this;
}

/// destructor
JSON::~JSON() {
    switch (type) {
        case (json_t::array):        delete getArray();   break;
        case (json_t::object):       delete getObject();  break;
        case (json_t::string):       delete getString();  break;
        case (json_t::boolean):      delete getBoolean(); break;
        case (json_t::number_int):   delete getInteger(); break;
        case (json_t::number_float): delete getFloat();   break;
        case (json_t::null):                              break;
    }
}

/**************
 * CONVERTERS *
 **************/

std::vector<JSON> * JSON::getArray() {
    return static_cast<std::vector<JSON>*>(payload);
}
std::vector<JSON>* JSON::getArray() const {
    return static_cast<std::vector<JSON>*>(payload);
}

std::map<std::string, JSON> * JSON::getObject() {
    return static_cast<std::map<std::string, JSON>*>(payload);
}
std::map<std::string, JSON> * JSON::getObject() const {
    return static_cast<std::map<std::string, JSON>*>(payload);
}

std::string * JSON::getString() {
    return static_cast<std::string*>(payload);
}
std::string * JSON::getString() const {
    return static_cast<std::string*>(payload);
}

bool * JSON::getBoolean() {
    return static_cast<bool*>(payload);
}
bool * JSON::getBoolean() const {
    return static_cast<bool*>(payload);
}

int * JSON::getInteger() {
    return static_cast<int*>(payload);
}
int * JSON::getInteger() const {
    return static_cast<int*>(payload);
}

double * JSON::getFloat() {
    return static_cast<double*>(payload);
}
double * JSON::getFloat() const {
    return static_cast<double*>(payload);
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
            return *getInteger();
        case (json_t::number_float):
            return static_cast<int>(*getFloat());
        default:
            throw std::runtime_error("cannot cast to JSON number");
    }
}

JSON::operator const double() const {
    switch (type) {
        case (json_t::number_int):
            return static_cast<double>(*getInteger());
        case (json_t::number_float):
            return *getFloat();
        default:
            throw std::runtime_error("cannot cast to JSON number");
    }
}

JSON::operator const bool() const {
    switch (type) {
        case (json_t::boolean):
            return *getBoolean();
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
            return std::string("\"") + *getString() + "\"";
        }

        case (json_t::boolean): {
            return *getBoolean() ? "true" : "false";
        }

        case (json_t::number_int): {
            return std::to_string(*getInteger());
        }

        case (json_t::number_float): {
            return std::to_string(*getFloat());
        }

        case (json_t::array): {
            std::string result;

            for (auto i = getArray()->begin(); i != getArray()->end(); ++i) {
                if (i != getArray()->begin()) {
                    result += ", ";
                }
                result += (*i).toString();
            }

            return "[" + result + "]";
        }

        case (json_t::object): {
            std::string result;

            for (auto i = getObject()->begin(); i != getObject()->end(); ++i) {
                if (i != getObject()->begin()) {
                    result += ", ";
                }
                result += "\"" + i->first + "\": " + (i->second).toString();
            }

            return "{" + result + "}";
        }
    }
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
        payload = new std::vector<JSON>;
    }

    getArray()->push_back(o);
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
        payload = new std::map<std::string, JSON>;
    }

    (*getObject())[n] = o;
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
        payload = new std::map<std::string, JSON>;
    }

    if (getObject()->find(key) == getObject()->end()) {
        (*(getObject()))[key] = JSON();
    }

    return (*(getObject()))[key];
}

/// operator to set an element in an object
JSON& JSON::operator[](const char* key) {
    std::lock_guard<std::mutex> lg(token);

    if (type == json_t::null) {
        type = json_t::object;
        payload = new std::map<std::string, JSON>;
    }

    if (getObject()->find(key) == getObject()->end()) {
        (*(getObject()))[key] = JSON();
    }

    return (*(getObject()))[key];
}

/// operator to get an element in an object
const JSON& JSON::operator[](const std::string& key) const {
    if (getObject()->find(key) == getObject()->end()) {
        throw std::runtime_error("key " + key + " not found");
    } else {
        return getObject()->find(key)->second;
    }
}
