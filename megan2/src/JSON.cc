#include "JSON.h"
#include <utility>
#include <stdexcept>
#include <fstream>
#include <cctype>
#include <iostream>

// a mutex to ensure thread safety
std::mutex JSON::token;

/*******************************
 * CONSTRUCTORS AND DESTRUCTOR *
 *******************************/

JSON::JSON(const std::string& s) : type(json_t::string), payload(new std::string(s)) {}
JSON::JSON(const char* s) : type(json_t::string), payload(new std::string(s)) {}
JSON::JSON(const bool b) : type(json_t::boolean), payload(new bool(b)) {}
JSON::JSON(const int i) : type(json_t::number_int), payload(new int(i)) {}
JSON::JSON(const double f) : type(json_t::number_float), payload(new double(f)) {}
JSON::JSON(std::initializer_list<JSON> l) : type(json_t::array), payload(new std::vector<JSON>(l)) {}

/// copy constructor
JSON::JSON(const JSON &o) : type(o.type) {
    switch (type) {
        case (json_t::array): payload = new std::vector<JSON>(*static_cast<std::vector<JSON>*>(o.payload)); break;
        case (json_t::object): payload = new std::map<std::string, JSON>(*static_cast<std::map<std::string, JSON>*>(o.payload)); break;
        case (json_t::string): payload = new std::string(*static_cast<std::string*>(o.payload)); break;
        case (json_t::boolean): payload = new bool(*static_cast<bool*>(o.payload)); break;
        case (json_t::number_int): payload = new int(*static_cast<int*>(o.payload)); break;
        case (json_t::number_float): payload = new double(*static_cast<double*>(o.payload)); break;
        case (json_t::null): break;
    }
}

/// move constructor
JSON::JSON(JSON &&o) : type(std::move(o.type)), payload(std::move(o.payload)) {}

/// copy assignment
JSON & JSON::operator=(JSON o) {
    std::swap(type, o.type);
    std::swap(payload, o.payload);
    return *this;
}

/// destructor
JSON::~JSON() {
    switch (type) {
        case (json_t::array):        delete static_cast<std::vector<JSON>*>(payload); break;
        case (json_t::object):       delete static_cast<std::map<std::string, JSON>*>(payload); break;
        case (json_t::string):       delete static_cast<std::string*>(payload); break;
        case (json_t::boolean):      delete static_cast<bool*>(payload); break;
        case (json_t::number_int):   delete static_cast<int*>(payload); break;
        case (json_t::number_float): delete static_cast<double*>(payload); break;
        case (json_t::null): break;
    }
}


/*****************************
 * OPERATORS AND CONVERSIONS *
 *****************************/

JSON::operator const std::string() const {
    return toString();
}


JSON::operator const int() const {
    switch (type) {
        case (json_t::number_int):
            return *static_cast<int*>(payload);
        case (json_t::number_float):
            return static_cast<int>(*static_cast<double*>(payload));
        default:
            throw std::runtime_error("cannot cast to JSON number");
    }
}

JSON::operator const double() const {
    switch (type) {
        case (json_t::number_int):
            return static_cast<double>(*static_cast<int*>(payload));
        case (json_t::number_float):
            return *static_cast<double*>(payload);
        default:
            throw std::runtime_error("cannot cast to JSON number");
    }
}

