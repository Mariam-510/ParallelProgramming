Problem: Mean, Variance, and Standard Deviation
Suppose that the entire population of interest is eight students in a particular class. For a finite set of numbers, the population standard deviation is found by taking the square root of the average of the squared deviations of the values subtracted from their average value. The marks of a class of eight students (that is, a statistical population) are the following eight values: 2, 4, 4, 4, 5, 5, 7, 9
Write a parallel program to calculate the above mean, variance, and standard deviation using the below algorithm:
1. Each process calculates its local sum on its portion of data. If the division result has a remainder, then send this remainder to the last process (the process that has the rank "n-1" or let the master process work on it).
2. The master process calculates the mean (dividing the total sum by the size of the elements) and sends it to all processes.
3. Each process calculates the squared difference on its portion of data.
4. The master process then calculates the variance (dividing the total
squared difference by the size of the elements).
5. The master process calculates the standard deviation by getting the
square root of the variance and prints the results.
Input: array size, array elements
Output: mean, variance, standard deviation
Sample input/output:
Sample1:
Array size: 8
Array elements: 2, 4, 4, 4, 5, 5, 7, 9
Output: Mean = 5.0, Variance = 4.0, Standard deviation = 2.0
Sample2:
Array size: 7
Array elements: 4, 5, 6, 6, 7, 9, 12
Output: Mean = 7.0, Variance = 6.2857, Standard deviation = 2.5071
Requirements
● Implement the problem once using MPI and another time using OpenMP.