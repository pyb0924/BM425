#include <iostream>
#include <mysqlx/xdevapi.h>

using namespace mysqlx;


int main(int argc, const char* argv[])
{
    // Connect to MySQL Server on a network machine
    string user = "pyb0924";
    string password = "peter2000";
    Session mySession("localhost", 33060, user,password);

    Schema myDb = mySession.getSchema("test");
}