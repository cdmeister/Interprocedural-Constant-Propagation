#include "stdio.h"

int baz (int z){
  int x = z+9;
  return x;
}

int foo(int a, int b){
  int g=a+5;
  int f=baz(g);
  f=f+g;
  return f;
}

int main(void){
  int x=4;
  int y=5;
  int z=foo(x,y);
  printf("!!!!!Value: %d\n",z);
  //int z=foo(5,8); 
}