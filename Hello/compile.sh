#!/bin/bash

rm test/test test/*.bc test/*.s test/*.ll
make
#cp test/583compress/input1/compress.in.bak test/input.in
#clang -emit-llvm -o test/test.bc -c test/583compress/src/compress.c
#clang -emit-llvm -o test/test.bc -c test/583simple/src/simple.c

clang -emit-llvm -O1 -o test/test.bc -c test/test1.c
llc test/test.bc -o test/test.s
g++ -o test/test test/test.s 
./test/test 

opt -stats --debug-pass=Structure -load=../lib/Hello.so -hello test/test.bc > test/test.ipco.bc
#opt -stats -sccp -ipsccp test/test.bc > test/test.ipco.bc
llvm-dis test/test.ipco.bc
llvm-dis test/test.bc

llc test/test.ipco.bc -o test/test.ipco.s
g++ -o test/test test/test.ipco.s
./test/test

#llc test/test.slicm.bc -o test/test.slicm.s
#g++ -o test/test test/test.slicm.s 

#./test/test


