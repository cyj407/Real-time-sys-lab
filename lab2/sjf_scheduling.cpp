#include"sjf_scheduling.h"

using namespace std;

void scheduling(int p_num, int t_num, struct Processor p[], struct Task t[]) {
	struct Task next_served = t[0];
	int next_served_end_time = 0;
	int remain_task = t_num;
	int cur_time = 0;
	bool finish[t_num + 1];
	memset(finish, false, t_num + 1 );

	// find the earliest arrival
	for(int i = 0; i < t_num; ++i) {
		if(t[i].release_t < next_served.release_t) {
			next_served = t[i];
		}
	}
	next_served_end_time = next_served.release_t + next_served.exec_t; 
	cur_time = next_served_end_time;
	--remain_task;
	finish[next_served.id] = true;

	printf("Processor 1:\n");
	printf("Task %d: %3d ~ %3d\n", next_served.id, next_served.release_t, next_served_end_time); 

	// find the next one -> the shortest burst time
	while(remain_task) {
		
		// init the next served 
		// always choose the smallest id task
		for(int i = 0; i < t_num ; ++i) {
			if(!finish[t[i].id]){
				next_served = t[i];
				break;
			}
		}
		
		// find the shortest exec_t as next served
		for(int i = 0; i < t_num; ++i) {
			if(finish[t[i].id])
				continue;
			if(t[i].exec_t < next_served.exec_t) {
				next_served = t[i];
			}
		}

		next_served_end_time = cur_time + next_served.exec_t; 
		--remain_task;
		finish[next_served.id] = true;

		printf("Task %d: %3d ~ %3d\n", next_served.id, cur_time, next_served_end_time); 
		cur_time = next_served_end_time;

	}
}

int main() {
	int processor_num, task_num;
	
	/* read file */
	freopen("input.txt", "r", stdin);	// use freopen and it can use scanf instead
	
	scanf("%d%d", &processor_num, &task_num);
	
	struct Processor processor[processor_num];
	struct Task task[task_num];

	for(int i = 0;i < processor_num;++i) 
		scanf("%d%d", &processor[i].id, &processor[i].ability);

	for(int j = 0;j < task_num;++j) {
		scanf("%d%d%d", &task[j].id, &task[j].release_t, &task[j].exec_t);
		scanf("%d%d", &task[j].deadline, &task[j].period);
		scanf("%d%d", &task[j].preemption, &task[j].type);
	}
	fclose(stdin);
//	printf("scanf\n");
/*
	// print file info 
	printf("number of processors: %d\n", processor_num);
	for(int i = 0;i < processor_num;++i)
		printf("Processor %d: ability %d\n", processor[i].id, processor[i].ability);

	printf("\nnumber of tasks: %d\n", task_num);
	for(int j = 0;j < task_num;++j) {
		printf("Task %d: release time %d, execution time %d",\
				task[j].id, task[j].release_t, task[j].exec_t);
		printf(", deadline %d, period %d", task[j].deadline, task[j].period);
		printf(", preemption %d, type %d\n", task[j].preemption, task[j].type);
	}
*/
	/* scheduling */
	scheduling(processor_num, task_num, processor, task);

	/* print result */

	return 0;
}
