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

#ifndef _QCLI_IFMT_H
#define _QCLI_IFMT_H

#include <CL/cl.h>
#include <cstdint>
#include <QImage>

namespace QCLI {

/// Image format enum type, packs format information such as BPP and channel count.
typedef uint32_t ifmt_t;

/// Packs an image format
constexpr inline
ifmt_t iFmtPack(uint8_t type, uint8_t bpp, uint8_t chanCount)
    { return (type << 16) | (bpp << 8) | chanCount; }

/// Unpack format type from an ifmt_t
constexpr inline
uint8_t iFmtType(ifmt_t format)
    { return (format >> 16) & 0xFF; }
/// Unpack format BPP from an ifmt_t
constexpr inline
uint8_t iFmtBPP(ifmt_t format)
    { return (format >> 8) & 0xFF; }
/// Unpack format chan count from an ifmt_t
constexpr inline
uint8_t iFmtChanCount(ifmt_t format)
    { return format & 0xFF; }

/// Strict enum of image formats supported as template parameter of Image
enum class IFmt : ifmt_t
{
    ARGB    = iFmtPack(0,  24, 4), /// ARGB:  8-bit unsigned integer [0..255]
    ARGB16  = iFmtPack(1,  48, 4), /// ARGB: 16-bit unsigned integer [0..32768]
    ARGB16F = iFmtPack(2,  48, 4), /// ARGB: 16-bit half-float       [0..1]
    ARGB32F = iFmtPack(3, 128, 4), /// ARGB: 32-bit float            [0..1]
    LUMA    = iFmtPack(4,   8, 1), /// Luma:  8-bit unsigned integer [0..255]
    LUMA16  = iFmtPack(5,  16, 1), /// Luma: 16-bit unsigned integer [0..32768]
    LUMA16F = iFmtPack(6,  16, 1), /// Luma: 16-bit half-float       [0..1]
    LUMA32F = iFmtPack(7,  32, 1)  /// Luma: 32-bit float            [0..1]
};

//
// Format equivalents: QCLI/QImage/OpenCL
//

/// All QCLI formats are valid OpenCL formats
cl_image_format toCLFormat(IFmt fmt);
/// @retval QImage::Format_Invalid if there is not a valid equivalent
QImage::Format toQtFormat(IFmt fmt);
/// @retval false if there is not valid equivalent
bool fromQtFormat(QImage::Format src, IFmt& dst);

} // namespace QCLI

#endif // _QCLI_IFMT_H
