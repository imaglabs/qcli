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

#ifndef _QCLI_CONTEXT_H 
#define _QCLI_CONTEXT_H 

#include <CL/cl.h>
#include <vector>
#include <string>

// WARNING: All methods must be thread-safe.

namespace QCLI {

using namespace std;

/// \brief OpenCL Context class 
class Context;
typedef Context& ContextRef;

class Context {
  
  public:
    ~Context();
    
    /// Default initialization for a context
    bool init();  
    
    /// Static Singleton instance method
    static ContextRef instance();
    
    /// Number of OpenCL devices that are part of this context
    uint numberOfDevices() const { return devices.size(); }
    //vector<string> listDevices() const;
    
    /// Not Copyable class
    Context(const Context & other) = delete;
    Context& operator=(const Context & other) = delete;
    
  private:
    
    Context() = default;
    
    /// Delete all OpenCL members with the corresponding OpenCL calls
    void cleanCLObjects();

    cl_context clContext { nullptr };
    vector<cl_device_id> devices;
    
};

/// Thread-safe getInstance()
ContextRef Context::instance() 
{
  static Context inst;
  return inst;
}
  
} // namespace QCLI

#endif // _QCLI_CONTEXT_H 
