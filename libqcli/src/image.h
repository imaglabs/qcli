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
#include <CL/cl.h>

#include "ifmt.h"

namespace QCLI {

/// \brief QCLI Image. Represents an image with a host and device version
/// This class is *not* thread-safe.

template<IFmt format>
class Image
{
public:
    /// Creates an empty image of a certain size
    Image(int width, int height, bool setBlack=false, bool allocHost=false,
          bool allocDev=false);

    /// Creates an empty image of a certain size
    Image(QSize size, bool setBlack=true, bool allocHost=false, bool allocDev=false)
        : Image(size.width(), size.height(), setBlack, allocHost, allocDev) { }

    /// Creates and image from a QImage
    Image(QImage image, bool allocDev=false, bool upload= false);

    ~Image();

    /// Load data from a QImage (must be of the same size)
    /// @retval false on error
    bool fromImage(QImage image, bool upload= false);

    int width() { return _width; }
    int height() { return _height; }

private:
    bool _allocHost();
    bool _allocDev();
    void _setBlack(bool host, bool dev);
    void _upload();
    void _download();

    // Host buffer
    cl_mem _devBuffer= nullptr;
    bool _devValid= false;
    // Device buffer
    char* _hostBuffer= nullptr;
    bool _hostValid= false;

    // Image properties
    int _bytes= 0;
    int _width= 0;
    int _height= 0;
};

} // namespace QCLI

#endif // _QCLI_IMAGE_H
