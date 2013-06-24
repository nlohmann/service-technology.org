#include "JSON.h"
#include <utility>
#include <stdexcept>
#include <fstream>
#include <cctype>
#include <iostream>

// a mutex to ensure thread safety
std::mutex JSON::_token;

/*******************************
 * CONSTRUCTORS AND DESTRUCTOR *
 *******************************/

JSON::JSON(const std::string& s) : _type(json_t::string), _payload(new std::string(s)) {}
JSON::JSON(const char* s) : _type(json_t::string), _payload(new std::string(s)) {}
JSON::JSON(const bool b) : _type(json_t::boolean), _payload(new bool(b)) {}
JSON::JSON(const int i) : _type(json_t::number_int), _payload(new int(i)) {}
JSON::JSON(const double f) : _type(json_t::number_float), _payload(new double(f)) {}
JSON::JSON(array a) : _type(json_t::array), _payload(new std::vector<JSON>(a)) {}

JSON::JSON(object o) : _type(json_t::object), _payload(new std::map<std::string, JSON>) {
    (*this)[std::get<0>(o)] = std::get<1>(o);
}


/// copy constructor
JSON::JSON(const JSON &o) : _type(o._type) {
    switch (_type) {
        case (json_t::array): _payload = new std::vector<JSON>(*static_cast<std::vector<JSON>*>(o._payload)); break;
        case (json_t::object): _payload = new std::map<std::string, JSON>(*static_cast<std::map<std::string, JSON>*>(o._payload)); break;
        case (json_t::string): _payload = new std::string(*static_cast<std::string*>(o._payload)); break;
        case (json_t::boolean): _payload = new bool(*static_cast<bool*>(o._payload)); break;
        case (json_t::number_int): _payload = new int(*static_cast<int*>(o._payload)); break;
        case (json_t::number_float): _payload = new double(*static_cast<double*>(o._payload)); break;
        case (json_t::null): break;
    }
}

/// move constructor
JSON::JSON(JSON &&o) : _type(std::move(o._type)), _payload(std::move(o._payload)) {}

/// copy assignment
JSON & JSON::operator=(JSON o) {
    std::swap(_type, o._type);
    std::swap(_payload, o._payload);
    return *this;
}

