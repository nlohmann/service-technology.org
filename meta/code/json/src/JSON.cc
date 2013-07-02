#include "JSON.h"

#include <utility>
#include <stdexcept>
#include <fstream>
#include <cctype>
#include <iostream>
#include <streambuf>
#include <sstream>

#ifndef __cplusplus11
#include <cstring>
#include <cstdlib>
#endif

#ifdef __cplusplus11
using std::to_string;
#else

inline std::string to_string(double f) {
    std::stringstream s;
    s << f;
    return s.str();
}
#endif

/******************
 * STATIC MEMBERS *
 ******************/

#ifdef __cplusplus11
/// a mutex to ensure thread safety
std::mutex JSON::_token;
#endif

/*******************************
 * CONSTRUCTORS AND DESTRUCTOR *
 *******************************/

JSON::JSON() : _type(null), _payload(nullptr) {}
JSON::JSON(const std::string& s) : _type(string), _payload(new std::string(s)) {}
JSON::JSON(const char* s) : _type(string), _payload(new std::string(s)) {}
JSON::JSON(char* s) : _type(string), _payload(new std::string(s)) {}
JSON::JSON(const bool b) : _type(boolean), _payload(new bool(b)) {}
JSON::JSON(const int i) : _type(number_int), _payload(new int(i)) {}
JSON::JSON(const double f) : _type(number_float), _payload(new double(f)) {}

#ifdef __cplusplus11
JSON::JSON(array_t a) : _type(array), _payload(new std::vector<JSON>(a)) {}

JSON::JSON(object_t o) : _type(object), _payload(new std::map<std::string, JSON>) {
    (*this)[std::get<0>(o)] = std::get<1>(o);
}
#endif

/// copy constructor
JSON::JSON(const JSON& o) : _type(o._type) {
    switch (_type) {
        case (array):
            _payload = new std::vector<JSON>(*static_cast<std::vector<JSON>*>(o._payload));
            break;
        case (object):
            _payload = new std::map<std::string, JSON>(*static_cast<std::map<std::string, JSON>*>(o._payload));
            break;
        case (string):
            _payload = new std::string(*static_cast<std::string*>(o._payload));
            break;
        case (boolean):
            _payload = new bool(*static_cast<bool*>(o._payload));
            break;
        case (number_int):
            _payload = new int(*static_cast<int*>(o._payload));
            break;
        case (number_float):
            _payload = new double(*static_cast<double*>(o._payload));
            break;
        case (null):
            break;
    }
}

#ifdef __cplusplus11
/// move constructor
JSON::JSON(JSON&& o) : _type(std::move(o._type)), _payload(std::move(o._payload)) {}
#endif

/// copy assignment
#ifdef __cplusplus11
JSON& JSON::operator=(JSON o) {
    std::swap(_type, o._type);
    std::swap(_payload, o._payload);
    return *this;
}
#else
JSON& JSON::operator=(const JSON& o) {
    _type = o._type;
    switch (_type) {
        case (array):
            _payload = new std::vector<JSON>(*static_cast<std::vector<JSON>*>(o._payload));
            break;
        case (object):
            _payload = new std::map<std::string, JSON>(*static_cast<std::map<std::string, JSON>*>(o._payload));
            break;
        case (string):
            _payload = new std::string(*static_cast<std::string*>(o._payload));
            break;
        case (boolean):
            _payload = new bool(*static_cast<bool*>(o._payload));
            break;
        case (number_int):
            _payload = new int(*static_cast<int*>(o._payload));
            break;
        case (number_float):
            _payload = new double(*static_cast<double*>(o._payload));
            break;
        case (null):
            break;
    }

    return *this;
}
#endif

/// destructor
JSON::~JSON() {
    switch (_type) {
        case (array):
            delete static_cast<std::vector<JSON>*>(_payload);
            break;
        case (object):
            delete static_cast<std::map<std::string, JSON>*>(_payload);
            break;
        case (string):
            delete static_cast<std::string*>(_payload);
            break;
        case (boolean):
            delete static_cast<bool*>(_payload);
            break;
        case (number_int):
            delete static_cast<int*>(_payload);
            break;
        case (number_float):
            delete static_cast<double*>(_payload);
            break;
        case (null):
            break;
    }
}


