#ifndef NOTIMPLEMENTEDEXCEPTION_H
#define NOTIMPLEMENTEDEXCEPTION_H

#include <exception>

class NotImplementedException : public std::logic_error
{
public:
    NotImplementedException():std::logic_error(what()){}
    char const * what() const noexcept override{ return "Function not yet implemented."; }
};


#endif // NOTIMPLEMENTEDEXCEPTION_H
