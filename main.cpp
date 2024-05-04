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
#endif

    ErrorLogger::errFile = "error_logs/main.log";

    assert(!"main() message 1");
    
    ErrorLogger::LogError("An error message");

    test();

    return 0;
}
