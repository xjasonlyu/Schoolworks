#include <iostream>

using namespace std;

class Student
{
public:
    int num;
    char name[10];
    // 显示学生的学号与成绩
    void display() { cout << "ID: " << num << " Name: " << name << endl; };
};

int main()
{
    Student me;

    me.num = 123456;
    strcpy(me.name, "jason");

    me.display();
    return 0;
}
