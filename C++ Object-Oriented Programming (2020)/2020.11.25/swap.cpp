#include <iostream>
#include <string>

using namespace std;

template <class T>
void _swap(T &x, T &y) {
    T t;
    t = x;
    x = y;
    y = t;
}

int main() {
    int a = 22;
    int b = 33;
    cout << "a: " << a << " b: " << b << endl;
    _swap(a, b);
    cout << "a: " << a << " b: " << b << endl;

    float s = 5.44;
    float t = 7.88;
    cout << "s: " << s << " t: " << t << endl;
    _swap(s, t);
    cout << "s: " << s << " t: " << t << endl;

    string mr = "tttttttt";
    string ms = "wwwwwwww";
    cout << "mr: " << mr << " ms: " << ms << endl;
    _swap(mr, ms);
    cout << "mr: " << mr << " ms: " << ms << endl;

    return 0;
}
