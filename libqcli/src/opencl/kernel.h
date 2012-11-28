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

#ifndef _QCLI_KERNEL_H
#define _QCLI_KERNEL_H

#include <QtCore>
#include <CL/cl.h>
#include <array>

namespace QCLI {

constexpr cl_uint layoutDim { 2 }; // 2D space, for image processing
using BlockDim = array<size_t, 2>;
using GridDim = array<size_t, 2>;
    
/// \brief OpenCL Kernel class
/**
 * All methods are thread-safe
 * 
 * 
*/

class Kernel
{
public:
    Kernel() = default;
    
    /// Create a kernel from a file and a function name
    Kernel(QString fileName, QString functionName);
    /// Create a kernel from a string
    Kernel(QString source);
    
    /// Loads the kernel from a file and a function name
    /// @retval false on error
    bool loadProgram(QString fileName, QString functionName);
    /// Loads the kernel from a string
    /// @retval false on error
    bool loadSource(QString code);
    
    /// State of the created kernel
    /// @retval false if the kernel failed to compile
    bool isNull() const { return _initialized; }
    
    /// Releases the OpenCL kernel
    ~Kernel();
    
    /// Set the argument index of a kernel
    /// @retval false on error
    template<typename T>
    bool setArg(int argIndex, T&& arg);
    
    /// Set the layout of execution
    /// @retval false on error
    bool setLayout(BlockDim blockDim, GridDim gridDim);
    
    /// Execute the kernel
    /// @retval false on error
    bool operator()();
    
    /// Execute the kernel with the given parameters
    /// @retval false on error
    template<typename... Args>
    bool operator()(Args... args);
    
private:
    
    template<int argN, typename First, typename... Rest>
    bool setArguments(First arg0, Rest... rest);
    
    template<typename T>
    void getImageSize(T&&);
    
    // State
    mutable QMutex _lock; // Mutable so it can be used in const getters
    QAtomicInt _initialized;
    QAtomicInt _compiled;
    
    // OpenCL
    size_t _globalWorkSize[layoutDim];
    size_t _localWorkSize[layoutDim] { 8, 8 };
    cl_kernel _kernel { nullptr };
    
};

} // namespace QCLI

#endif // _QCLI_CONTEXT_H
