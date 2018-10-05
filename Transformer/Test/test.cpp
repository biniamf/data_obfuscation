struct test {
	int somevar, someothervar;
};


int sum (int a, int b)
{	
	test test1;
	int * d, j, k;
	int c, variable=100;	
	int u, f = c+10+j+k,p; //obf

	/*
		int c;
		int f=0;
		int *d;
		int j;
		int k;
		int variable=100;
	*/

	unsigned long e=c+f;

	a = 1;
	b = a + 1;
	a = a;
	a = c;
	c = a;

	j=k; 
	
	f=10;
	
	c = a + b + (int)test1.somevar + c + k * 100 + k/2;
		
	c += 1;

	if (c > 10)
		f ++;

	for (int i=0, kk=0; (p=0 && i<10); i++) {
		f++;
		int xx, yy, zz=100;
	}

	return c;
}


int main()
{
	return sum(1, 2);
}

/*
void sum1 (int *a, int *b, int *c)
{
	c[0] = a[0]+b[0];
	c[1] = a[1]+b[1];
}
*/


