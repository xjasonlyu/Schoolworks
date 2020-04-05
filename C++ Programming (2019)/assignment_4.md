# Ⅳ

## 第7题

有以下程序，请完成下面工作

1. 读程序，写出运行时输出的结果。
2. 然后上机运行，验证结果是否正确。
3. 分析程序执行过程，尤其是调用构造函数的过程。

```C++
#include <iostream>
using namespace std;
// 定义A类
class A {
public:
    // A无参数构造函数
    A() {
        a = 0;
        b = 0;
    }
    // A单参数构造函数
    A(int i) {
        a = i;
        b = 0;
    }
    // A双参数构造函数
    A(int i, int j) {
        a = i;
        b = j;
    }
    // 打印函数
    void display() { cout << "a=" << a << " b=" << b; }

private:
    // A私有变量
    int a;
    int b;
};
// 定义B类，从A类中继承
class B : public A {
public:
    // B构造函数显式调用父类A的构造函数

    // B无参数构造函数，继承了A的构造函数，
    // 直接将a，b，c都赋值为0。
    B() { c = 0; }
    // B单参数构造函数，需要一个参数i，用
    // 父类A的单参数构造函数调用。
    B(int i) : A(i) { c = 0; }
    // B双参数构造函数，功能同上，调用A(i,j)初始化。
    B(int i, int j) : A(i, j) { c = 0; }
    // B三参数构造函数，功能同上，其中i，j形式参数
    // 用于调用父类构造函数初始化，k用于内置变量c的初始化。
    B(int i, int j, int k) : A(i, j) { c = k; }
    // 打印函数（重用了A的方法并添加了自己的功能）
    void display1() {
        // 调用A的display方法
        display();
        cout << " c=" << c << endl;
    }

private:
    // B私有变量
    int c;
};

int main() {
    // 声明B类的实例对象
    B b1;
    /*
    这里以`B b3(1,3)`为例分析构造函数调用过
    程，其他初始化语句与之相同。
    1. b3(1,3)首相调用B类的B(int i,int j)
    构造函数，参数为B(1,3)。
    2. B(1,3)再调用A类的A(int i,int j)构造
    函数，并把1赋值给a，3赋值给b。
    3. B构造函数完成了父类的构造函数调用，接着
    继续执行，将c使用默认值赋值为0。
    4. 以上便完成了所有的`B b3(1,3)`语句所有
    的初始化。
    */
    B b2(1);
    B b3(1, 3);
    B b4(1, 3, 5);
    // 打印a，b，c
    b1.display1();
    b2.display1();
    b3.display1();
    b4.display1();
    return 0;
}
```

运行结果

```
a=0 b=0 c=0
a=1 b=0 c=0
a=1 b=3 c=0
a=1 b=3 c=5
```

程序分析过程在代码注释中

## 第8题

有以下程序，请完成下面工作

1. 读程序，写出运行时输出的结果。
2. 然后上机运行，验证结果是否正确。
3. 分析程序执行过程，尤其是调用构造函数的过程。

