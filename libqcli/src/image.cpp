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

#include <assert.h>
#include "opencl/context.h"
#include "util/utils.h"

namespace QCLI {

//
// Constructors and destructor
//

template<IFmt format>
Image<format>::Image(int width, int height, bool setBlack, bool allocHost, bool allocDev)
    : _width(width), _height(height)
{
    assert(width > 0);
    assert(height > 0);
    assert(!setBlack or (allocHost or allocDev));

    if(allocDev)  _allocDev();
    if(allocHost) _allocHost();

    if(setBlack)
        _setBlack(allocHost, allocDev);
}

template<IFmt format>
Image<format>::Image(QImage image, bool allocDev, bool upload)
    : _width(image.width()), _height(image.height())
{    
    assert(!image.isNull());
    assert(!upload or allocDev);

    if(allocDev)
        _allocDev();
    fromImage(image, upload);
}

template<IFmt format>
Image<format>::~Image()
{
    if(_devBuffer) {
        clReleaseMemObject(_devBuffer);
        _devBuffer= nullptr;
    }
    free(_hostBuffer);
    _hostBuffer= nullptr;
}

//
// QImage interoperability
//

template<IFmt format>
bool Image<format>::fromImage(QImage image, bool upload)
{
    // Make sure the image is not null and is the correct size
    if(!image.isNull() or image.size() != QSize(_width, _height)) {
        qDebug() << "Invalid image";
        return false;
    }
    // Make sure the host buffer is allocated
    if(!_hostBuffer and !_allocHost())
        return false;

    // Convert image format if necessary
    if(image.format() != QImage::Format_ARGB32 or image.format() != QImage::Format_RGB32)
        image= image.convertToFormat(QImage::Format_ARGB32);

    // Check if we can memcpy or a conversion must be performed
    if(qtFormat(format) != QImage::Format_Invalid) {
        memcpy(_hostBuffer, image.constBits(), _bytes);
    } else {
        // TODO perform conversion (ARGB32 to format)
    }

    // Mark buffers
    _hostValid= true;
    _devValid= false;

    if(upload)
        _upload();

    return true;
}

template<IFmt format>
bool Image<format>::_allocHost()
{
    _hostValid= false;
    _bytes= _width * _height * iFmtBPP((ifmt_t)format);

    // Malloc/realloc host buffer
    _hostBuffer= static_cast<char*>(realloc(_hostBuffer, _bytes));
    if(!_hostBuffer) {
        qDebug() << "Could not alloc host buffer!";
        return false;
    }
    return true;
}

template<IFmt format>
bool Image<format>::_allocDev()
{
    _devValid= false;
    _bytes= _width * _height * iFmtBPP((ifmt_t)format);

    // Malloc / delete+malloc device buffer (no realloc in opencl)
    cl_int err;
    if(_devBuffer) {
        err= clReleaseMemObject(_devBuffer);
        checkCLError(err, "clReleaseMemObject");
    }
    _devBuffer= clCreateBuffer(clctx(), CL_MEM_READ_WRITE, _bytes, nullptr, &err);
    if(checkCLError(err, "clCreateBuffer")) {
        _devBuffer= nullptr;
        qDebug() << "Could not alloc dev buffer!";
        return false;
    }
    return true;
}

template<IFmt format>
void Image<format>::_setBlack(bool host, bool dev)
{
    if(!host and !dev)
        return;

    bool wroteHost= false;
    bool wroteDev= false;

    // Clear host memory
    if(host and _hostBuffer) {
        memset(_hostBuffer, 0, _bytes);
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
}

template<IFmt format>
void Image<format>::_upload()
{
    // The host buffer should exist
    assert(_hostBuffer);
    // Make sure the device buffer is allocated
    if(!_devBuffer) _allocDev();

    // Upload the buffer

    _devValid= true;
}

template<IFmt format>
void Image<format>::_download()
{
}


} // namespace QCLI
