#ifndef COMMON_H_
#define COMMON_H_

#include <iostream>
#include <stdexcept>


class FinException : public std::logic_error
{
public:
    FinException(const std::string &msg)
        : std::logic_error(msg)
    {}

    virtual ~FinException() throw()
    {}
};

#endif /* COMMON_H_ */

