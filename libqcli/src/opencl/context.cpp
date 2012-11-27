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

// Includes for OS-specific OpenGL support
#ifdef __MACOSX
#  define GL_SHARING_EXTENSION "cl_APPLE_gl_sharing"
#  include <OpenGL/OpenGL.h>
#elif _WIN32
#  define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#  include <wingdi.h>
#else // Linux
#  define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#  include <GL/glx.h>
#endif

namespace QCLI {

bool Context::init(bool useOpenGL, cl_device_type devType, QList<cl_device_id> devs)
{
    QMutexLocker locker(&_lock);

    if(_initialized) {
        qDebug() << "Already initialized.";
        return false;
    }
    _openGL= useOpenGL;
    _devType= devType;

    QString fname(__func__);
    cl_int err {};

    // Get the platform information
    cl_platform_id platform;
    err= clGetPlatformIDs(1, &platform, nullptr);
    if(checkerr(err, fname + ": clGetPlatformIDs()"))
        return false;

    // Select all the OpenCL devices that are going to be part of this context
    // First get the number of OpenCL devices
    const int maxDevices= 32;
    cl_uint numDevices {};
    err= clGetDeviceIDs(platform, devType, maxDevices, nullptr, &numDevices);
    if(checkerr(err, fname + ": clGetDeviceIDs()"))
        return false;

    // resize the vector to put the devices
    _devices.resize(numDevices, nullptr);
    err= clGetDeviceIDs(platform, devType, maxDevices, _devices.data(), &numDevices);
    if(checkerr(err, fname + ": clGetDeviceIDs()"))
        return false;

    // If the user specified the device list, check that they exist and use it
    foreach(const cl_device_id& id, devs) {
        if(!_devices.contains(id)) {
            qDebug() << "Device" << id << "not found.";
            return false;
        }
    }
    if(!devs.empty())
        _devices= devs.toVector().toStdVector();

    QVector<cl_context_properties> props;
    if(useOpenGL) {
        // Add OpenGL properties
        #ifdef __MACOSX // Apple (untested)
            props << CL_CGL_SHAREGROUP_KHR;
            props << static_cast<cl_context_properties>(CGLGetShareGroup(CGLGetCurrentContext()));
            props << CL_CONTEXT_PLATFORM;
            props << static_cast<cl_context_properties>(platform);
        #elif _WIN32 // Windows (untested)
            props << CL_GL_CONTEXT_KHR
            props << static_cast<cl_context_properties>(wglGetCurrentContext());
            props << CL_WGL_HDC_KHR
            props << static_cast<cl_context_properties>(wglGetCurrentDC());
        #else // Linux/GLX
            props << CL_GL_CONTEXT_KHR;
            props << static_cast<cl_context_properties>(glXGetCurrentContext());
            props << CL_GLX_DISPLAY_KHR;
            props << static_cast<cl_context_properties>(glXGetCurrentDisplay());
            props << static_cast<cl_context_properties>(platform);
        #endif
        props << 0;
    }

    // Create OpenCL context
    const auto propsPtr= props.empty() ? nullptr : props.data();
    _context= clCreateContext(propsPtr, _devices.size(), _devices.data(), nullptr, nullptr, &err);
    if(checkerr(err, fname + ": clCreateContext()"))
        return false;

    _initialized= true;
    return true;
}

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

cl_context Context::context() const
{
    // Hidden initialization with default parameters
    if(!_initialized)
        init();
    QMutexLocker locker(&_lock);
    return _context;
}

} // namespace QCLI
