#include <iostream>

using namespace std;

class Boat;

class Car
{
public:
    int weight;
    friend int getTotalWeight(Boat &b, Car &c);
};

class Boat
{
public:
    int weight;
    friend int getTotalWeight(Boat &b, Car &c);
};

int getTotalWeight(Boat &b, Car &c)
{
    return b.weight + c.weight;
}

int main()
{
    Boat b;
    Car c;

    b.weight = 1000;
    c.weight = 999;

    cout << "total weight: " << getTotalWeight(b, c) << endl;

    return 0;
}
