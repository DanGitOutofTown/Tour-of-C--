#include "assert.h"
#include "LogError.h"

void test()
{
    assert(!"test() message 1");

    LogError("My error message!");
}