```C++
#include <iostream>
using namespace std;
// 定义A类
class A {
public:
    // A类构造函数
    A() { cout << "constructing A " << endl; }
    // A类析构函数
    ~A() { cout << "destructing A " << endl; }
};
// 定义A的子类B
class B : public A {
public:
    // B类构造函数
    B() { cout << "constructing B " << endl; }
    // B类析构函数
    ~B() { cout << "destructing B " << endl; }
};
// 定义B的子类C
class C : public B {
public:
    // C类构造函数
    C() { cout << "constructing C " << endl; }
    // C类析构函数
    ~C() { cout << "destructing C " << endl; }
};

int main() {
    /*
    C++构造原则如下：
    1. 如果子类没有定义构造方法，则调用父类的无参数的构造方法。
    2. 如果子类定义了构造方法，不论是无参数还是带参数，在创建
    子类的对象的时候，首先执行父类无参数的构造方法，然后执行自
    己的构造方法。
    3. 在创建子类对象时候，如果子类的构造函数没有显示调用父类
    的构造函数，则会调用父类的默认无参构造函数。
    4. 在创建子类对象时候，如果子类的构造函数没有显示调用父类
    的构造函数且父类自己提供了无参构造函数，则会调用父类自己的
    无参构造函数。
    5. 在创建子类对象时候，如果子类的构造函数没有显示调用父类
    的构造函数且父类只定义了自己有参构造函数，则会出错（如果父
    类只有有参数的构造方法，则子类须显示调用此带参构造方法）。
    6. 如果子类调用父类带参数的构造方法，需要用初始化父类成员
    对象的方式。

    知道了上面的规则，那么调用构造函数过程分析如下：
    1. 首先`C c1`语句声明了C类的c1实例变量
    2. C类继承于B类，B类继承于A类，c1是C实例。程序试图调用C
    的构造函数时候，要先去调用B的构造函数而B构造函数又要去调
    用A的构造函数，所以会依次调用A()->B()->C()构造函数，屏
    幕上依次输出：
    constructing A
    constructing B
    constructing C
    3. 对象生命周期结束后要释放空间，所以又依次调用析构函数，
    析构函数调用的次序是先派生类的析构后基类的析构，所以调用
    顺序为 ~C()->~B()->~A()，屏幕上依次输出：
    destructing C
    destructing B
    destructing A
    */
    C c1;
    return 0;
}
```

运行结果

```
constructing A
constructing B
constructing C
destructing C
destructing B
destructing A
```

程序分析过程在代码注释中

## 第9题

分别定义Teacher（教师）类和Cadre（干部）类，采用多重继承方式由这两个类派生出新类Teacher_Cadre（教师兼干部）类。要求：

1. 在两个基类中都包含姓名、年龄、性别、地址、电话等数据成员。
2. 在Teacher类中还包含数据成员title（职称），在Cadre类中还包含数据成员post（职务），在Teacher_Cadre类中还包含数据成员wages（工资）。
3. 对两个基类中的姓名、年龄、性别、地址、电话等数成员用相同的名字，在引用这些数据成员时，指定作用域。
4. 在类体中声明成员函数，在类外定义成员函数。
5. 在派生类Teacher_Cadre的成员函数show中调Teacher类中的display函数，输出姓名、年龄、性别、职称、地址、电话，然后再用cout语句输出职务与工资。

```C++
#include<string>
#include <iostream>
using namespace std;
// 教师类
class Teacher {
public:
    // 教师类构造函数
    Teacher(string n, int a, char s, string d, string p, string t) :
            name(n), age(a), sex(s), addr(d), tel(p), title(t) {}
    // 教师类打印函数
    void display() {
        cout << "name: " << name << endl;
        cout << "age: " << age << endl;
        cout << "sex: " << sex << endl;
        cout << "address: " << addr << endl;
        cout << "tel: " << tel << endl;
        cout << "title: " << title << endl;
    }
// 类内私有变量
protected:
    string name;
    int age;
    char sex;
    string addr;
    string tel;
    string title;
};
// 干部类
class Cadre {
public:
    // 干部类构造函数
    Cadre(string n, int a, char s, string d, string p, string t) :
            name(n), age(a), sex(s), addr(d), tel(p), post(t) {}
    // 干部类打印函数
    void display() {
        cout << "name: " << name << endl;
        cout << "age: " << age << endl;
        cout << "sex: " << sex << endl;
        cout << "address: " << addr << endl;
        cout << "tel: " << tel << endl;
        cout << "post: " << post << endl;
    }
// 类内私有变量
protected:
    string name;
    int age;
    char sex;
    string addr;
    string tel;
    string post;
};
// 基于教师类和干部类多重继承出的教师兼干部类
class Teacher_Cadre : public Teacher, public Cadre {
public:
    // 教师兼干部类构造函数
    Teacher_Cadre(string n, int a, char s, string d, string p, string ti, string po, float w) :
        // 显示调用父类的构造函数初始化变量
        Teacher(n, a, s, d, p, ti), Cadre(n, a, s, d, p, po), wages(w) {}
    // 类体中声明显示函数
    void show();
// 类内私有变量
private:
    float wages;
};

// 类外定义成员函数
void Teacher_Cadre::show() {
    // 使用命名空间作用域调用Teacher类的display函数
    Teacher::display();
    // 输出Cadre作用域的post变量
    cout << "post: " << Cadre::post << endl;
    // 输出wages变量
    cout << "wages: " << wages << endl;
}

int main() {
    // 实例化教师兼干部类，并赋予初值
    Teacher_Cadre tc("Jimmy", 35, 'm',
                     "HDU, Hangzhou", "12345678910",
                     "normal", "president", 23333.3);
    // 调用函数显示
    tc.show();
    // 程序返回并退出
    return 0;
}
```

