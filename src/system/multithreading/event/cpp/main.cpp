#include <iostream>
#include "event.h"

using namespace std;

int main()
{
    Event myEvent(false);
    cout << myEvent.TryWaitFor() << endl;
    myEvent.Set();
    cout << myEvent.TryWaitFor() << endl;
    return 0;
}
