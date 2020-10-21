#include <iostream>

using namespace std;

class Student
{
public:
    int num;
    char name[10];

    static int ref;
    // 显示学生的学号与成绩
    void display()
    {
        ref++;
        cout << "ID: " << num << " Name: " << name << endl;
    };
    static void printRef() { cout << "Ref: " << ref << endl; };
};

int Student::ref = 0;

int main()
{
    Student me;

    me.num = 123456;
    strcpy(me.name, "Jason");
    me.display();
    Student::printRef();

    me.num = 789012;
    strcpy(me.name, "Tom");
    me.display();
    Student::printRef();

    return 0;
}