JSON::operator const bool() const {
    switch (type) {
        case (json_t::boolean):
            return *static_cast<bool*>(payload);
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
            return std::string("\"") + *static_cast<std::string*>(payload) + "\"";
        }

        case (json_t::boolean): {
            return *static_cast<bool*>(payload) ? "true" : "false";
        }

        case (json_t::number_int): {
            return std::to_string(*static_cast<int*>(payload));
        }

        case (json_t::number_float): {
            return std::to_string(*static_cast<double*>(payload));
        }

        case (json_t::array): {
            std::string result;

            const auto array = static_cast<std::vector<JSON>*>(payload);
            for (auto i = array->begin(); i != array->end(); ++i) {
                if (i != array->begin()) {
                    result += ", ";
                }
                result += (*i).toString();
            }

            return "[" + result + "]";
        }

        case (json_t::object): {
            std::string result;

            const auto object = static_cast<std::map<std::string, JSON>*>(payload);
            for (auto i = object->begin(); i != object->end(); ++i) {
                if (i != object->begin()) {
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

JSON& JSON::operator+=(const JSON& o) {
    std::lock_guard<std::mutex> lg(token);

    if (not(type == json_t::null or type == json_t::array)) {
        throw std::runtime_error("cannot add element to " + _typename());
    }

    if (type == json_t::null) {
        type = json_t::array;
        payload = new std::vector<JSON>;
    }

    static_cast<std::vector<JSON>*>(payload)->push_back(o);

    return *this;
}

JSON& JSON::operator+=(const std::string& s) {
    JSON tmp(s);
    operator+=(tmp);
    return *this;
}

JSON& JSON::operator+=(const char* s) {
    JSON tmp(s);
    operator+=(tmp);
    return *this;
}

JSON& JSON::operator+=(bool b) {
    JSON tmp(b);
    operator+=(tmp);
    return *this;
}

JSON& JSON::operator+=(int i) {
    JSON tmp(i);
    operator+=(tmp);
    return *this;
}

JSON& JSON::operator+=(double f) {
    JSON tmp(f);
    operator+=(tmp);
    return *this;
}

/// operator to set an element in an object
JSON& JSON::operator[](const std::string& key) {
    std::lock_guard<std::mutex> lg(token);

    if (type == json_t::null) {
        type = json_t::object;
        payload = new std::map<std::string, JSON>;
    }

    if (type != json_t::object) {
        throw std::runtime_error("cannot add entry with key " + std::string(key) + " to " + _typename());
    }

    auto object = static_cast<std::map<std::string, JSON>*>(payload);
    if (object->find(key) == object->end()) {
        (*object)[key] = JSON();
    }

    return (*object)[key];
}

/// operator to set an element in an object
JSON& JSON::operator[](const char* key) {
    std::lock_guard<std::mutex> lg(token);

    if (type == json_t::null) {
        type = json_t::object;
        payload = new std::map<std::string, JSON>;
    }

    if (type != json_t::object) {
        throw std::runtime_error("cannot add entry with key " + std::string(key) + " to " + _typename());
    }

    auto object = static_cast<std::map<std::string, JSON>*>(payload);
    if (object->find(key) == object->end()) {
        (*object)[key] = JSON();
    }

    return (*object)[key];
}

/// operator to get an element in an object
const JSON& JSON::operator[](const std::string& key) const {
    if (type != json_t::object) {
        throw std::runtime_error("cannot get entry with key " + std::string(key) + " from " + _typename());
    }

    const auto object = static_cast<std::map<std::string, JSON>*>(payload);
    if (object->find(key) == object->end()) {
        throw std::runtime_error("key " + key + " not found");
    } else {
        return object->find(key)->second;
    }
}

/// return the type as string
std::string JSON::_typename() const {
    switch (type) {
        case(json_t::array):        return "array";
        case(json_t::object):       return "object";
        case(json_t::null):         return "null";
        case(json_t::string):       return "string";
        case(json_t::boolean):      return "boolean";
        case(json_t::number_int):   return "number";
        case(json_t::number_float): return "number";
    }
}


////////////////////////////////////////////////////

/// reports a parse error in case an excpeted token is not present
void JSON::parseError(unsigned int pos, std::string tok) const {
    throw std::runtime_error("parse error: expected " + tok + " at position " + std::to_string(pos));
}


bool JSON::checkDelim(char*& buf, unsigned int& len, unsigned int max, char tok) const {
    // skip whitespace
    while (std::isspace(*buf) && len > 0) { ++buf; --len; }

    // check if next character is given token
    if (len==0 || *buf != tok)
        return false;

    ++buf; --len;
    return true;
}

std::string JSON::getString(char*& buf, unsigned int& len, unsigned int max) const {
    // string should begin with quotes
    if (!checkDelim(buf,len,max,'"'))
        parseError(max-len, "\"");

    // string should end with quotes
    char* aim(strchr(buf,'"'));
    if (!aim) parseError(max-len, "\"");

    // return string
    *aim = 0;
    std::string tmp(buf);
    len -= aim-buf+1;
    buf = aim+1;
    return tmp;	
}

void JSON::parse(char*& buf, unsigned int& len, unsigned int max) {
    // skip whitespace
    while (std::isspace(*buf) && len > 0) { ++buf; --len; }

    // we are not done with parsing yet
    if (len == 0)
        parseError(max, "?");

    char* tmp;
    switch (*buf)
    {
        // parse an object
        // TODO: objects may be empty
        case '{': {
            ++buf; --len;
            do {
                // we first expect a string
                std::string left(getString(buf,len,max));

                // then a colon
                if (!checkDelim(buf,len,max,':'))
                    parseError(max-len, ":");

                // then a value which we store using the first string
                (*this)[left].parse(buf,len,max);
            } while (checkDelim(buf,len,max,','));

            // objects should end with '}'
            if (!checkDelim(buf,len,max,'}'))
                parseError(max-len, "}");
            break;
        }

        // parse an array
        // TODO: arrays may be empty
        case '[': {
            ++buf; --len;
            do {
                // parse the next value
                JSON tmp;
                tmp.parse(buf,len,max);
                operator+=(tmp);
            } while (checkDelim(buf,len,max,','));

            // arrays should end with "]"
            if (!checkDelim(buf,len,max,']'))
                parseError(max-len, "]");
            break;
        }

        // parse a string
        case '"': {
            *this = getString(buf,len,max);
            break;
        }

        // parse a Boolean (true)
        case 't': {
            const auto length_of_true = 4;
            if (strncmp(buf, "true", length_of_true) == 0) {
                buf += length_of_true; len -= length_of_true;
                *this = true;
            } else {
                parseError(max-len, "t");
            }
            break;
        }

        // parse a Boolean (false)
        case 'f': {
            const auto length_of_false = 5;
            if (strncmp(buf, "false", length_of_false) == 0) {
                buf += length_of_false; len -= length_of_false;
                *this = false;
            } else {
                parseError(max-len, "f");
            }
            break;
        }

        // parse null
        case 'n': {
            const auto length_of_null = 4;
            if (strncmp(buf, "null", length_of_null) == 0) {
                buf += length_of_null; len -= length_of_null;
                // nothing to do
            } else {
                parseError(max-len, "n");
            }
            break;
        }

        // parse a number
        default: {
            char* tmp(buf);
            unsigned int tmplen(len);
            while (*tmp != '.' && *tmp != ',' && *tmp != ']' && *tmp != '}' && tmplen > 0) {
                ++tmp; --tmplen;
            }

            if (*tmp == '.')
                *this = strtod(buf, &tmp);
            else
                *this = (int)strtol(buf, &tmp, 10);

            len -= tmp-buf;
            buf = tmp;
        }
    }
}

///parse input into JSON object
void JSON::parseStream(std::istream& is) {
    int pos1 = is.tellg();
    is.seekg(0,is.end);
    int pos2 = is.tellg();
    is.seekg(pos1);

    char* buffer = new char[pos2-pos1+1];
    is.read(buffer,pos2-pos1);
    //is.close();

    unsigned int len(pos2-pos1);	
    char* buf(buffer);
    parse(buf,len,len);

    delete[] buffer;
}

// TODO: http://www.cplusplus.com/reference/istream/istream/get/

/*

*/