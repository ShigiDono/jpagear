#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <Windows.h>
using namespace std;
//writes three consequential numbers
int main() {
    srand(time(NULL) - 1000 + GetCurrentProcessId());
    int n, k = 0;
    cin >> n;
    while (k != n) {
        for (int i = 0; i < n; ++i)
            cout << (1 + rand()%n) << " ";
        cout << endl;
        cin >> k;
    }
    return 0;
}