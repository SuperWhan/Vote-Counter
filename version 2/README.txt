/*login: huxxx990, shixx566
date: 03/09/18                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
name: Hanyu Hu, Xicun Shi
id: 5272396, 5090878 */

The purpose of program:
	To use Operating System's I/O and file system call to implement a vote counter program. The program counts the votes of multiple candidates. There are multiple leaf node s and parent node. Each leaf node have votes, the parent will count the total votes from each leaf node, and until the votes from all leaf nodes have been counted, the winner is determined. Leaf_Counter function read the "votes.txt" and count votes for each candidates. Aggregrate_Votes function add total votes from all subdirectories and calculate the sum of votes from each candidates. Vote_Counter function will analysis most votes and determine the winner.

To compile and run the program:
	The makefile have provided, so the program can be compiled by typing make in the terminal, then the three executable files would be created. And when you type "make clean", it will delete all the executabel files. To run the program, the format is like "./<program> <path>". The argument "path" is the relative path to a directory. 

Hanyu's work: Implement Leaf_counter.c, Vote_counter.c, and Aggregate_votes.c
Xicun's work: wrote README, Makefile and documentation in the code.
Worked together: Debugging and testing and fixing error. 

Attention: when passing argument path, dont add slash on the end, For example:
									./Vote_counter TestCase01/Who_won/ -> Error: This is not root
									./Vote_counter TestCase01/Who_won -> Correct run
