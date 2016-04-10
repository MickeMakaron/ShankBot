#include <unistd.h>

#include <iostream>

#include <dlfcn.h>

pid_t getpid()
{

    pid_t (*originalGetPid)() = (pid_t (*)())dlsym(RTLD_NEXT, "getpid");

    std::cout << "Screw you, Lina! :D" << std::endl;
    return originalGetPid();
}
