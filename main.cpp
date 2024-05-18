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

    ErrorLogger::Init("error_logs/main.log", "AVI", "error_logger.ini");

    assert(!"main() message 1");

    // Will only log if ErrorLogger::Init() has been called in main executable
    // To be replaced with TrainerLog::Msg:Error which in turn will call LogError()
    // and also output to LogFileViewer file.
    ErrorLogger::LogError("An error message");

    test();

    cout << "Hello world!" << endl;

    return 0;
}
