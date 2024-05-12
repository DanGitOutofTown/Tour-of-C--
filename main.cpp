#include <iostream>
#include <cassert>

#include "LogError.h"

using namespace std;

void test();

int main()
{

#if defined(NDEBUG) && defined(RELEASE_ASSERT)
    ReleaseAssert::ParseConfig("release_assert.ini");
#endif

    ErrorLogger::Init("error_logs/main.log");

    assert(!"main() message 1");

    ErrorLogger::LogError("An error message");

    test();

    cout << "Hello world!" << endl;

    return 0;
}
