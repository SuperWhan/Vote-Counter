CC=gcc

make: Leaf_Counter Aggregate_Votes Vote_Counter

Leaf_Counter: Leaf_Counter.c
	$(CC) -o Leaf_Counter Leaf_Counter.c

Aggregate_Votes: Aggregate_Votes.c
	$(CC) -o Aggregate_Votes Aggregate_Votes.c

Vote_Counter: Vote_Counter.c
	$(CC) -o Vote_Counter Vote_Counter.c

clean:
	rm Leaf_Counter Aggregate_Votes Vote_Counter
