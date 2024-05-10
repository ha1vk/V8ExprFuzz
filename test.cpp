#include <iostream>
#include "generator.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/resource.h>

using namespace std;
using namespace Generator;

int main(int argc, char *argv[])
{
    int fd = open(argv[1],0);
    int len = lseek(fd,0,SEEK_END);
    char *buf = (char *)malloc(len);
    lseek(fd,0,SEEK_SET);
    read(fd,buf,len);
    close(fd);
    Generator::init();
    Engine *engine = new Engine();
    engine->generate(buf, len);
    string s = engine->operator string();
    cout << s << endl;
    delete engine;
    return 0;
}
/*Engine *engine = NULL;
extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    Generator::init();
    engine = new Engine();
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    engine->generate(Data, Size);
    string s = engine->operator string();
    cout << s << endl;
    return 0;
}*/