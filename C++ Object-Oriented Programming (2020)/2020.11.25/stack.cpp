#include <iostream>
#include <string>

using namespace std;

template <class T, int size>
class Stack {
  public:
    Stack() : empty(-1) { top = empty; };
    void push(T);
    T pop();
    int is_empty();
    int is_full();

  private:
    T items[size];
    int empty, top;
};

template <class T, int size>
void Stack<T, size>::push(T c) { items[++top] = c; }

template <class T, int size>
T Stack<T, size>::pop() { return items[top--]; }

template <class T, int size>
int Stack<T, size>::is_full() { return top + 1 == size; }

template <class T, int size>
int Stack<T, size>::is_empty() { return top == empty; }

int main() {
    Stack<char, 10> s;
    char ch;
    while ((ch = cin.get()) != '\n')
        if (!s.is_full())
            s.push(ch);
    while (!s.is_empty())
        cout << s.pop();
    cout << endl;

    Stack<int, 7> ds;
    double d[] = {1, 3, 5, 7, 9, 0};
    int i = 0;
    while (d[i] != 0 && !ds.is_full())
        if (!ds.is_full())
            ds.push(d[i++]);
    while (!ds.is_empty())
        cout << ds.pop() << "  ";
    cout << endl;

    return 0;
}
