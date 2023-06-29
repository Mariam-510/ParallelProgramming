#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mpi.h"

void printMatrix(int** matrix, int row, int col,int i,int j)
{
    printf("Result Matrix is (%dx%d):\n", row, col);
    for (i = 0; i < row; i++)
    {
        for (j = 0; j < col; j++)
        {
            printf("%d", matrix[i][j]);
            if (j != col - 1)
                printf(" ");
        }
        if (i != row - 1)
            printf("\n");
    }
}

int main(int argc, char* argv[])
{
	int my_rank;		/* rank of process */
	int p;			/* number of process */
	int source;		/* rank of sender */
	int dest;		/* rank of reciever	*/
	int tag = 0;		/* tag for messages	*/
	char message[100];	/* storage for message */
	MPI_Status status;	/* return status for recieve */
    int i,j,k,r,n;  /* counters */
    int f_row, f_col, s_row, s_col;  /* dimensions */
    int** first_matrix;  /* 2d first matrix */
    int** second_matrix;  /* 2d second matrix */
    int* first_arr;  /* 1d first matrix */
    int* second_arr;  /* 1d second matrix */
    int** result_matrix;  /* 2d result matrix */
    int* result_arr;  /* 1d result matrix */
    int mode;  /* mode num */
    int* result_arr1;/* local result arr */
    int s_row1, s_col1; /* local dimensions */
    int row_index_rev, f_col_rev; /* local dimensions */
    int* f_row_arr; /* local row */
    int* second_arr1; /* local 1d second matrix */
    int** second_matrix1; /* local 2d second matrix */
    int row_num_rev; /* local num of row */

    /* Start up MPI */
	MPI_Init(&argc, &argv);

	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

	/* Find out number of process */
	MPI_Comm_size(MPI_COMM_WORLD, &p);

    /* master */
	if (my_rank == 0)
	{
		printf("\nWelcome to vector Matrix multiplication program!\n");
		printf("To read dimensions and values from file press 1\n");
		printf("To read dimensions and values from console press 2\n");

        /* enter which mode */
		scanf("%d", &mode);

        /* reads the input from a file */
        if (mode == 1)
        {
            /* open the file */
            FILE* file = fopen("textFile.txt", "r");

            /* check if file is not exist */
            if (file == NULL)
            {
                printf("no file with that name\n");
            }
            /* file is exist */
            else
            {
                /* read dimensions of the first matrix */
                fscanf(file, "%d %d", &f_row, &f_col);
                /* initialize first matrixe */
                first_matrix = (int**)malloc(f_row * sizeof(int*));
                for (i = 0; i < f_row; i++)
                    first_matrix[i] = (int*)malloc(f_col * sizeof(int));
                /* read its elements */
                for (i = 0; i < f_row; i++)
                {
                    for (j = 0; j < f_col; j++)
                        fscanf(file, "%d", &first_matrix[i][j]);
                }

                /* read dimensions of the second matrix */
                fscanf(file, "%d %d", &s_row, &s_col);
                /* initialize second matrixe */
                second_matrix = (int**)malloc(s_row * sizeof(int*));
                for (i = 0; i < s_row; i++)
                    second_matrix[i] = (int*)malloc(s_col * sizeof(int));
                /* read its elements */
                for (i = 0; i < s_row; i++)
                {
                    for (j = 0; j < s_col; j++)
                        fscanf(file, "%d", &second_matrix[i][j]);
                }
            }

            /* close the file */
            fclose(file);
        }
        /* reads the input from the console*/
        else if (mode == 2)
        {
            /* read dimensions of the first matrix */
            printf("Please enter dimensions of the first matrix: ");
            scanf("%d %d", &f_row, &f_col);
            /* initialize first matrixe */
            first_matrix = (int**)malloc(f_row * sizeof(int*));
            for (i = 0; i < f_row; i++)
                first_matrix[i] = (int*)malloc(f_col * sizeof(int));
            /* read its elements */
            printf("Please enter its elements:\n");
            for (i = 0; i < f_row; i++)
            {
                for (j = 0; j < f_col; j++)
                {
                    scanf("%d", &first_matrix[i][j]);
                }
            }

            /* read dimensions of the second matrix */
            printf("Please enter dimensions of the second matrix: ");
            scanf("%d %d", &s_row, &s_col);
            /* initialize second matrixe */
            second_matrix = (int**)malloc(s_row * sizeof(int*));
            for (i = 0; i < s_row; i++)
                second_matrix[i] = (int*)malloc(s_col * sizeof(int));
            /* read its elements */
            printf("Please enter its elements:\n");
            for (i = 0; i < s_row; i++)
            {
                for (j = 0; j < s_col; j++)
                {
                    scanf("%d", &second_matrix[i][j]);
                }
            }
        }
        /* mode isn't 1 or 2 */
        else
        {
            printf("Invalid Number.\n");
            exit(0);
        }
        /* master send and receives */
        if (mode == 1 || mode == 2)
        {
            /* check if can multiply these 2 matrices */
            if (f_col == s_row)
            {
                /* convert 2d second matrix to 1d array to send it to slave */
                second_arr = (int*)malloc(s_row * s_col * sizeof(int));
                for (i = 0; i < s_row; i++) {
                    for (j = 0; j < s_col; j++) {
                        second_arr[(i * s_col) + j] = second_matrix[i][j];
                    }
                }
                /* Distributes the rows among slaves processes */
                int rows_num, row_index;
                rows_num = f_row / (p - 1);

                for (source = 1; source < p; source++)
                {
                    /* Send second row and column size */
                    MPI_Send(&s_row, 1, MPI_INT, source, tag, MPI_COMM_WORLD);
                    MPI_Send(&s_col, 1, MPI_INT, source, tag, MPI_COMM_WORLD);
                    /* Send second matrix */
                    MPI_Send(&second_arr[0], s_row* s_col, MPI_INT, source, tag, MPI_COMM_WORLD);
                    /* calculate which row the slave will start from */
                    row_index = (source - 1) * rows_num;
                    if (source == p - 1)
                    {
                        rows_num += (f_row - (row_index + rows_num));
                    }
                    /* Sends num of row to multiply */
                    MPI_Send(&rows_num, 1, MPI_INT, source, tag, MPI_COMM_WORLD);
                    for (i = 0; i < rows_num; i++)
                    {
                        /* Sends index of row to multiply */
                        MPI_Send(&row_index + i, 1, MPI_INT, source, tag, MPI_COMM_WORLD);
                        /* Sends size of row */
                        MPI_Send(&f_col, 1, MPI_INT, source, tag, MPI_COMM_WORLD);
                        /* Make array contains values of row */
                        first_arr = (int*)malloc(f_col * sizeof(int));
                        for (j = 0; j < f_col; j++)
                        {
                            first_arr[j] = first_matrix[row_index + i][j];
                        }
                        /* Sends row */
                        MPI_Send(&first_arr[0], f_col, MPI_INT, source, tag, MPI_COMM_WORLD);
                    }
                }

                /* initialize result matrixe */
                result_matrix = (int**)malloc(f_row * sizeof(int*));
                for (i = 0; i < f_row; i++)
                    result_matrix[i] = (int*)malloc(s_col * sizeof(int));

                /* Receives rows which each slave are multiplied from each process */
                for (source = 1; source < p; source++)
                {
                    /* Receives num of row */
                    MPI_Recv(&rows_num, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
                    for (i = 0; i < rows_num; i++)
                    {
                        /* Receives index of row start from to multiply */
                        MPI_Recv(&row_index, 1, MPI_INT, source, tag, MPI_COMM_WORLD, &status);
                        /* initialize 1d result matrix */
                        result_arr = (int*)malloc(s_col * sizeof(int));
                        /* Receives 1d result matrix */
                        MPI_Recv(result_arr, s_col, MPI_INT, source, tag, MPI_COMM_WORLD, &status);

                        /* add rows which each slave are multiplied to final result matrix */
                        for (j = 0; j < s_col; j++)
                        {
                            result_matrix[row_index][j] = result_arr[j];
                        }
                    }
                }
                /* print result matrix */
                printf("\n");
                printMatrix(result_matrix, f_row, s_col, i, j);
                printf("\n\n");

                /* write result matrix to file if mode = 1 */
                if (mode == 1)
                {
                    printf("The matrix is written in file out.txt\n\n");
                    FILE* out_file = fopen("out.txt", "w");
                    for (i = 0; i < f_row; i++)
                    {
                        for (j = 0; j < s_col; j++)
                            fprintf(out_file, "%-5d", result_matrix[i][j]);
                        fprintf(out_file, "\n");
                    }
                    fclose(out_file);
                }
            }
            /* f_col != s_row */
            else
            {
                printf("\ncan't multiply these two matrices.\n");
                exit(0);
            }
        }
	}
    /* slave */
	else
	{
        dest = 0;
        /* Receives second row and column size */
        MPI_Recv(&s_row1, 1, MPI_INT, dest, tag, MPI_COMM_WORLD, &status);
        MPI_Recv(&s_col1, 1, MPI_INT, dest, tag, MPI_COMM_WORLD, &status);
        /* Receives 1d second matrix */
        second_arr1 = (int*)malloc(s_row1 * s_col1 * sizeof(int));
        MPI_Recv(second_arr1, s_row1* s_col1, MPI_INT, dest, tag, MPI_COMM_WORLD, &status);

        /* initialize second matrixe */
        second_matrix1 = (int**)malloc(s_row1 * sizeof(int*));
        for (i = 0; i < s_row1; i++)
            second_matrix1[i] = (int*)malloc(s_col1 * sizeof(int));

        /* convert 1d array to 2d second matrix */
        for (i = 0; i < s_row1; i++) {
            for (j = 0; j < s_col1; j++) {
                second_matrix1[i][j] = second_arr1[(i * s_col1) + j];
            }
        }
        /* Receives num of row to multiply */
        MPI_Recv(&row_num_rev, 1, MPI_INT, dest, tag, MPI_COMM_WORLD, &status);

        /* Sends num of row to multiply to master */
        MPI_Send(&row_num_rev, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);

        for (k = 0; k < row_num_rev; k++)
        {
            /* Receives index of row to multiply */
            MPI_Recv(&row_index_rev, 1, MPI_INT, dest, tag, MPI_COMM_WORLD, &status);
            /* Receives size of row */
            MPI_Recv(&f_col_rev, 1, MPI_INT, dest, tag, MPI_COMM_WORLD, &status);
            /* Receives row */
            f_row_arr = (int*)malloc(f_col_rev * sizeof(int));
            MPI_Recv(f_row_arr, f_col_rev, MPI_INT, dest, tag, MPI_COMM_WORLD, &status);

            /* initialize 1d result arr */
            result_arr1 = (int*)malloc(s_col1 * sizeof(int));

            /* multiply */
            for (i = 0; i < s_col1; i++)
            {
                result_arr1[i] = 0;
                for (j = 0; j < f_col_rev; j++)
                {
                    result_arr1[i] += f_row_arr[j] * second_matrix1[j][i];
                }
            }

            /* Sends index of row which slave start from to master */
            MPI_Send(&row_index_rev, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
            /* Sends result arr */
            MPI_Send(&result_arr1[0], s_col1, MPI_INT, dest, tag, MPI_COMM_WORLD);
        }
	}

    /* shutdown MPI */
    MPI_Finalize();
	return 0;
}

