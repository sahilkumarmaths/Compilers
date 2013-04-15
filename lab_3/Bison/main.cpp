#include <iostream>
using namespace std;
extern int yyparse();

int main(int argc, char **argv)
{
    yyparse();
    cout<<"accepted!!!\n";
    return 0;
}
