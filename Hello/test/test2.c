#include "stdio.h"

int baz (int z){
  int x = z+9;
  return x;
}

int bar (int barx,int bary, int barz){
  //int g=x+y+z;
  int w=baz(barz);
  return w;
}

int boo(int boox){
  return baz(boox);
}

int foo(int a, int b){
  int g=a+5;
  int f=baz(g);
  int r=boo(g);
  int q = bar (a,b,g);
  f=f+g+r;
  return q+f;
}

int main(void){
  int x=4;
  int y=5;
  int z=foo(x,y);
  printf("!!!!!Value: %d\n",z);
  //int z=foo(5,8); 
}
