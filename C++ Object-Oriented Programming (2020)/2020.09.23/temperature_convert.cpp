#include <iostream>

double f2c(double fahrenheit)
{
    return (fahrenheit - 32) / 1.8;
}

void print_temperature_table(void)
{
    for (int i = 0; i <= 100; i += 5)
#ifndef _WIN32
        std::cout << i << " ℉"
                  << "\t" << (int)f2c(i) << " ℃" << std::endl;
#else
        std::cout << i << " F"
                  << "\t" << (int)f2c(i) << " C" << std::endl;
#endif
}

int main(void)
{
    print_temperature_table();
    return 0;
}
