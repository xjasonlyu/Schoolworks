//
// Created by Jason Lyu on 8/24/19.
//

#ifndef PROGRAM_STUDENT_H
#define PROGRAM_STUDENT_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_STR_LEN 100

typedef struct {
    int id;
    int chi, eng, math;
    char name[MAX_STR_LEN];
    char gender[MAX_STR_LEN];
} STUDENT;

typedef struct node {
    STUDENT student;
    struct node *next;
} NODE;

NODE *Append(NODE *, STUDENT);

int Delete(NODE **, int);

STUDENT *SearchByID(NODE *, int);

STUDENT *SearchByName(NODE *, char *);

int Length(NODE *);

void Sort(NODE *);

void Close(NODE *);

void Print(NODE *, FILE *, int);

#endif //PROGRAM_STUDENT_H
