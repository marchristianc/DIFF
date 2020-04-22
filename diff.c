#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>

#include "diff.h"

void storeLines(FILE* file, char a[1024][1024], int *lines){
    int ctr = 0;
    char c;
    while((c = fgetc(file)) != EOF){
        if(c == '\n'){
            a[*lines][ctr] = '\0';
            (*lines)++;
            ctr = 0;
            continue;
        }
        a[*lines][ctr++] = c;
    }
}

double isSimilar(char src[1024], char tgt[1024]){
    int wordLength = 0;
    int ctr = 0; //misspell counter
    while(src[wordLength] != '\0'){
        if(src[wordLength] != tgt[wordLength])
            ctr++;
        wordLength++;
    }
    return (ctr/wordLength)*100;
}

int getStrLen(char src[1024]){
    int wLen = 0;
    while(src[wLen++] != '\0');
    return wLen;
}

void compare(char fOne[1024][1024], char fTwo[1024][1024], int oLen, int tLen, int mode, int inverted){
    int isDIFF = 0;
    int c_mode = 0;

    int last_i = 0;
    int fl_found = 0;
    int last_j = 0;
    int i, j;
    for(i = 0; i < oLen; i++){
        if(fOne[i] == NULL || fOne[i] == '\0')
            continue;
        fl_found = 0;
        for(j = last_j; j < tLen; j++){
            if(fTwo[j] == NULL || fTwo[j] == '\0'){
                last_j = j+1;
                break;
            }
            if(strcmp(fOne[i], fTwo[j]) == 0){ //add
                fl_found = 1;
                if(j-last_j > 1){
                    if((mode == 1) || (mode == 3)){
                        printf("Files are different\n");
                        return;
                    }
                    isDIFF = 1;
                    if(j == (i+1)){
                        if(isSimilar(fOne[i], fTwo[j]) < 35.00){ //misspelling error less than 35%
                            if(!inverted){
                                printf("%dc%d\n", i, j);
                                printf("< %s\n", fOne[i-1]);
                                printf("--\n");
                            }else{
                                printf("%dc%d\n", j, i);
                                printf("< %s\n", fTwo[j-1]);
                                printf("--\n");
                                c_mode = 1;
                            }
                        }else{
                            if(!inverted){
                                printf("%da%d\n", i, j);
                            }else{
                                printf("%dd%d\n", j, i);
                            }
                        }
                    }else{
                        if(!inverted){
                            printf("%da%d,%d\n", i, i+1, j);
                        }else{
                            printf("%d,%dd%d\n", i+1, j, i);
                        }
                    }
                    if(!inverted){
                        for(int k = i; k < j; k++)
                            printf("> %s\n", fTwo[k]);
                    }else{
                        if(c_mode){
                            for(int k = i; k < j; k++)
                                printf("> %s\n", fOne[k-1]);
                            c_mode = 0;
                        }else{
                            for(int k = i; k < j; k++)
                                printf("< %s\n", fTwo[k]);
                        }
                    }
                }
                last_j = j;
                break;
            }
        }
        if(!fl_found){
            last_i++;
        }else if(last_i > 1){
            if((mode == 1) || (mode == 3)){
                printf("Files are different\n");
                return;
            }
            isDIFF = 1;
            if(!inverted){
                printf("%d,%dd%d\n", i-(last_i-1), i+1, last_j+1);
                for(int k = i-last_i; k < i+1; k++)
                    printf("< %s\n", fOne[k]);
                i++;
                last_i = 0;
            }else{
                printf("%da%d,%d\n", last_j+1, i-(last_i-1), i+1);
                for(int k = i-last_i; k < i+1; k++)
                    printf("> %s\n", fOne[k]);
                i++;
                last_i = 0;
            }
        }else{
            last_i = 0;
        }
    }
    if(oLen < tLen){ //add remaining content
        isDIFF = 1;
        printf("%da%d,%d\n", oLen-1, oLen+1, tLen);
        for(int k = oLen; k < tLen; k++)
            printf("> %s\n", fTwo[k]);
    }else if(oLen > tLen){ //add remaining content
        isDIFF = 1;
        i++;
        printf("%d,%dd%d\n", i-(last_i-1), i-1, last_j);
        for(int k = i-last_i; k < i-1; k++)
            printf("< %s\n", fTwo[k]);
        last_i = 0;
    }
    if(!isDIFF && (mode == 2 || mode == 3))
        printf("Files are identical\n");
}

