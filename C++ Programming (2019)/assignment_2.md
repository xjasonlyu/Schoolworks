# Ⅱ

## 第2题

分析下面程序，写出运行时的输出结果

```C++
#include <iostream>
using namespace std;
class Date {
public:
    Date(int, int, int);
    Date(int, int);
    Date(int);
    Date();
    void display();
private:
    int month;
    int day;
    int year;
};

Date::Date(int m, int d, int y) : month(m), day(d), year(y) {}

Date::Date(int m, int d) : month(m), day(d) {
    year = 2005;
}

Date::Date(int m) : month(m) {
    day = 1;
    year = 2005;
}

Date::Date() {
    month = 1;
    day = 1;
    year = 2005;
}

void Date::display() {
    cout << month << "/" << day << "/" << year << endl;
}

int main() {
    Date d1(10, 13, 2005);
    Date d2(12, 30);
    Date d3(10);
    Date d4;
    d1.display();
    d2.display();
    d3.display();
    d4.display();
    return 0;
}
```

运行结果

```
10/13/2005
12/30/2005
10/1/2005
1/1/2005
```

## 第3题

如果将第2题中的程序的第五行改为用默认参数，即
`Date(int=1, int=1, int=2015);`
分析程序有无问题。

> 这里编译会报错，是因为`Date(int=1, int=1, int=2015);`定义了三个带有默认值的参数，与其他三个构造函数产生了歧义。
> 所以当`main`函数里声明`Date`类型的实例时编译器就会因为无法分辨具体调用的是哪个构造函数而报错：
> `error: call to constructor of 'Date' is ambiguous`。

解决方法：直接删除其他三个构造函数即可，e.g.

```C++
class Date {
public:
    Date(int= 1, int= 1, int= 2015);
    void display();
private:
    int month;
    int day;
    int year;
};

Date::Date(int m, int d, int y) : month(m), day(d), year(y) {}

void Date::display() {
    cout << month << "/" << day << "/" << year << endl;
}
```

上面的代码因为`Date`的构造函数已经有了三个默认参数值，所以不需要再声明三个其他的构造函数来指明不同情况下的默认值，直接在需要的时候调用就可以了。测试运行结果与之前程序输出相同。

## 第4题

建立一个对象数组，内放五个学生的数据（学号、成绩），用指针指向数组首元素，输出第1，3，5个学生的数据。

```C++
#include <iostream>

using namespace std;

class Student {
public:
    int num;    //学号
    int score;  //成绩
    // 显示学生的学号与成绩
    void display() { cout << "ID: " << num << " Score: " << score << endl; };
};

int main() {
    // 初始化对象数组并赋值给p指针变量
    auto *p = new Student[5];

    // 给所有学生对象的学号与成绩赋值
    for (int i = 1; i <= 5; i++) {
        p[i-1].num = i;
        p[i-1].score = i * 20;
    }

    // 显示第1个学生的学号与成绩
    p[0].display();
    // 显示第3个学生的学号与成绩
    p[2].display();
    // 显示第5个学生的学号与成绩
    p[4].display();

    return 0;
}
```

运行结果

```
ID: 1 Score: 20
ID: 3 Score: 60
ID: 5 Score: 100
```

## 第5题

建立一个对象数组，内放五个学生的数据（学号、成绩），设立一个函数max，用指向对象的指针作函数参数，在max函数中找出5个学生中分数最高者，并输出其学号。

```C++
#include <iostream>

using namespace std;

class Student {
public:
    int num;    //学号
    int score;  //成绩
    // 显示学生的学号与成绩
    void display() { cout << "ID: " << num << " Score: " << score << endl; };
};

void max(Student *p) {
    int _max = 0; // 储存最高成绩的变量
    int _num = 0; // 储存最高成绩的学生的变量

    // 遍历对象数组，每次把分数较高的放进上面的变量中
    for (int i = 0; i < 5; i++, p++) {
        if (p->score > _max) {
            _max = p->score;
            _num = p->num;
        }
    }

    // 输出分数最高的学生学号
    cout << "Student " << _num << " who has max score." << endl;
}

int main() {
    // 初始化对象数组并赋值给p指针变量
    auto *p = new Student[5];

    // 给所有学生对象的学号赋值
     for (int i = 1; i <= 5; i++) {
        p[i - 1].num = i;
    }

    // 给所有学生的成绩随机赋值，以测试max函数
    p[0].score = 45;
    p[1].score = 55;
    p[2].score = 78;
    p[3].score = 90;
    p[4].score = 63;

    // 调用max函数
    max(p);

    return 0;
}
```

运行结果

```
Student 4 who has max score.
```

## 第6题

阅读下面程序，分析其执行过程，写出输出结果。

- 执行过程分析（见注释）

```C++
#include <iostream>
using namespace std;
class Student {
public:
    // 构造函数，需要 n、s 两个参数
    Student(int n, float s) : num(n), score(s) {}
    // 通过change函数来改变Student类的私有变量 num、score
    void change(int n, float s) { num=n; score=s; }
    // 输出学生的学号和成绩信息
    void display() { cout << num << " " << score << endl; }
private:
    int num;
    float score;
};

int main() {
    // 以(num=101,score=78.5)的参数初始化Student类实例stud
    Student stud(101, 78.5);
    // 输出现在的学生信息
    stud.display();
    // 调用change函数来修改stud的信息
    stud.change(101, 80.5);
    // 再次输出修改过后的学生信息
    stud.display();
    return 0;
}
```

运行结果

```
101 78.5
101 80.5
```

## 第7题

