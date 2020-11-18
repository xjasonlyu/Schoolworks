#include <iostream>
#include <string>

using namespace std;

class animal {
  public:
    string name;
    double length;
    double weight;
};

class ter_animal : virtual public animal {
  public:
    double running_speed;
};

class aqu_animal : virtual public animal {
  public:
    double swimming_speed;
};

class amp_animal : public ter_animal,
                   public aqu_animal {
};

int main() {
    animal a;
    ter_animal ter_a;
    aqu_animal aqu_a;
    amp_animal amp_a;

    cout << "sizeof(double): " << sizeof(double) << endl;
    cout << "sizeof(double*): " << sizeof(double *) << endl;
    cout << "sizeof(string): " << sizeof(string) << endl;

    cout << endl;

    cout << "sizeof(animal): " << sizeof(a) << endl;
    cout << "sizeof(ter_animal): " << sizeof(ter_a) << endl;
    cout << "sizeof(aqu_animal): " << sizeof(aqu_a) << endl;
    cout << "sizeof(amp_animal): " << sizeof(amp_a) << endl;

    cout << endl;

    amp_a.name = "snake";
    amp_a.length = 100;
    amp_a.weight = 200;
    amp_a.running_speed = 50;
    amp_a.swimming_speed = 30;

    cout << "name: " << amp_a.name << endl;
    cout << "length: " << amp_a.length << endl;
    cout << "weight: " << amp_a.weight << endl;
    cout << "running_speed: " << amp_a.running_speed << endl;
    cout << "swimming_speed: " << amp_a.swimming_speed << endl;

    return 0;
}
