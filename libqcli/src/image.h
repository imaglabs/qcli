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

namespace QCLI {

#include <QtCore>
#include <CL/cl.h>
#include <stdint.h>

/// Image format enum type, packs format information such as BPP and channel count.
typedef uint32_t ifmt_t;

/// Function to pack an image forma
constexpr ifmt_t iFmtPack(uint8_t type, uint8_t bpp, uint8_t chanCount)
    { return (type << 16) | (bpp << 8) | chanCount; }
/// Unpack format type from an ifmt_t
constexpr uint8_t iFmtType(ifmt_t format)
    { return (format >> 16) & 0xFF; }
/// Unpack format BPP from an ifmt_t
constexpr uint8_t iFmtBPP(ifmt_t format)
    { return (format >> 8) & 0xFF; }
/// Unpack format chan count from an ifmt_t
constexpr uint8_t iFmtChanCount(ifmt_t format)
    { return format & 0xFF; }

/// Strict enum of image formats supported as template parameter of Image
enum class ImageFmt : ifmt_t {
    RGBA24= iFmtPack(0, 24, 4),
    LUMA8 = iFmtPack(1,  8, 1)
};

/// \brief QCLI Image. Represents an image with a host and device version

template<ImageFmt format>
class Image
{
public:
    /// Creates an empty image of a certain size
    Image(int width, int height, bool setBlack=true, bool allocHostMem=false,
          bool allocDevMem=false);
    /// Creates an empty image of a certain size
    Image(QSize size, bool setBlack=true, bool allocHostMem=false, bool allocDevMem=false)
        : Image(size.width(), size.height(), setBlack, allocHostMem, allocDevMem) { }
    /// Creates and image from a QImage
    Image(QImage image, bool allocHost=false, bool allocDev=false);

    ~Image();

private:
    bool allocHost();
    bool allocDev();
    void clear(bool host, bool dev);

    // State
    mutable QMutex _lock;

    // Host buffer
    cl_mem _devBuffer= nullptr;
    int _devSize= 0;
    bool _devValid= false;
    // Device buffer
    char* _hostBuffer= nullptr;
    int _hostSize= 0;
    bool _hostValid= false;

    // Image properties
    int _width;
    int _height;
};

} // namespace QCLI

#endif // _QCLI_IMAGE_H
