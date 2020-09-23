#include <iostream>

double f2c(double fahrenheit)
{
    return (fahrenheit - 32) / 1.8;
}

void print_temperature_table(void)
{
    for (int i = 0; i <= 100; i += 5)
        std::cout << i << " ℉"
                  << "\t" << (int)f2c(i) << " ℃" << std::endl;
}

int main(void)
{
    print_temperature_table();
    return 0;
}
