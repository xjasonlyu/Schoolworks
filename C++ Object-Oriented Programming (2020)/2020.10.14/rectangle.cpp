#include <iostream>

using namespace std;

class Rectangle
{
    // 公共函数
public:
    void set_values(double, double);
    void show_square();

    // 私有成员变量
private:
    // 初始化成员变量
    double hight = 0;
    double width = 1.0;
    // 该函数只做简单的乘法运算，
    // 所以可以用static函数声明
    static double calc_square(double, double);
};

double Rectangle::calc_square(double a, double b)
{
    return a * b;
}

void Rectangle::set_values(double x, double y)
{
    hight = x;
    width = y;
}

void Rectangle::show_square()
{
    double s;
    // 计算体积并赋值给volume变量
    s = calc_square(hight, width);
    // 输出体积到终端
    cout << "Rectangle Square is: " << s << endl;
}

int main()
{
    Rectangle r;
    r.set_values(10, 34);
    r.show_square();
    return 0;
}