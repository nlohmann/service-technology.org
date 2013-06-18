#pragma once

#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <mutex>

class JSON {
    private:
        /// possible types of a JSON object
        enum class json_t {
            array,
            object,
            null,
            string,
            boolean,
            number_int,
            number_float
        };
        /// the type of this object
        json_t type;
        /// the payload of this object
        union {
            
            std::vector<JSON>* a;           ///< array
            std::map<std::string, JSON>* o; ///< object
            std::string* s;                 ///< string
            bool b;                         ///< Boolean
            int i;                          ///< number: integer
            double f;                       ///< number: float
        } payload;

        // mutex to guard payload
        static std::mutex token;

    public:
        /// explicit conversion to string representation (C style)
        const char* c_str() const;
        /// explicit conversion to string representation (C++ style)
        const std::string toString() const;

        /// implicit conversion to string representation (C style)
        operator const char*() const;
        /// implicit conversion to string representation (C++ style)
        operator const std::string() const;
        /// implicit conversion to integer (only for numbers)
        operator const int() const;
        /// implicit conversion to double (only for numbers)
        operator const double() const;
        /// implicit conversion to Boolean (only for Booleans)
        operator const bool() const;

        /// add a string to an array
        void add(const std::string&);
        /// add a string to an array
        void add(const char*);
        /// add a Boolean to an array
        void add(bool);
        /// add a number to an array
        void add(int);
        /// add a number to an array
        void add(double);
        /// add an object/array to an array
        void add(JSON&);

        /// operator to set an element in an object
        JSON& operator[](const std::string&);
        /// operator to set an element in an object
        JSON& operator[](const char*);
        /// operator to get an element in an object
        const JSON& operator[](const std::string&) const;

        /// add a string to an object
        void add(std::string, std::string);
        /// add a string to an object
        void add(std::string, const char*);
        /// add a Boolean to an object
        void add(std::string, bool);
        /// add a number to an object
        void add(std::string, int);
        /// add a number to an object
        void add(std::string, double);
        /// add an object/array to an object
        void add(std::string, JSON&);

        /// create an empty (null) object
        JSON();
        /// create a string object from C++ string
        JSON(const std::string&);
        /// create a string object from C string
        JSON(const char*);
        /// create a Boolean object
        JSON(const bool);
        /// create a number object
        JSON(const int);
        /// create a number object
        JSON(const double);

        /// destructor
        ~JSON();
};
