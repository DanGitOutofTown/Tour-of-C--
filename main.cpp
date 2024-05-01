#include <iostream>

#include "assert.h"
#include "LogError.h"

using namespace std;

void test();

int main()
{
    cout << "Hello world!" << endl;

#ifdef USE_RELEASEBUILD_ASSERT
    assertBehavior = AssertBehavior::Popup;
#endif

    errorLogFile = "assert_logs/assert.log";

    test();

    assert(!"This is a test");
    assert(!"This is a test");
    assert(!"main() message 1");

    return 0;
}
