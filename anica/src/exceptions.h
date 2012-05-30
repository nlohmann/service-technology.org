#pragma once

#include <sstream>

namespace anica {

namespace exceptions {

class Error 
{ 
    public:
    /// exception message 
    const std::string message; 

    /* public methods */ 
    /// constructor 
    explicit Error(const std::string&);
    /// output method 
    virtual std::ostream& output(std::ostream& os) const; 
};


class ToolError: public Error
{
    public:
    /// tool the error occured in 
    const std::string tool;

    public:
    /// constructor 
    ToolError(const std::string&, const std::string&);
    /// output method 
    virtual std::ostream& output(std::ostream&) const; 
};

class InputError: public Error
{
    public:
    /// user error types 
    enum IE_Type 
    { 
        IE_INCOMPLETE_TRIPLE,
        IE_UNKNOWN_TRANSITION,
        IE_UNKNOWN_PLACE
    }; 

    public:
    /// user error type 
    const IE_Type type;
    
    public:
    /// constructor 
    InputError(IE_Type, const std::string&);
    /// output method 
    virtual std::ostream& output(std::ostream&) const; 
};

class UserError: public Error
{
    public:
    /* public methods */ 
    /// constructor 
    explicit UserError(const std::string&);
    /// output method 
    virtual std::ostream& output(std::ostream& os) const; 
};


}

}
