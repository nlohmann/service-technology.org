#include "exceptions.h"

namespace anica {

namespace exceptions {

Error::Error(const std::string & msg)
    : 
    message(msg) 
{ 
} 

std::ostream& Error::output(std::ostream & os) const 
{ 
    return (os << message); 
}

ToolError::ToolError(const std::string& tool_, const std::string& error_)
    :
    Error(error_),
    tool(tool_)
{
} 


std::ostream& ToolError::output(std::ostream & os) const 
{ 
    return (os << tool << ":" << message); 
}

InputError::InputError(InputError::IE_Type type_, const std::string& error_)
    :
    Error(error_),
    type(type_)
{
} 


std::ostream& InputError::output(std::ostream & os) const 
{ 
    switch(type) 
    { 
        case IE_INCOMPLETE_TRIPLE: 
            return (os << "Triple field '" << message << "' is empty"); 
        case IE_UNKNOWN_TRANSITION: 
            return (os << "Transition '" << message << "' is unknown"); 
        case IE_UNKNOWN_PLACE: 
            return (os << "Place '" << message << "' is unknown");
    }
    return (os << "one should never read this");
}


UserError::UserError(const std::string & error_)
    :
    Error(error_)
{ 
} 

std::ostream& UserError::output(std::ostream & os) const 
{ 
    return (os << "user caused error: " << message); 
}

}

}
