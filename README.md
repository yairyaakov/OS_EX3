# OS_EX3
Operating Systems Test

First, few general points about question 1 and 2:

1.  at first, implement korsaju by list.
   
    To run the program:
    
    make

    ./q1


    example input:

    5 5

    1 2

    2 3

    3 1

    3 4

    4 5

    output should be :

    1 2 3 

    4 

    5 

3. analyze the required data: 

2a. list is faster than deque.

   To run the program:
   
   make
   
   ./q2WithList < input.txt  
   
   gprof ./q2WithList gmon.out > profile_output.txt
2b. the graph represntation is preferd at list.
conclusion: after the profiling, the best implementation: graph implementation by adjancy list, kosaraju algorithem preferd list over deque.
full analyze is at the files, 2a,2b(txt format).
To run the program:
   make
   ./q2WithList < input.txt  
   gprof ./q2WithList gmon.out > profile_output.txt

rest of the required data is at each folder q3,q4,....,q10.

Natureally the code improve at all section of the assignment , obsessively the most improved code is at section 10.