/// destructor
JSON::~JSON() {
    switch (_type) {
        case (json_t::array):        delete static_cast<std::vector<JSON>*>(_payload); break;
        case (json_t::object):       delete static_cast<std::map<std::string, JSON>*>(_payload); break;
        case (json_t::string):       delete static_cast<std::string*>(_payload); break;
        case (json_t::boolean):      delete static_cast<bool*>(_payload); break;
        case (json_t::number_int):   delete static_cast<int*>(_payload); break;
        case (json_t::number_float): delete static_cast<double*>(_payload); break;
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
    switch (_type) {
        case (json_t::number_int):
            return *static_cast<int*>(_payload);
        case (json_t::number_float):
            return static_cast<int>(*static_cast<double*>(_payload));
        default:
            throw std::runtime_error("cannot cast to JSON number");
    }
}

JSON::operator const double() const {
    switch (_type) {
        case (json_t::number_int):
            return static_cast<double>(*static_cast<int*>(_payload));
        case (json_t::number_float):
            return *static_cast<double*>(_payload);
        default:
            throw std::runtime_error("cannot cast to JSON number");
    }
}

JSON::operator const bool() const {
    switch (_type) {
        case (json_t::boolean):
            return *static_cast<bool*>(_payload);
        default:
            throw std::runtime_error("cannot cast to JSON Boolean");
    }
}

const std::string JSON::toString() const {
    switch (_type) {
        case (json_t::null): {
            return "null";
        }

        case (json_t::string): {
            return std::string("\"") + *static_cast<std::string*>(_payload) + "\"";
        }

        case (json_t::boolean): {
            return *static_cast<bool*>(_payload) ? "true" : "false";
        }

        case (json_t::number_int): {
            return std::to_string(*static_cast<int*>(_payload));
        }

        case (json_t::number_float): {
            return std::to_string(*static_cast<double*>(_payload));
        }

        case (json_t::array): {
            std::string result;

            const auto array = static_cast<std::vector<JSON>*>(_payload);
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

            const auto object = static_cast<std::map<std::string, JSON>*>(_payload);
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
    std::lock_guard<std::mutex> lg(_token);

    if (not(_type == json_t::null or _type == json_t::array)) {
        throw std::runtime_error("cannot add element to " + _typename());
    }

    if (_type == json_t::null) {
        _type = json_t::array;
        _payload = new std::vector<JSON>;
    }

    static_cast<std::vector<JSON>*>(_payload)->push_back(o);

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
JSON& JSON::operator[](int index) {
    std::lock_guard<std::mutex> lg(_token);

    if (_type != json_t::array) {
        throw std::runtime_error("cannot add entry with index " + std::to_string(index) + " to " + _typename());
    }

    auto array = static_cast<std::vector<JSON>*>(_payload);
    return array->at(index);
}

/// operator to get an element in an object
const JSON& JSON::operator[](const int index) const {
    if (_type != json_t::array) {
        throw std::runtime_error("cannot get entry with index " + std::to_string(index) + " from " + _typename());
    }

    auto array = static_cast<std::vector<JSON>*>(_payload);
    return array->at(index);
}

/// operator to set an element in an object
JSON& JSON::operator[](const std::string& key) {
    std::lock_guard<std::mutex> lg(_token);

    if (_type == json_t::null) {
        _type = json_t::object;
        _payload = new std::map<std::string, JSON>;
    }

    if (_type != json_t::object) {
        throw std::runtime_error("cannot add entry with key " + std::string(key) + " to " + _typename());
    }

    auto object = static_cast<std::map<std::string, JSON>*>(_payload);
    if (object->find(key) == object->end()) {
        (*object)[key] = JSON();
    }

    return (*object)[key];
}

/// operator to set an element in an object
JSON& JSON::operator[](const char* key) {
    std::lock_guard<std::mutex> lg(_token);

    if (_type == json_t::null) {
        _type = json_t::object;
        _payload = new std::map<std::string, JSON>;
    }

    if (_type != json_t::object) {
        throw std::runtime_error("cannot add entry with key " + std::string(key) + " to " + _typename());
    }

    auto object = static_cast<std::map<std::string, JSON>*>(_payload);
    if (object->find(key) == object->end()) {
        (*object)[key] = JSON();
    }

    return (*object)[key];
}

/// operator to get an element in an object
const JSON& JSON::operator[](const std::string& key) const {
    if (_type != json_t::object) {
        throw std::runtime_error("cannot get entry with key " + std::string(key) + " from " + _typename());
    }

    const auto object = static_cast<std::map<std::string, JSON>*>(_payload);
    if (object->find(key) == object->end()) {
        throw std::runtime_error("key " + key + " not found");
    } else {
        return object->find(key)->second;
    }
}

/// return the number of stored values
size_t JSON::size() const {
    switch (_type) {
        case(json_t::array):        return static_cast<std::vector<JSON>*>(_payload)->size();
        case(json_t::object):       return static_cast<std::map<std::string, JSON>*>(_payload)->size();
        case(json_t::null):         return 0;
        case(json_t::string):       return 1;
        case(json_t::boolean):      return 1;
        case(json_t::number_int):   return 1;
        case(json_t::number_float): return 1;
    }
}

/// checks whether object is empty
bool JSON::empty() const {
    switch (_type) {
        case(json_t::array):        return static_cast<std::vector<JSON>*>(_payload)->empty();
        case(json_t::object):       return static_cast<std::map<std::string, JSON>*>(_payload)->empty();
        case(json_t::null):         return true;
        case(json_t::string):       return false;
        case(json_t::boolean):      return false;
        case(json_t::number_int):   return false;
        case(json_t::number_float): return false;
    }
}

/// return the type of the object
JSON::json_t JSON::type() const {
    return _type;
}

/// lexicographically compares the values
bool JSON::operator==(const JSON &o) const {
    switch (_type) {
        case(json_t::array): {
            if (o._type == json_t::array) {
                auto a = static_cast<std::vector<JSON>*>(_payload);
                auto b = static_cast<std::vector<JSON>*>(o._payload);
                return *a == *b;
            }
        }
        case(json_t::object): {
            if (o._type == json_t::object) {
                auto a = static_cast<std::map<std::string, JSON>*>(_payload);
                auto b = static_cast<std::map<std::string, JSON>*>(o._payload);
                return *a == *b;
            }
        }
        case(json_t::null): {
            if (o._type == json_t::null) {
                return true;
            }
        }
        case(json_t::string): {
            if (o._type == json_t::string) {
                const std::string a = *this;
                const std::string b = o;
                return a == b;
            }
        }
        case(json_t::boolean): {
            if (o._type == json_t::boolean) {
                bool a = *this;
                bool b = o;
                return a == b;
            }
        }
        case(json_t::number_int): {
            if (o._type == json_t::number_int or o._type == json_t::number_float) {
                int a = *this;
                int b = o;
                return a == b;
            }
        }
        case(json_t::number_float): {
            if (o._type == json_t::number_int or o._type == json_t::number_float) {
                double a = *this;
                double b = o;
                return a == b;
            }
        }
    }

    return false;
}



/// return the type as string
std::string JSON::_typename() const {
    switch (_type) {
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


bool JSON::checkDelim(char*& buf, unsigned int& len, char tok) const {
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
    if (!checkDelim(buf,len,'"'))
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
                if (!checkDelim(buf,len,':'))
                    parseError(max-len, ":");

                // then a value which we store using the first string
                (*this)[left].parse(buf,len,max);
            } while (checkDelim(buf,len,','));

            // objects should end with '}'
            if (!checkDelim(buf,len,'}'))
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
            } while (checkDelim(buf,len,','));

            // arrays should end with "]"
            if (!checkDelim(buf,len,']'))
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