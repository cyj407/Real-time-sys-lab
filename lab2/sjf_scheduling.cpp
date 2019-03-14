#include"sjf_scheduling.h"

using namespace std;

void scheduling(int p_num, int t_num, struct Processor p[], struct Task t[]) {

	int cur_time = 0;						// current time
	bool finish[t_num + 1];					// record the finished task
	memset(finish, false, t_num + 1 );
	
	struct Task next_served = t[0];			// initialize next served as first task
	int next_served_end_time = 0;
	int remain_task = t_num;

	/* find the earliest arrival as the first task */
	for(int i = 0; i < t_num; ++i)
		if(t[i].release_t < next_served.release_t)
			next_served = t[i];

	next_served_end_time = next_served.release_t + next_served.exec_t; 
	printf("Processor 1:\n");
	printf("Task %d: %3d ~ %3d\n", next_served.id, next_served.release_t, next_served_end_time); 
	
	/* update the full information */
	cur_time = next_served_end_time;
	--remain_task;
	finish[next_served.id] = true;

	/* find the next one (the shortest burst time task) */
	while(remain_task) {
		
		// init the next served as the smallest id task
		for(int i = 0; i < t_num ; ++i) {
			if(!finish[t[i].id]){
				next_served = t[i];
				break;
			}
		}
		
		// find the shortest exec_t as the next served task
		for(int i = 0; i < t_num; ++i) {
			if(finish[t[i].id])
				continue;
			if(t[i].exec_t < next_served.exec_t)
				next_served = t[i];
		}

		next_served_end_time = cur_time + next_served.exec_t; 
		printf("Task %d: %3d ~ %3d\n", next_served.id, cur_time, next_served_end_time); 
		
		// update the full information
		cur_time = next_served_end_time;
		--remain_task;
		finish[next_served.id] = true;
	}
}

int main() {
	int processor_num, task_num;
	
	/* read file */
	freopen("input.txt", "r", stdin);	// use freopen and it can use scanf instead
	
	scanf("%d%d", &processor_num, &task_num);
	
	if(processor_num > 1) {
		printf("Only handle the single processor !\n");
		return 0;
	}

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
	
	/* scheduling and print the result */
	scheduling(processor_num, task_num, processor, task);

	return 0;
}
