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

#include <QtCore>
#include <CL/cl.h>
#include <CL/cl_gl.h>

namespace QCLI {

/** \brief OpenCL context singleton
 *
 *  All functions are thread-safe.
 *  Calling any functions other than instance(), init() and initialized()
 *  initializes the context if not initialized before.
*/

class Context
{
public:
    ~Context();

    /// Static Singleton instance method (thread safe in C++11)
    static Context& instance() {
        static Context inst;
        return inst;
    }

    /// Initialize the context for devices of a certain type
    /// Use CL_DEVICE_TYPE_ALL to get all devices
    /// @retval false on error or if already initialized
    bool init(cl_device_type devType= CL_DEVICE_TYPE_GPU, bool glInterop= true);
    /// Initialize the context for a list devices indexes
    /// @retval false on error or if already initialized
    bool init(QList<int> devIds, bool glInterop= true);

    /// Returns true if the context was initialized
    bool initialized() const { return _initialized; }
    /// Returns true if the context was initialized with OpenGL support
    bool supportsGL();

    /// Returns true if the context supports images of format format
    bool supportedFormat(const cl_image_format& format);

    /// Returns the OpenCL context
    cl_context context();

    /// Disable copying
    Context(const Context &other) = delete;
    /// Disable assignments
    Context& operator=(const Context& other) = delete;

private:
    /// Hide constructor
    Context() = default;
    bool createContext(bool glInterop);
    bool createQueues();

    // State
    mutable QMutex _lock; // Mutable so it can be used in const getters
    QAtomicInt _initialized;

    // OpenCL
    cl_context _context= nullptr;
    bool _glInterop= true;

    // Supported image formats
    QVector<cl_image_format> _imgFormats;
};

/// Global function to access the Context instance
inline Context& ctx() { return Context::instance(); }
/// Global function to access the OpenCL context object
inline cl_context clctx() { return ctx().context(); }


} // namespace QCLI

#endif // _QCLI_CONTEXT_H
