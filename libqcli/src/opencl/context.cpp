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

#include <context.h>
#include <utils.h>

#define QCLI_MAX_OPENCL_DEVICES 32

namespace QCLI {
  
using namespace std;

bool Context::init() 
{
  string fname(__func__);
  cl_int clError{};
  
  // Get the platform information
  cl_platform_id platform;
  clError= clGetPlatformIDs(1, &platform, nullptr);
  if(checkCLError(clError, fname + ": clGetPlatformIDs()"))
    return false;

  // Select all the OpenCL devices that are going to be part of this context
  // First get the number of OpenCL devices
  cl_uint numDevices{};
  clError= clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, QCLI_MAX_OPENCL_DEVICES, nullptr, &numDevices);
  if(checkCLError(clError, fname + ": clGetDeviceIDs()"))
    return false;
  // resize the vector to put the devices
  devices.resize(numDevices, nullptr);
  clError= clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, QCLI_MAX_OPENCL_DEVICES, devices.data(), &numDevices);
  if(checkCLError(clError, fname + ": clGetDeviceIDs()"))
    return false;
  
  // Create the OpenCL context with all the OpenCL devices
  clContext= clCreateContext(nullptr, devices.size(), devices.data(), nullptr, nullptr, &clError);
  if(checkCLError(clError, fname + ": clCreateContext()"))
    return false;

  return true;
}

/*
vector<string> Context::listDevices() const
{
  
}
*/


Context::~Context() 
{
  cleanCLObjects();
}

void Context::cleanCLObjects() 
{
  /*
  // NOTE: Supported in OpenCL 1.2
  // Release the OpenCL devices
  for(auto & device: devices) 
  {
    clReleaseDevice(device);
  }
  */
  clReleaseContext(clContext);
}

} // namespace QCLI
