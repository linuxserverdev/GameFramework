#include <iostream>
#include <google/once.h>
#include <thread>
#include <zconf.h>


void print()
{
    std::cout << "init once!\n";
}
GOOGLE_PROTOBUF_DECLARE_ONCE(onceVar);

void onceFunction()
{
    GoogleOnceInit(&onceVar, &print);
}


int main()
{

    for (int i = 0; i < 10; ++i)
    {
        std::thread(onceFunction).detach();
    }
    sleep(5);
    return 0;
}