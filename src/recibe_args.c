/*
 * Recibe argumentos.
 *
 */

#include<stdio.h>
#include<stdlib.h>

int main(int argc, char **argv)
{
    if(atoi(*(argv + 1)) > 0)
        printf("1");
    else
        printf("0");
    return 0;
}
