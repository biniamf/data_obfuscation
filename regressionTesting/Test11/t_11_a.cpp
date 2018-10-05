#include<stdio.h>

int global1;

class x {

private:
  int x, y, z;

  int something() {
    int a, b, c;
  }

};

int f(int a, int b)
{
	int result = a + b;
	return result;
}

int global2;

 int g(int a, int b)
{
	int result = a - b;
	return result;
}
