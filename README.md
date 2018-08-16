# Vote-Counter
Project of Vote Counter, there are four different versions

# Method:
Find the winner from different regions votes. For example, Hennepin county, Minneapolise, Minnesota, US. It will count the vote from all counties to cities to states and finally all the votes will be on the "US", then find the winner. It is a voting system from local to the overall.
There are four different voting methods, the version1,2 keeps all the voting files in one directory, and if it is leaf (the smallest unit of an area), call leaf_coutner, if it is parent node, call aggarate vote, if it is Who_Won, means that you reach the end of vote, call find_winner to find the winner at this voting.The version3 are creating thread to synchronize the vote,  and write them into different directory. The version4, are creating server and client to vote at different computer clients, also with thread to synchronize the vote. 

# completion:
- [ ] version 1
- [x] version 2
- [x] version 3
- [ ] version 4


# Applied skills:
Linux OS, I/O, thread, pid, networking, Makefile, and C programming.
# Hardest part:
in the version3, we were telling to synchronize the vote with using thread, but when two threads are accessing same directory or same file, one of them should wait, till another one finished. But the voting graph is like a tree graph, when three or more thread goes into same flie, the error accured very often. I think make the vote synchronized is the hardest part for all versions' vout-counter.
