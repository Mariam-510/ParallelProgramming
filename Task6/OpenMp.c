#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int main(int argc, char* argv[])
{
	double start, end;
	double* values;
	double sum = 0.0, squared_difference = 0.0;
	double mean, variance, standard_deviation;
	int size;
	int i;

	omp_set_num_threads(4);

	/* input array and size */
	/*printf("Array size:\n");
	scanf_s("%d", &size);
	values = (double*)malloc(size * sizeof(double));
	printf("\nArray elements: \n");
	for (i = 0; i < size; i++)
	{
		scanf_s("%lf", &values[i]);
	}*/

	/* fixed array and size*/
	size = 1024;
	values = (double*)malloc(size * sizeof(double));
	for (i = 0; i < size; i++)
	{
		values[i]=i+1;
	}

	start = omp_get_wtime(); /* start time */

#pragma omp parallel for reduction(+:sum)
	for (i = 0; i < size; i++)
	{
		sum += values[i];
	}

	mean = sum / size;

#pragma omp parallel for reduction(+:squared_difference)
	for (i = 0; i < size; i++)
	{
		squared_difference += pow((values[i] - mean), 2);
	}

	variance = squared_difference / size;

	standard_deviation = sqrt(variance);

	end = omp_get_wtime(); /* end time */

	printf("\nElapsed time is %f in MilliSeconds.\n", (end - start) * 1000);

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

	return 0;
}