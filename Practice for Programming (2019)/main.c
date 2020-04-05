//
// Created by Jason Lyu on 8/24/19.
//

#include <stdio.h>
#include <string.h>
#include "student.h"

#define _VERSION_ "v0.0.1"
#define _DATA_FILE_ "student_data.txt"

void show_menu();

void show_version();

void clear_screen();

void format_screen();

void input(char *str);

NODE *load_data(NODE *, FILE *);

int main() {
    NODE *head = NULL;

    FILE *fp;

    // basic variables
    int cmd, subCmd, flag;
    char str[MAX_STR_LEN];

    STUDENT student, *studentTemp;
    int id, chi, eng, math;
    char name[MAX_STR_LEN];
    char gender[MAX_STR_LEN];

    // open file or create file
    fp = fopen(_DATA_FILE_, "ab+");
    if (fp == NULL) {
        printf("cannot open file");
        exit(EXIT_FAILURE);
    }

    // re-open in read mode
    fp = freopen(_DATA_FILE_, "rb", fp);
    // load data from file
    head = load_data(head, fp);

    // display menu
    format_screen();

    while (1) {
        printf("输入命令 [1-7]>>>");
        input(str);
        cmd = atoi(str);
        switch (cmd) {
            case 1:
                // clear screen
                format_screen();

                printf("添加学生 学号：");
                input(str);
                id = atoi(str);

                if (SearchByID(head, id) != NULL) {
                    // clear screen
                    format_screen();
                    printf("无法添加：学号为%d的学生已存在\n", id);
                    break;
                }

                printf("添加学生 姓名：");
                input(str);
                strcpy(name, str);

                printf("添加学生 性别：");
                input(str);
                strcpy(gender, str);

                printf("添加学生 语文成绩：");
                input(str);
                chi = atoi(str);

                printf("添加学生 英语成绩：");
                input(str);
                eng = atoi(str);

                printf("添加学生 数学成绩：");
                input(str);
                math = atoi(str);

                student.id = id;
                strcpy(student.name, name);
                strcpy(student.gender, gender);
                student.chi = chi;
                student.eng = eng;
                student.math = math;
                head = Append(head, student);

                // success
                printf("学生添加成功！ 学号：%d 姓名：%s\n", id, name);
                break;
            case 2:
                // clear screen
                format_screen();

                printf("输入需要删除的学生 学号：");
                input(str);
                id = atoi(str);

                // delete from linked list
                flag = Delete(&head, id);
                if (!flag) {
                    printf("此学生不存在 学号：%d\n", id);
                    break;
                }
                printf("成功删除学生 学号：%d\n", id);
                break;
            case 3:
                // clear screen
                format_screen();

                printf("1)使用学号查找学生\n");
                printf("2)使用姓名查找学生\n");
                printf("查找学生：");
                input(str);
                subCmd = atoi(str);

                if (subCmd == 1) {
                    // ID
                    printf("输入需要查找的学生 学号：");
                    input(str);
                    id = atoi(str);

                    studentTemp = SearchByID(head, id);
                    if (studentTemp == NULL) {
                        printf("此学生不存在 学号：%d\n", id);
                    } else {
                        student = *studentTemp;
                        printf("学号：%d，姓名：%s，性别：%s，语文：%d，英语：%d，数学：%d\n",
                               student.id, student.name, student.gender,
                               student.chi, student.eng, student.math);
                    }
                } else if (subCmd == 2) {
                    // name
                    printf("输入需要查找的学生 姓名：");
                    input(str);
                    strcpy(name, str);

                    studentTemp = SearchByName(head, name);
                    if (studentTemp == NULL) {
                        printf("此学生不存在 姓名：%s\n", name);
                    } else {
                        student = *studentTemp;
                        printf("学号：%d，姓名：%s，性别：%s，语文：%d，英语：%d，数学：%d\n",
                               student.id, student.name, student.gender,
                               student.chi, student.eng, student.math);
                    }
                } else {
                    printf("命令不存在：%d\n", subCmd);
                }
                break;
            case 4:
                // clear screen
                format_screen();

                printf("输入需要修改的学生 学号：");
                input(str);
                id = atoi(str);

                studentTemp = SearchByID(head, id);
                if (studentTemp == NULL) {
                    printf("此学生不存在 学号：%d\n", id);
                    break;
                } else {
                    student = *studentTemp;
                    printf("学号：%d，姓名：%s，性别：%s，语文：%d，英语：%d，数学：%d\n",
                           student.id, student.name, student.gender,
                           student.chi, student.eng, student.math);
                }
                printf("1)学号 2)姓名 3)性别\n");
                printf("4)语文 5)英语 6)数学\n");
                printf("选择需要修改的项目：");
                input(str);
                subCmd = atoi(str);

                if (subCmd == 1) {
                    printf("修改学号（旧学号 %d）：", student.id);
                    input(str);
                    int _id;
                    _id = atoi(str);

                    if (SearchByID(head, _id) != NULL) {
                        // clear screen
                        format_screen();
                        printf("无法修改：学号为%d的学生已存在\n", _id);
                        break;
                    }
                    student.id = _id;
                } else if (subCmd == 2) {
                    printf("修改姓名（旧姓名 %s）：", student.name);
                    input(str);
                    strcpy(student.name, str);
                } else if (subCmd == 3) {
                    printf("修改性别（旧性别 %s）：", student.gender);
                    input(str);
                    strcpy(student.gender, str);
                } else if (subCmd == 4) {
                    printf("修改语文成绩（旧成绩 %d）：", student.chi);
                    input(str);
                    student.chi = atoi(str);
                } else if (subCmd == 5) {
                    printf("修改英语成绩（旧成绩 %d）：", student.eng);
                    input(str);
                    student.eng = atoi(str);
                } else if (subCmd == 6) {
                    printf("修改数学成绩（旧成绩 %d）：", student.math);
                    input(str);
                    student.math = atoi(str);
                }

                Delete(&head, id);
                head = Append(head, student);
                printf("学生信息修改成功！");
                printf("学号：%d，姓名：%s，性别：%s，语文：%d，英语：%d，数学：%d\n",
                       student.id, student.name, student.gender,
                       student.chi, student.eng, student.math);
                break;
            case 5:
                // clear screen
                format_screen();

                if (Length(head) == 0) {
                    printf("没有学生信息！");
                } else {
                    Sort(head);
                    Print(head, stdout, 1);
                }
                break;
            case 6:
                // clear screen
                format_screen();

                show_version();
                break;
            case 7:
                // clear screen
                format_screen();

                // exit
                printf("退出系统...\n");
                goto exit;
            default:
                // clear screen
                format_screen();

                printf("找不到该命令：%d\n", cmd);
                break;
        }

        // re-open file and write
        fp = freopen(_DATA_FILE_, "wb", fp);
        Sort(head);
        Print(head, fp, 0);
    }

    exit:

    // close file
    fclose(fp);

    // close linked list
    Close(head);

    return 0;
}

