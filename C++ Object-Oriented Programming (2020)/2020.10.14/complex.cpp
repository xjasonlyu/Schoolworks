#include <iostream>

using namespace std;

class Complex
{
public:
    // Complex构造函数
    Complex() : real(0), imag(0) {}
    Complex(double r) : real(r) {}
    Complex(double r, double i) : real(r), imag(i) {}
    void show();
    void add(Complex &);

private:
    double real;
    double imag;
};

void Complex::show()
{
    std::cout << real << "+" << imag << "i" << std::endl;
}

void Complex::add(Complex &c)
{
    real += c.real;
    imag += c.imag;
}

int main()
{
    // 定义测试对象，初始化值
    Complex c1(3, 5);
    Complex c2(4);

    // 直接计算，然后输出
    c1.add(c2);
    c1.show();

    return 0;
}
