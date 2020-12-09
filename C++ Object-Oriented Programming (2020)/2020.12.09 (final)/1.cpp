#include <iostream>

using namespace std;

int main() {
    // new dynamic array
    int *array = new int[10]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    for (int i = 0; i < 10; i++)
        cout << *(array + i) << endl;

    // delete array
    delete[] array;

    return 0;
}
