#include <cstdlib>

int main()
{
    unsigned num_elements = 200000;
    int *a = (int *)malloc(num_elements * sizeof(int));
    int *b = (int *)malloc(num_elements * sizeof(int));
    int *c = (int *)malloc(num_elements * sizeof(int));

    for (unsigned i = 0; i < num_elements; i++)
    {
        c[i] = a[i] + b[i];
    }

    return 0;
}
