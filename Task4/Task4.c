#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mpi.h"


int main(int argc, char* argv[])
{
    int my_rank;		/* rank of process */
    int p;			/* number of process */
    int source;		/* rank of sender */
    int dest;		/* rank of reciever	*/
    int tag = 0;		/* tag for messages	*/
    char message[100];	/* storage for message */
    MPI_Status status;	/* return status for recieve */
    int i, key, E, x;
    char filename[100];
    FILE* inputFile, * outputFile;
    int size, sub_size, remainder;
    char c;
    char* send_message, * recv_message, * encrypted_part, * encrypted_message;

    /* Start up MPI */
    MPI_Init(&argc, &argv);

    /* Find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* Find out number of process */
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    /* master */
    if (my_rank == 0)
    {
        printf("\nKEY/SHIFT:");
        scanf("%d", &key);

        printf("FILENAME:");
        scanf("%s", filename);
        printf("\n");

        /* open the file */
        inputFile = fopen(filename, "r");

        /* check if file is not exist */
        while (inputFile == NULL)
        {
            printf("no file with that name\n");
            printf("Enter FILENAME again:");
            scanf("%s", filename);
            printf("\n");
            inputFile = fopen(filename, "r");
        }

        /* Size of File */
        fseek(inputFile, 0L, SEEK_END);
        size = ftell(inputFile);
        rewind(inputFile);

        /* Read the entire message in a char array from file */
        send_message = (char*)malloc(size * sizeof(char));
        for (i = 0; i < size; i++)
        {

            send_message[i] = getc(inputFile);

        }

        sub_size = size / p;

        encrypted_message = (char*)malloc(size * sizeof(char));

        /* Handle remaining part of a message */
        remainder = (p - 1) * sub_size + sub_size;
        if (remainder < size)
        {
            for (i = remainder; i < size; i++)
            {
                if (send_message[i] >= 'A' && send_message[i] <= 'Z')
                {
                    x = (int)send_message[i] - 65;
                    E = (x + key) % 26;
                    c = (char)(E + 65);
                    encrypted_message[i] = c;
                }
                else
                {
                    encrypted_message[i] = send_message[i];
                }
            }
        }

        fclose(inputFile);
    }

    MPI_Bcast(&sub_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
    recv_message = (char*)malloc(sub_size * sizeof(char));

    /* Broadcast the key value to all slaves */
    MPI_Bcast(&key, 1, MPI_INT, 0, MPI_COMM_WORLD);
    /* Scatter message on all slaves */
    MPI_Scatter(send_message, sub_size, MPI_CHAR, recv_message, sub_size, MPI_CHAR, 0, MPI_COMM_WORLD);

    /* Do the encryption on the received part */
    encrypted_part = (char*)malloc(sub_size * sizeof(char));
    for (i = 0; i < sub_size; i++)
    {
        if (recv_message[i] >= 'A' && recv_message[i] <= 'Z')
        {
            x = (int)recv_message[i] - 65;
            E = (x + key) % 26;
            c = (char)(E + 65);
            encrypted_part[i] = c;
        }
        else
        {
            encrypted_part[i] = recv_message[i];
        }
    }

    /* Send the encrypted part to the master using gather */
    MPI_Gather(encrypted_part, sub_size, MPI_CHAR, encrypted_message, sub_size, MPI_CHAR, 0, MPI_COMM_WORLD);

    if (my_rank == 0)
    {
        /* Write the entire encrypted message to a file */
        printf("Encrypted Text: %s\n\n", encrypted_message);
        outputFile = fopen("Ciphertext.txt", "w");
        fprintf(outputFile, encrypted_message);
        fclose(outputFile);

    }

    /* shutdown MPI */
    MPI_Finalize();
    return 0;
}
