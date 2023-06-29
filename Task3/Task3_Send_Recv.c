#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <mpi.h>

bool is_prime(int num, int j)
{
	if (num == 0 || num == 1)
	{
		return false;
	}
	for (j = 2; j <= num / 2; j++)
	{
		if (num % j == 0)
		{
			return false;
		}
	}
	return true;
}

int main(int argc, char* argv[])
{
	int my_rank;		/* rank of process	*/
	int p;			/* number of process	*/
	int source;		/* rank of sender	*/
	int dest;		/* rank of reciever	*/
	int tag = 0;		/* tag for messages	*/
	char message[100];	/* storage for message	*/
	MPI_Status status;	/* return status for recieve */
	int i, j;
	int a, b, count = 0, reminder;
	int x, y, r, totalCount = 0;
	struct timeval start, end;

	/* Start up MPI */
	MPI_Init(&argc, &argv);

	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* Find out number of process */
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	/* Master */
	if (my_rank == 0)
	{
		/* Reads Lower bound number x */
		printf("\nPlease Enter Lower bound number x...\n");
		scanf("%d", &x);
		/* Reads Upper bound number y */
		printf("\nPlease Enter Upper bound number y...\n");
		scanf("%d", &y);

		/* Start time */
		gettimeofday(&start, NULL);

		/* Calculate the subrange size (without master) */
		r = (y - x) / (p-1);

		printf("n = %d, x = %d, y = %d -> r = (%d - %d) / (%d-1) = %d\n", p, x, y, y, x, p, r);

		/* Send x and r to each slave process using loop of MPI_Send */
		for (source = 1; source < p; source++)
		{

			MPI_Send(&x, 1, MPI_INT, source, tag, MPI_COMM_WORLD);
			MPI_Send(&r, 1, MPI_INT, source, tag, MPI_COMM_WORLD);
		}

		/* handle the length of the range isn't be divisible by the number of processes */
		reminder = (p - 2) * r + x + r - 1;
		if (reminder != y - 1)
		{
			a = reminder + 1;
			b = y - 1;
			int* arr1 = (int*)malloc((b - a + 1) * sizeof(int));
			for (i = a; i <= b; i++)
			{
				if (is_prime(i, j) == true)
				{
					arr1[count] = i;
					count++;
				}
			}
			printf("p%d: calculate partial count of prime numbers from %d to %d -> Count = %d", my_rank, a, b, count);
			for (i = 0; i < count; i++)
			{
				if (i == 0)
					printf("(");
				printf("%d", arr1[i]);
				if (i == count - 1)
					printf(")");
				else
					printf(", ");
			}
			printf("\n");
		}

		totalCount += count;

		/* Receive sub-count from each slave process loop of MPI_Recv */
		for (source = 1; source < p; source++)
		{
			MPI_Recv(&count, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
			totalCount += count;

			if (source == p - 1)
			{
				/* Print total count of primes between x and y */
				printf("After reduction, P%d will have Count = %d\n", my_rank, totalCount);

				/* End Time */
				gettimeofday(&end, NULL);
				/* Duration */
				printf("Duration = %lu microseconds\n", (end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec);
			}
		}
	}

	/* slave */
	else
	{
		dest = 0;
		/* Receive x and r through the MPI_Recv */
		MPI_Recv(&x, 1, MPI_INT, dest, tag, MPI_COMM_WORLD, &status);
		MPI_Recv(&r, 1, MPI_INT, dest, tag, MPI_COMM_WORLD, &status);
		
		/* Calculate the lower bound a, and upper bound b according to its rank */
		a = ((my_rank-1) * r) + x;
		b = a + r - 1;
		int* arr = (int*)malloc((b - a + 1) * sizeof(int));
		/* Count primes in its subrange */
		for (i = a; i <= b; i++)
		{
			if (is_prime(i, j) == true)
			{
				arr[count] = i;
				count++;
			}
		}

		printf("p%d: calculate partial count of prime numbers from %d to %d -> Count = %d", my_rank, a, b, count);
		for (i = 0; i < count; i++)
		{
			if (i == 0)
				printf("(");
			printf("%d", arr[i]);
			if (i == count - 1)
				printf(")");
			else
				printf(", ");
		}
		printf("\n");

		/* Send partial count to the master process using the MPI_Send */
		MPI_Send(&count, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
	}

	/* shutdown MPI */
	MPI_Finalize();

	return 0;
}
