#include <iostream>
#include "assert.h"

#include "LogError.h"

using namespace std;

void test();

int main()
{
    cout << "Hello world!" << endl;

#if defined(NDEBUG) && defined(USE_F15_ASSERT)
    F15Assert::behavior = F15Assert::Behavior::Popup;
    F15Assert::logAsserts = true;
#endif

    errorLogFile = "error_logs/main.log";
    logErrors = true;

    test();

    assert(!"main() message 1");
    
    LogError("An error message");

    return 0;
}
