Task1 Statement:
MPI_SEND & MPI_RECV ONLY
Developing a parallel program that outputs the max number in a given array.
You will use the Master-Slave paradigm where
Master:
- Reads the size of the array.
- Reads elements inside an array.
- Distributes the work among slaves processes:
• Sends the size of the array.
• Sends the assigned partition of the array.
- After each slave finishes its work, master process receives max number and its index from each process.
- Then master computes the max number from max numbers returned from each slave.
- Output the final max number and its index in the original array to user.
Slaves each one will:
- Receives size of the array.
- Receives the portion of the array.
- Calculates max number.
- Sends max number and its index back to the master process.
Size of array may not be divisible by number of processes. So, I handle this case.