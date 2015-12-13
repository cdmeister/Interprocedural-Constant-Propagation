#include "stdio.h"
#include "stdlib.h"
#include "limits.h"

int baz (int z){
  int x = z+9;
  return x;
}

int monkey(int monkeyz){
  return baz(monkeyz);
}

int bar (int barx,int bary, int barz){
  int w = 0;
  int i = 0;
  int o = 4;
  for(i=barx+1;(i+o)<99999;i++){
    int * r =(int *)malloc((bary+barz+barx)*sizeof(int));  
    *r=monkey(barz);
    o = o +*r; 
    w = *r + w-o;
    free(r);
  }
  return w+barx+bary;
}

int boo(int boox){
  int w = 70;
  int q = 30;
  return bar(boox,w,q);
}

int foo(int a, int b){
  int g=a+5+b;
  int i=0;
  i = boo(g);
  return i;
}

int main(void){
  int x=9995;
  int y=5;
  int i;
  int z = 0;
  int r = 0;
  for(i=x+1;(i+r)<INT_MAX;i++){
    r = z+foo(x,y);
    z = z - r + 1;
  }
  printf("!!!!!Value: %d\n",z*y);
  //int z=foo(5,8); 
}
