# Ⅰ

## 第一题 

### time.cpp

```C++
#include <iostream>

using namespace std;

class Time
{
// 以下两个成员函数应该为公共函数
public:
    // 函数不需要参数时可以省去(void)
    void set_time();
    void show_time();

// 以下三个变量可以设为私有
private:
    // 初始化成员变量，防止为定义行为发生
    int hour=0;
    int minute=0;
    int sec=0;
};

// 把Time类的实例t声明在主函数内更合适
// Time t;

int main() {
    // 内部声明Time类t实例变量
    Time t;
    t.set_time();
    t.show_time();
    return 0;
}

// 函数返回值类型应该为：void
// Time类成员函数前应用'::'指明类型
void Time::set_time()
{
    // t变量实例声明在主函数体内，类成员函数可以直接
    // 访问自身的私有变量，所以直接省略"t."
    cin>>hour;
    cin>>minute;
    cin>>sec;
}

// 函数返回值类型应该为：void
// Time类成员函数前应用'::'指明类型
void Time::show_time()
{
    // t变量实例声明在主函数体内，类成员函数可以直接
    // 访问自身的私有变量，所以直接省略"t."
    cout<<hour<<":"<<minute<<":"<<sec<<endl;
}
```

## 第二题

### cuboid.cpp

```C++
#include <iostream>

using namespace std;

class Cuboid
{
// 公共函数
public:
    void set_variables();
    void show_volume();

// 私有成员变量
private:
    // 初始化成员变量
    // 因为长方体的长宽高可能为小数
    // 所以此处应用浮点类型，double
    double length=0;
    double width=0;
    double height=0;
    // 该函数只做简单的乘法运算，
    // 所以可以用static函数声明
    static double calc_volume(double, double, double);
};

// set_variables()函数获取
// 长宽高变量值并赋值给相应的变量
void Cuboid::set_variables()
{
    // 提示需要的变量值
    cout<<"length: ";
    // 获取相应的变量值，下同
    cin>>length;
    cout<<"width: ";
    cin>>width;
    cout<<"height: ";
    cin>>height;
}

// calc_volume()函数计算相应的长方体体积
double Cuboid::calc_volume(double a, double b, double c)
{
    return a*b*c;
}

// show_volume()函数先计算长方体体积后输出
void Cuboid::show_volume()
{
    double volume; // 定义volume体积内部变量
    // 计算体积并赋值给volume变量
    volume = calc_volume(length, width, height);
    // 输出体积到终端
    cout<<"Cuboid Volume is: "<<volume<<endl;
}

int main() {
    // 声明Cuboid类c实例变量
    Cuboid c;
    // 调用Cuboid的set_variables()方法获取输入
    c.set_variables();
    // 调用Cuboid的show_volume()方法输出体积
    c.show_volume();
    // 结束程序，成功返回0
    return 0;
}
```