void printSide(char left[1024], char* prefix, char right[1024], int mode){
    if(strcmp(prefix, " ") == 0 && mode == 6)
        return;
    printf("%s", left);
    int _len = getStrLen(left);
    if(_len < 2)
        printf("\t\t\t\t\t");
    else if(_len < 16)
        printf("\t\t\t\t");
    else if(_len < 25)
        printf("\t\t\t");
    else
        printf("\t\t");
    if(strcmp(prefix, " ") == 0 && mode == 5){
        prefix = "(";
        right = "";
    }
    printf("|%s%s\n", prefix, right);
}

void sideBySide(char fOne[1024][1024], char fTwo[1024][1024], int oLen, int tLen, int mode, int inverted){
    int c_mode = 0;

    int last_i = 0;
    int fl_found = 0;
    int last_j = 0;
    int i, j;
    for(i = 0; i < oLen; i++){
        if(fOne[i] == NULL || fOne[i] == '\0')
            continue;
        fl_found = 0;
        for(j = last_j; j < tLen; j++){
            if(fTwo[j] == NULL || fTwo[j] == '\0'){
                last_j = j+1;
                break;
            }
            if(strcmp(fOne[i], fTwo[j]) == 0){ //add
                fl_found = 1;
                if(j-last_j > 1){
                    if(j == (i+1)){
                        if(isSimilar(fOne[i-1], fTwo[j-1]) < 35.00){ //misspelling error less than 35%
                            if(!inverted){
                                printSide(fOne[i-1], "! ", fTwo[j-1], 0);
                            }else{
                                printSide(fTwo[j-1], "! ", fOne[i-1], 0);
                            }
                            c_mode = 1;
                        }
                    }
                    if(!inverted){
                        if(!c_mode)
                            for(int k = i; k < j; k++)
                                printf("%s\t\t\t|> %s\n", "\t\t", fTwo[k]);
                    }else{
                        if(!c_mode)
                            for(int k = i; k < j; k++)
                                printSide(fTwo[k], "< ", "", 0);
                    }
                    i--;
                }else{
                    printSide(fOne[i], " ", fTwo[j], mode);
                }
                last_j = j;
                break;
            }
        }
        if(!fl_found){
            last_i++;
        }else if(last_i > 1){
            if(!inverted){
                for(int k = i-last_i; k < i+1; k++){
                    printSide(fOne[k], "< ", "", 0);
                }
                last_i = 0;
            }else{
                for(int k = i-last_i; k < i+1; k++)
                    printf("%s\t\t\t|> %s\n", "\t\t", fOne[k]);
                last_i = 0;
            }
        }else{
            last_i = 0;
        }
    }
    if(oLen < tLen){ //add remaining content
        for(int k = oLen; k < tLen; k++)
            printf("%s\t\t\t|> %s\n", "\t\t", fTwo[k]);
    }else if(oLen > tLen){ //add remaining content
        i++;
        for(int k = i-last_i; k < i-1; k++)
            printSide(fTwo[k], "< ", "", 0);
        last_i = 0;
    }
}

