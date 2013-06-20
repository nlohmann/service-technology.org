#pragma once

#include <string>
#include <vector>
#include <map>
#include <mutex>

class JSON {
    private:
        /// mutex to guard payload
        static std::mutex token;

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
        json_t type = json_t::null;

        /// the payload
        void *payload = nullptr;

    private:
        // getter for typed pointers to the payload
        std::vector<JSON>* getArray();
        std::vector<JSON>* getArray() const;
        std::map<std::string, JSON>* getObject();
        std::map<std::string, JSON>* getObject() const;
        std::string * getString();
        std::string * getString() const;
        bool * getBoolean();
        bool * getBoolean() const;
        int * getInteger();
        int * getInteger() const;
        double * getFloat();
        double * getFloat() const;

    public:
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
        JSON() = default;
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

        /// copy constructor
        JSON(const JSON&);
        
        /// move constructor
        JSON(JSON &&);

        /// copy assignment
        JSON &operator=(JSON);

        /// destructor
        ~JSON();
};
