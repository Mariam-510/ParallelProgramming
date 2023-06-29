Task3 Statement: “Counting Primes”
Write a parallel C program to count the prime numbers within an input range using the following two methods, then compare the execution times of both programs:
MPI_Bcast and MPI_Reduce ONLY
MPI_Send and MPI_RecvONLY
Given
- Lower bound number x
- Upper bound number y
Output
- Count of prime numbers occurring between x and y.
MasterProcess:
- Calculate the subrange size r = (y ‐ x) / p (if including master) or (y ‐ x) / (p - 1) processes (without master).
- Broadcast x and r to each slave process using MPI_Bcast (or loop of MPI_Send).
- Receive sub-count from each slave process using MPI_Reduce (or loop of MPI_Recv).
- Print total count of primes between x and y.
Slave Process:
- Receive x and r through the MPI_Bcast call (or MPI_Recv).
- Calculate the lower bound a, and upper bound b according to its rank.
- Count primes in its subrange (between a and b).
- Send this partial count to the master process using the MPI_Reduce call (or MPI_Send).