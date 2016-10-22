#include <cstdio>
#include "server.h"

int main(int argc, char** argv)
{
    Server* S1 = new Server(8001,NULL);

    S1->Start();
    S1->Wait();
}
