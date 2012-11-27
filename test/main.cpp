#include <iostream> 
#include <context.h>

using namespace std;
using namespace QCLI;

int main() 
{
  
  cout << "QCLI Test" << endl;
  
  auto context= QCLI::ctx();
  context.init();
  
  cout << "End." << endl;
  
  return EXIT_SUCCESS;
}
