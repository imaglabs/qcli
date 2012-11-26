#include <iostream> 
#include <context.h>

using namespace std;
using namespace QCLI;

int main() 
{
  
  cout << "QCLI Test" << endl;
  
  ContextRef context = Context::instance();
  context.init();
  
  cout << "End." << endl;
  
  return EXIT_SUCCESS;
}
