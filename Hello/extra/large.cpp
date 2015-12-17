#include <iostream>
#include <sstream>
#include <fstream>


int main(int argc, char ** argv){
  std::ofstream myfile;
  myfile.open("./test/nested_large.c");
  myfile <<"#include \"stdio.h\"\n";
  
  myfile << "int monkey"<<1000<<"(int x, int y, int z){\n";
  myfile << "\tint a = x+y+x+z;\n";
  myfile << "\treturn a;\n";
  myfile << "}\n";
  myfile << "\n";

  for(int i = 999; i>-1;--i){
    myfile << "int monkey"<<i<<"(int x, int y, int z){\n";
    myfile << "\treturn monkey"<<i+1<<"(x,y,z);\n";
    myfile << "}\n";
    myfile << "\n";
  }

  myfile << "int main(int argc, char ** argv){\n";
  myfile << "\tint a=100; int b = 101; int c = 102;\n";
  myfile << "\tint d = monkey0(a,b,c);\n";
  myfile << "\tprintf(\"Value%d\",d);\n";
  myfile << "return 0;\n";
  myfile << "}\n";
  myfile.close();
}
