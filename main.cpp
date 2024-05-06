#include <iostream>
#include <cassert>

#include "LogError.h"

using namespace std;

void test();

int main()
{

#if defined(NDEBUG) && defined(F15ASSERT)
    F15Assert::behavior = F15Assert::Behavior::Popup;
#endif

    ErrorLogger::errFile = "error_logs/main.log";

    assert(!"main() message 1");
    
    ErrorLogger::LogError("An error message");

    test();

    cout << "Hello world!" << endl;

    return 0;
}
