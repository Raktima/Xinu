/*  main.c  - main */

#include <xinu.h>
#define N 10
pid32 producer_id;
pid32 consumer_id;
pid32 timer_id;
sid32 produced;
sid32 consumed;
int32 consumed_count = 0;
const int32 CONSUMED_MAX = 100;

/* Define your circular buffer structure and semaphore variables here */
/* */
int32 buffer[N];
int32 head=0;
int32 tail=0;
int32 n=0;

/* Place your code for entering a critical section here */
void mutex_acquire(sid32 mutex)
{
	/* */
	wait(mutex);
}

/* Place your code for leaving a critical section here */
void mutex_release(sid32 mutex)
{
	/* */
	signal(mutex);
}

/* Place the code for the buffer producer here */
process producer(sid32 produced, sid32 consumed)
{
	/* */
	int32 n=0;
	kprintf("Producer process starts\n");
	//mutex_acquire(mutex);
	
	//for(i=0;i<1500;i++)
	while(1)
	{

		mutex_acquire(consumed);
		kprintf("Producer acquires lock\n");
		if(tail==N)
		{
			kprintf("Buffer is full \n");
		}
		else
		{
			buffer[head]=n;
			n++;
			head=(head+1)%N;
		}	
        mutex_release(produced);
		
	}
	
	printf("head is%d\n", head);
	printf("Producer process exits\n");

	return OK;

	
}

/* Place the code for the buffer consumer here */
process consumer(sid32 produced, sid32 consumed)
{
	/* Every time your consumer consumes another buffer element,
	 * make sure to include the statement:
	 *   consumed_count += 1;
	 * this will allow the timing function to record performance */
	/* */
	 int32 item;
	 int32 i;
	 kprintf("Consumer process starts\n");
	 
	 //mutex_acquire(mutex);
	 //for(i=0;i<1500;i++)
	 while(1)
	 {
	 	//wait(produced);
	 	mutex_acquire(produced);
	 	//wait(mutex);
	 	kprintf("Consumer acquires lock\n");
	 	if(head==tail)
	 	{
	 		kprintf("Buffer is empty\n");


	 	}
	 	else
	 	{
	 		item=buffer[tail];
	 		tail=(tail+1)%N;
	 		
            
	 		consumed_count+=1;
	 	}	
	 	mutex_release(consumed);       
	}
	kprintf("Consumer consumed %d",consumed_count);
	kprintf("Consumer process ends\n");
	//mutex_release(mutex);

	return OK;
}


/* Timing utility function - please ignore */
process time_and_end(void)
{
	int32 times[5];
	int32 i;

	for (i = 0; i < 5; ++i)
	{
		times[i] = clktime_ms;
		yield();

		consumed_count = 0;
		while (consumed_count < CONSUMED_MAX * (i+1))
		{
			yield();
		}

		times[i] = clktime_ms - times[i];
	}

	kill(producer_id);
	kill(consumer_id);

	for (i = 0; i < 5; ++i)
	{
		kprintf("TIME ELAPSED (%d): %d\n", (i+1) * CONSUMED_MAX, times[i]);
	}
}

process	main(void)
{
	kprintf("Inside main\n");
	recvclr();

	/* Create the shared circular buffer and semaphores here */
	/* */

	produced=semcreate(1);
	consumed=semcreate(0);
	producer_id = create(producer, 4096, 50, "producer", 2,produced,consumed);
	consumer_id = create(consumer, 4096, 50, "consumer", 2,produced,consumed);
	timer_id = create(time_and_end, 4096, 50, "timer", 0);

	resched_cntl(DEFER_START);
	resume(producer_id);
	resume(consumer_id);
	/* Uncomment the following line for part 3 to see timing results */
	resume(timer_id);
	resched_cntl(DEFER_STOP);

	return OK;
}
