#include <iostream>

using namespace std;

class Complex {
  public:
    Complex() : real(0), imag(0) {}
    Complex(double r, double i) : real(r), imag(i) {}

    Complex operator+(Complex &);
    Complex operator+(int &);

    Complex operator-(Complex &);
    Complex operator-(int &);

    friend ostream &operator<<(ostream &, Complex &);

  private:
    double real;
    double imag;
};

Complex Complex::operator+(Complex &c) { return Complex(real + c.real, imag + c.imag); }

Complex Complex::operator+(int &i) { return Complex(real + i, imag); }

Complex Complex::operator-(Complex &c) { return Complex(real - c.real, imag - c.imag); }

Complex Complex::operator-(int &i) { return Complex(real - i, imag); }

ostream &operator<<(ostream &out, Complex &c) {
    if (c.imag > 0)
        cout << c.real << "+" << c.imag << "i";
    else
        cout << c.real << c.imag << "i";
    return out;
}

int main() {
    int i = 10;
    Complex c1(1, 2);
    Complex c2(3, -4);
    Complex c3(7, -9);
    Complex tmp;

    cout << "i = " << i << endl;
    cout << "c1 = " << c1 << endl;
    cout << "c2 = " << c2 << endl;
    cout << "c3 = " << c3 << endl;

    tmp = c1 + c2;
    cout << "c1 + c2 = " << tmp << endl;

    tmp = c1 + i;
    cout << "c1 + i = " << tmp << endl;

    tmp = c3 - c2 + c1;
    cout << "c3 - c2 + c1 = " << tmp << endl;

    tmp = c1 + c2 - i;
    cout << "c1 + c2 - i = " << tmp << endl;

    return 0;
}
