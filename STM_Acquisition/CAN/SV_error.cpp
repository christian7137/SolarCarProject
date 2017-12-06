/***************************************************
 *
 * File: SV_error.cpp
 *
 * Purpose: Defines  routines  error handling
 *
 * Author: J. C. Wiley, Mar 2013
 *
****************************************************/



/**************************************************
	-
	- SV_perror - local version of perror
	-      a_errno - captured errno at error
	-      err - local error number
	-
**************************************************/
#include "SV_stdlibs.h"

void SV_perror(const string& s, const int a_errno, const int err){
	stringstream ss;
	if(a_errno != 0){
		ss << s << " err " << err << ": " << strerror(a_errno) << endl;
	}
	else{
		ss  << s << " err " << err  << endl;
	}
	cerr << ss.str();
}

