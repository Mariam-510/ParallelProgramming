#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

int main(int argc, char* argv[])
{
	int my_rank;		/* rank of process	*/
	int p;			/* number of process	*/
	int source;		/* rank of sender	*/
	int dest;		/* rank of reciever	*/
	int tag = 0;		/* tag for messages	*/
	char message[100];	/* storage for message	*/
	MPI_Status status;	/* return status for recieve */
	int size, my_size, my_size_rev, rest_size, s;  /* local size */
	int count_master, count_slave; /*counters*/
	int partition; /* partition of the array */
	int final_max, final_index, max, max_index; /* max info */

	/* Start up MPI */
	MPI_Init(&argc, &argv);

	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* Find out number of process */
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	/* Master */
	if (my_rank == 0)
	{
		printf("\nHello from master process.\n");
		printf("Number of slave processes is %d\n", (p - 1));

		/* Reads size of array */
		printf("\nPlease enter size of array...\n");
		scanf("%d", &size);

		/* Reads elements inside array */
		int arr[5000];
		printf("\nPlease enter array elements ...\n");
		for (count_master = 0; count_master < size; count_master++)
		{
			scanf("%d", &arr[count_master]);
		}

		/* Distributes the work among slaves processes */
		my_size = size / (p - 1);
		/* Handle number of processes > size of array */
		if (my_size < 1)
			my_size = 1;

		for (source = 1; source < p; source++)
		{
			partition = (source - 1) * my_size;
			/* Handle size of array is not divisible by number of processes */
			/* Put remain work in last slave */
			if (source == p - 1)
			{
				my_size += (size - (partition + my_size));
			}
			/* Handle number of processes <= size of array */
			if (partition < size)
			{
				/* Sends size of the array */
				MPI_Send(&my_size, 1, MPI_INT, source, tag, MPI_COMM_WORLD);
				/* Sends the assigned partition of the array */
				MPI_Send(&arr[partition], my_size, MPI_INT, source, tag, MPI_COMM_WORLD);
			}
			/* Handle number of processes > size of array */
			else
			{
				/* send 0 to slave, so it will not work */
				s = 0;
				/* Sends size of the array */
				MPI_Send(&s, 1, MPI_INT, source, tag, MPI_COMM_WORLD);
				/* Sends the assigned partition of the array */
				MPI_Send(&arr[s], s, MPI_INT, source, tag, MPI_COMM_WORLD);
			}
		}

		for (source = 1; source < p; source++)
		{
			/* Receives max number and its index from each process */
			MPI_Recv(&max, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
			MPI_Recv(&max_index, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);

			/* If slave works */
			if (max_index != -1 && max != INT_MIN)
			{
				printf("Hello from slave# %d Max number in my partition is %d and index is %d.\n", source, max, max_index);
				/* Computes the max number from max numbers returned from each slave */
				if (source == 1)
				{
					final_max = max;
				}
				else
				{
					if (max > final_max)
					{
						final_max = max;
					}
				}
			}
			else
			{
				printf("No Work for Slave# %d.\n", source);
			}
		}

		/* search for index of final max in original array */
		for (count_master = 0; count_master < size; count_master++)
		{
			if (arr[count_master] == final_max)
			{
				final_index = count_master;
				break;
			}
		}

		/* Output the final max number and its index in the original array */
		printf("Master process announce the final max which is %d and its index is %d.\n", final_max, final_index);
		printf("\nThanks for using our program\n\n");
	}

	/* slave */
	else
	{
		/* set dest to master rank 0 */
		dest = 0;
		int arr_rev[5000];

		/* Receives size of the array */
		MPI_Recv(&my_size_rev, 1, MPI_INT, dest, tag, MPI_COMM_WORLD, &status);
		/* Receives the portion of the array */
		MPI_Recv(&arr_rev, my_size_rev, MPI_INT, dest, tag, MPI_COMM_WORLD, &status);

		int max1;
		int max_index1;
		/* if there is work for this slave, size != 0 */
		if (my_size_rev != 0)
		{
			max1 = arr_rev[0];
			max_index1 = 0;
			/* Calculates max number */
			for (count_slave = 1; count_slave < my_size_rev; count_slave++)
			{
				if (arr_rev[count_slave] > max1)
				{
					max1 = arr_rev[count_slave];
					max_index1 = count_slave;
				}
			}
			/* Sends max number and its index back to master process */
			MPI_Send(&max1, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
			MPI_Send(&max_index1, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
		}
		/* if no work */
		else
		{
			max1 = INT_MIN;
			max_index1 = -1;
			/* Sends smallest num and -1 to master process */
			MPI_Send(&max1, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
			MPI_Send(&max_index1, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
		}
	}

	/* shutdown MPI */
	MPI_Finalize();

	return 0;
}
