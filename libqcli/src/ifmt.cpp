#include "ifmt.h"

#include <cassert>

namespace QCLI {

template<IFmt fmt> constexpr
cl_image_format toCLFormat()
{
    cl_channel_order order= CL_ARGB;
    cl_channel_type type= CL_UNORM_INT8;

    switch(fmt) {
        case IFmt::ARGB:    order= CL_ARGB;      type= CL_UNORM_INT8;  break;
        case IFmt::ARGB16:  order= CL_ARGB;      type= CL_UNORM_INT16; break;
        case IFmt::ARGB16F: order= CL_ARGB;      type= CL_HALF_FLOAT;  break;
        case IFmt::ARGB32F: order= CL_ARGB;      type= CL_FLOAT;       break;
        case IFmt::LUMA:    order= CL_LUMINANCE; type= CL_UNORM_INT8;  break;
        case IFmt::LUMA16:  order= CL_LUMINANCE; type= CL_UNORM_INT16; break;
        case IFmt::LUMA16F: order= CL_LUMINANCE; type= CL_HALF_FLOAT;  break;
        case IFmt::LUMA32F: order= CL_LUMINANCE; type= CL_FLOAT;       break;
    }

    cl_image_format ret;
    ret.image_channel_data_type= type;
    ret.image_channel_order= order;
    return ret;
}

template<IFmt fmt> constexpr
QImage::Format toQtFormat()
{
    // ARGB is the only QCLI format supported directly by QImage
    return fmt==IFmt::ARGB ? QImage::Format_ARGB32 : QImage::Format_Invalid;
}

bool fromQtFormat(QImage::Format src, IFmt& dst)
{
    // ARGB32 and RGB32 are mapped to QCLI's ARGB
    dst= IFmt::ARGB;
    return src==QImage::Format_ARGB32 or src==QImage::Format_RGB32;
}


} // namespace QCLI
