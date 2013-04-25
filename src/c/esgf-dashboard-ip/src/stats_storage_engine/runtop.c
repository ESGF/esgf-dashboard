#include <stdio.h>

int main()
{

 system("/usr/bin/top -b -n 1 i > top.txt");

 return 0;
}
