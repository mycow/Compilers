int i1, *p1, z;
long l1, *q1;
char c1, *s1, *s3, s[100], t[100];

int abs(), printf(), putchar(), rand();
long labs();

int g(int x)
{
    return abs(x) % (abs(rand()) + 1);
}

int f(int x, int y)
{
    x = x % 100000;
    y = y % 100000;

    if (s3 < s + sizeof(s) - 1) {
	*s3 = 102;
	s3 = s3 + 1;
    }

    if (x >= y * 2)
	return x - y * 3;
    else if (x % 2 == 0)
	return f(x * 4, y / 3) + f(x + 100, y - 4);
    else
	return f(x * 2, y / 3) + f(x + g(g(x)), y - 10);
}


int main(void)
{
    int i2, *p2;
    long l2, *q2;
    char c2, *s2, *s4;
    int n;


    n = 500;

    p1 = &i2;
    p2 = &i1;

    q1 = &l2;
    q2 = &l1;

    s1 = &c2;
    s2 = &c1;

    while (n > 0) {
	n = n - 1;

	putchar(putchar(putchar(putchar(putchar(65 + rand() % 10)+1)+2)+3)+4);
	putchar(32);

	s3 = s;
	s4 = t;

	i1 = rand() % 100 + 1;
	l1 = rand() % 1000 + 1;
	c1 = rand() % 10 + 1;

	i2 = rand() % 30 + 1;
	l2 = rand() % 300 + 1;
	c2 = rand() % 3 + 1;

	printf("i1 = %d  i2 = %d\n", i1, i2);

	while (i1 % 3 != 0) {
	    printf("%d ", (i2 * *p1 + c2 - *q1 % (abs(*s2) + 1)));
	    printf("%d ", f(i1, i2));
	    i1 = i1 + (rand() % 20);

	    while (labs(l2) / 5 > 10) {
		l2 = l2 / (rand() % 7 + 1);

		if (l2 != 0)
		    printf("%d ", (char) (c1 + c2 * (*s2 / l2) + *q2));

		c1 = c1 + 2;
	    }

	    i2 = i2 * 3;
	}

	while (c1 > 1) {
	    c1 = c1 - (rand() % 3);
	    printf("%d ", (l1 > c2 || (c2 <= *p1 && p1 == p2)));

	    if (l2 * *q2 % (*p2 + 1) > 5)
		printf("%ld ", (long) (l1 - i1 * c2 - *s1 % *p2 + (l1 >= l2)));
	    else if (rand() % (abs(*s2) + 4) < 10)
		printf("%d ", (*s1 + **&s2 + (c1 != l2) * (c2 != i2)));
	    else {
		printf("%d ", (l1 / 7 - c2 - *s1 % *p2 + (l1 >= l2)));
		s1 = &c1;
		s2 = &c2;
	    }

	    if (i1 + (*s1 + s2) + (c1 * *q2))
		printf("%d ", (l1 + l2 + c1));

	    if (sizeof(t) + t - 1 >= s4) {
		*s4 = 65 + rand() % 26;
		s4 = s4 + 1;
	    }

	    l1 = l1 - (rand() % 100);
	}

	p2 = (int *) t;
	/*{ void *p; p = t; p2 = p; }*/

	i1 = 0;
	*s3 = 0;
	*s4 = 0;

	while (i1 < 5) {
	    printf("%d ", p2[i1]);
	    i1 = i1 + 1;
	}


	printf("%d ", (*s1 + *s2 * *p1 * *p2));
	printf("%.20s %.20s", s, t);
	putchar(10);
    }
}
