#ifndef NOTIMPLEMENTEDEXCEPTION_H
#define NOTIMPLEMENTEDEXCEPTION_H

#include <exception>

class NotImplementedException : public std::logic_error
{
public:
    NotImplementedException():std::logic_error(what()){}
    virtual char const * what() const noexcept{ return "Function not yet implemented."; }
};


#endif // NOTIMPLEMENTEDEXCEPTION_H
