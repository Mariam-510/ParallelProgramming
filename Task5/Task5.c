#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char* argv[])
{
	int my_rank;		/* rank of process	*/
	int p;			/* number of process	*/
	int source;		/* rank of sender	*/
	int dest = 0;		/* rank of reciever	*/
	int tag = 0;		/* tag for messages	*/
	char message[100];	/* storage for message	*/
	MPI_Status status;	/* return status for recieve */
	static long num_steps = 1000000;
	double step;
	int i;
	double x, pi, partial_sum=0.0, sum;
	long range,lower_bound, upper_bound, remainder;

	/* Start up MPI */
	MPI_Init(&argc, &argv);

	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* Find out number of process */
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	/* Master */
	if (my_rank == 0)
	{
		step = 1.0 / (double)num_steps;
		/* divide the number of steps into ranges */
		range = num_steps / p;

		/* handle remainder */
		remainder = (p - 1) * range + range;
		if (remainder != num_steps)
		{
			for (i = remainder; i < num_steps; i++)
			{
				x = ((double)(i + 0.5)) * step;
				partial_sum += 4.0 / (1.0 + x * x);
			}
		}
	}

	/* Broadcast the step and range to all slaves */
	MPI_Bcast(&range, 1, MPI_LONG, 0, MPI_COMM_WORLD);
	MPI_Bcast(&step, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	/* calculate partial sum */
	lower_bound = my_rank * range;
	upper_bound = lower_bound + range;
	for (i = lower_bound; i < upper_bound; i++)
	{
		x = ((double)(i + 0.5)) * step;
		partial_sum += 4.0 / (1.0 + x * x);
	}

	/*  reduce the value of PI using the reduced sum */
	MPI_Reduce(&partial_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	/* print PI */
	if (my_rank == 0)
	{
		pi = step * sum;
		printf("\n%0.20f\n\n", pi);
	}


	/* shutdown MPI */
	MPI_Finalize();

	return 0;
}
