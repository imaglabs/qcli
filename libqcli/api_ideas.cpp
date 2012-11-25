// libQCLI API ideas...

// No-extension header including all public QCLI classes
#include <QCLI>
#include <QtGui>

// Creates an image from a PNG file
// Image(QString path, bool allocateAll=false);
// allocateAll allocates both the CPU and GPU buffers and uploads the image
QCLI::Image input("input.png");
if(image.isNull()) ... // The image could not be loaded

// Creates an empty image from a QSize
// Image(QSize size, Image::Format format= Image::Format::RGB24, bool blank= false);
QCLI::Image output(input.size());

// Generates a perPixel operation for 2 images of the same size
// Try to put the static kernel parameters (number of arguments) as template parameters
auto kernel= QCLI::Kernel::perPixel<2>("y= 2.0f * x;");
if(kernel.isNull()) ... // The compilation failed
kernel(input, output);
// one-liner version, see if a hash can be used in the kernel manager to only have one instance
QCLI::Kernel::perPixel("y= 2.0f * x").run(input, output);

// Kernel as a variadic template for the kernel parameters?
auto kernel= QCLI::Kernel<2>(R"(
    ...
    )");
kernel(param1, param2);

// maybe make Kernel a variadic template and pass all argtypes as template params?
auto kernel= QCLI::Kernel<QCL::Image, QCL::Image>(R"(
    full kernel definition, including header
    )");

// Other idea
// See if it is possible to pass (class T, string N) * n template params, else use some sort of QPair o "KArg<class T, string N>"
// Texture will be some sort of typdef to define that that parameter is __read_only
auto kernel= Kernel<Texture, "input", Image, "output">(R"(
    // Only type the kernel body
    const sampler_t sampler= CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_BILINEAR;
    float3 value= read_imagef(input, sampler, (int2)(x  , y-1)).xyz;
    write_imagef(output, (int2)(x,y), value);
)");

The kernel header can be auto-generated:
    __kernel void fdmHeat(
        __read_only image2d_t input,
        __write_only image2d_t output)
    {

// The template parameters can be used to extract static type information, and to define a single run/operator() function
kernel(Texture, Image);
        

// Loads image from a QImage and changes all the parameters (size, remalloc, etc.) as needed
// if neccessary discard the pimpl and create a new one with the new params
input.fromImage(QImage("image.png"));
input.load("image.png"); // Same as input.fromImage(QImage("image.png")), allows to add other formats in the future

