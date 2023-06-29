The following piece of code is a serial program to calculate the value of PI using integration.
#include <stdio.h>
int main()
{
    static long num_steps = 1000000;
    double step;
    int i;
    double x, pi, sum = 0.0;
    step = 1.0/(double)num_steps;

    for(i = 0; i < num_steps; i++)
    {
        x= ((double)(i + 0.5))*step;
        sum += 4.0/(1.0+x*x);
    }
    pi = step*sum;
    printf("%.20f", pi);
    return 0;
}
You are required to parallelize the program by distributing the number of steps on multiple processes and broadcast the step value to all slaves:
● Each slave process will receive the step and the range it will work on, then calculate its partial sum.
● Master process will divide the number of steps into ranges and broadcast the value of step, then perform reduce to output the value of PI using the reduced sum.