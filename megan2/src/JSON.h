#pragma once

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <initializer_list>

class JSON {
    private:
        /// mutex to guard payload
        static std::mutex _token;

    private:
        /// possible types of a JSON object
        enum class json_t {
            array,          ///< array
            object,
            null,
            string,
            boolean,
            number_int,
            number_float
        };
        /// the type of this object
        json_t _type = json_t::null;

        /// the payload
        void *_payload = nullptr;

    public:
        /// a type for objects
        typedef std::tuple<std::string, JSON> object;
        /// a type for arrays
        typedef std::initializer_list<JSON> array;

    public:
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
        /// create from an initializer list (to an array)
        JSON(array);
        /// create from a mapping (to an object)
        JSON(object);

        /// copy constructor
        JSON(const JSON&);

        /// move constructor
        JSON(JSON &&);

        /// copy assignment
        JSON &operator=(JSON);

        /// destructor
        ~JSON();

        /// implicit conversion to string representation
        operator const std::string() const;
        /// implicit conversion to integer (only for numbers)
        operator const int() const;
        /// implicit conversion to double (only for numbers)
        operator const double() const;
        /// implicit conversion to Boolean (only for Booleans)
        operator const bool() const;

        /// write to stream
        friend std::ostream& operator<<(std::ostream &o, const JSON &j) {
            o << j.toString();
            return o;
        }

        /// read from stream
        friend std::istream& operator>>(std::istream &i, JSON &j) {
            j.parseStream(i);
            return i;
        }

        /// explicit conversion to string representation (C++ style)
        const std::string toString() const;

        /// add an object/array to an array
        JSON& operator+=(const JSON&);
        /// add a string to an array
        JSON& operator+=(const std::string&);
        /// add a string to an array
        JSON& operator+=(const char *);
        /// add a Boolean to an array
        JSON& operator+=(bool);
        /// add a number to an array
        JSON& operator+=(int);
        /// add a number to an array
        JSON& operator+=(double);

        /// operator to set an element in an array
        JSON& operator[](int);
        /// operator to get an element in an array
        const JSON& operator[](const int) const;

        /// operator to set an element in an object
        JSON& operator[](const std::string&);
        /// operator to set an element in an object
        JSON& operator[](const char*);
        /// operator to get an element in an object
        const JSON& operator[](const std::string&) const;

        /// return the number of stored values
        size_t size() const;
        /// checks whether object is empty
        bool empty() const;

        /// return the type of the object
        json_t type() const;

        /// lexicographically compares the values
        bool operator==(const JSON&) const;

    private:
        /// return the type as string
        std::string _typename() const;

        // additional parser functions
        void parseStream(std::istream&);
        void parseError(unsigned int pos, std::string tok) const;
        bool checkDelim(char*& buf, unsigned int& len, char tok) const;
        std::string getString(char*& buf, unsigned int& len, unsigned int max) const;
        void parse(char*& buf, unsigned int& len, unsigned int max);
};
