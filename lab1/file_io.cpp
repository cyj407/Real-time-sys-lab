#include"file_io.h"

using namespace std;


int main() {
	
	int processor_num, task_num;
	struct Processor processor[processor_num];
	struct Task task[task_num];
	
	/* read file */
	freopen("input.txt", "r", stdin);	// use freopen and it can use scanf instead
	
	scanf("%d%d", &processor_num, &task_num);
	
	for(int i = 0;i < processor_num;++i) 
		scanf("%d%d", &processor[i].id, &processor[i].ability);

	for(int j = 0;j < task_num;++j) {
		scanf("%d%d%d", &task[j].id, &task[j].release_t, &task[j].exec_t);
		scanf("%d%d", &task[j].deadline, &task[j].period);
		scanf("%d%d", &task[j].preemption, &task[j].type);
	}
	fclose(stdin);


	/* print file info */
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

	return 0;
}