将第6题分别做以下修改，分析所修改部分的含义以及编译和运行的情况。

##### (1) 将main函数第二行改为

`const Student stud(101, 78.5);`

> `const Student stud(101, 78.5);`语句将stud变量声明为了`const Student`类型。
> 
> 但是这里编译其实是会报错的：
> `error: 'this' argument to member function 'display' has type 'const Student', but function is not marked const`
> 
> 这是因为`const object`只能调用`const method`所造成的编译错误。

##### (2) 在(1)的基础上修改程序，使之能正常运行，用change函数修改数据成员num和score的值。

修改代码如下：

```C++
#include <iostream>
using namespace std;
class Student {
public:
    // 构造函数，需要 n、s 两个参数
    Student(int n, float s) : num(n), score(s) {}
    // 将change函数声明为const function
    void change(int n, float s) const { num=n; score=s; }
    // 将display函数声明为const function
    void display() const { cout << num << " " << score << endl; }
private:
    // 使用mutable关键字来声明num和score变量，
    // 从而可以被const function改变值。
    mutable int num;
    mutable float score;
};

int main() {
    // 声明const Student类型变量stud
    const Student stud(101, 78.5);
    stud.display();
    stud.change(101, 80.5);
    stud.display();
    return 0;
}
```

##### (3) 将main函数改为

```C++
int main() {
    Student stud(101, 78.5);
    Student *p = &stud;
    p->display();
    p->change(101, 80.5);
    p->display();
    return 0;
}
```

**其他部分仍同第6题的程序。**

> 这里p是指向Student类型的stud变量的指针，然后在后面用`->`操作符来调用Student实例的方法。
> 
> 编译没有问题，运行结果与第6题相同，仍为：

```
101 78.5
101 80.5
```

##### (4) 在(2)的基础上将main函数第三行改为

`const Student *p = &stud;`

> 这里p是指向const Student类型的stud变量的指针。
> 
> 编译依旧没有问题，运行结果与(2)相同，仍为：

```
101 78.5
101 80.5
```

##### (5) 再把main函数第3行改为

`Student *p = &stud;`

> 这里p的赋值是错误的，因为p的类型是`Student*`，而`&stud`则是`const Student*`类型，类型不同赋值过去会报错。
> 
> 编译报错：
> `error: cannot initialize a variable of type 'Student *' with an rvalue of type 'const Student *'`

## 第8题

修改第6题的程序，增加一个fun函数，改写main函数。改为在fun函数中调用change和display函数。在fun函数中使用对象引用(Student &)作为形参。

```C++
#include <iostream>
using namespace std;
class Student {
public:
    // 构造函数，需要 n、s 两个参数
    Student(int n, float s) : num(n), score(s) {}
    // 通过change函数来改变Student类的私有变量 num、score
    void change(int n, float s) { num=n; score=s; }
    // 输出学生的学号和成绩信息
    void display() { cout << num << " " << score << endl; }
private:
    int num;
    float score;
};

// fun函数使用Student&类型作为形参
void fun(Student& s) {
    // 这里的s变量就是main函数里的stud变量的引用，
    // 剩余的调用方式和第6题类似。
    s.display();
    // 调用change函数来修改stud的信息
    s.change(101, 80.5);
    // 再次输出修改过后的学生信息
    s.display();
}

int main() {
    Student stud(101, 78.5);
    // 以stud作为实参调用fun函数
    fun(stud);
    return 0;
}
```

## 第9题

商店销售某一商品，商店每天公布统一的折扣(discount)。同时允许销售人员在销售时灵活掌握售价(price)，在此基础上，对一次购10件以上者，还可以享受9.8折优惠。现已知当天3名销货员的销售状况为：

| 销货员号(num) | 销货件数(quantity) | 销货单价(price) |
|-----|-----|-------|
| 101 | 5   | 23.5  |
| 102 | 12  | 24.56 |
| 103 | 100 | 21.5  |

请编写程序，计算出当日此商品的总销售款sum，以及每件商品的平均售价。要求用静态数据成员和静态成员函数。
> 提示：将折扣discount、总销售款sum和商品销售总件数n声明为静态数据成员，再定义静态成员函数average（求平均售价）和diaplay（输出结果）。

程序代码如下

```C++
#include <iostream>

using namespace std;

// 商店类
class Store {
public:
    static int n;           // 总销售数
    static double sum;      // 总销售款
    static double discount; // 优惠折扣
    // 求平均售价函数
    static double average() { return sum / n; }
    // 输出显示函数
    static void display() {
        cout << "Total sale: " << sum << endl;
        cout << "Average price: " << average() << endl;
    }
};

// 销售员类
class Salesman {
private:
    int num;       // 销售员号
    int quantity;  // 销售件数
    double price;  // 销售单价
public:
    // 销售员类的构造函数
    Salesman(int, int, double);
};

Salesman::Salesman(int m, int q, double p) : num(m), quantity(q), price(p) {
    // 商店的静态销售数变量加上该销售员的销售件数
    Store::n += q;
    // 商店的静态销售款变量加上该销售员的销售总款
    Store::sum += q * p;
}

// 初始化Store的n和sum静态变量，不然编译器会报linked errors错误
int Store::n = 0;
double Store::sum = 0;

int main() {
    // 创建三个销售员的匿名Salesman对象
    new Salesman(101, 5, 23.5);
    new Salesman(102, 12, 24.56);
    new Salesman(103, 100, 21.5);

    // 调用商店类的display函数输出结果
    Store::display();

    return 0;
}
```

运行结果

```
Total sale: 2562.22
Average price: 21.8993
```
