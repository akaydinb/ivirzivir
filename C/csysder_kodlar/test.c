
int g_x = 10;

int func(int a, int b)	 {
   return a + b;
}

int main(void)  {
    int i, j, k;

    i = 10; j = 8;
    k = func(i, j) + g_x;

    return 0;
}

