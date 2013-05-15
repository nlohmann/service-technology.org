#pragma once
#include <string>
#include <vector>
#include <map>

class JSON {
    private:
        typedef enum { JSON_ARRAY, JSON_OBJECT, JSON_NULL, JSON_STRING, JSON_BOOLEAN, JSON_NUMBER_INT, JSON_NUMBER_FLOAT } json_t;
        json_t type;
        union {
            std::vector<JSON> *a;
            std::map<std::string, JSON> *o;
            std::string *s;
            bool b;
            int i;
            float f;
        } payload;

    public:
        const std::string toString() const;
        operator const char *() const;
        operator const std::string() const;

        operator const int() const;
        operator const float() const;
        operator const bool() const;

        void add();
        void add(std::string);
        void add(const char*);
        void add(bool);
        void add(int);
        void add(float);
        void add(JSON);

        void add(std::string, std::string);
        void add(std::string, const char*);
        void add(std::string, bool);
        void add(std::string, int);
        void add(std::string, float);
        void add(std::string, JSON);

        JSON();
        JSON(std::string);
        JSON(const char*);
        JSON(bool);
        JSON(int);
        JSON(float);
        JSON(const JSON&);
};
