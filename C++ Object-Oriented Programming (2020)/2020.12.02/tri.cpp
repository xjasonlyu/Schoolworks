#include <cmath>
#include <iostream>
#include <string>

using namespace std;

class Point {
  public:
    Point() : x(0), y(0) {}
    Point(double a, double b) : x(a), y(b) {}
    double x, y;
};

class Tri {
  public:
    Tri(Point &i, Point &j, Point &k) : a(i), b(j), c(k) {
        x = sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
        y = sqrt((b.x - c.x) * (b.x - c.x) + (b.y - c.y) * (b.y - c.y));
        z = sqrt((a.x - c.x) * (a.x - c.x) + (a.y - c.y) * (a.y - c.y));
        num++;
        No = num;
    }

    void Cong(Tri &t) {
        if (!(x + y > z && x + z > y && y + z > x))
            throw No;
        else if (!(t.x + t.y > t.z && t.x + t.z > t.y && t.y + t.z > t.x))
            throw t.No;

        cout << "No." << No << " and "
             << "No." << t.No << " ";

        if ((x == t.x && y == t.y && z == t.z) ||
            (x == t.y && y == t.z && z == t.x) ||
            (x == t.z && y == t.x && z == t.y) ||
            (x == t.y && y == t.x && z == t.z) ||
            (x == t.z && y == t.y && z == t.x) ||
            (x == t.x && y == t.z && z == t.y)) {
            cout << "is equal." << endl;
        } else {
            cout << "is unequal." << endl;
        }
    }

  public:
    static int num;

  private:
    int No;

    Point a, b, c;
    double x, y, z;
};

int Tri::num = 0;

int main() {
    try {

        Point a1(0, 0);
        Point b1(1, 0);
        Point c1(0, 1);
        Tri t1(a1, b1, c1);

        Point a2(1, 0);
        Point b2(2, 0);
        Point c2(1, 1);
        Tri t2(a2, b2, c2);

        Point a3(1, 0);
        Point b3(3, 0);
        Point c3(1, 1);
        Tri t3(a3, b3, c3);

        Point a4(1, 0);
        Point b4(2, 0);
        Point c4(3, 0);
        Tri t4(a4, b4, c4);

        t1.Cong(t2);
        t1.Cong(t3);
        t1.Cong(t4);

    } catch (int e) {
        cout << "No." << e << " is not a triangle." << '\n';
    }
}