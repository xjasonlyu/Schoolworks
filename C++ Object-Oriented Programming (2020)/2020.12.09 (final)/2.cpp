#include <iostream>

using namespace std;

class Student {
  public:
    Student(int a, int b) : number(a), score(b) {}

    void show() { cout << "number: " << number << "\t"
                       << "score: " << score << endl; }

  public:
    int number;
    int score;
};

int main() {
    Student student[5]{
        Student(1001, 90),
        Student(1002, 80),
        Student(1003, 92),
        Student(1004, 73),
        Student(1005, 66),
    };

    Student *p = student;

    // number 1
    p->show();

    // number 3
    (p + 2)->show();

    // number 3
    (p + 4)->show();

    return 0;
}
