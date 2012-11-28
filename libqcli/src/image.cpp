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

#include "image.h"
#include "opencl/context.h"
#include "util/utils.h"

namespace QCLI {

template<ImageFmt format>
Image<format>::~Image()
{
    if(_devBuffer) {
        clReleaseMemObject(_devBuffer);
        _devBuffer= nullptr;
    }
    free(_hostBuffer);
    _hostBuffer= nullptr;
}

template<ImageFmt format>
Image<format>::Image(int width, int height, bool setBlack, bool allocHostMem, bool allocDevMem)
    : _width(width), _height(height)
{
    assert(width > 0);
    assert(height > 0);

    if(allocDevMem)  allocDev();
    if(allocHostMem) allocHost();

    if(setBlack)
        clear(allocHostMem, allocDevMem);
}

template<ImageFmt format>
Image<format>::Image(QImage image, bool allocHost=false, bool allocDev=false)
{
}

template<ImageFmt format>
bool Image<format>::allocHost()
{
    QMutexLocker locker(&_lock);

    _hostValid= false;
    _hostSize= _width * _height * iFmtBPP(format);

    // Malloc/realloc host buffer
    _hostBuffer= static_cast<char*>(realloc(_hostBuffer, _hostSize));
    if(!_hostBuffer) {
        _devSize= -1;
        qDebug() << "Could not alloc host buffer!";
        return false;
    }
    return true;
}

template<ImageFmt format>
bool Image<format>::allocDev()
{
    QMutexLocker locker(&_lock);

    _devValid= false;
    _devSize= _width * _height * iFmtBPP(format);

    // Malloc / delete+malloc device buffer (no realloc in opencl)
    cl_int err;
    if(_devBuffer) {
        err= clReleaseMemObject(_devBuffer);
        checkCLError(err, "clReleaseMemObject");
    }
    _devBuffer= clCreateBuffer(clctx(), CL_MEM_READ_WRITE, _devSize, nullptr, &err);
    if(checkCLError(err, "clCreateBuffer")) {
        _devSize= -1;
        qDebug() << "Could not alloc dev buffer!";
        return false;
    }
    return true;
}

template<ImageFmt format>
void Image<format>::clear(bool host, bool dev)
{
    if(!host and !dev)
        return;

    QMutexLocker locker(&_lock);
    bool wroteHost= false;
    bool wroteDev= false;

    // Clear host memory
    if(host and _hostBuffer) {
        memset(_hostBuffer, 0, _hostSize);
        wroteHost= true;
    }
    // Clear dev memory
    if(dev and _devBuffer) {
#ifdef CL_VERSION_1_2
        // TODO implement using clEnqueueFillImage()
#else
        // TODO implement with kernel!
#endif
        wroteDev= true;
    }

    _hostValid= wroteHost;
    _devValid= wroteDev;

    if(wroteHost and wroteDev) {
        _hostValid= true;
        _devValid= true;
    }
}

} // namespace QCLI
