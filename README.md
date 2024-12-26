# OS_EX3
Operating Systems Task

First, few general points about question 1 and 2:

1.  At first, implement korsaju by list.
   
    To run the program:
    
    make

    ./q1


    Example input:

    5 5

    1 2

    2 3

    3 1

    3 4

    4 5

    Output should be :

    1 2 3 

    4 

    5 

3. Analyze the required data: 

2a. List is faster than deque.

   To run the program:
   
   make
   
   ./q2WithList < input.txt  
   
   gprof ./q2WithList gmon.out > profile_output.txt

2b. The graph represntation is preferd at list.

   To run the program:

   make
   
   ./q2WithList < input.txt  
   
   gprof ./q2WithList gmon.out > profile_output.txt

Conclusion: After the profiling, the best implementation: graph implementation by adjancy list, kosaraju algorithem preferd list over deque.

Full analyze is at the files, 2a,2b(txt format).

Rest of the required data is at each folder q3,q4,....,q10.

Natureally the code improve at all section of the assignment , obsessively the most improved code is at section 10.

10. To run the program:
    Server:
    make
    ./kosaraju
    Client:
    telnet 127.0.0.1 9034
    newgraph
    6 2
    1 2
    2 3
    newedge
    3 1
    remove edge
    3 1
