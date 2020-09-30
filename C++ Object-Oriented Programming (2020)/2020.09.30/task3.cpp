#include <cstdio>
#include <iostream>

#define MAX_LEN = 0xff

int main()
{

    int l = 0, n[MAX_CANON];
    int max, min, sum = 0;

    std::cout << "input length: ";
    std::cin >> l;

    std::cout << "input some integers: " << std::endl;
    for (int i = 0; i < l; i++)
    {
        std::cin >> n[i];

        sum += n[i];

        if (i == 0)
        {
            min = n[i];
            max = n[i];
        }

        if (n[i] < min)
            min = n[i];
        if (n[i] > max)
            max = n[i];
    }

    printf("min: %d, max: %d, avg: %.2lf\n", min, max, (double)sum / l);

    return 0;
}