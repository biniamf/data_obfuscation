

/*
* Encoded.cpp
* BIGNUM alternative to accomodate larger primes
*
*  Created on: Dec 11, 2013
*      Author: Biniam Fisseha Demissie
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/bn.h>

class Encoded {

public:
	Encoded() : x(0), y(0) {
		char _p[128], _q[128];

		memset(_p, 0, sizeof(_p));
		memset(_q, 0, sizeof(_q));

		strcpy(_p, "104729");
		strcpy(_q, "104623");

		CTX = BN_CTX_new();
		BN_CTX_init(CTX);

		p = BN_new();
		q = BN_new();

		BN_dec2bn(&p, _p);
		BN_dec2bn(&q, _q);
	};

	Encoded(int a) {
		BIGNUM *M, *temp2, *temp3;
		BIGNUM *p_rand, *q_rand, *bn_a, *temp;
		char _a[128] = "";

		char _p[128], _q[128];

		memset(_p, 0, sizeof(_p));
		memset(_q, 0, sizeof(_q));

		strcpy(_p, "104729");
		strcpy(_q, "104623");

		CTX = BN_CTX_new();
		BN_CTX_init(CTX);

		p = BN_new();
		q = BN_new();

		temp2 = BN_new();
		temp3 = BN_new();

		bn_a = BN_new();
		temp = BN_new();
		M = BN_new();

		BN_dec2bn(&p, _p);
		BN_dec2bn(&q, _q);

		BN_mul(M, p, q, CTX);

		sprintf(_a, "%d", a);

		BN_dec2bn(&bn_a, _a);

//        if (a > M/2-1 || a < -M/2) {
//        	printf("Error out of range!\n");
//        	exit (1);
//        }

       p_rand = _rand("100", CTX);
       q_rand = _rand("100", CTX);

       BN_mod(temp, bn_a, p, CTX);
       printf("%s = %s mod %s\n", BN_bn2dec(temp), BN_bn2dec(bn_a), BN_bn2dec(p));
       BN_mul(temp2, p, p_rand, CTX);
       printf("%s = %s x %s\n", BN_bn2dec(temp2), BN_bn2dec(p), BN_bn2dec(p_rand));
       BN_add(temp3, temp2, temp);
       printf("%s = %s + %s\n", BN_bn2dec(temp3), BN_bn2dec(temp2), BN_bn2dec(temp));
   	x = atoi(BN_bn2dec(temp3)) ;//+ p*p_rand;
   	printf("x:%lld\n", x);

   	BN_mod(temp, bn_a, q, CTX);
       printf("%s = %s mod %s\n", BN_bn2dec(temp), BN_bn2dec(bn_a), BN_bn2dec(p));
       BN_mul(temp2, q, q_rand, CTX);
       printf("%s = %s x %s\n", BN_bn2dec(temp2), BN_bn2dec(p), BN_bn2dec(p_rand));

       BN_add(temp3, temp2, temp);
       printf("%s = %s + %s\n", BN_bn2dec(temp3), BN_bn2dec(temp2), BN_bn2dec(temp));

   	y = atoi(BN_bn2dec(temp3)) ;//+ q*q_rand;
   	printf("y:%lld\n", y);


   	printf("x:%lld\ny:%lld\n", x, y);

   	BN_free(bn_a);
   	BN_free(temp);
   	BN_free(p_rand);
   	BN_free(q_rand);
   	BN_free(M);
   	BN_free(temp2);
   	BN_free(temp3);
	}

	~Encoded()
	{
		BN_free(p);
		BN_free(q);
		BN_CTX_free(CTX);
	};

	BIGNUM *_rand(const char *mod, BN_CTX *ctx)
	{
	    FILE *fp;
	    unsigned char seed[64];
	    BIGNUM *rem, *m, *d;
	    int i;

	    rem = BN_new();
	    d = BN_new();
	    m = BN_new();

		fp = fopen("/dev/urandom", "r");
	    for (i=0; i<64; i++) {
	      fscanf(fp, "%c", (char*)&(seed[i]));
	    }
	    fclose(fp);

	    BN_bin2bn(seed, 64, d);
	    BN_dec2bn(&m, mod);

	    BN_mod(rem, d, m, ctx);

	    BN_free(m);
	    BN_free(d);

	    return rem;
	}

	void e_gcd2(BIGNUM *a, BIGNUM *b, BIGNUM *x, BIGNUM *y)
	{
	    BIGNUM *temp, *temp1, *temp2, *r;

	    temp = BN_new();
	    temp1 = BN_new();
	    temp2 = BN_new();
	    r = BN_new();

//	    if (b > a)
	    if (BN_cmp(b, a) == 1)
	        return e_gcd2(b, a, y, x);

	    if BN_is_zero(b) {
	        //*x = 1;
	    	BN_one(x);

	        //*y = 0;
	    	BN_zero(y);
	        return;
	    }

	    BN_mod(temp1, a, b, CTX);
	    e_gcd2(b, temp1, x, y);

	    BN_copy(temp, x);

	    //temp = *x;

	    BN_copy(x, y);

	    BN_div(temp2, temp1, a, b, CTX);
	    BN_mul(r, temp2, y, CTX);

	    BN_sub(y, temp, r);
	    // *y = temp - (a / b) * *y;

	    BN_free(temp1);
	    BN_free(temp2);
	    BN_free(temp);
	    BN_free(r);
	}

	long long int DECODE()
	{
	    BIGNUM *x1, *x2, *y1, *y2;
	    long long int _p, _q, M, X1, X2, result;

	    x1 = BN_new();
	    x2 = BN_new();
	    y1 = BN_new();
	    y2 = BN_new();

	    // A(M / M1) + B(M1) = 1
	    //M1 = M / p; // * A
	    //M2 = M / q; // * b
	    e_gcd2(q, p, x1, y1);
	    e_gcd2(p, q, x2, y2);

	    _p = atoi(BN_bn2dec(p));
	    printf("%d", _p);
	    _q = atoi(BN_bn2dec(q));

	    M = _p * _q;

	    X1 = atoi(BN_bn2dec(x1));
	    X2 = atoi(BN_bn2dec(x2));

	    result = (((this->x * X1 * _q + this->y * X2 * _p) % M) + M) % M;

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

			result.x = this->x * rhs.x;
			result.y = this->y * rhs.y;

			return result;
	}

	Encoded operator*(Encoded rhs)
	{
			Encoded result;

			// this might go out of bound
			// TODO: mod p and mod q
			result.x = (this->x) * rhs.x;
			result.y = (this->y) * rhs.y;

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

	void operator*=(Encoded rhs)
	{
		this->x *= rhs.x;
		this->y *= rhs.y;
	}

private:
	long long int x;	// x component
	long long int y;	// y component
	BN_CTX *CTX;
	BIGNUM *p;// = 7919*7;
	BIGNUM *q;// = 7867*13;
};


int main()
{
	// Encoded enc(10, 20);
	// Encoded enc2(20, 20);

	// Encoded enc3(100);
	// Encoded enc4(200);
	Encoded i;
	i = Encoded(214748364);

	Encoded j;
	j = Encoded(3);


	j *= i;

	printf("%lld\n", j.DECODE());
	printf("%lld\n", 214748364*3);
//    Encoded day1;
//    day1 = Encoded(12321);

	// printf("%d %d\n", enc3.getX(), enc3.getY());
	// // printf("%d %d\n", (enc3 + enc4).getX(), (enc3 + enc4).getY());
	// char test[] = "1234 1 2013"7
	//  for (i=Encoded(0); test[(int)Encoded::DECODE(i)] != ' '; i++) {
//        day1 = Encoded(10) * day1 + Encoded((char)test[(int)Encoded::DECODE(i)]) - Encoded('0');
//    }
   //for (;;i++)
//        printf("%d -- %d\n", 12341; *12321, (int)Encoded::DECODE(i*day1));
//	 printf("X:%d \nY:%d \n original: \n", i.getX(), i.getY());
//	 printf("ori: %d\n", i.DECODE());

	return 0;
}



