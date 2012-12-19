#include "ifmt.h"

#include <cassert>

namespace QCLI {

cl_image_format toCLFormat(IFmt format)
{
    cl_channel_order order= CL_ARGB;
    cl_channel_type type= CL_UNORM_INT8;

    switch(format) {
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

QImage::Format toQtFormat(IFmt format)
{
    // ARGB is the only QCLI format supported directly by QImage
    return format==IFmt::ARGB ? QImage::Format_ARGB32 : QImage::Format_Invalid;
}

IFmt fromQtFormat(QImage::Format src, bool* error)
{
    // ARGB32 and RGB32 are mapped to QCLI's ARGB
    if(error)
        *error= src!=QImage::Format_ARGB32 and src!=QImage::Format_RGB32;
    return IFmt::ARGB;
}


} // namespace QCLI
