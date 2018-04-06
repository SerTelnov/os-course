#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
    freopen(argv[1], "r", stdin);
    char c;
    int cnt = 0;
    while (cin >> c) {
        if (c == 'a') {
            ++cnt;
        }
    }
    cout << cnt;
    return 0;
}