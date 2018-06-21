int f(int x)
{
    x = y;			/* 'y' undeclared */
}


int g(int x, int y)
{
    int z;

    x = y + z + p();		/* 'p' undeclared */
}


int x;

int **h(int y)
{
    int z;

    x = y + z + w;		/* 'w' undeclared */
}
