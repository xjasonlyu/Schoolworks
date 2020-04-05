# Ⅴ

## 第1题

写一个程序，定义抽象基类Shape，由它派生出2个派生类：Circle（圆形）、Rectangle（矩形）。

- 用一个函数printArea()分别输出以上二者的面积
- 用一个函数ShapeName()分别输出以上二者的名称
- 两个图形的数据在定义对象时给定，需要使用动态多态实现

```C++
#include <iostream>
using namespace std;
//声明抽象基类Shape
class Shape {
public:
    // 预留打印面积的抽象虚函数，默认返回值为0.0
    virtual float printArea() const { return 0.0; }
    // 纯虚函数，在派生类中具体实现
    virtual void shapeName() const = 0;
};
// 继承于Shape类的子类Circle
class Circle : public Shape {
public:
    // Circle构造函数，默认参数半径为0
    Circle(float r = 0): radius(r) {}
    // 重写Shape的printArea方法，返回圆的面积
    virtual float printArea() const { return 3.1415926 * radius * radius; }
    // 再次定义shapeName虚函数
    virtual void shapeName() const { cout << "I'm Circle!" << endl; }
protected:
    float radius;
};
// 继承于Shape类的子类Rectangle
class Rectangle : public Shape {
public:
    // Rectangle构造函数，默认参数长度和高度都为0
    Rectangle(float l = 0, float h = 0): length(l), height(h) {}
    // 重写Shape的printArea方法，返回矩形的面积
    virtual float printArea() const { return length * height; }
    // 再次定义shapeName虚函数
    virtual void shapeName() const { cout << "I'm Rectangle!" << endl; }
protected:
    float length, height;
};

int main() {
    // 创建Circle类实例circle，半径参数为10
    Circle circle(10);
    // 创建Rectangle类实例rectangle，长为6宽为4
    Rectangle rectangle(6, 4);
    // 静态调用的写法：直接用实例调用它的方法
    cout << "@静态关联" << endl;
    circle.shapeName();
    cout << "Circle Area: " << circle.printArea() << endl;
    rectangle.shapeName();
    cout << "Rectangle Area: " << rectangle.printArea() << endl;

    // 动态调用的写法：通过抽象类Shape的指针指向实例地址再调用其方法
    cout << "@动态关联" << endl;
    Shape *ptr; // 定义Shape抽象类指针
    // 将ptr赋值为circle实例的地址
    ptr = &circle;
    // 通过虚函数动态调用Circle类的方法
    ptr->shapeName();
    cout << "Circle Area: " << ptr->printArea() << endl;
    // 同上，实现了多态调用，即使用同一个变量在不同时间调用了不同类型的函数
    ptr = &rectangle;
    ptr->shapeName();
    cout << "Rectangle Area: " << ptr->printArea() << endl;
    // 返回并退出程序
    return 0;
}
```

运行结果

```
@静态关联
I'm Circle!
Circle Area: 314.159
I'm Rectangle!
Rectangle Area: 24
@动态关联
I'm Circle!
Circle Area: 314.159
I'm Rectangle!
Rectangle Area: 24
```

## 第2题

某小型公司的人员管理信息系统。原有三类人员：经理（manager）、技术人员（technician）、销售人员（salesman），后期又增加一类人员：销售经理（sales_manager）。要求存储这些人员的姓名、性别、工号、当月工资、计算月薪总额并显示全部信息。

**月薪计算：**

- 经理=固定月薪8000元
- 技术人员=100元/小时
- 销售人员=当月个人销售额*4%
- 销售经理=固定月薪5000+所辖部门当月销售额*5%

employee.h

```C++
// 雇员管理系统头文件
#ifndef _EMPLOYEE_H
#define _EMPLOYEE_H

#include <string>
#include <iostream>
using namespace std;
// 定义雇员基类
class Employee {
private:
    // 定义静态&私有雇员起始基数
    static int baseEmployeeNumber;
protected:
    // 定义相关变量
    string Name;
    string Gender;
    string Job;
    int Number;
    int Grade;
    float Salary;
    // 定义一个子类可以共用的显示工资的函数
    virtual void showSalary();
public:
    // 构造函数及相关雇员类方法
    Employee();
    virtual void Promote(int);
    virtual void CalcSalary()=0;
    virtual void ShowInfo();
};
// 经理类，虚继承自雇员类
class Manager : virtual public Employee {
protected:
    // 声明月工资变量
    float MonthlyPay;
public:
    // 构造函数及重写的CalcSalary
    Manager();
    virtual void CalcSalary();
};
// 技术人员类，虚继承自雇员类
class Technician : virtual public Employee {
protected:
    // 声明每小时工资与工作小时
    float HourlyRate;
    float WorkHours;
public:
    // 构造函数及重写的CalcSalary
    Technician();
    virtual void CalcSalary();
};
// 销售员类，虚继承自雇员类
class Salesman : virtual public Employee {
protected:
    // 声明回扣比重与销售量
    float CommonRate;
    float Sales;
public:
    // 构造函数及重写的CalcSalary
    Salesman();
    virtual void CalcSalary();
};
// 销售经理类，多继承自经理类和销售员类
class SalesManager : public Manager , public Salesman {
public:
    // 声明回扣比重与销售量
    SalesManager();
    virtual void CalcSalary();
};
#endif //_EMPLOYEE_H
```

