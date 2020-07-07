> Danmarks Tekniske Universitet
> 62588 - Operativsystemer
> Mandatory 3
> s185120

The original code in this project, is made by the lecturer in the 62588 course.

1. How to use the program.
# 1. Open or download and open your favorite UNIX terminal.
# 2. Navigate via the command: 'cd <path>' to the path where you downloaded the program folder to.
# 3. Type 'make' to have the program build itself. You most likely need gcc for this.
# 4. Make sure the program works, but giving the command: "./mem -try worst" and/or 
#    "./mem -test all worst".
# 5. See the source code by opening the 'mymem.c' in a text editor.

2. Implementation description.
# This implementation covers the 'worst fit' part of the assignment. The guide was mostly followed, 
# and code is commented both where it was requested (mandatory parts) and also where it was not.
# Basically, the program works by looking through the linked list that contains information on 
# whether a certain part of an already allocated memory is allocated or not. This means that the 
# linked list controls if a block of for instance 100 bytes in a 500 byte memory is allocated or not, 
# with a pointer to the actual 500 bytes memory. The program is capable of both allocating and free'ing
# (de-allocating) the part of the memory, and does so depending on what part of the memory has the 
# most space. This is because the 'worst fit' means that where the memory has the most space, a new 
# allocation will take the requested space in that space. So for instance, if there was 500 bytes of 
# memory, and the blocks where: 50 allocated, 150 free, 100 allocated, 50 free, 150 allocated, this 
# program will put a request for allocating 25 bytes in the 150 bytes free space, instead of the 
# smaller 50 bytes free space, because the 150 bytes is a worse fit than the smaller 50 bytes.

3. Performed tests
# For testing the program, the given test files was used by calling the commands:
# './mem -try worst' for a simple test if the mymalloc and myfree functions worked properly, and:
# './mem -test all worst' for properly and in-depth testing the written program and its functions.

4. Test log
# When having run the '-test' command from '3. Performed tests', a 'Tests.log' file appears, which
# summarizes the tests. When testing this program, the times seems fair for a linear time solution,
# and it records that there was no failed allocations in any of the tests.
