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
auto kernel= QCLI::Kernel<2>(R"
    ...
    ");
kernel(param1, param2);

// maybe make Kernel a variadic template and pass all argtypes as template params?
auto kernel= QCLI::Kernel<int, QCL::Image, QSize>(R("
    ...
    "));


// Loads image from a QImage and changes all the parameters (size, remalloc, etc.) as needed
// if neccessary discard the pimpl and create a new one with the new params
input.fromImage(QImage("image.png"));
input.load("image.png"); // Same as input.fromImage(QImage("image.png")), allows to add other formats in the future

