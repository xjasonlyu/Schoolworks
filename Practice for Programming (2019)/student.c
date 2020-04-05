//
// Created by Jason Lyu on 8/24/19.
//

#include <stdio.h>
#include <string.h>
#include "student.h"

NODE *Append(NODE *head, STUDENT student) {
    NODE *temp = (NODE *) malloc(sizeof(NODE));
    temp->student = student;
    temp->next = head;
    head = temp;
    return head;
}

int Delete(NODE **head, int id) {
    NODE *current = *head;
    NODE *previous = NULL;

    // if list is empty
    if (*head == NULL) {
        return 0;
    }

    // navigate through list
    while (current->student.id != id) {
        // if it is last node
        if (current->next == NULL) {
            return 0;
        } else {
            // store reference to current link
            previous = current;
            // move to next link
            current = current->next;
        }
    }

    // found a match, update the link
    if (previous == NULL) {
        // change first to point to next link
        *head = (*head)->next;
        return 1;
    } else {
        // bypass the current link
        previous->next = current->next;
    }

    // free memory
    free(current);
    return 1;
}

STUDENT *SearchByID(NODE *head, int id) {
    NODE *temp = head;
    while (temp != NULL) {
        if (temp->student.id == id) {
            return &(temp->student);
        }
        temp = temp->next;
    }
    return NULL;
}

STUDENT *SearchByName(NODE *head, char *name) {
    NODE *temp = head;
    while (temp != NULL) {
        if (!strcmp(temp->student.name, name)) {
            return &(temp->student);
        }
        temp = temp->next;
    }
    return NULL;
}

int Length(NODE *head) {
    int length = 0;
    NODE *current;
    for (current = head; current != NULL; current = current->next) {
        length++;
    }
    return length;
}

void Sort(NODE *head) {
    if (head == NULL) {
        return;
    }

    int i, j, k;
    STUDENT student;
    NODE *current;
    NODE *next;

    int size = Length(head);
    k = size;

    for (i = 0; i < size - 1; i++, k--) {
        current = head;
        next = head->next;

        for (j = 1; j < k; j++) {

            if (current->student.id > next->student.id) {
                student = current->student;
                current->student = next->student;
                next->student = student;
            }

            current = current->next;
            next = next->next;
        }
    }
}

void Close(NODE *head) {
    NODE *temp = head;
    NODE *p;
    while (temp != NULL) {
        p = temp;
        temp = temp->next;
        free(p);
    }
}

void Print(NODE *head, FILE *f, int flag) {
    NODE *current;
    STUDENT student;
    for (current = head; current != NULL; current = current->next) {
        student = current->student;
        if (flag == 0) {
            fprintf(f, "%d\t%s\t%s\t%d\t%d\t%d\n",
                    student.id, student.name, student.gender,
                    student.chi, student.eng, student.math);
        } else if (flag == 1) {
            fprintf(f, "学号：%d，姓名：%s，性别：%s，语文：%d，英语：%d，数学：%d\n",
                    student.id, student.name, student.gender,
                    student.chi, student.eng, student.math);
        }
    }
    fflush(f);
}
