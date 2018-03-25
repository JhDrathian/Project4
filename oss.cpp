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

using namespace std;

void *create_shared_memory (size_t size);
void initialize_process_block (struct process_control_block *process_table, int key);
void display_process_block (struct process_control_block *process_table, int key);


struct process_control_block
{
	int total_cpu_used;
	int total_system_time;
	int last_burst_time_used;
	int PID;
	int priority;
};


int main ()
{
	int key = 1;
	int MAX_PCB_SIZE = 1;
	char *process_name = "./child_process";
	char passing_key[20];
	snprintf(passing_key, 20, "%d", key);
	char* argv[2];
	argv[0] = process_name;
	argv[1] = passing_key;
	cout << "Filename (argv[0]): " << argv[0] << endl;
	cout << "Key (argv[1]): " <<  argv[1] << endl;
	struct process_control_block process_table[MAX_PCB_SIZE];
	initialize_process_block (process_table, key);
	display_process_block(process_table, key);

}

void *create_shared_memory(size_t size)
{
	int protection = PROT_READ | PROT_WRITE;	
	int visibility = MAP_ANONYMOUS | MAP_SHARED;
	return mmap(NULL, size, protection, visibility, 0, 0);
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
	cout << process_table[key].total_cpu_used << endl;
	cout << process_table[key].total_system_time << endl;
	cout << process_table[key].last_burst_time_used << endl;
	cout << process_table[key].PID << endl;
	cout << process_table[key].priority << endl;
}


