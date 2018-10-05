/*
 * Encoded.cpp
 *
 *  Created on: Dec 11, 2013
 *      Author: Biniam Fisseha Demissie
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const long long int p = 93103;//7919*7;
const long long int q = 93113;//7867*13;

#ifdef COUNT
	int _decode_count = 0;
	FILE *f;
	char filename[] = "decode_count.txt";
#endif

class Encoded {

public:
	Encoded() : x(0), y(0) {};
	
	Encoded(int a) {
		long long int M = p*q;
		long long int p_rand, q_rand;
		// printf("a: %d\n", a);
        if (a > M/2-1 || a < -M/2) {
        	printf("Error out of range!\n");
        	exit (1);
        }  
#ifdef COUNT
        printf("closing file\n");
#endif
        // TEMPORARY: should pick a right place to initialize the seed
        srand(time(NULL));

        p_rand = rand()%100;
        q_rand = rand()%100;

    	x = (a % p) + p*p_rand;
    	y = (a % q) + q*q_rand;

    	//printf("x: %d, y: %d\n", x, y);
	}

	Encoded(int a, int b) : x(a), y(b) {};

	~Encoded() {};

	static void e_gcd2(long long int a, long long int b, long long int *x, long long int *y)
	{
	    long long int temp;
	    if (b > a)
	        return e_gcd2(b, a, y, x);

	    if (b == 0) {        
	        *x = 1;
	        *y = 0;
	        return;
	    }

	    e_gcd2(b, a%b, x, y);
	    temp = *x;
	    *x = *y;
	    *y = temp - (a / b) * *y;
	}

	static long long int DECODE(Encoded encoded)
	{
	    long long int M = p*q, sign = 1, i;
	    long long int d1, d2, x1, x2, y1, y2;

	    long long int M1, M2, result;

#ifdef COUNT
	_decode_count++;
#endif	    
	   // printf("DECODE(): %d %d\n", encoded.getX(), encoded.getY());
	    // A(M / M1) + B(M1) = 1
	    M1 = M / p; // * A
	    M2 = M / q; // * b

	    e_gcd2(q, p, &x1, &y1);    
	    e_gcd2(p, q, &x2, &y2);

	    result = (((encoded.getX() * x1 * q + encoded.getY() * x2 * p) % M) + M) % M;

	    if (result > M/2-1)
	        result -= M;

	    return result;   
	}

	void operator=(Encoded rhs)
	{
			this->x = rhs.x;
			this->y = rhs.y;
	}

	// possible because 1 mod p is 1
	void operator++(int one)
	{
			this->x++;
			this->y++;
	}

	void operator++()
	{
			this->x++;
			this->y++;
	}

	void operator--(int one)
	{
			this->x--;
			this->y--;
	}

	void operator--()
	{
			this->x--;
			this->y--;
	}

	Encoded operator-(Encoded rhs)
	{
			Encoded result;

			result.x = this->x - rhs.x;
			result.y = this->y - rhs.y;

			return result;
	}

	Encoded operator+(Encoded rhs)
	{
			Encoded result;
						
			result.x = this->x + rhs.x;
			result.y = this->y + rhs.y;

			return result;
	}

	Encoded operator*(int in)
	{
			Encoded result;
			Encoded rhs(in);
						
			// printf("this->x:%d\nrhs.x:%d\nthis->y:%d\nrhs.y:%d\n", this->x, rhs.x, this->y, rhs.y);


			result.x = this->x * rhs.x;
			result.y = this->y * rhs.y;

			// printf("result.x:%d, result.y:%d", result.x, result.y);

			return result;
	}

	Encoded operator*(Encoded rhs)
	{
			Encoded result;

			// printf("this->x:%d\nrhs.x:%d\nthis->y:%d\nrhs.y:%d\n", this->x, rhs.x, this->y, rhs.y);
			// printf("this:%d\n", (int)DECODE(*this));
						
			// this might go out of bound
			result.x = ((this->x % p) * (rhs.x % p)) % p;
			result.y = ((this->y % q) * (rhs.y % q)) % q;

			// result = Encoded(DECODE(result));
			// printf("result.x:%d, \result.y:%d", result.x, result.y);
			// printf("%d\n", (int)DECODE(result));

			return result;
	}

	void operator-=(int rhs)
	{
		Encoded eRHS(rhs);

		this->x -= eRHS.x;
		this->x -= eRHS.y;
	}

	void operator*=(int rhs)
	{
		Encoded eRHS(rhs);

		this->x *= eRHS.x;
		this->x *= eRHS.y;
	}

	int getX()
	{
		return this->x;
	}

	int getY()
	{
		return this->y;
	}

	void operator+=(int rhs)
	{		
		Encoded eRHS(rhs);

		this->x += eRHS.x;
		this->x += eRHS.y;
	}

	void operator-=(Encoded rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
	}

	void operator+=(Encoded rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
	}

	Encoded operator-()
	{
		Encoded zero(0);
		Encoded res;

		res.x = zero.x - this->x;
		res.y = zero.y - this->y;

		return res;
	}

	
private:
	long long int x;	// x component
	long long int y;	// y component
};