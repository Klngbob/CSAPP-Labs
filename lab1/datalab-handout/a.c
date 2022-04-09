#include <stdio.h>

int logicalNeg(int x) {
    int y = (~x) + 1; //if x==0, then -x==0, x^(-x) must be 0 except INT_MIN.
    int b = (x | y) >> 31;
    printf("%d\n",b);
    return b + 1;
}

int main(){
    printf("%d\n",(int)1.9);
    return 0;
}