#include <iostream>
#include <cassert>

#include "LogError.h"

using namespace std;

void test();

bool desktop = true;

int main()
{

#if defined(NDEBUG) && defined(F15ASSERT)
    if (desktop)
    {
        // Override f15assert.ini settings
        F15Assert::enableBehavior = true;
        F15Assert::behavior = F15Assert::Behavior::Popup;
        
        ErrorLogger::logErrors = false;
    }
#endif

    ErrorLogger::errFile = "error_logs/main.log";

    assert(!"main() message 1");

    ErrorLogger::LogError("An error message");

    test();

    cout << "Hello world!" << endl;

    return 0;
}