运行结果

```
name: Jimmy
age: 35
sex: m
address: HDU, Hangzhou
tel: 12345678910
title: normal
post: president
wages: 23333.3
```

## 第10题

将本章11.8中的程序片段加以补充完善，成为一个完的程序。程序中使用继承和组合。在定义Professor类对象prof1时给出所有数据的初值，然后修改prof1的旧数据，最后输出prof1的全部最新数据。

```C++
#include <iostream>
#include <string>
using namespace std;
// 生日类
class BirthDate {
public:
    // 生日类构造函数
    BirthDate(int y, int m, int d) : year(y), month(m), day(d) {}
    // 修改函数
    void change(int y, int m, int d) {
        year = y;
        month = m;
        day = d;
    }
    // 打印函数
    void display() {
        cout << "birthday: " << year << "/" << month << "/" << day << endl;
    }
private:
    // 私有变量
    int year;
    int month;
    int day;
};
// 教师类
class Teacher {
public:
    // 教师类构造函数
    Teacher(int i, string n, char s) : num(i), name(n), sex(s) {}
    // 打印函数
    void display() {
        cout << "num: " << num << endl;
        cout << "name: " << name << endl;
        cout << "sex: " << sex << endl;
    }
private:
    // 私有变量
    int num;
    string name;
    char sex;
};
/* 继承的使用 */
// 基于教师类派生出的教授类
class Professor : public Teacher {
public:
    // 教授类构造函数
    Professor(int i, string n, char s, int y, int m, int d, float a) :
        // 显示调用教师类和生日类初始化
        Teacher(i, n, s), birthday(y, m, d), area(a) {}
    // 教授类修改函数
    void change(int y, int m, int d) {
        // 调用生日类的修改函数
        birthday.change(y, m, d);
    }
    // 打印函数
    void display() {
        // 调用教师类作用域打印函数
        Teacher::display();
        // 调用生日类作用域打印函数
        birthday.display();
        // 输出area变量的值到屏幕
        cout << "area: " << area << endl;
    }
private:
    // 私有变量
    float area;
    /* 组合的使用 */
    // 定义生日类型变量
    BirthDate birthday;
};

int main() {
    // 声明并初始化prof1实例
    Professor prof1(2333, "Jack", 'm', 1989, 6, 4, 120.5);
    cout << endl << "@Previous:" << endl;
    // 打印输出当前的内部变量的值
    prof1.display();
    // 修改相关数据
    prof1.change(1998, 6, 4);
    cout << endl << "@Current:" << endl;
    // 打印修改完的内部数据变量的值
    prof1.display();
    // 返回并退出程序
    return 0;
}
```

运行结果

```
@Previous:
num: 2333
name: Jack
sex: m
birthday: 1989/6/4
area: 120.5

@Current:
num: 2333
name: Jack
sex: m
birthday: 1998/6/4
area: 120.5
```
