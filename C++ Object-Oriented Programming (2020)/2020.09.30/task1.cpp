#include <iostream>

int main()
{
    int n = 0;
    int square = 0;
    int a, b, c, d;

    while (square < 10000)
    {
        square = n * n;
        a = square / 1000;
        b = square / 100 % 10;
        c = square % 100 / 10;
        d = square % 10;

        if (square > 999 && a == b && c == d)
        {
            std::cout << square << std::endl;
        }
        n++;
    }

    return 0;
}
