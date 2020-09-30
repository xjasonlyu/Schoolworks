#include <iostream>

int main()
{
    int i = 0, n = 0;

    std::cout << "input a integer: ";
    std::cin >> n;

    while (n != 1)
    {
        if (n % 2)
            n = 3 * n + 1;
        else
            n = n / 2;
        i++;
    }

    std::cout << "count: " << i << std::endl;

    return 0;
}
