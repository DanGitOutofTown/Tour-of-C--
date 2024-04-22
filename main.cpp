

#include <iostream>
#include "expect.h"

using namespace std;

int main()
{
    cout << "Hello world!" << endl;
    expect(3 * 4 + 1 == 7 * 2);
    return 0;
}
