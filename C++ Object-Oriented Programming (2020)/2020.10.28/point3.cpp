#include <cmath>
#include <iostream>

using namespace std;

class Point3 {
   public:
    Point3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {
        ref++;
    }

    static int getNum() {
        return ref;
    }

    float friend Area(Point3 &a, Point3 &b, Point3 &c);

   private:
    float x;
    float y;
    float z;
    static int ref;
};

int Point3::ref = 0;

float Area(Point3 &a, Point3 &b, Point3 &c) {
    float edge_ab = sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
    float edge_bc = sqrt((b.x - c.x) * (b.x - c.x) + (b.y - c.y) * (b.y - c.y) + (b.z - c.z) * (b.z - c.z));
    float edge_ac = sqrt((a.x - c.x) * (a.x - c.x) + (a.y - c.y) * (a.y - c.y) + (a.z - c.z) * (a.z - c.z));

    float p = (edge_ab + edge_bc + edge_ac) / 2;
    float s = sqrt(p * (p - edge_ac) * (p - edge_bc) * (p - edge_ac));
    return s;
}

int main() {
    Point3 p1(1, 0, 0);
    Point3 p2(0, 4, 5);
    Point3 p3(12, 1, 1);
    cout << "Sum: " << Point3::getNum() << "\t"
         << "Area: " << Area(p1, p2, p3) << endl;

    return 0;
}
