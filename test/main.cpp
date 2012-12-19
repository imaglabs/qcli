#include <QtCore>
#include <QCLI>

using namespace std;
using namespace QCLI;

int main()
{
    qDebug() << "QCLI Test";

    // Manually initialize the context
    //auto& context= QCLI::qcliCtx();
    //context.init();
    // ...or
    //qcliCtx().init();

    Image image("input.jpg");


    qDebug() << "End";
    return EXIT_SUCCESS;
}
