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
#include "opencl/kernel.h"
#include "image.h"

namespace QCLI {

Kernel::Kernel(QString fileName, QString functionName) 
{
    loadProgram(fileName, functionName);
}

Kernel::Kernel(QString source) 
{
    loadSource(source);
}

bool Kernel::loadProgram(QString fileName, QString functionName)
{
    if(_initialized) {
        qDebug() << "A kernel is already loaded.";
        return false;
    }

    Q_UNUSED(fileName);
    Q_UNUSED(functionName);
    
    // TODO
    // QMutexLocker locker(&_lock);
    // prgMng().addFile(fileName);
    // _kernel= prgMng().getKernel(functionName)
    
    _initialized= true;
    return true;
}

bool Kernel::loadSource(QString source) 
{
    if(_initialized) {
        qDebug() << "A kernel is already loaded.";
        return false;
    }
    
    Q_UNUSED(source);
    
    // TODO
    // QMutexLocker locker(&_lock);
    // prgMng().addSource(source);
    // QString functionName = getKernelFunctionName(source);
    // _kernel = prgMng().getKernel(functionName);
    
    _initialized= true;
    return true;
}

template<typename T>
bool Kernel::setArg(int argIndex, T&& arg) 
{
    if(isNull()) {
        qDebug() << "No kernel loaded.";
        return false;
    }
    
    QMutexLocker locker(&_lock);
    
    cl_int err = clSetKernelArg(_kernel, argIndex, sizeof(T), (const void*)&arg);
    
    return checkCLError(err, "clSetKernelArg");
}

bool Kernel::setLayout(BlockDim blockDim, GridDim gridDim) 
{
    Q_UNUSED(blockDim); Q_UNUSED(gridDim);
    // TODO
    // _globalWorkSize[0] = 
    // _globalWorkSize[1] = 
    // _localWorkSize[0] = 
    // _localWorkSize[1] = 
    return true;
}

template<typename T>
void Kernel::getImageSize(T&&) { }

// TODO We could also check if the user specified at least one Image type!
template<>
void Kernel::getImageSize<Image>(Image&& arg) 
{
    Q_UNUSED(arg);
    // TODO: set the global_work_size from the size of the image and the local_work_size
    // _globalWorkSize[0] = roundUp(arg.width, _localWorkSize[0]);
    // _globalWorkSize[1] = roundUp(arg.height, _localWorkSize[1]);
}

template<int argN, typename First, typename... Rest>
bool Kernel::setArguments(First arg0, Rest... rest)
{
    cl_int err = clSetKernelArg(_kernel, argN, sizeof(First), (const void*)&arg0);
    getImageSize(arg0);
    return checkCLError(err, "clSetKernelArg") // TODO, we could show the index too (argN)
           and setArguments<argN+1>(rest...);
}

template<typename... Args>
bool Kernel::operator()(Args... args)
{
    if(isNull()) {
        qDebug() << "No kernel loaded.";
        return false;
    }

    QMutexLocker locker(&_lock);

    // 1) Set the kernel arguments 
    if(sizeof...(Args) != 0) {
        bool argSetOk= setArguments<0>(args...);
        if (!argSetOk) 
            return false;
    }
    
    // 2) Enqueue the kernel for execution
    cl_command_queue queue; // = TODO, queue from where?
    cl_int err = clEnqueueNDRangeKernel(queue, _kernel, layoutDim, nullptr, _globalWorkSize, _localWorkSize, 0, nullptr, nullptr);
    
    return checkCLError(err, "clEnqueuNDRangeKernel");
}


Kernel::~Kernel()
{
    QMutexLocker locker(&_lock);
    clReleaseKernel(_kernel);
}

} // namespace QCLI
