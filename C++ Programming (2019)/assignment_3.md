# Ⅲ

## 第3题

定义一个复数类Complex，重载运算符“+”，使之能用于复数的加法运算。参加运算的两个运算量可以都是类对象，也可以其中有一个是整数，顺序任意。例如： c1+c2，c2+c1，c1+i均合法(设i为整数，c1,c2为复数）。编程序,分别求两个复数之和、整数和复数之和。

```C++
#include <iostream>
using namespace std;
class Complex
{
public:
    // Complex构造函数
    Complex():real(0), imag(0){}
    Complex(double r, double i): real(r), imag(i){}
    // Complex重载符（右边为Complex类）
    Complex operator+(Complex&);
    // Complex重载符（右边为整形）
    Complex operator+(int&);
    // Complex重载符（用友元函数重载）
    friend Complex operator+(int&,Complex&);
    // 重载cout输出，方便查看
    friend ostream& operator<<(ostream&, Complex&);
private:
    double real;
    double imag;
};

/* 下面是函数具体结构 */
Complex Complex::operator+(Complex &c)
{return Complex(real+c.real,imag+c.imag);}

Complex Complex::operator+(int &i)
{return Complex(real+i,imag);}

Complex operator+(int &i,Complex &c)
{return Complex(i+c.real,c.imag);}

ostream& operator<<(ostream& out, Complex& c)
{
    if (c.imag > 0)
        cout<<c.real<<"+"<<c.imag<<"i";
    else
        cout<<c.real<<c.imag<<"i";
    return out;
}

int main()
{
    // 定义测试对象，初始化值
    int i=10;
    Complex c1(1,2);
    Complex c2(3,-4);
    Complex c3;

    // 直接计算，然后输出
    c3=c1+c2;
    cout<<"c1+c2 = "<<c3<<endl;
    c3=i+c1;
    cout<<"i+c1 = "<<c3<<endl;
    c3=c1+i;
    cout<<"c1+i = "<<c3<<endl;
    return 0;
}
```

运行结果

```
c1+c2 = 4-2i
i+c1 = 11+2i
c1+i = 11+2i
```

## 第4题

有两个矩阵a和b，均为2行3列。求两个矩阵之和。重载运算符“+”,使之能用于矩阵相加。如：c = a+b。

```C++
#include <iostream>
using namespace std;
class Matrix
{
public:
    // 默认构造函数
    Matrix() = default;
    // 带参数的构造函数
    explicit Matrix(const int[2][3]);
    // 重载“+”操作符
    friend Matrix operator+(Matrix&, Matrix&);
    // 显示矩阵的函数
    void display();
private:
    // 私有二维数组变量
    int matrix[2][3]= {};
};

/* 函数具体内容 */
Matrix::Matrix(const int m[2][3])
{
    for(int i=0;i<2;i++)
        for(int j=0;j<3;j++)
            matrix[i][j] = m[i][j];
}

Matrix operator+(Matrix &a,Matrix &b)
{
    int m[2][3];
    for(int i=0;i<2;i++)
        for(int j=0;j<3;j++)
            m[i][j]=a.matrix[i][j]+b.matrix[i][j];
    return Matrix(m);
}

void Matrix::display()
{
    // 使用range-based for loop
    for(int i: {0,1})
    {
        for(int j: {0,1,2})
            cout<<matrix[i][j]<<" ";
        cout<<endl;
    }
}

int main()
{
    // 定义测试用的二维数组
    int a1[2][3] = {1,2,3,4,5,6};
    int b1[2][3] = {9,8,7,6,5,4};
    // 初始化a，b矩阵对象
    Matrix a(a1), b(b1);
    cout<<"a:"<<endl;
    a.display();
    cout<<"b:"<<endl;
    b.display();
    // 初始化c矩阵对象
    Matrix c = a+b;
    cout<<"c = a+b:"<<endl;
    c.display();
    return 0;
}
```

运行结果

```
a:
1 2 3 
4 5 6 
b:
9 8 7 
6 5 4 
c = a+b:
10 10 10 
10 10 10 
```

## 第5题

在第4题的基础上，重载流插人运算符“ << ”和流提取运算符“ >> ”，使之能用于该矩阵的输人和输出。

```C++
#include <iostream>
using namespace std;
class Matrix
{
public:
    // 默认构造函数
    Matrix() = default;
    // 带参数的构造函数
    explicit Matrix(const int[2][3]);
    // 重载“+”操作符
    friend Matrix operator+(Matrix&, Matrix&);
    // 重载“<<”和“>>”操作符
    friend ostream& operator<<(ostream&,Matrix&);
    friend istream& operator>>(istream&,Matrix&);
    // 显示矩阵的函数
    void display();
private:
    // 私有二维数组变量
    int matrix[2][3]= {};
};

/* 函数具体内容 */
Matrix::Matrix(const int m[2][3])
{
    for(int i=0;i<2;i++)
        for(int j=0;j<3;j++)
            matrix[i][j] = m[i][j];
}

Matrix operator+(Matrix &a,Matrix &b)
{
    int m[2][3];
    for(int i=0;i<2;i++)
        for(int j=0;j<3;j++)
            m[i][j]=a.matrix[i][j]+b.matrix[i][j];
    return Matrix(m);
}

istream& operator>>(istream &in,Matrix &m)
{
    for(int i: {0,1})
        for(int j: {0,1,2})
            in>>m.matrix[i][j];
    return in;
}

ostream& operator<<(ostream &out,Matrix &m)
{
    for(int i: {0,1})
    {
        for(int j: {0,1,2})
            out<<m.matrix[i][j]<<" ";
        out<<endl;
    }
    return out;
}

void Matrix::display()
{
    // 使用range-based for loop
    for(int i: {0,1})
    {
        for(int j: {0,1,2})
            cout<<matrix[i][j]<<" ";
        cout<<endl;
    }
}


int main()
{
    // 定义矩阵对象a, b, c
    Matrix a, b, c;
    // 输入数据
    cout<<"input value of matrix a:"<<endl;
    cin>>a;
    cout<<"input value of matrix b:"<<endl;
    cin>>b;
    // 空一行
    cout<<endl;
    // 直接打印矩阵内容
    cout<<"a:"<<endl<<a<<endl;
    cout<<"b:"<<endl<<b<<endl;
    // 初始化c矩阵对象，然后打印内容
    c = a+b;
    cout<<"c = a+b:"<<endl<<c<<endl;
    return 0;
}
```

运行结果

```
input value of matrix a:
1
3
6
4
3
7
input value of matrix b:
4
6
3
5
45
4

a:
1 3 6
4 3 7

b:
4 6 3
5 45 4

c = a+b:
5 9 9
9 48 11
```
