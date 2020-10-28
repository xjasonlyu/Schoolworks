#include <cmath>
#include <iostream>

using namespace std;

class Point {
   public:
    Point(float _x, float _y) : x(_x), y(_y) {}
    float getX() { return x; }
    float getY() { return y; }
    float friend lineFit(Point *p, int num);

   private:
    float x, y;
};

float lineFit(Point *p, int num) {
    float avg_x = 0, avg_y = 0, L_xx = 0, L_yy = 0, L_xy = 0;

    for (int i = 0; i < num; i++) {
        avg_x += p[i].x / num;
    }

    for (int i = 0; i < num; i++) {
        L_xx += (p[i].x - avg_x) * (p[i].x - avg_x);
        L_yy += (p[i].y - avg_y) * (p[i].y - avg_y);
        L_xy += (p[i].x - avg_x) * (p[i].y - avg_y);
    }

    cout << "This line can be fitted by y=ax+b." << endl;
    cout << "a=" << L_xy / L_xx << "\t"
         << "b=" << avg_y - L_xy * avg_x / L_xx << endl;
    return float(L_xy / sqrt(L_xx * L_yy));
}

int main() {
    Point p[10] = {Point(9, 12), Point(11, 30), Point(20, 30),
                   Point(35, 42), Point(36, 49), Point(51, 63), Point(63, 71),
                   Point(74, 79), Point(80, 88), Point(90, 100)};

    float r = lineFit(p, 10);

    cout << "Line coefficient r=" << r << endl;
}
