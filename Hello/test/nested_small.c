#include "stdio.h"
#include "stdlib.h"
#include "limits.h"

int baz (int z){
  int x = z+9;
  return x;
}

int monkey(int monkeyz){
  int i = 0;
  int x = 0;
  for(;(i*monkeyz)<INT_MAX;++i){
    x = monkeyz + baz(monkeyz);
  }
  return x;
}

int bar (int barx,int bary, int barz){
  int w = 0;
  int i = 0;
  int o = 4;
  int * r =(int *)malloc((bary+barz+barx)*sizeof(int));
  *r=monkey(barz);
  o = o+*r;
  free(r);
  for(;(i+o)<(99999+barz);i++){
    int q = bary +barx;
    int wi = barx + barz + q;
    int v = wi+q;
    int e = v + wi;
    int l = e - barz;
    int k = l + e + v;
    int g = barx + k;
    int d = barz / (wi*g);
    o = bary + d;
    wi = barx - g - wi;
    v = barz - wi;
    e = v - (o *o);
    l = e / barz;
    k = l / (e + v);
    g = barx + k;
    d = barz / (wi*g);
    wi = v - d + barz;
    w = v - wi;
  }
  return w-o;
}

int boo(int boox, int booy, int booz){
  
  return bar(boox,booy,booz);
}

int foo(int a, int b){
  int g=a+5+b;
  int i=0;
  i = boo(g,a,b);
  return i;
}

int main(void){
  int x=9995;
  int y=5;
  int i;
  int z = 0;
  int r = 0;
  for(i=1;(i-x)<INT_MAX;i++){
    r = z+foo(x,y);
    z = z + r + 1;
    r = y+foo(x,y);
    z = z + r + 2;
  }
  printf("!!!!!Value: %d\n",z*y);
  //int z=foo(5,8); 
}

