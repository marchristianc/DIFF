#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

void storeLines(FILE* file, char a[1024][1024], int *lines);
double isSimilar(char src[1024], char tgt[1024]);
int getStrLen(char src[1024]);
void compare(char fOne[1024][1024], char fTwo[1024][1024], int oLen, int tLen, int mode, int inverted);
void printSide(char left[1024], char* prefix, char right[1024], int mode);
void sideBySide(char fOne[1024][1024], char fTwo[1024][1024], int oLen, int tLen, int mode, int inverted);
void context(char fOne[1024][1024], char fTwo[1024][1024], char* fo_name, char* ft_name, int oLen, int tLen, int inverted, int tContext);
void unified(char fOne[1024][1024], char fTwo[1024][1024], char* fo_name, char* ft_name, int oLen, int tLen, int inverted, int tContext);
void print(char a[1024][1024]);
bool isNumber(char number[]);