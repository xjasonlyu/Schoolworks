// COPYFROM: https://blog.csdn.net/qq_41700374/article/details/81305046

#include <iostream>
#include <algorithm>
#include <stack>

using namespace std;

int main()
{

    int m, n, k, q;
    cin >> m >> n >> k;

    stack<int> s;

    for (int i = 0; i < k; i++)
    {
        int a[n];
        q = 1;
        bool flag = false;
        for (int j = 0; j < n; j++)
        {
            cin >> a[j];
        }
        for (int j = 0; j < n; j++)
        {
            if (s.size() > 0 && s.top() == a[j])
            {
                s.pop();
            }
            else
            {
                if (q > a[j])
                {
                    flag = true;
                    break;
                }
                for (; q < a[j]; q++)
                {
                    s.push(q);
                }
                if (s.size() >= m)
                {
                    flag = true;
                    break;
                }
                q = a[j] + 1;
            }
        }
        if (flag)
            cout << "NO" << endl;
        else
            cout << "YES" << endl;
        while (!s.empty())
            s.pop();
    }

    return 0;
}