void context(char fOne[1024][1024], char fTwo[1024][1024], char* fo_name, char* ft_name, int oLen, int tLen, int inverted, int tContext){
    struct stat t_1sb;
    struct stat t_2sb;
    stat(fo_name, &t_1sb);
    stat(ft_name, &t_2sb);
    char a_buffer[80];
    char b_buffer[80];
    strftime(a_buffer, 26, "%Y-%m-%d %H:%M:%S", localtime(&t_1sb.st_atime));
    strftime(b_buffer, 26, "%Y-%m-%d %H:%M:%S", localtime(&t_2sb.st_atime));
    printf("*** %s %s -%lo\n", fo_name, a_buffer, (unsigned long) t_1sb.st_mode);
    printf("--- %s %s -%lo\n", ft_name, b_buffer, (unsigned long) t_2sb.st_mode);
    int c_mode = 0;

    int last_i = 0;
    int fl_found = 0;
    int last_j = 0;
    int i, j;
    for(i = 0; i < oLen; i++){
        if(fOne[i] == NULL || fOne[i] == '\0')
            continue;
        fl_found = 0;
        for(j = last_j; j < tLen; j++){
            if(fTwo[j] == NULL || fTwo[j] == '\0'){
                last_j = j+1;
                break;
            }
            if(strcmp(fOne[i], fTwo[j]) == 0){ //add
                fl_found = 1;
                if(j-last_j > 1){
                    printf("***************\n");
                    if(j == (i+1)){
                        if(isSimilar(fOne[i-1], fTwo[j-1]) < 35.00){ //misspelling error less than 35%
                            if(!inverted){
                                if(tContext > 0){
                                    if((i-1)-tContext > 0){
                                        printf("*** %d,%d ****\n", i-tContext, i+tContext);
                                        for(int k = i-1-tContext; k < i+tContext; k++){
                                                if(k != i-1){
                                                    printf(" %s\n", fOne[k]);
                                                }else{
                                                    printf("! %s\n", fOne[k]);
                                                }
                                        }
                                        printf("--- %d,%d ----\n", j-tContext, j+tContext);
                                        for(int k = j-1-tContext; k < j+tContext; k++){
                                                if(k != j-1){
                                                    printf(" %s\n", fTwo[k]);
                                                }else{
                                                    printf("! %s\n", fTwo[k]);
                                                }
                                        }
                                    }else{
                                        printf("*** %d,%d ****\n", i-1, i+tContext);
                                        for(int k = i-1; k < i+tContext; k++){
                                            if(k != i-1){
                                                printf(" %s\n", fOne[k]);
                                            }else{
                                                printf("! %s\n", fOne[k]);
                                            }
                                        }
                                        printf("--- %d,%d ----\n", j-1, j+tContext);
                                        for(int k = j-1; k < j+tContext; k++){
                                            if(k != j-1){
                                                printf(" %s\n", fTwo[k]);
                                            }else{
                                                printf("! %s\n", fTwo[k]);
                                            }
                                        }
                                    }
                                }else{
                                    printf("*** %d ****\n", i);
                                    printf("! %s\n", fOne[i-1]);
                                    printf("--- %d ----\n", j);
                                    printf("! %s\n", fTwo[j-1]);
                                }
                            }else{
                                if(tContext > 0){
                                    if((i-1)-tContext > 0){
                                        printf("*** %d,%d ****\n", j-tContext, j+tContext);
                                        for(int k = j-1-tContext; k < j+tContext; k++){
                                                if(k != j-1){
                                                    printf(" %s\n", fTwo[k]);
                                                }else{
                                                    printf("! %s\n", fTwo[k]);
                                                }
                                        }
                                        printf("--- %d,%d ----\n", i-tContext, i+tContext);
                                        for(int k = i-1-tContext; k < i+tContext; k++){
                                                if(k != i-1){
                                                    printf(" %s\n", fOne[k]);
                                                }else{
                                                    printf("! %s\n", fOne[k]);
                                                }
                                        }
                                    }else{
                                        printf("*** %d,%d ****\n", j-1, j+tContext);
                                        for(int k = j-1; k < j+tContext; k++){
                                            if(k != j-1){
                                                printf(" %s\n", fTwo[k]);
                                            }else{
                                                printf("! %s\n", fTwo[k]);
                                            }
                                        }
                                        printf("--- %d,%d ----\n", i-1, i+tContext);
                                        for(int k = i-1; k < i+tContext; k++){
                                            if(k != i-1){
                                                printf(" %s\n", fOne[k]);
                                            }else{
                                                printf("! %s\n", fOne[k]);
                                            }
                                        }
                                    }
                                }else{
                                    printf("*** %d ****\n", j);
                                    printf("! %s\n", fTwo[j-1]);
                                    printf("--- %d ----\n", i);
                                    printf("! %s\n", fOne[i-1]);
                                }
                            }
                            c_mode = 1;
                        }
                    }
                    if(!inverted){
                        if(!c_mode){
                            if(tContext > 0){
                                if(i-tContext > 0){
                                    printf("*** %d,%d ****\n", i-tContext, i+tContext);
                                    printf("--- %d,%d -----\n", i-tContext, j+tContext);
                                    for(int k = i-tContext; k < j+tContext; k++){
                                        if(k >= j || k < i){
                                            printf(" %s\n", fTwo[k]);
                                        }else
                                            printf("+ %s\n", fTwo[k]);
                                    }
                                }else{
                                    printf("*** %d,%d ****\n", i+1, i+tContext);
                                    printf("--- %d,%d -----\n", i+1, j+tContext);
                                    for(int k = i; k < j+tContext; k++){
                                        if(k >= j){
                                            printf(" %s\n", fTwo[k]);
                                        }else
                                            printf("+ %s\n", fTwo[k]);
                                    }
                                }
                            }else{
                                printf("*** %d ****\n", i);
                                printf("--- %d,%d -----\n", i+1, j);
                                for(int k = i; k < j; k++)
                                    printf("+ %s\n", fTwo[k]);
                            }
                        }
                    }else{
                        if(!c_mode){
                            if(tContext > 0){
                                if((i+1)-tContext > 0){
                                    printf("*** %d,%d ****\n", (i+1)-tContext, j+tContext);
                                    for(int k = i-tContext; k < j+tContext; k++){
                                        if(k >= j || k < i){
                                            printf(" %s\n", fTwo[k]);
                                        }else
                                            printf("- %s\n", fTwo[k]);
                                    }
                                }else{
                                    printf("*** %d,%d ****\n", i+1, j+tContext);
                                    for(int k = i; k < j+tContext; k++){
                                        if(k >= j){
                                            printf(" %s\n", fTwo[k]);
                                        }else
                                            printf("- %s\n", fTwo[k]);
                                    }
                                }
                                printf("--- %d -----\n", i);
                            }else{
                                printf("*** %d,%d ****\n", i+1, j);
                                for(int k = i; k < j; k++)
                                    printf("- %s\n", fTwo[k]);
                                printf("--- %d -----\n", i);
                            }
                        }
                    }
                    i--;
                }
                last_j = j;
                break;
            }
        }
        if(!fl_found){
            last_i++;
        }else if(last_i > 1){
            printf("***************\n");
            if(!inverted){
                if(tContext > 0){
                    if((i-(last_i-1))-tContext > 0){
                        printf("*** %d,%d ****\n", i-(last_i-1)-tContext, i+1+tContext);
                        for(int k = (i-last_i)-tContext; k < i+1+tContext; k++){
                            if(k >= j || k < i-last_i)
                                printf(" %s\n", fOne[k]);
                            else
                                printf("- %s\n", fOne[k]);
                        }
                        printf("--- %d,%d -----\n", last_j+1-tContext,last_j+1+tContext);
                        last_i = 0;
                    }else{
                        printf("*** %d,%d ****\n", i-(last_i-1), i+1+tContext);
                        for(int k = i-last_i; k < i+1+tContext; k++){
                            if(k >= i+1)
                                printf(" %s\n", fOne[k]);
                            else
                                printf("- %s\n", fOne[k]);
                        }
                        printf("--- %d,%d -----\n", last_j+1,last_j+1+tContext);
                        last_i = 0;
                    }
                }else{
                    printf("*** %d,%d ****\n", i-(last_i-1), i+1);
                    for(int k = i-last_i; k < i+1; k++)
                        printf("- %s\n", fOne[k]);
                    printf("--- %d -----\n", last_j+1);
                    last_i = 0;
                }
            }else{
                if(tContext > 0){
                    if((i-(last_i-1))-tContext > 0){
                        printf("*** %d,%d ****\n", i-(last_i-1)-tContext, i+1+tContext);
                        printf("--- %d,%d -----\n", last_j+1-tContext,last_j+1+tContext);
                        for(int k = (i-last_i)-tContext; k < i+1+tContext; k++){
                            if(k >= j || k < i-last_i)
                                printf(" %s\n", fOne[k]);
                            else
                                printf("+ %s\n", fOne[k]);
                        }
                        last_i = 0;
                    }else{
                        printf("*** %d,%d ****\n", i-(last_i-1), i+1+tContext);
                        printf("--- %d,%d -----\n", last_j+1,last_j+1+tContext);
                        for(int k = i-last_i; k < i+1+tContext; k++){
                            if(k >= i+1)
                                printf(" %s\n", fOne[k]);
                            else
                                printf("+ %s\n", fOne[k]);
                        }
                        last_i = 0;
                    }
                }else{
                    printf("*** %d ****\n", last_j+1);
                    printf("--- %d,%d -----\n", i-(last_i-1), i+1);
                    for(int k = i-last_i; k < i+1; k++)
                        printf("+ %s\n", fOne[k]);
                    last_i = 0;
                }
            }
        }else{
            last_i = 0;
        }
    }
    if(oLen < tLen){ //add remaining content
        if(tContext > 0){
            if(oLen+2-tContext < tLen){
                printf("***************\n");
                printf("*** %d,%d ****\n", oLen-tContext, oLen+tContext);
                printf("--- %d,%d -----\n", oLen+2-tContext, tLen+tContext);
                for(int k = oLen+1-tContext; k < tLen; k++){
                    if(k >= oLen+1 && k < tLen)
                        printf("+ %s\n", fTwo[k]);
                    else
                        printf(" %s\n", fTwo[k]);
                }
            }else{
                printf("***************\n");
                printf("*** %d,%d ****\n", oLen, oLen);
                printf("--- %d,%d -----\n", oLen+2, tLen);
                for(int k = oLen+1; k < tLen; k++){
                    if(k >= oLen+1 && k < tLen)
                        printf("+ %s\n", fTwo[k]);
                    else
                        printf(" %s\n", fTwo[k]);
                }
            }   
        }else{
            printf("***************\n");
            printf("*** %d ****\n", oLen);
            printf("--- %d,%d -----\n", oLen+2, tLen);
            for(int k = oLen+1; k < tLen; k++)
                printf("+ %s\n", fTwo[k]);
        }
    }else if(oLen > tLen){ //add remaining content
        i++;
        if(tContext > 0){
            if(oLen+2+tContext < tLen){
                printf("***************\n");
                printf("*** %d,%d ****\n", oLen-tContext, oLen+tContext);
                for(int k = oLen+1-tContext; k < tLen; k++){
                    if(k >= oLen+1 && k < tLen)
                        printf("- %s\n", fTwo[k]);
                    else
                        printf(" %s\n", fTwo[k]);
                }
                printf("--- %d,%d -----\n", oLen+2-tContext, tLen+tContext);
            }else{
                printf("***************\n");
                printf("*** %d,%d ****\n", tLen+2-tContext, tLen);
                for(int k = oLen+1-tContext; k < tLen; k++){
                    if(k >= oLen+1 && k < tLen)
                        printf("- %s\n", fTwo[k]);
                    else
                        printf(" %s\n", fTwo[k]);
                }
                printf("--- %d,%d -----\n", tLen+2-tContext, tLen);
            }   
        }else{
            printf("***************\n");
            printf("*** %d,%d ****\n", tLen+2,oLen);
            for(int k = oLen+1; k < tLen; k++)
                printf("- %s\n", fTwo[k]);
            printf("--- %d -----\n", tLen);
        }
    }
}

