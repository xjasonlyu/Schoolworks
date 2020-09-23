#include <iostream>

void print_nine_times_table(void)
{
    for (int p = 1; p <= 9; p++)
    {
        for (int q = 1; q <= 9; q++)
            std::cout << p << " * " << q << " = " << p * q << "\t";
        std::cout << std::endl;
    }
}

int main(void)
{
    print_nine_times_table();
    return 0;
}
