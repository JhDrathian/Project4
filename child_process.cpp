//child process
//OSS Scheduling Simulator
//
//

#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>

using namespace std;

struct process_control_block
{
	int total_cpu_used;
	int total_system_time;
	int last_burst_time_used;
	int PID;
	int priority;
};

struct message_buffer
{
        long type;
        char messagetext[100];
};

void display_process_block (struct process_control_block *process_table, int key);

int main(int argc, char **argv)
{
	//make our passed key a usable integer
	int key = atoi(argv[1]);
	cout << "PID | " << argv[1] << " | Created." << endl;

	//process control block initialization
	size_t shmsize2 = sizeof(struct process_control_block) * 18;
	int process_block_key = shmget(2000, shmsize2, IPC_CREAT | 0666); 	
	if (process_block_key == -1)
		perror("Failed to create shared memory.");
	process_control_block *process_table = (process_control_block*)shmat(process_block_key, NULL, 0);
	cout << "My Process Table at runtime is: ";
	display_process_block (process_table, key);		

	//message queue initialization
	int msgid;
	struct message_buffer message_queue;
	msgid = msgget(3000, 0666 | IPC_CREAT);
	if (msgid == -1)
		perror("Failed to attach to message queue.");
	if (msgrcv(msgid, &message_queue, sizeof(message_queue), key,  0) == -1)
	{
		perror("Can't retrieve message.");
	}
	else
	{
		cout << "Child has received message with type: " << message_queue.type << endl;
		process_table[key].total_cpu_used = 1000;
		cout << "My Process Table at term is: ";
	        display_process_block (process_table, key);
		message_queue.type = 100;
		msgsnd(msgid, &message_queue, sizeof(message_queue), 0);
	}

	return 1;
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
