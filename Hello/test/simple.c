#include "stdio.h"

int baz (int z){
  int x = z+9;
  return x;
}

int foo(int a, int b){
  int f=baz(a);
  return f;
}

int main(void){
  int x=4;
  int y=5;
  int z=0;
  z=foo(x,y);
 
  printf("!!!!!Value: %d\n",z);
}