void unified(char fOne[1024][1024], char fTwo[1024][1024], char* fo_name, char* ft_name, int oLen, int tLen, int inverted, int tContext){
    struct stat t_1sb;
    struct stat t_2sb;
    stat(fo_name, &t_1sb);
    stat(ft_name, &t_2sb);
    char a_buffer[80];
    char b_buffer[80];
    strftime(a_buffer, 26, "%Y-%m-%d %H:%M:%S", localtime(&t_1sb.st_atime));
    strftime(b_buffer, 26, "%Y-%m-%d %H:%M:%S", localtime(&t_2sb.st_atime));
    printf("--- %s %s -%lo\n", fo_name, a_buffer, (unsigned long) t_1sb.st_mode);
    printf("+++ %s %s -%lo\n", ft_name, b_buffer, (unsigned long) t_2sb.st_mode);
    int c_mode = 0;

    int last_i = 0;
    int fl_found = 0;
    int last_j = 0;
    int i, j;
    for(i = 0; i < oLen; i++){
        if(fOne[i] == NULL || fOne[i] == '\0')
            continue;
        fl_found = 0;
        for(j = last_j; j < tLen; j++){
            if(fTwo[j] == NULL || fTwo[j] == '\0'){
                last_j = j+1;
                break;
            }
            if(strcmp(fOne[i], fTwo[j]) == 0){ //add
                fl_found = 1;
                if(j-last_j > 1){
                    if(j == (i+1)){
                        if(isSimilar(fOne[i-1], fTwo[j-1]) < 35.00){ //misspelling error less than 35%
                            if(!inverted){
                                if(tContext > 0){
                                    if((i-1)-tContext > 0){
                                        printf("@ -%d,%d +%d,%d @\n", i-tContext, i+tContext, j-tContext, j+tContext);
                                        for(int k = i-1-tContext; k < i+tContext; k++){
                                                if(k != i-1){
                                                    printf(" %s\n", fOne[k]);
                                                }else{
                                                    printf("+%s\n", fOne[k]);
                                                }
                                        }
                                        for(int k = j-1-tContext; k < j+tContext; k++){
                                                if(k != j-1){
                                                    printf(" %s\n", fTwo[k]);
                                                }else{
                                                    printf("-%s\n", fTwo[k]);
                                                }
                                        }
                                    }else{
                                        printf("@ -%d,%d +%d,%d @*\n", i-1, i+tContext, j-1, j+tContext);
                                        for(int k = i-1; k < i+tContext; k++){
                                            if(k != i-1){
                                                printf(" %s\n", fOne[k]);
                                            }else{
                                                printf("-%s\n", fOne[k]);
                                            }
                                        }
                                        for(int k = j-1; k < j+tContext; k++){
                                            if(k != j-1){
                                                printf(" %s\n", fTwo[k]);
                                            }else{
                                                printf("+%s\n", fTwo[k]);
                                            }
                                        }
                                    }
                                }else{
                                    printf("@ -%d +%d@\n", j, i);
                                    printf("-%s\n", fOne[i-1]);
                                    printf("+%s\n", fTwo[j-1]);
                                }
                            }else{
                                if(tContext > 0){
                                    if((i-1)-tContext > 0){
                                        printf("@ %d,%d %d,%d @\n", j-tContext, j+tContext, i-tContext, i+tContext);
                                        for(int k = j-1-tContext; k < j+tContext; k++){
                                                if(k != j-1){
                                                    printf(" %s\n", fTwo[k]);
                                                }else{
                                                    printf("!%s\n", fTwo[k]);
                                                }
                                        }
                                        for(int k = i-1-tContext; k < i+tContext; k++){
                                                if(k != i-1){
                                                    printf(" %s\n", fOne[k]);
                                                }else{
                                                    printf("!%s\n", fOne[k]);
                                                }
                                        }
                                    }else{
                                        printf("@ %d,%d %d,%d @\n", j-1, j+tContext, i-1, i+tContext);
                                        for(int k = j-1; k < j+tContext; k++){
                                            if(k != j-1){
                                                printf(" %s\n", fTwo[k]);
                                            }else{
                                                printf("!%s\n", fTwo[k]);
                                            }
                                        }
                                        for(int k = i-1; k < i+tContext; k++){
                                            if(k != i-1){
                                                printf(" %s\n", fOne[k]);
                                            }else{
                                                printf("!%s\n", fOne[k]);
                                            }
                                        }
                                    }
                                }else{
                                    printf("@ -%d +%d@\n", j, i);
                                    printf("-%s\n", fTwo[j-1]);
                                    printf("+%s\n", fOne[i-1]);
                                }
                            }
                            c_mode = 1;
                        }
                    }
                    if(!inverted){
                        if(!c_mode){
                            if(tContext > 0){
                                if(i-tContext > 0){
                                    printf("@ -%d,%d +%d,%d @\n", i-tContext, i+tContext, i-tContext, j+tContext);
                                    for(int k = i-tContext; k < j+tContext; k++){
                                        if(k >= j || k < i){
                                            printf(" %s\n", fTwo[k]);
                                        }else
                                            printf("+%s\n", fTwo[k]);
                                    }
                                }else{
                                    printf("@ -%d,%d +%d,%d @\n", i+1, i+tContext, i+1, j+tContext);
                                    for(int k = i; k < j+tContext; k++){
                                        if(k >= j){
                                            printf(" %s\n", fTwo[k]);
                                        }else
                                            printf("+%s\n", fTwo[k]);
                                    }
                                }
                            }else{
                                printf("@ -%d,0 +%d,%d @\n", i, i+1, j);
                                for(int k = i; k < j; k++)
                                    printf("+%s\n", fTwo[k]);
                            }
                        }
                    }else{
                        if(!c_mode){
                            if(tContext > 0){
                                if((i+1)-tContext > 0){
                                    printf("@ -%d,%d +%d,%d @\n", (i+1)-tContext, j+tContext, i, i);
                                    for(int k = i-tContext; k < j+tContext; k++){
                                        if(k >= j || k < i){
                                            printf(" %s\n", fTwo[k]);
                                        }else
                                            printf("-%s\n", fTwo[k]);
                                    }
                                }else{
                                    printf("@ -%d,%d +%d,%d @\n", i+1, j+tContext, i, i);
                                    for(int k = i; k < j+tContext; k++){
                                        if(k >= j){
                                            printf(" %s\n", fTwo[k]);
                                        }else
                                            printf("-%s\n", fTwo[k]);
                                    }
                                }
                            }else{
                                printf("@ -%d,%d +%d,%d @\n", i+1, j, i, i);
                                for(int k = i; k < j; k++)
                                    printf("-%s\n", fTwo[k]);
                            }
                        }
                    }
                    i--;
                }
                last_j = j;
                break;
            }
        }
        if(!fl_found){
            last_i++;
        }else if(last_i > 1){
            if(!inverted){
                if(tContext > 0){
                    if((i-(last_i-1))-tContext > 0){
                        printf("@ -%d,%d +%d,%d @\n", i-(last_i-1)-tContext, i+1+tContext, last_j+1-tContext,last_j+1+tContext);
                        for(int k = (i-last_i)-tContext; k < i+1+tContext; k++){
                            if(k >= j || k < i-last_i)
                                printf(" %s\n", fOne[k]);
                            else
                                printf("-%s\n", fOne[k]);
                        }
                        last_i = 0;
                    }else{
                        printf("@ -%d,%d +%d,%d @\n", i-(last_i-1), i+1+tContext, last_j+1,last_j+1+tContext);
                        for(int k = i-last_i; k < i+1+tContext; k++){
                            if(k >= i+1)
                                printf(" %s\n", fOne[k]);
                            else
                                printf("-%s\n", fOne[k]);
                        }
                        last_i = 0;
                    }
                }else{
                    printf("@ -%d,%d +%d,%d @\n", i-(last_i-1), i+1, last_j+1, last_j+1);
                    for(int k = i-last_i; k < i+1; k++)
                        printf("-%s\n", fOne[k]);
                    last_i = 0;
                }
            }else{
                if(tContext > 0){
                    if((i-(last_i-1))-tContext > 0){
                        printf("@ -%d,%d +%d,%d @\n", i-(last_i-1)-tContext, i+1+tContext, last_j+1-tContext,last_j+1+tContext);
                        for(int k = (i-last_i)-tContext; k < i+1+tContext; k++){
                            if(k >= j || k < i-last_i)
                                printf(" %s\n", fOne[k]);
                            else
                                printf("+%s\n", fOne[k]);
                        }
                        last_i = 0;
                    }else{
                        printf("@ -%d,%d +%d,%d @\n", i-(last_i-1), i+1+tContext, last_j+1,last_j+1+tContext);
                        for(int k = i-last_i; k < i+1+tContext; k++){
                            if(k >= i+1)
                                printf(" %s\n", fOne[k]);
                            else
                                printf("+%s\n", fOne[k]);
                        }
                        last_i = 0;
                    }
                }else{
                    printf("@ -%d,%d +%d,%d @\n", last_j+1, last_j+1, i-(last_i-1), i+1);
                    for(int k = i-last_i; k < i+1; k++)
                        printf("+%s\n", fOne[k]);
                    last_i = 0;
                }
            }
        }else{
            last_i = 0;
        }
    }
    if(oLen < tLen){ //add remaining content
        if(tContext > 0){
            if(oLen+2-tContext < tLen){
                printf("@ -%d,%d +%d,%d @\n", oLen-tContext, oLen+tContext, oLen+2-tContext, tLen+tContext);
                for(int k = oLen+1-tContext; k < tLen; k++){
                    if(k >= oLen+1 && k < tLen)
                        printf("+%s\n", fTwo[k]);
                    else
                        printf(" %s\n", fTwo[k]);
                }
            }else{
                printf("@ -%d,%d +%d,%d @\n", oLen, oLen, oLen+2, tLen);
                for(int k = oLen+1; k < tLen; k++){
                    if(k >= oLen+1 && k < tLen)
                        printf("+%s\n", fTwo[k]);
                    else
                        printf(" %s\n", fTwo[k]);
                }
            }   
        }else{
            printf("@ -%d,%d +%d,%d @\n", oLen, oLen, oLen+2, tLen);
            for(int k = oLen+1; k < tLen; k++)
                printf("+%s\n", fTwo[k]);
        }
    }else if(oLen > tLen){ //add remaining content
        i++;
        if(tContext > 0){
            if(oLen+2+tContext < tLen){
                printf("@ -%d,%d +%d,%d @\n", oLen-tContext, oLen+tContext, oLen+2-tContext, tLen+tContext);
                for(int k = oLen+1-tContext; k < tLen; k++){
                    if(k >= oLen+1 && k < tLen)
                        printf("-%s\n", fTwo[k]);
                    else
                        printf(" %s\n", fTwo[k]);
                }
            }else{
                printf("@ -%d,%d +%d,%d @\n", tLen+2-tContext, tLen, tLen+2-tContext, tLen);
                for(int k = oLen+1-tContext; k < tLen; k++){
                    if(k >= oLen+1 && k < tLen)
                        printf("-%s\n", fTwo[k]);
                    else
                        printf(" %s\n", fTwo[k]);
                }
            }   
        }else{
            printf("@ -%d,%d +%d,%d @\n", tLen+2,oLen, tLen, tLen);
            for(int k = oLen+1; k < tLen; k++)
                printf("-%s\n", fTwo[k]);
        }
    }
}

