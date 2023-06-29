#include <stdio.h>
#include <stdlib.h>
#include <math.h>
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
	int size, i, sub_size, remainder;
	double partial_sum = 0.0, total_sum, remainder_sum = 0.0;
	double total_squared_difference, partial_squared_difference = 0.0, remainder_squared_difference = 0.0;
	double mean, variance, standard_deviation;
	double* values, * recv_values;
	double start, end;

	/* Start up MPI */
	MPI_Init(&argc, &argv);

	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* Find out number of process */
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	if (my_rank == 0)
	{
		values = (double*)malloc(size * sizeof(double));
		for (i = 0; i < size; i++)
		{
			values[i] = i + 1;
		}

		/* input array and size */
		/*printf("Array size:\n");
		scanf("%d", &size);
		values = (double*)malloc(size * sizeof(double));
		printf("\nArray elements: \n");
		for (i = 0; i < size; i++)
		{
			scanf("%lf", &values[i]);
		}*/
		
		/* fixed array and size*/
		size = 1024;
		values = (double*)malloc(size * sizeof(double));
		for (i = 0; i < size; i++)
		{
			values[i] = i + 1;
		}

		start = MPI_Wtime(); //elapsed time in seconds

		sub_size = size / p;
		remainder = (p - 1) * sub_size + sub_size;
	}

	MPI_Bcast(&sub_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

	recv_values = (double*)malloc(sub_size * sizeof(double));

	MPI_Scatter(values, sub_size, MPI_DOUBLE, recv_values, sub_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	for (i = 0; i < sub_size; i++) //sum
	{
		partial_sum += recv_values[i];
	}

	MPI_Reduce(&partial_sum, &total_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (my_rank == 0)
	{
		if (remainder < size)
		{
			for (i = remainder; i < size; i++)
			{
				remainder_sum += values[i];
			}
			total_sum += remainder_sum;
		}
		mean = total_sum / size;
	}
	MPI_Bcast(&mean, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	for (i = 0; i < sub_size; i++) //squared difference
	{
		partial_squared_difference += pow((recv_values[i] - mean), 2);
	}

	MPI_Reduce(&partial_squared_difference, &total_squared_difference, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (my_rank == 0)
	{
		if (remainder < size)
		{
			for (i = remainder; i < size; i++)
			{
				remainder_squared_difference += pow((values[i] - mean), 2);
			}
			total_squared_difference += remainder_squared_difference;
		}

		variance = total_squared_difference / size;

		standard_deviation = sqrt(variance);

		end = MPI_Wtime();
		printf("\nElapsed time is %lf in MilliSeconds.\n\n", (end - start) * 1000);

		printf("\nMean = ");
		if (mean == (int)mean) {
			printf("%.1f, ", mean);
		}
		else {
			printf("%.4f, ", mean);
		}

		printf("Variance = ");
		if (variance == (int)variance) {
			printf("%.1f, ", variance);
		}
		else {
			printf("%.4f, ", variance);
		}

		printf("Standard deviation = ");
		if (standard_deviation == (int)standard_deviation) {
			printf("%.1f\n\n", standard_deviation);
		}
		else {
			printf("%.4f\n\n", standard_deviation);
		}

		// clean up memory
		free(values);
	}

	// clean up memory
	free(recv_values);

	/* shutdown MPI */
	MPI_Finalize();

	return 0;
}
