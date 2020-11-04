#include <iostream>

using namespace std;

class Matrix {
   public:
    Matrix(int *a, int len) {
        arr = new int[10][10];
        length = len;
        int k = 0;
        for (int i = 0; i < len; i++)
            for (int j = 0; j < len; j++)
                arr[i][j] = a[k++];
    }

    Matrix(Matrix &mat) {
        arr = new int[10][10];
        length = mat.length;
        for (int i = 0; i < length; i++)
            for (int j = 0; j < length; j++)
                arr[i][j] = mat.arr[i][j];
    }

    void add(Matrix &mat) {
        for (int i = 0; i < mat.length; i++)
            for (int j = 0; j < mat.length; j++)
                arr[i][j] += mat.arr[i][j];
    }

    void show() {
        for (int i = 0; i < length; i++) {
            for (int j = 0; j < length; j++)
                cout << "\t" << arr[i][j];
            cout << "\n";
        }
        cout << "\n";
    }

   private:
    int (*arr)[10];
    int length;
};

int main() {
    int a[9] = {1, 2, 3, 2, 3, 4, 3, 4, 5};
    Matrix mat1(a, 3);
    mat1.show();

    Matrix mat2(mat1);
    mat2.show();

    mat1.add(mat2);
    mat1.show();
    return 0;
}