void print(char a[1024][1024]){
    for(int i = 0; i < 30; i++){
        printf("%s\n", a[i]);
    }
}


bool isNumber(char number[]){
    int i = 0;
    for (; number[i] != 0; i++)
        if (!isdigit(number[i]))
            return false;
    return true;
}

int main(int argc, char *argv[]){
    if(argc < 2)
        return 0;

    int mode = 0;

    char* one_fname = argv[argc-2];
    char* two_fname = argv[argc-1];

    int tContext = 3;
    while(*++argv){
        switch((*argv)[1]){
            case 'v':
                printf("diff - Copyright 2020\n");
                printf("Version 1.0\n\n");
                printf("Written by Mar Christian Contreras\n");
                break;
            case 'q': //different, else silent !mode 1
                if(mode == 2)
                    mode++;
                mode = 1;
                break;
            case 's': //same, else usual !mode 2
                if(mode == 1)
                    mode = 3;
                else
                    mode = 2;
                break;
            //both -q & -s report diff if diff, same if same !mode 3
            case 'y': //side by side mode !mode 4
                mode = 4;
                break;
            //-y has to be set for the bottom to work
            //below are different output styles
            case 'c':
                mode = 7;
                break;
            case 'u':
                mode = 8;
                break;
            default: 
                if(strcmp((*argv), "--left-column") == 0){
                    mode = 5;
                }
                if(strcmp((*argv), "--suppress-common-lines") == 0){
                    mode = 6;
                }
                if(isNumber(*argv)){
                    if(mode >= 7){
                        tContext = atoi((*argv));
                        printf("%d\n", tContext);
                    }
                }
            break;
        }
    }

    char fOne[1024][1024];
    char fTwo[1024][1024];
    memset(fOne, 0, 1024*sizeof(fOne[0]));
    memset(fTwo, 0, 1024*sizeof(fTwo[0]));

    int oSize = 0;
    int tSize = 0;

    //argv[1] file1
    //argv[2] fiile2
    FILE* file;
    file = fopen(one_fname, "r");
    storeLines(file, fOne, &oSize);
    fclose(file);

    file = fopen(two_fname, "r");
    storeLines(file, fTwo, &tSize);
    fclose(file);

    if(mode >= 8){
        if(oSize < tSize)
            unified(fOne, fTwo, one_fname, two_fname, oSize, tSize, (oSize < tSize) ? 0 : 1, tContext);
        else
            unified(fTwo, fOne, one_fname, two_fname, tSize, oSize, (oSize < tSize) ? 0 : 1, tContext);
    }else if(mode >= 7){
        if(oSize < tSize)
            context(fOne, fTwo, one_fname, two_fname, oSize, tSize, (oSize < tSize) ? 0 : 1, tContext);
        else
            context(fTwo, fOne, one_fname, two_fname, tSize, oSize, (oSize < tSize) ? 0 : 1, tContext);
    }else if(mode >= 4){
        if(oSize < tSize)
            sideBySide(fOne, fTwo, oSize, tSize, mode, (oSize < tSize) ? 0 : 1);
        else
            sideBySide(fTwo, fOne, tSize, oSize, mode, (oSize < tSize) ? 0 : 1);
    }else{
        if(oSize < tSize)
            compare(fOne, fTwo, oSize, tSize, mode, (oSize < tSize) ? 0 : 1);
        else
            compare(fTwo, fOne, tSize, oSize, mode, (oSize < tSize) ? 0 : 1);
    }
    return 0;
}