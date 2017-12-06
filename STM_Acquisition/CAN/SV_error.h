/***************************************************
 *
 * File: SV_error.h
 *
 * Purpose: Defines  routines and constants for error handling
 *
 * Author: J. C. Wiley, Mar 2013
 *
****************************************************/
#ifndef SV_PERROR_H
#define SV_PERROR_H

#include "SV_stdlibs.h"

void SV_perror(const string& s, const int a_errno, const int err);
#define TRACE 
#undef TRACE


#endif
