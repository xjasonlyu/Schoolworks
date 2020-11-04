#include <iostream>

#define PI 3.1415926

using namespace std;

class Shape {
   public:
    Shape() {
        a = 0;
        b = 0;
    }
    Shape(double i) {
        a = i;
        b = 0;
    }
    Shape(double i, double j) {
        a = i;
        b = j;
    }
    double getArea() { return a * b; }

   private:
    double a;
    double b;
};

class Circle : public Shape {
   public:
    Circle(double i) : r(i) {}
    double getArea() { return PI * r * r; }

   private:
    double r;
};

class Rectangle : public Shape {
   public:
    Rectangle() : Shape() {}
    Rectangle(double i, double j) : Shape(i, j) {}
};

int main() {
    Circle c(3);
    Rectangle r(5, 6);

    cout << "Circle Area: " << c.getArea() << endl;
    cout << "Rectangle Area: " << r.getArea() << endl;

    return 0;
}
