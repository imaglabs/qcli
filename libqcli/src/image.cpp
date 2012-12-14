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

#include <cassert>
#include "opencl/context.h"
#include "opencl/devicemanager.h"
#include "util/utils.h"

namespace QCLI {

//
// Constructors and destructor
//

template<IFmt format>
Image<format>::Image(QImage image, bool allocDev, int devId, bool upload, bool freeConvBuffer)
    : Image(image.width(), image.height(), devId, false, false, allocDev)
{
    assert(!image.isNull());
    assert(!upload or allocDev);
    fromQImage(image, upload, freeConvBuffer);
}

template<IFmt format>
Image<format>::Image(int width, int height, int devId, bool setBlack, bool allocHost, bool allocDev)
    : _width(width), _height(height), _devId(devId)
{
    assert(width > 0);
    assert(height > 0);
    assert(!setBlack or (allocHost or allocDev));
    // Make sure the context is initialized so the device queue is ready
    if(!ctx().initialized()) ctx().init();
    // Get the device queue and verify devId at the same time
    _queue= devMgr().queue(devId)
    assert(_queue);
    // Verify the image format is supported
    assert(ctx().supportedFormat(toCLFormat<format>()));

    // Use {} ctor when QtCreator parses it ok...
    _region[0]= width;
    _region[1]= height;
    _region[2]= 1;

    if(allocDev)  _allocDev();
    if(allocHost) _allocHost();

    if(setBlack)
        _setBlack(allocHost, allocDev);
}

template<IFmt format>
Image<format>::~Image()
{
    if(_devBuffer)
        clReleaseMemObject(_devBuffer);
    free(_hostBuffer);
}

//
// QImage interoperability
//

template<IFmt format>
bool Image<format>::fromQImage(QImage image, bool upload)
{
    // Make sure the image is not null and is the correct size
    if(!image.isNull() or image.size() != QSize(_width, _height)) {
        qDebug() << "Invalid image";
        return false;
    }
    // Make sure the host buffer is allocated
    if(!_hostBuffer and !_allocHost())
        return false;

    // Make sure the QImage is ARGB32 or RGB32
    if(image.format() != QImage::Format_ARGB32 or image.format() != QImage::Format_RGB32)
        image= image.convertToFormat(QImage::Format_ARGB32);

    // Check if we can memcpy or a conversion must be performed
    if(toQtFormat<format>() != QImage::Format_Invalid) {
        memcpy(_hostBuffer, image.constBits(), _bytes);
        _hostValid= true;
        _devValid= false;
        if(upload)
            _upload();
        return true;
    }
    // Map QImage to the GPU to perform a conversion

    // Make sure the device buffer is allocated
    if(!_devBuffer and !_allocDev())
        return false;

    // Upload QImage data to conversion format
    cl_int err;
    auto qimageFormat= toCLFormat<IFmt::ARGB>();
    cl_mem convBuffer= clCreateImage2D(clctx(), CL_MEM_READ_ONLY, &qimageFormat, _width,
                                       _height, _width*4, image.constBits(), &err);
    if(checkCLError(err, "clCreateImage2D"))
        return false;

    // Convert QImage to the desired format
    // TODO implement, make sure the conversion is blocking (the upload isn't)

    // Unmap
    err= clReleaseMemObject(convBuffer);
    if(checkCLError(err, "clReleaseMemObject"))
        return false;

    // Now _devBuffer has the valid image, no uploading is necessary
    _devBuffer= true;
    _hostBuffer= false;

    return true;
}

template<IFmt format>
bool Image<format>::_allocHost()
{
    _hostValid= false;
    _bytes= _width * _height * iFmtBPP(format);

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
    _bytes= _width * _height * iFmtBPP(format);

    // Malloc / delete+malloc device buffer (no realloc in opencl)
    cl_int err;
    if(_devBuffer) {
        err= clReleaseMemObject(_devBuffer);
        checkCLError(err, "clReleaseMemObject");
    }
    auto clFormat= toCLFormat<format>();
    _devBuffer= clCreateImage2D(clctx(), CL_MEM_READ_WRITE, &clFormat, _width, _height,
                                0, nullptr, &err);
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
        cl_int err;
        #ifdef CL_VERSION_1_2
            // TODO test
            err= clEnqueueFillImage(_queue, _devBuffer, clFillingBlack().data(),
                                    _origin, _region, 0, nullptr, nullptr);
            checkCLError(err, "clEnqueueFillImage");
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
