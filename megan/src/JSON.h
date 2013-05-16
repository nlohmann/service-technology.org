#pragma once

#include <string>
#include <vector>
#include <map>

class JSON {
    private:
        /// possible types of a JSON object
        typedef enum { JSON_ARRAY, JSON_OBJECT, JSON_NULL, JSON_STRING, JSON_BOOLEAN, JSON_NUMBER_INT, JSON_NUMBER_FLOAT } json_t;
        /// the type of this object
        json_t type;
        /// the payload of this object
        union {
            /// array
            std::vector<JSON>* a;
            /// object
            std::map<std::string, JSON>* o;
            /// string
            std::string* s;
            /// Boolean
            bool b;
            /// number: integer
            int i;
            /// number: float
            float f;
        } payload;

    public:
        /// explicit conversion to string representation
        const std::string toString() const;

        /// implicit conversion to string representation (C style)
        operator const char* () const;
        /// implicit conversion to string representation (C++ style)
        operator const std::string() const;
        /// implicit conversion to integer (only for numbers)
        operator const int() const;
        /// implicit conversion to float (only for numbers)
        operator const float() const;
        /// implicit conversion to Boolean (only for Booleans)
        operator const bool() const;

        /// add a null object to an array
        void add();
        /// add a string to an array
        void add(std::string);
        /// add a string to an array
        void add(const char*);
        /// add a Boolean to an array
        void add(bool);
        /// add a number to an array
        void add(int);
        /// add a number to an array
        void add(float);
        /// add an object/array to an array
        void add(JSON);

        /// add a string to an object
        void add(std::string, std::string);
        /// add a string to an object
        void add(std::string, const char*);
        /// add a Boolean to an object
        void add(std::string, bool);
        /// add a number to an object
        void add(std::string, int);
        /// add a number to an object
        void add(std::string, float);
        /// add an object/array to an object
        void add(std::string, JSON);

        /// create an empty (null) object
        JSON();
        /// create a string object from C++ string
        JSON(std::string);
        /// create a string object from C string
        JSON(const char*);
        /// create a Boolean object
        JSON(bool);
        /// create a number object
        JSON(int);
        /// create a number object
        JSON(float);
        /// copy constructor (also for objects and arrays)
        JSON(const JSON&);
};
