#ifndef BOOK_H
#define BOOK_H

#include <iostream>

#define BOOK_NAME_LEN 100
#define BOOK_NUMBER_LEN 10

typedef struct book {
    char  name[BOOK_NAME_LEN];
    char  number[BOOK_NUMBER_LEN];
    float price;
}BOOK;

std::ostream& operator<<(std::ostream& os, const BOOK& b);

bool operator==(const BOOK& a, const BOOK&  b);

bool operator!=(const BOOK& a, const BOOK&  b);

#endif