employee.cpp

```C++
#include <iostream>
#include "employee.h"
using namespace std;
// 初始化定义雇员类工号的起始基数为23333
int Employee::baseEmployeeNumber = 23333;
// 定义雇员类构造函数
Employee::Employee() {
    // 输入相关信息
    cout << "Input Employee Name: ";
    cin >> Name;
    cout << "Input " << Name << "'s Gender: ";
    cin >> Gender;
    // 将现在的基数赋值给员工号并自加1
    Number = baseEmployeeNumber++;
    Salary = 0;
    Grade = 1;
}
// 定义showSalary方法
void Employee::showSalary() {
    cout << Job <<": "<< Name << " Salary: "
    << Salary <<endl;
}
// 定义ShowInfo方法
void Employee::ShowInfo() {
    cout << Job << ": " << Name << " Gender: "
    << Gender << " Number: " <<Number << " Grade: " << Grade
    <<" Salary: "<< Salary << endl;
}
// 定义Promote方法
void Employee::Promote(int i) {
    Grade += i;
}
// 定义经理类构造函数
Manager::Manager() {
    Job = "Manager";
    // 经理=固定月薪8000元
    MonthlyPay = 8000;
}
// 重写CalcSalary方法
void Manager::CalcSalary() {
    Salary = MonthlyPay;
    showSalary();
}
// 定义技术人员类构造函数
Technician::Technician() {
    Job = "Technician";
    // 技术人员=100元/小时
    HourlyRate = 100;
}
// 重写CalcSalary方法
void Technician::CalcSalary() {
    cout << "Input " << Name << " working hours: ";
    cin >> WorkHours;
    Salary = HourlyRate * WorkHours;
    showSalary();
}
// 定义销售员类构造函数
Salesman::Salesman() {
    Job = "Salesman";
    // 销售人员=当月个人销售额*4%
    CommonRate = 0.04;
}
// 重写CalcSalary方法
void Salesman::CalcSalary() {
    cout << "Input " << Name << " sales: ";
    cin >> Sales;
    Salary = CommonRate * Sales;
    showSalary();
}
// 定义销售经理类构造函数
SalesManager::SalesManager() {
    Job = "SalesManager";
    // 销售经理=固定月薪5000+所辖部门当月销售额*5%
    CommonRate = 0.05;
    MonthlyPay = 5000;
}
// 重写CalcSalary方法
void SalesManager::CalcSalary() {
    cout << "Input " << Name << " division sales: ";
    cin >> Sales;
    Salary = MonthlyPay + CommonRate * Sales;
    showSalary();
}
```

main.cpp

```C++
#include "employee.h"

int main() {
    // 雇员管理系统可行性测试

    // 定义各实例对象
    auto m = new Manager();
    auto t = new Technician();
    auto s = new Salesman();
    auto sm = new SalesManager();
    // 抽象的雇员类指针数组
    Employee *emp[4] = {m, t, s, sm};
    // 使用抽象基类指针调用，C++多态的体现
    for (int i = 0; i < 4; i++) {
        // 调用相关函数
        emp[i]->Promote(i);
        emp[i]->CalcSalary();
        emp[i]->ShowInfo();
    }
    // 返回并退出程序
    return 0;
}
```

运行结果

```
Input Employee Name: ZhangSan
Input ZhangSan's Gender: M
Input Employee Name: LiSi
Input LiSi's Gender: M
Input Employee Name: WangWu
Input WangWu's Gender: M
Input Employee Name: XiaLiu
Input XiaLiu's Gender: F
Manager: ZhangSan Salary: 8000
Manager: ZhangSan Gender: M Number: 23333 Grade: 1 Salary: 8000
Input LiSi working hours: 240
Technician: LiSi Salary: 24000
Technician: LiSi Gender: M Number: 23334 Grade: 2 Salary: 24000
Input WangWu sales: 123000
Salesman: WangWu Salary: 4920
Salesman: WangWu Gender: M Number: 23335 Grade: 3 Salary: 4920
Input XiaLiu division sales: 123000
SalesManager: XiaLiu Salary: 11150
SalesManager: XiaLiu Gender: F Number: 23336 Grade: 4 Salary: 11150
```
