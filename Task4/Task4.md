Task4: Encrypt a Message (Caesar Cipher)
[Using BCast, Scatter, and Gather ONLY]
You are required to take a message from a file and encrypt the message using caesar cipher and store the output in a new file. The encryption can be represented using modular arithmetic by first transforming the letters into numbers, according to the scheme, A = 0, B = 1,…, Z = 25. Encryption of a letter x by a shift n can be described mathematically as:
En(x) = (x + n) mod 26
Any other character outside the range from A to Z (or a to z) is transferred to the output file as it is.
Input: filename and key/shift value.
Output: new file that contains the encrypted message.
Master Scenario:
- Read the entire message in a char array from the given file.
- Read the key (shift) value from the user.
- Broadcast the key value to all slaves
- Scatter message on all slaves
- Handle remaining part of a message (if exists)
- Gather encrypted message parts from slaves.
- Write the entire encrypted message to a file.
Slave Scenario:
- Receive the key (shift) value from master.
- Get the part of the message to be encrypted using scatter.
- Do the encryption on the received part.
- Send the encrypted part to the master using gather.
Sample input/output:
KEY/SHIFT: 23
Plaintext.txt: THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG
Ciphertext.txt: QEB NRFZH YOLTK CLU GRJMP LSBO QEB IXWV ALD