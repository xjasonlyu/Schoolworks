#include <iostream>
#include <cstring>

using namespace std;

#define STACKSIZE 100
typedef char DataType;

typedef struct {
	DataType* base;
	DataType* top;
	int	stacksize;
}SqStack;

//初始化空顺序栈
int InitStack(SqStack& S) {
    S.base = new DataType[STACKSIZE];
    if (S.base == NULL)
        return 0;
    // init
    S.top = S.base;
    S.stacksize = STACKSIZE;
    return 1;
}

//判栈空
int StackEmpty(SqStack S) {
    return (S.base == S.top) ? 1 : 0;
}

//入栈
int Push(SqStack& S, DataType item) {
    if (S.top - S.base == S.stacksize)
        return 0;
    *(S.top++) = item;
    return 1;
}


//出栈
int Pop(SqStack& S, DataType& item) {
    if (S.top - S.base == 0)
        return 0;
    item = *(--S.top);
    return 1;
}

//取栈顶，细节在此不表
int GetTop(SqStack S, DataType &item) {
    item = *(S.top-1);
    return 1;
}

//表达式括号匹配判断函数，自定义
int matching(char* exp) {
    char c;
    char w;

    int err;

    SqStack L;
    InitStack(L);

    for (int i=0; i < strlen(exp); i++) {
        c = exp[i];
        switch (c)
        {
        case '{':
            Push(L, '{');
            break;

        case '[':
            Push(L, '[');
            break;

        case '(':
            Push(L, '(');
            break;

        case '}':
            err = Pop(L, w);
            if (!err || w != '{')
                goto out;
            break;

        case ']':
            err = Pop(L, w);
            if (!err || w != '[')
                goto out;
            break;

        case ')':
            err = Pop(L, w);
            if (!err || w != '(')
                goto out;
            break;

        default:
            continue;
        }
    }

    if (StackEmpty(L))
        return 1;

out:
    return 0;
}

int main() {
	char expr[100];
	//cout << "请输入表达式:";
	cin >> expr;
	char* ex = expr;
	if (matching(ex))
        cout << "匹配成功";
	else
        cout << "匹配不成功";

	return 0;
}
