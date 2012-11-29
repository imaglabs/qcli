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

#ifndef _QCLI_IMAGE_H
#define _QCLI_IMAGE_H

#include <QtCore>
#include <QImage>

#define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#include <CL/cl.h>

#include "ifmt.h"

namespace QCLI {

/** \brief Represents a QCLI image that has both a host and device version.
 *
 *  This class is *not* thread-safe.
 */

template<IFmt format>
class Image
{
public:
    /// Creates an empty image of a certain size
    Image(int width, int height, int devId= 0, bool setBlack=false, bool allocHost=false,
          bool allocDev=false);

    /// Creates an empty image of a certain size
    Image(QSize size, int devId= 0, bool setBlack=true, bool allocHost=false, bool allocDev=false)
        : Image(size.width(), size.height(), devId, setBlack, allocHost, allocDev) { }

    /// Creates and image from a QImage
    Image(QImage image, bool allocDev=false, int devId=0, bool upload=false, bool freeConvBuffer=false);

    ~Image();

    /// Load data from a QImage (must be of the same size)
    /// @retval false on error
    bool fromQImage(QImage image, bool upload= false, bool freeConvBuffer=false);

    int width() { return _width; }
    int height() { return _height; }

private:
    bool _allocHost();
    bool _allocDev();
    bool _allocConv();
    void _setBlack(bool host, bool dev);
    void _upload();
    void _download();

    // Host buffer
    char* _hostBuffer= nullptr;
    bool _hostValid= false;
    // Device buffer
    cl_mem _devBuffer= nullptr;
    bool _devValid= false;
    // Device conversion buffer (always of type ARGB)
    cl_mem _convBuffer= nullptr;

    // Image properties
    int _width;  // Forced init in the ctors
    int _height; // Forced init in the ctors
    int _devId;  // Forced init in the ctors
    int _bytes= 0;

    // Copy of the device queue (OpenCL calls using queue are thread-safe)
    cl_command_queue _queue= nullptr;
    // "origin and region" for the full image, used for OpenCL image operations
    size_t _origin[3] {0, 0, 0};
    size_t _region[3]; // Initialized in the ctors
};

} // namespace QCLI

#endif // _QCLI_IMAGE_H
