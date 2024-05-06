#include <cassert>

#include "LogError.h"

void test()
{
    assert(!"test() message 1");

    ErrorLogger::LogError("My error message!");
}