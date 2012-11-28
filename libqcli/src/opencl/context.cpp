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

#include "context.h"
#include "util/utils.h"
#include "devicemanager.h"

// Includes for OS-specific OpenGL support
#ifdef __MACOSX
#  include <OpenGL/OpenGL.h>
#elif _WIN32
#  include <wingdi.h>
#else // Linux
#  include <GL/glx.h>
#endif

namespace QCLI {

Context::~Context()
{
    QMutexLocker locker(&_lock);

    /*
    // NOTE: Supported in OpenCL 1.2
    // Release the OpenCL devices
    for(auto & device: devices)
    {
      clReleaseDevice(device);
    }
    */
    clReleaseContext(_context);
}


bool Context::init(cl_device_type devType, bool glInterop)
{
    // Select the devices of type devType (thread-safe)
    if(!devMgr().selectDevices(devType))
        return false;
    // Create context
    return createContext(glInterop);
}

bool Context::init(QList<int> devIds, bool glInterop)
{
    // Select devices in the list (thread-safe)
    if(!devMgr().selectDevices(devIds))
        return false;
    // Create context
    return createContext(glInterop);
}

bool Context::createContext(bool glInterop)
{
    if(_initialized) {
        qDebug() << "Already initialized.";
        return false;
    }

    QMutexLocker locker(&_lock);

    _glInterop= glInterop;

    QVector<cl_context_properties> props;
    if(_glInterop) {
        // Add OpenGL properties
        #ifdef __MACOSX // Apple (untested)
            props << CL_CGL_SHAREGROUP_KHR;
            props << reinterpret_cast<cl_context_properties>(CGLGetShareGroup(CGLGetCurrentContext()));
            props << CL_CONTEXT_PLATFORM;
            props << reinterpret_cast<cl_context_properties>(devMgr().platform());
        #elif _WIN32 // Windows (untested)
            props << CL_GL_CONTEXT_KHR
            props << reinterpret_cast<cl_context_properties>(wglGetCurrentContext());
            props << CL_WGL_HDC_KHR
            props << reinterpret_cast<cl_context_properties>(wglGetCurrentDC());
        #else // Linux/GLX
            props << CL_GL_CONTEXT_KHR;
            props << reinterpret_cast<cl_context_properties>(glXGetCurrentContext());
            props << CL_GLX_DISPLAY_KHR;
            props << reinterpret_cast<cl_context_properties>(glXGetCurrentDisplay());
            props << reinterpret_cast<cl_context_properties>(devMgr().platform());
        #endif
        props << 0;
    }

    // Create OpenCL context
    cl_int err;
    const auto propsPtr= props.empty() ? nullptr : props.data();
    const auto devs= devMgr().devices(); // Get selected devices from the dev manager
    _context= clCreateContext(propsPtr, devs.size(), devs.data(), nullptr, nullptr, &err);
    if(checkCLError(err, "clCreateContext"))
        return false;

    _initialized= true;
    return true;
}

bool Context::supportsGL()
{
    if(!_initialized and !init())
        return false;
    QMutexLocker l(&_lock);
    return _glInterop;
}

cl_context Context::context()
{
    if(!_initialized and !init())
        return nullptr;
    QMutexLocker locker(&_lock);
    return _context;
}

} // namespace QCLI