/*****************************
 * OPERATORS AND CONVERSIONS *
 *****************************/

JSON::operator const std::string() const {
    switch (_type) {
        case (string):
            return *static_cast<std::string*>(_payload);
        default:
            throw std::runtime_error("cannot cast " + _typename() + " to JSON string");
    }
}


JSON::operator int() const {
    switch (_type) {
        case (number_int):
            return *static_cast<int*>(_payload);
        case (number_float):
            return static_cast<int>(*static_cast<double*>(_payload));
        default:
            throw std::runtime_error("cannot cast " + _typename() + " to JSON number");
    }
}

JSON::operator double() const {
    switch (_type) {
        case (number_int):
            return static_cast<double>(*static_cast<int*>(_payload));
        case (number_float):
            return *static_cast<double*>(_payload);
        default:
            throw std::runtime_error("cannot cast " + _typename() + " to JSON number");
    }
}

JSON::operator bool() const {
    switch (_type) {
        case (boolean):
            return *static_cast<bool*>(_payload);
        default:
            throw std::runtime_error("cannot cast " + _typename() + " to JSON Boolean");
    }
}

const std::string JSON::toString() const {
    switch (_type) {
        case (null): {
            return "null";
        }

        case (string): {
            return std::string("\"") + *static_cast<std::string*>(_payload) + "\"";
        }

        case (boolean): {
            return *static_cast<bool*>(_payload) ? "true" : "false";
        }

        case (number_int): {
            return to_string(*static_cast<int*>(_payload));
        }

        case (number_float): {
            return to_string(*static_cast<double*>(_payload));
        }

        case (array): {
            std::string result;

            const std::vector<JSON>* array = static_cast<std::vector<JSON>*>(_payload);
            for (std::vector<JSON>::const_iterator i = array->begin(); i != array->end(); ++i) {
                if (i != array->begin()) {
                    result += ", ";
                }
                result += (*i).toString();
            }

            return "[" + result + "]";
        }

        case (object): {
            std::string result;

            const std::map<std::string, JSON>* object = static_cast<std::map<std::string, JSON>*>(_payload);
            for (std::map<std::string, JSON>::const_iterator i = object->begin(); i != object->end(); ++i) {
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
#ifdef __cplusplus11
    std::lock_guard<std::mutex> lg(_token);
#endif

    if (not(_type == null or _type == array)) {
        throw std::runtime_error("cannot add element to " + _typename());
    }

    if (_type == null) {
        _type = array;
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
#ifdef __cplusplus11
    std::lock_guard<std::mutex> lg(_token);
#endif

    if (_type != array) {
        throw std::runtime_error("cannot add entry with index " + to_string(index) + " to " + _typename());
    }

    std::vector<JSON>* array = static_cast<std::vector<JSON>*>(_payload);

    if (index >= array->size()) {
        throw std::runtime_error("cannot access element at index " + to_string(index));
    }

    return array->at(index);
}

/// operator to get an element in an object
const JSON& JSON::operator[](const int index) const {
    if (_type != array) {
        throw std::runtime_error("cannot get entry with index " + to_string(index) + " from " + _typename());
    }

    std::vector<JSON>* array = static_cast<std::vector<JSON>*>(_payload);

    if (index >= array->size()) {
        throw std::runtime_error("cannot access element at index " + to_string(index));
    }

    return array->at(index);
}

/// operator to set an element in an object
JSON& JSON::operator[](const std::string& key) {
#ifdef __cplusplus11
    std::lock_guard<std::mutex> lg(_token);
#endif

    if (_type == null) {
        _type = object;
        _payload = new std::map<std::string, JSON>;
    }

    if (_type != object) {
        throw std::runtime_error("cannot add entry with key " + std::string(key) + " to " + _typename());
    }

    std::map<std::string, JSON>* object = static_cast<std::map<std::string, JSON>*>(_payload);
    if (object->find(key) == object->end()) {
        (*object)[key] = JSON();
    }

    return (*object)[key];
}

/// operator to set an element in an object
JSON& JSON::operator[](const char* key) {
#ifdef __cplusplus11
    std::lock_guard<std::mutex> lg(_token);
#endif

    if (_type == null) {
        _type = object;
        _payload = new std::map<std::string, JSON>;
    }

    if (_type != object) {
        throw std::runtime_error("cannot add entry with key " + std::string(key) + " to " + _typename());
    }

    std::map<std::string, JSON>* object = static_cast<std::map<std::string, JSON>*>(_payload);
    if (object->find(key) == object->end()) {
        (*object)[key] = JSON();
    }

    return (*object)[key];
}

/// operator to get an element in an object
const JSON& JSON::operator[](const std::string& key) const {
    if (_type != object) {
        throw std::runtime_error("cannot get entry with key " + std::string(key) + " from " + _typename());
    }

    const std::map<std::string, JSON>* object = static_cast<std::map<std::string, JSON>*>(_payload);
    if (object->find(key) == object->end()) {
        throw std::runtime_error("key " + key + " not found");
    } else {
        return object->find(key)->second;
    }
}

/// return the number of stored values
size_t JSON::size() const {
    switch (_type) {
        case (array):
            return static_cast<std::vector<JSON>*>(_payload)->size();
        case (object):
            return static_cast<std::map<std::string, JSON>*>(_payload)->size();
        case (null):
            return 0;
        case (string):
            return 1;
        case (boolean):
            return 1;
        case (number_int):
            return 1;
        case (number_float):
            return 1;
    }
}

/// checks whether object is empty
bool JSON::empty() const {
    switch (_type) {
        case (array):
            return static_cast<std::vector<JSON>*>(_payload)->empty();
        case (object):
            return static_cast<std::map<std::string, JSON>*>(_payload)->empty();
        case (null):
            return true;
        case (string):
            return false;
        case (boolean):
            return false;
        case (number_int):
            return false;
        case (number_float):
            return false;
    }
}

/// return the type of the object
JSON::json_t JSON::type() const {
    return _type;
}

/// direct access to the underlying payload
void* JSON::data() {
    return _payload;
}

/// direct access to the underlying payload
const void* JSON::data() const {
    return _payload;
}

/// lexicographically compares the values
bool JSON::operator==(const JSON& o) const {
    switch (_type) {
        case (array): {
            if (o._type == array) {
                std::vector<JSON>* a = static_cast<std::vector<JSON>*>(_payload);
                std::vector<JSON>* b = static_cast<std::vector<JSON>*>(o._payload);
                return *a == *b;
            }
        }
        case (object): {
            if (o._type == object) {
                std::map<std::string, JSON>* a = static_cast<std::map<std::string, JSON>*>(_payload);
                std::map<std::string, JSON>* b = static_cast<std::map<std::string, JSON>*>(o._payload);
                return *a == *b;
            }
        }
        case (null): {
            if (o._type == null) {
                return true;
            }
        }
        case (string): {
            if (o._type == string) {
                const std::string a = *this;
                const std::string b = o;
                return a == b;
            }
        }
        case (boolean): {
            if (o._type == boolean) {
                bool a = *this;
                bool b = o;
                return a == b;
            }
        }
        case (number_int): {
            if (o._type == number_int or o._type == number_float) {
                int a = *this;
                int b = o;
                return a == b;
            }
        }
        case (number_float): {
            if (o._type == number_int or o._type == number_float) {
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
        case (array):
            return "array";
        case (object):
            return "object";
        case (null):
            return "null";
        case (string):
            return "string";
        case (boolean):
            return "boolean";
        case (number_int):
            return "number";
        case (number_float):
            return "number";
    }
}


////////////////////////////////////////////////////

/// reports a parse error in case an excpeted token is not present
void JSON::parseError(unsigned int pos, std::string tok) const {
    throw std::runtime_error("parse error: expected " + tok + " at position " + to_string(pos));
}


bool JSON::checkDelim(char*& buf, unsigned int& len, char tok) const {
    // skip whitespace
    while (std::isspace(*buf) && len > 0) {
        ++buf;
        --len;
    }

    // check if next character is given token
    if (len == 0 || *buf != tok) {
        return false;
    }

    ++buf;
    --len;
    return true;
}

std::string JSON::getString(char*& buf, unsigned int& len, unsigned int max) const {
    // string should begin with quotes
    if (!checkDelim(buf, len, '"')) {
        parseError(max - len, "\"");
    }

    // string should end with quotes
    char* aim(strchr(buf, '"'));
    if (!aim) {
        parseError(max - len, "\"");
    }

    // return string
    *aim = 0;
    std::string tmp(buf);
    len -= aim - buf + 1;
    buf = aim + 1;
    return tmp;
}

void JSON::parse(char*& buf, unsigned int& len, unsigned int max) {
    // skip whitespace
    while (std::isspace(*buf) && len > 0) {
        ++buf;
        --len;
    }

    // we are not done with parsing yet
    if (len == 0) {
        parseError(max, "?");
    }

    switch (*buf) {
        // parse an object
        // TODO: objects may be empty
        case '{': {
            ++buf;
            --len;
            do {
                // we first expect a string
                std::string left(getString(buf, len, max));

                // then a colon
                if (!checkDelim(buf, len, ':')) {
                    parseError(max - len, ":");
                }

                // then a value which we store using the first string
                (*this)[left].parse(buf, len, max);
            } while (checkDelim(buf, len, ','));

            // objects should end with '}'
            if (!checkDelim(buf, len, '}')) {
                parseError(max - len, "}");
            }
            break;
        }

        // parse an array
        // TODO: arrays may be empty
        case '[': {
            ++buf;
            --len;
            do {
                // parse the next value
                JSON tmp;
                tmp.parse(buf, len, max);
                operator+=(tmp);
            } while (checkDelim(buf, len, ','));

            // arrays should end with "]"
            if (!checkDelim(buf, len, ']')) {
                parseError(max - len, "]");
            }
            break;
        }

        // parse a string
        case '"': {
            *this = getString(buf, len, max);
            break;
        }

        // parse a Boolean (true)
        case 't': {
            const size_t length_of_true = 4;
            if (strncmp(buf, "true", length_of_true) == 0) {
                buf += length_of_true;
                len -= length_of_true;
                *this = true;
            } else {
                parseError(max - len, "t");
            }
            break;
        }

        // parse a Boolean (false)
        case 'f': {
            const size_t length_of_false = 5;
            if (strncmp(buf, "false", length_of_false) == 0) {
                buf += length_of_false;
                len -= length_of_false;
                *this = false;
            } else {
                parseError(max - len, "f");
            }
            break;
        }

        // parse null
        case 'n': {
            const size_t length_of_null = 4;
            if (strncmp(buf, "null", length_of_null) == 0) {
                buf += length_of_null;
                len -= length_of_null;
                // nothing to do
            } else {
                parseError(max - len, "n");
            }
            break;
        }

        // parse a number
        default: {
            char* tmp(buf);
            unsigned int tmplen(len);
            while (*tmp != '.' && *tmp != ',' && *tmp != ']' && *tmp != '}' && tmplen > 0) {
                ++tmp;
                --tmplen;
            }

            if (*tmp == '.') {
                *this = strtod(buf, &tmp);
            } else {
                *this = (int)strtol(buf, &tmp, 10);
            }

            len -= tmp - buf;
            buf = tmp;
        }
    }
}

///parse input into JSON object
void JSON::parseStream(std::istream& is) {
    int pos1 = is.tellg();
    is.seekg(0, is.end);
    int pos2 = is.tellg();
    is.seekg(pos1);

    char* buffer = new char[pos2 - pos1 + 1];
    is.read(buffer, pos2 - pos1);
    //is.close();

    unsigned int len(pos2 - pos1);
    char* buf(buffer);
    parse(buf, len, len);

    delete[] buffer;
}







JSON::parser::parser(std::istream &is) : _is(is) {
    next();
}

void JSON::parser::error(std::string msg) {
    throw std::runtime_error("parse error at position " + to_string(_is.tellg()) + ": " + msg + ", last read: " + _current);
}

void JSON::parser::skipSpace() {
    while (std::isspace(_current)) {
        next();
    }
}

bool JSON::parser::next() {
    _current = _is.get();
    return true;
}

/// \todo: escaped strings
std::string JSON::parser::parseString() {
    // strings should begin with an opening quote
    if (_current != '\"') {
        error("expected \"");
    }

    // read everything until closing quotes
    char buf[1000];
    _is.get(buf, 1000, '\"');

    // eat closing quote
    next();

    // read next character (optional?)
    next();

    return std::string(buf);
}

void JSON::parser::parseTrue() {
    if (_current != 't') {
        error("expected true");
    }

    char buf[4];
    _is.get(buf, 4);
    
    if (strcmp(buf, "rue")) {
        error("expected true");
    }

    // read next character (optional?)
    next();
}

void JSON::parser::parseFalse() {
    if (_current != 'f') {
        error("expected false");
    }

    char buf[5];
    _is.get(buf, 5);
    
    if (strcmp(buf, "alse")) {
        error("expected false");
    }

    // read next character (optional?)
    next();
}

void JSON::parser::parseNull() {
    if (_current != 'n') {
        error("expected null");
    }

    char buf[4];
    _is.get(buf, 4);
    
    if (strcmp(buf, "ull")) {
        error("expected null");
    }

    // read next character (optional?)
    next();
}

bool JSON::parser::delimiter(char c) {
    return (_current == c);
}

void JSON::parser::parse() {
    skipSpace();

    if (_is.eof()) {
        error("unexpected end of file");
    }
    
    switch(_current) {
        case('{'): {
            std::cerr << "parsing object\n";
            next();
            skipSpace();

            if (!delimiter('}')) {
                do {
                    skipSpace();

                    // string
                    std::string s = parseString();
                    std::cerr << s << '\n';

                    // whitespace
                    skipSpace();

                    // colon
                    if (!delimiter(':')) {
                        error("expected :");
                    } else {
                        next();
                    }

                    // whitespace
                    skipSpace();

                    // value
                    parse();

                    // whitespace
                    skipSpace();
                } while (delimiter(',') && next());
            } else {
                // object is empty
                next();
            }

            std::cerr << "parsed object\n";
            break;
        }

        case('['): {
            std::cerr << "parsing array\n";
            next();

            if (!delimiter(']')) {
                do {
                    // whitespace
                    skipSpace();

                    // value
                    parse();
                
                    // whitespace
                    skipSpace();
                } while (delimiter(',') && next());
            } else {
                // array is empty
                next();
            }

            std::cerr << "parsed array\n";
            break;
        }

        case('\"'): {
            std::cerr << "parsing string\n";
            parseString();
            std::cerr << "parsed string\n";
            break;
        }

        case('t'): {
            std::cerr << "parsing true\n";
            parseTrue();
            std::cerr << "parsed true\n";
            break;
        }

        case('f'): {
            std::cerr << "parsing false\n";
            parseFalse();
            std::cerr << "parsed false\n";
            break;
        }

        case('n'): {
            std::cerr << "parsing null\n";
            parseNull();
            std::cerr << "parsed null\n";
            break;
        }
        
        default: {
            if (std::isdigit(_current) || _current == '-') {
                std::cerr << "parsing number\n";

                std::string tmp;
                do {
                    tmp += _current;
                    next();
                } while (std::isdigit(_current) || _current == '.' || _current == 'e' || _current == 'E' || _current == '+' || _current == '-');
                
                if (tmp.find(".") != std::string::npos) {
                    // integer
                } else {
                    // float
                }

                std::cerr << "parsed number\n";
            } else {
                error("unexpected character");
            }
        }
    }
}

// TODO: http://www.cplusplus.com/reference/istream/istream/get/

/*

*/

// http://stackoverflow.com/questions/7758580/writing-your-own-stl-container/7759622#7759622

JSON::iterator JSON::begin() {
    return JSON::iterator(this);
}
JSON::iterator JSON::end() {
    return JSON::iterator();
}

JSON::iterator::iterator() : _object(nullptr), _vi(nullptr), _oi(nullptr) {}

JSON::iterator::iterator(JSON* j) : _object(j), _vi(nullptr), _oi(nullptr) {
    switch (_object->_type) {
        case (array): {
            _vi = new std::vector<JSON>::iterator(static_cast<std::vector<JSON>*>(_object->_payload)->begin());
            break;
        }
        case (object): {
            _oi = new std::map<std::string, JSON>::iterator(static_cast<std::map<std::string, JSON>*>(_object->_payload)->begin());
            break;
        }
        default:
            break;
    }
}

JSON::iterator::iterator(const JSON::iterator& o) : _object(o._object), _vi(nullptr), _oi(nullptr) {
    switch (_object->_type) {
        case (array): {
            _vi = new std::vector<JSON>::iterator(static_cast<std::vector<JSON>*>(_object->_payload)->begin());
            break;
        }
        case (object): {
            _oi = new std::map<std::string, JSON>::iterator(static_cast<std::map<std::string, JSON>*>(_object->_payload)->begin());
            break;
        }
        default:
            break;
    }
}

JSON::iterator::~iterator() {
    delete _vi;
    delete _oi;
}

JSON::iterator& JSON::iterator::operator=(const JSON::iterator& o) {
    _object = o._object;
    return *this;
}

bool JSON::iterator::operator==(const JSON::iterator& o) const {
    return _object == o._object;
}

bool JSON::iterator::operator!=(const JSON::iterator& o) const {
    return _object != o._object;
}


JSON::iterator& JSON::iterator::operator++() {
    // iterator cannot be incremented
    if (_object == nullptr) {
        return *this;
    }

    switch (_object->_type) {
        case (array): {
            if (++(*_vi) == static_cast<std::vector<JSON>*>(_object->_payload)->end()) {
                _object = nullptr;
            }
            break;
        }
        case (object): {
            if (++(*_oi) == static_cast<std::map<std::string, JSON>*>(_object->_payload)->end()) {
                _object = nullptr;
            }
            break;
        }
        default: {
            _object = nullptr;
        }
    }
    return *this;
}

JSON& JSON::iterator::operator*() const {
    if (_object == nullptr) {
        throw std::runtime_error("cannot get value");
    }

    switch (_object->_type) {
        case (array):
            return **_vi;
        case (object):
            return (*_oi)->second;
        default:
            return *_object;
    }
}

JSON* JSON::iterator::operator->() const {
    if (_object == nullptr) {
        throw std::runtime_error("cannot get value");
    }

    switch (_object->_type) {
        case (array):
            return &(**_vi);
        case (object):
            return &((*_oi)->second);
        default:
            return _object;
    }
}

std::string JSON::iterator::key() const {
    if (_object != nullptr and _object->_type == object) {
        return (*_oi)->first;
    } else {
        throw std::runtime_error("cannot get key");
    }
}

JSON& JSON::iterator::value() const {
    if (_object == nullptr) {
        throw std::runtime_error("cannot get value");
    }

    switch (_object->_type) {
        case (array):
            return **_vi;
        case (object):
            return (*_oi)->second;
        default:
            return *_object;
    }
}




JSON::const_iterator JSON::begin() const {
    return JSON::const_iterator(this);
}
JSON::const_iterator JSON::end() const {
    return JSON::const_iterator();
}

JSON::const_iterator JSON::cbegin() const {
    return JSON::const_iterator(this);
}
JSON::const_iterator JSON::cend() const {
    return JSON::const_iterator();
}

JSON::const_iterator::const_iterator() : _object(nullptr), _vi(nullptr), _oi(nullptr) {}

JSON::const_iterator::const_iterator(const JSON* j) : _object(j), _vi(nullptr), _oi(nullptr) {
    switch (_object->_type) {
        case (array): {
            _vi = new std::vector<JSON>::const_iterator(static_cast<std::vector<JSON>*>(_object->_payload)->begin());
            break;
        }
        case (object): {
            _oi = new std::map<std::string, JSON>::const_iterator(static_cast<std::map<std::string, JSON>*>(_object->_payload)->begin());
            break;
        }
        default:
            break;
    }
}

JSON::const_iterator::const_iterator(const JSON::const_iterator& o) : _object(o._object), _vi(nullptr), _oi(nullptr) {
    switch (_object->_type) {
        case (array): {
            _vi = new std::vector<JSON>::const_iterator(static_cast<std::vector<JSON>*>(_object->_payload)->begin());
            break;
        }
        case (object): {
            _oi = new std::map<std::string, JSON>::const_iterator(static_cast<std::map<std::string, JSON>*>(_object->_payload)->begin());
            break;
        }
        default:
            break;
    }
}

JSON::const_iterator::const_iterator(const JSON::iterator& o) : _object(o._object), _vi(nullptr), _oi(nullptr) {
    switch (_object->_type) {
        case (array): {
            _vi = new std::vector<JSON>::const_iterator(static_cast<std::vector<JSON>*>(_object->_payload)->begin());
            break;
        }
        case (object): {
            _oi = new std::map<std::string, JSON>::const_iterator(static_cast<std::map<std::string, JSON>*>(_object->_payload)->begin());
            break;
        }
        default:
            break;
    }
}

JSON::const_iterator::~const_iterator() {
    delete _vi;
    delete _oi;
}

JSON::const_iterator& JSON::const_iterator::operator=(const JSON::const_iterator& o) {
    _object = o._object;
    return *this;
}

bool JSON::const_iterator::operator==(const JSON::const_iterator& o) const {
    return _object == o._object;
}

bool JSON::const_iterator::operator!=(const JSON::const_iterator& o) const {
    return _object != o._object;
}


JSON::const_iterator& JSON::const_iterator::operator++() {
    // iterator cannot be incremented
    if (_object == nullptr) {
        return *this;
    }

    switch (_object->_type) {
        case (array): {
            if (++(*_vi) == static_cast<std::vector<JSON>*>(_object->_payload)->end()) {
                _object = nullptr;
            }
            break;
        }
        case (object): {
            if (++(*_oi) == static_cast<std::map<std::string, JSON>*>(_object->_payload)->end()) {
                _object = nullptr;
            }
            break;
        }
        default: {
            _object = nullptr;
        }
    }
    return *this;
}

const JSON& JSON::const_iterator::operator*() const {
    if (_object == nullptr) {
        throw std::runtime_error("cannot get value");
    }

    switch (_object->_type) {
        case (array):
            return **_vi;
        case (object):
            return (*_oi)->second;
        default:
            return *_object;
    }
}

const JSON* JSON::const_iterator::operator->() const {
    if (_object == nullptr) {
        throw std::runtime_error("cannot get value");
    }

    switch (_object->_type) {
        case (array):
            return &(**_vi);
        case (object):
            return &((*_oi)->second);
        default:
            return _object;
    }
}

std::string JSON::const_iterator::key() const {
    if (_object != nullptr and _object->_type == object) {
        return (*_oi)->first;
    } else {
        throw std::runtime_error("cannot get key");
    }
}

const JSON& JSON::const_iterator::value() const {
    if (_object == nullptr) {
        throw std::runtime_error("cannot get value");
    }

    switch (_object->_type) {
        case (array):
            return **_vi;
        case (object):
            return (*_oi)->second;
        default:
            return *_object;
    }
}
