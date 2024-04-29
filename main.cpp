

#include <iostream>

#define ASSERT_MAIN
#include "assert.h"

using namespace std;
void test();

int main()
{
    cout << "Hello world!" << endl;

    assertBehavior = AssertBehavior::None;
    assertLogFile = "assert_logs/assert.log";

    test();

    assert(!"This is a test");
    assert(!"This is a test");
    assert(!"main() message 1");

    return 0;
}
