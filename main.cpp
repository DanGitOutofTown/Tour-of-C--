

#include <iostream>
#define EXPECT_MAIN
#include "expect.h"

using namespace std;
void test();

int main()
{
    cout << "Hello world!" << endl;
    Expect::expectConfig = Expect::ExpectBoth;
    test();
    Expect::expect(3 * 4 + 1 == 7 * 2);
    return 0;
}
