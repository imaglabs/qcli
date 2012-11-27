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

namespace QCLI {

/// Global functions to access the Context instance
Context& ctx() { return Context::instance(); }
cl_context clctx() { return ctx().context(); }

/// \brief OpenCL context singleton
/// All Context functions are thread-safe.

class Context
{
public:
    ~Context();

    /// Static Singleton instance method (thread safe in C++11)
    static Context& instance() {
        static Context inst;
        return inst;
    }

    /// Default initialization for a context
    /// @retval false on error
    bool init(bool useOpenGL= true, cl_device_type devType= CL_DEVICE_TYPE_ALL,
              QList<cl_device_id> devs= QList<cl_device_id>());

    /// Disable copies
    Context(const Context &other) = delete;
    /// Disable assignments
    Context& operator=(const Context& other) = delete;

    /// Returns true if the context was initialized
    bool initialized() const { return _initialized; }
    /// Returns a copy of the context device IDs
    QVector<cl_device_id> devices() const { QMutexLocker l(&_lock); return devices; }
    /// Number of OpenCL devices that are part of this context
    uint deviceCount() const { QMutexLocker l(&_lock); return _devices.size(); }
    /// Returns true if the context was initialized with OpenGL support
    bool supportsGL() const { QMutexLocker l(&_lock); return _openGL; }

    /// Returns the OpenCL context
    /// Calls init if not initialized to support hidden initialization
    cl_context context() const;

private:
    /// Hide constructor
    Context() = default;

    // Init parameters
    QVector<cl_device_id> _devices;
    bool _openGL= false;
    cl_device_type _devType= CL_DEVICE_TYPE_ALL;

    // Context state
    QMutex _lock;
    QAtomicInt _initialized= 0;

    // OpenCL
    cl_context _context= nullptr;
};


} // namespace QCLI

#endif // _QCLI_CONTEXT_H
