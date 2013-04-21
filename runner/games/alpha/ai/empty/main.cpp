#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <Windows.h>
using namespace std;
//writes three consequential numbers
int counter = 1, new_count = 0;
bool p(int *array_param, int pos, int depth) {
    if (pos == depth) {
        return true;
    }
    for (int i = 1; i <= depth; i++) {
        array_param[pos] = i;
        if (depth - 1 == pos) {
            new_count++;
        }
        if (new_count == counter) {
            new_count = 0;
            counter++;
            return false;
        }
        if (!p(array_param, pos + 1, depth)) {
            return false;
        }
    }
    return true;
}
int main() {
    //srand(time(NULL) - 1000 + GetCurrentProcessId());
    int n, k = 0;
    int array[512];
    cin >> n;
    while (k != n) {
        p(array, 0, n);
        for (int i = 0; i < n; ++i)
            cout << array[i]/*(1 + rand()%n)*/ << " ";
        cout << endl;
        cin >> k;
    }
    return 0;
}