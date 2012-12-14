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
    if(!createContext(glInterop))
        return false;
    // Now we must create the device queues and pass them to the DeviceManager
    if(!createQueues())
        return false;
    _initialized= true;
    return true;
}

bool Context::init(QList<int> devIds, bool glInterop)
{
    // Select devices in the list (thread-safe)
    if(!devMgr().selectDevices(devIds))
        return false;
    // Create context
    if(!createContext(glInterop))
        return false;
    // Now we must create the device queues and pass them to the DeviceManager
    if(!createQueues())
        return false;
    _initialized= true;
    return true;
}

bool Context::createContext(bool glInterop)
{
    if(_initialized)
        return false;

    QMutexLocker locker(&_lock);

    _glInterop= glInterop;

    QVector<cl_context_properties> props;
    if(_glInterop) {
        // Add OpenGL properties
        #ifdef __MACOSX
            // Apple (untested)
            props << CL_CGL_SHAREGROUP_KHR;
            props << reinterpret_cast<cl_context_properties>(CGLGetShareGroup(CGLGetCurrentContext()));
            props << CL_CONTEXT_PLATFORM;
            props << reinterpret_cast<cl_context_properties>(devMgr().platform());
        #elif _WIN32
            // Windows (untested)
            props << CL_GL_CONTEXT_KHR
            props << reinterpret_cast<cl_context_properties>(wglGetCurrentContext());
            props << CL_WGL_HDC_KHR
            props << reinterpret_cast<cl_context_properties>(wglGetCurrentDC());
        #else
            // Linux/GLX
            props << CL_GL_CONTEXT_KHR;
            props << (cl_context_properties)glXGetCurrentContext();
            props << CL_GLX_DISPLAY_KHR;
            props << (cl_context_properties)glXGetCurrentDisplay();
            props << (cl_context_properties)(devMgr().platform());
        #endif
        props << 0;
    }


    cl_context_properties pro[]= {
        CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
        CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
        CL_CONTEXT_PLATFORM, (cl_context_properties)(devMgr().platform()),
        0
    };
    cl_int err;
    cl_device_id* devs= &(devMgr().devices()[0]); // Get selected devices from the dev manager
    _context= clCreateContext(pro, 1, devs, nullptr, nullptr, &err);
    if(checkCLError(err, "clCreateContext11"))
        return false;


    // Create OpenCL context
    /*
    cl_int err;
    const auto propsPtr= props.count() ? props.data() : nullptr;
    const auto devs= devMgr().devices(); // Get selected devices from the dev manager
    _context= clCreateContext(propsPtr, devs.count(), devs.data(), nullptr, nullptr, &err);
    if(checkCLError(err, "clCreateContext"))
        return false;
    */

    // Get list of supported formats
    cl_uint formatCount;
    err= clGetSupportedImageFormats(_context, CL_MEM_READ_WRITE, CL_MEM_OBJECT_IMAGE2D,
                                    0, nullptr, &formatCount);
    if(checkCLError(err, "clGetSupportedImageFormats") or formatCount<=0)
        return false;
    _imgFormats.resize(formatCount);
    err= clGetSupportedImageFormats(_context, CL_MEM_READ_WRITE, CL_MEM_OBJECT_IMAGE2D,
                                    formatCount, _imgFormats.data(), nullptr);
    if(checkCLError(err, "clGetSupportedImageFormats"))
        return false;

    return true;
}

bool Context::createQueues()
{
    // Get selected devices from the dev manager
    const auto devs= devMgr().devices();
    if(!devs.count())
        return false;

    QVector<cl_command_queue> queues(devs.count());
    cl_int err;
    for(int i=0; i<devs.count(); i++) {
        queues[i]= clCreateCommandQueue(_context, devs[i], CL_QUEUE_PROFILING_ENABLE, &err);
        if(checkCLError(err, "clCreateCommandQueue"))
            return false;
    }
    // Pass the queues to the DeviceManager
    devMgr().setQueues(queues);
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

bool Context::supportedFormat(const cl_image_format& format)
{
    if(!_initialized and !init())
        return false;
    QMutexLocker locker(&_lock);
    foreach(const cl_image_format& fmt, _imgFormats) {
        if(format.image_channel_data_type==fmt.image_channel_data_type and
           format.image_channel_order==fmt.image_channel_order) {
            return true;
        }
    }
    return false;
}

} // namespace QCLI
