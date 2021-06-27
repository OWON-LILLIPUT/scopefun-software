%module scopefunapi

// include
%{

#define SCOPEFUN_API_EXPORT
#include "scopefunapi.h"
   
%}

// swig
%include "cpointer.i"
%include "carrays.i"
%include "typemaps.i"
%include "scopefunapi.h"