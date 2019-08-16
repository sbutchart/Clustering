#ifndef EXCEPTIONS_HH
#define EXCEPTIONS_HH 1

#include <exception>

struct PDSInfoJunk : public std::exception{
  const char * what () const throw () {
    return "The PDS information is unusable!";
  }
};

struct NoConfig : public std::exception{
  const char* what () const throw () {
    return "No configuration available!";
  }
};

#endif
