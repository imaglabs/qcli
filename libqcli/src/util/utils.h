/*
 *   Copyright (C) 2012 by the libQCLI authors (see AUTHORS)
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 */

#ifndef _QCLI_CLUTILS_H
#define _QCLI_CLUTILS_H

#include <CL/cl.h>
#include <string>

namespace QCLI {

using namespace std;

/// \brief Utility functions

/// Maps the OpenCL error code to a string
string clErrorToString(cl_int err);

/// Checks for errors in OpenCL API calls, and prints useful information
bool checkCLError(cl_int error, string message);

} // namespace QCLI

#endif // CLUTILS_H
