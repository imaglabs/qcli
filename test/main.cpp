#include <QtCore>
#include <QCLI>

using namespace std;
using namespace QCLI;

int main()
{
    qDebug() << "QCLI Test";

    auto& context= QCLI::ctx();
    context.init(CL_DEVICE_TYPE_GPU);
    qDebug() << context.initialized();

    qDebug() << "End";
    return EXIT_SUCCESS;
}