void show_menu() {
    printf("+-----------------------------------------+\n");
    printf("|  ,---.   ,---.  ,--.,--.   ,--. ,---.   |\n");
    printf("| '   .-' '   .-' |  ||   `.'   |'   .-'  |\n");
    printf("| `.  `-. `.  `-. |  ||  |'.'|  |`.  `-.  |\n");
    printf("| .-'    |.-'    ||  ||  |   |  |.-'    | |\n");
    printf("| `-----' `-----' `--'`--'   `--'`-----'  |\n");
    printf("|Simple Student Info Manage System, %s|\n", _VERSION_);
    printf("+-----------------------------------------+\n");
    printf("+-----------------+\n");
    printf("|*) 1.添加学生信息|\n");
    printf("+-----------------+\n");
    printf("|*) 2.删除学生信息|\n");
    printf("+-----------------+\n");
    printf("|*) 3.查找学生信息|\n");
    printf("+-----------------+\n");
    printf("|*) 4.修改学生信息|\n");
    printf("+-----------------+\n");
    printf("|*) 5.输出学生成绩|\n");
    printf("+-----------------+\n");
    printf("|*) 6.输出开发信息|\n");
    printf("+-----------------+\n");
    printf("|*) 7.退出信息系统|\n");
    printf("+-----------------+\n");
    fflush(stdout);
}

void show_version() {
    printf("S-S-I-M-S, %s\n", _VERSION_);
    printf("Date: 2019/08/24\n");
    printf("Author: Jason Lyu\n");
    printf("Designed by Jason Lyu for Programing Class in HDU.\n");
    fflush(stdout);
}

void clear_screen() {
#if defined(_WIN32) || defined(__MINGW32__)
    // For Microsoft Windows (MS)
    system("cls");
#elif defined(__unix__) || defined(__posix__) \
 || defined(__linux__) || defined(__APPLE__)
    // For Unix liked system
    system("clear");
#endif
}

void format_screen() {
    clear_screen();
    show_menu();
}

void input(char *str) {
    if (fgets(str, MAX_STR_LEN, stdin) != NULL) {
        size_t len = strlen(str);
        // removing trailing newline
        if (len > 0 && str[len - 1] == '\n') {
            str[--len] = '\0';
        }
    }
}

NODE *load_data(NODE *head, FILE *fp) {
    // variables
    STUDENT student;
#if defined(_WIN32) || defined(__MINGW32__)
    char line[MAX_STR_LEN];
    while ((fgets(line, MAX_STR_LEN, fp)) != NULL) {
#elif defined(__unix__) || defined(__posix__) \
 || defined(__linux__) || defined(__APPLE__)
    char *line = NULL;
    size_t len = 0;
    while ((getline(&line, &len, fp)) >= 0) {
#endif
        sscanf(line, "%d\t%s\t%s\t%d\t%d\t%d\n",
               &student.id, student.name, student.gender,
               &student.chi, &student.eng, &student.math);
        head = Append(head, student);
    }
    return head;
}
