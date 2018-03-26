//header
//Project 4
//OSS Scheduling Simulator
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <queue>
#include <signal.h>

using namespace std;

void initialize_process_block (struct process_control_block *process_table, int key);
void display_process_block (struct process_control_block *process_table, int key);
void display_clock(struct os_clock clock);


struct process_control_block
{
	int total_cpu_used;
	int total_system_time;
	int last_burst_time_used;
	int PID;
	int priority;
};

struct os_clock
{
	unsigned int seconds;
	unsigned int nanoseconds;
};

struct message_buffer
{
	long type;
	char messagetext[100];
};

void signal_callback_handler (int signum)
{
	cout << "Caught Signal.";
	exit (signum);
}

int main ()
{
	signal(SIGINT, signal_callback_handler);
		
	long key_long = 1;
	int key = 1;
	int MAX_PCB_SIZE = 1;
	int number_current_children = 0;
	char *process_name = "./child_process";
	char passing_key[20];
	snprintf(passing_key, 20, "%d", key);
	char* argv[2];
	argv[0] = process_name;
	argv[1] = passing_key;
	cout << "Filename (argv[0]): " << argv[0] << endl;
	cout << "Key (argv[1]): " <<  argv[1] << endl;

	//initialize process table in shared memory
	size_t shmsize2 = sizeof(struct process_control_block) * 18;
	int key_id2 = shmget(2000, shmsize2, IPC_CREAT | 0666);
	if (key_id2 == -1)
	{
		perror("Failed to create shared memory.");
	}
	process_control_block *process_table = (process_control_block*) shmat(key_id2, NULL, 0);
	for (int x = 1; x < 19; x++)
	{
		initialize_process_block (process_table, x);
		display_process_block (process_table, x);
	}

	//initialize clock in shared memory
	size_t shmsize = sizeof(struct os_clock);
	int key_id = shmget(1000, shmsize, IPC_CREAT | 0666);
	if (key_id ==-1)
	{
		perror("Failed to create shared memory.");
	}
	os_clock *shared_clock = (os_clock *) shmat(key_id, NULL, 0);
	shared_clock->seconds = 0;
	shared_clock->nanoseconds = 1;
	display_clock(*shared_clock);

	//initialize message queue in shared memory
	int msgid;
	struct message_buffer message_queue;
	msgid = msgget(3000, 0666 | IPC_CREAT);
	message_queue.type = 1;
	//cout << "Parent has initilized type to: " << message_queue.type << endl;

	queue<int> round_robin_queue;
	int new_process;

	int pid;
	for(shared_clock->nanoseconds; shared_clock->nanoseconds < 10000; shared_clock->nanoseconds++)
	{
		//if (shared_clock->nanoseconds > 1000000)
		//{
			//shared_clock->seconds++;
			//display_clock(*shared_clock);
			if (shared_clock->nanoseconds % 500 == 0)
			{
				pid = fork();
				if (pid == 0)
				{
                                        cout << "Child created at: ";
                                        display_clock(*shared_clock);
					execl("./child_process", argv[0], argv[1], (char*) NULL);
				}
				number_current_children++;
				round_robin_queue.push(key);
				cout << "Added the following process to the queue." << endl;
				new_process = round_robin_queue.front();
				display_process_block(process_table, new_process);
			}
		
			if (number_current_children > 0)
			{
				cout << "Attempting to send message." << endl;
				//key = round_robin_queue.first();
				msgsnd(msgid, &message_queue, sizeof(message_queue), 0);
        	                cout << "Sending Message." << endl;
               			msgrcv(msgid, &message_queue, sizeof(message_queue), 100, 0);
               			{
					display_process_block(process_table, key);
                        		cout << "Child has responded with type:" << message_queue.type << endl;
					shared_clock->nanoseconds = shared_clock->nanoseconds + process_table[key].total_cpu_used;
                       			key++;
                        		key_long++;;
                        		message_queue.type = key_long;
                        		cout << "Parent is setting type to: " << message_queue.type  << endl;
                        		snprintf(passing_key, 20, "%d", key);
                        		argv[1] = passing_key;
					number_current_children--;
				}
			}
	//	}
			
	
		
	}
	shmdt(shared_clock);
	shmctl(key_id, IPC_RMID, NULL);
	shmdt(process_table);
	shmctl(key_id2, IPC_RMID, NULL);	
	msgctl(msgid, IPC_RMID, NULL);

	

}


void initialize_process_block (struct process_control_block *process_table, int key)
{
	process_table[key].total_cpu_used = 0;
	process_table[key].total_system_time = 0;
	process_table[key].last_burst_time_used = 0;
	process_table[key].PID = key;
	process_table[key].priority = 0;
}

void display_process_block (struct process_control_block *process_table, int key)
{
	cout << process_table[key].total_cpu_used << " | ";
	cout << process_table[key].total_system_time << " | ";
	cout << process_table[key].last_burst_time_used << " | ";
	cout << process_table[key].PID << " | ";
	cout << process_table[key].priority << " | ";
	cout << endl;
}

void display_clock (struct os_clock clock)
{
	cout << "Shared Clock:";
	cout << " | S: " << clock.seconds;
	cout << " | NS: " << clock.nanoseconds << endl;
}
