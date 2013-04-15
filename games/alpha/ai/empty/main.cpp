#include <iostream>
using namespace std;
//writes three consequential numbers
int main() {
    int n, k = 0;
    cin >> n;
    while (k != n) {
        for (int i = 0; i < n; ++i)
            cout << (1 + rand()%n) << " ";
        cout << endl;
        cin >> k;
    }
}