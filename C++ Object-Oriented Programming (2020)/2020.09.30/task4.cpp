#include <iostream>

int factorial(int x)
{
    if (x == 0 || x == 1)
        return 1;
    return x * factorial(x - 1);
}

int main()
{
    int n = 0, result = 0;

    std::cout << "n: ";
    std::cin >> n;

    for (int i = 1; i <= n; i++)
    {
        result += factorial(i);
    }

    std::cout << "1!+2!+...+n! = " << result << std::endl;

    return 0;
}