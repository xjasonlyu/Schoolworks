#include <iostream>

#define PI 3.1415926

using namespace std;

class SimpleCircle {
   public:
    SimpleCircle(float r) : radius(r) {
        itsRadius = &radius;
    }

    float Area() {
        return PI * (*itsRadius) * (*itsRadius);
    }

    float Perimeter() {
        return 2 * PI * (*itsRadius);
    }

   private:
    float radius;
    float *itsRadius;
};

int main() {
    SimpleCircle R1(5);
    cout << "Area:\t" << R1.Area() << endl;
    cout << "Perimeter:\t" << R1.Perimeter() << endl;

    return 0;
}
