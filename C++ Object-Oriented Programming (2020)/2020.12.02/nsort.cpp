#include <iostream>
#include <string>

using namespace std;

template <class T>
void get_objects(T *t, int n) {
    for (int i = 0; i < n; i++)
        cin >> *(t++);
}

template <class T>
void show_objects(T *t, int n) {
    for (int i = 0; i < n; i++)
        cout << *(t++) << "\t";
    cout << endl;
}

void int2double(int *a, double *b, int n) {
    for (int i = 0; i < n; i++)
        *(b++) = (double)(*(a++));
}

void string2double(string *a, double *b, int n) {
    for (int i = 0; i < n; i++)
        *(b++) = atof((a++)->c_str());
}

int main() {
    int n = 0;
    cout << "input n: ";
    cin >> n;

    double a[n * 3];
    int a1[n];
    // double a2[n];
    string a3[n];

    get_objects(a1, n);
    get_objects(a + n, n);
    get_objects(a3, n);

    int2double(a1, a, n);
    string2double(a3, a + n * 2, n);

    // show_objects(a1, n);
    // show_objects(a2, n);
    // show_objects(a3, n);
    // show_objects(a, n * 3);

    sort(a, a + (n * 3));

    show_objects(a, n * 3);

    return 0;
}
