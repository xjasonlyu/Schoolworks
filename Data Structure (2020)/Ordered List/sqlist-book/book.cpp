#include <iostream>
#include <string.h>
#include "book.h"

// 重载输出符，方便直接打印 book 信息
std::ostream& operator<<(std::ostream& os, const BOOK& b)
{
    return os << "Name: \"" << b.name << "\"\tNumber: "
        << b.number << "\tPrice: " << b.price << std::endl;
}

// 重载 == 比较符
bool operator==(const BOOK& a, const BOOK&  b)
{
    // 这里仅通过比较 number 唯一书号来确认是同一本书，下同
    return strcmp(a.number, b.number) ? false : true;
}

// 重载 ！= 比较符
bool operator!=(const BOOK& a, const BOOK&  b)
{
    return strcmp(a.number, b.number) ? true : false;
}
