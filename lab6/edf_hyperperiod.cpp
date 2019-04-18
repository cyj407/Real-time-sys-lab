#include"edf_hyperperiod.h"

using namespace std;

vector<struct Processor> processor;
vector<struct Task> task;

int gcd(int a, int b) {
	if(a == 0)
		return b;
	return gcd( b%a, a);
}

int lcm(int a, int b) {
	int curGCD = gcd(a, b);
	return (a*b)/curGCD;
}

int getHyperPeriod(int t_num) {
	int curLCM = task[0].period;
	for(int i = 0; i < t_num; ++i) {
		curLCM = lcm(curLCM, task[i].period);
	}
	return curLCM;
}

void addFutureTask(int t_num, int hyperPeriod) {
	for(int i = 0; i < t_num; ++i) {
		// original task deadline = next period release time
		task[i].deadline = 1* task[i].period;
		for(int p = 1; p*task[i].period < hyperPeriod;++p) {
			struct Task tmp = task[i];
			tmp.release_t = p * task[i].period;
			tmp.deadline = (p+1) * task[i].period;
			task.push_back(tmp);
		}
	}
}


void scheduling(int p_num, int init_t_num) {
	int hyperPeriod = getHyperPeriod(init_t_num);
	int deadline;				// save the earliest deadline
	int cur_time = 0;			// current time
	int cur_task_i;				// current doing task
	int pre_task_i = -1;		// previous doing task
	int wait_time = 0;			// total waiting time
	int cpu_exec = 0;			// CPU execution time
	bool no_task = false;		// no task to run now

	addFutureTask(init_t_num, hyperPeriod);	// add all tasks in the future hyperperiod

	bool finish[task.size()];		// record the finished task
	int exec_time[task.size()];		// save all current execution time
	memset(exec_time, 0, sizeof(exec_time));
	memset(finish, false, sizeof(finish));
	
	printf("Hyper period = %d\n", hyperPeriod);
	printf("Processor %d:\n",processor[0].id + 1);			// single processor
	
	while(cur_time <= hyperPeriod) {

		// init the next task as the smallest id task and record the deadline
		for(int i = 0; i < task.size() ; ++i) {
			if(!finish[i] && task[i].release_t <= cur_time){
				cur_task_i = i;
				deadline = task[cur_task_i].deadline;
				break;
			}
		}
		
		// find the earliest deadline as the next served task
		if(no_task || finish[pre_task_i]) {
			deadline = INT_MAX;
			no_task = false;
		}
		for(int i = 0; i < task.size(); ++i) {
			if(finish[i])
				continue;
			if(task[i].release_t <= cur_time) {
				if(task[i].deadline < deadline) {
					deadline = task[i].deadline;
					cur_task_i = i;
				}
				else if(task[i].deadline == deadline &&\
						task[i].id < task[cur_task_i].id) {
					deadline = task[i].deadline;
					cur_task_i = i;
				}
			}
		}

		if(cur_time == 0)
			printf("%3d Task%d ", cur_time, task[cur_task_i].id);

		// no task to do now
		if(deadline == INT_MAX) {
			++cur_time;
			no_task = true;
			continue;
		}
		else {
			// pre_task_i initialize = -1
			if(pre_task_i != cur_task_i && pre_task_i != -1) {
				if(!finish[pre_task_i])
					printf("%3d\n", cur_time);
				printf("%3d Task%d ", cur_time, task[cur_task_i].id);
			}
		}

		++exec_time[cur_task_i], ++cur_time;
		pre_task_i = cur_task_i;
		
		if(task[cur_task_i].exec_t == exec_time[cur_task_i]) {
			finish[cur_task_i] = true;
			cpu_exec += task[cur_task_i].exec_t;
			wait_time += cur_time - task[cur_task_i].release_t - task[cur_task_i].exec_t;
			printf("%3d\n", cur_time);
		}
	}

	printf("Average Waiting Time = %.2f\n", (float) wait_time/ (float)task.size());
	printf("CPU utilization = %.2f\n", (float) cpu_exec/ (float) hyperPeriod);
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

	for(int i = 0;i < processor_num;++i) {
		struct Processor tmp;
		scanf("%d%d", &tmp.id, &tmp.ability);
		processor.push_back(tmp);
	}

	for(int j = 0;j < task_num;++j) {
		struct Task tmp;
		scanf("%d%d%d", &tmp.id, &tmp.release_t, &tmp.exec_t);
		scanf("%d%d", &tmp.deadline, &tmp.period);
		scanf("%d%d", &tmp.preemption, &tmp.type);
		task.push_back(tmp);
	}
	fclose(stdin);

	/* scheduling and print the result */
	scheduling(processor_num, task_num);

	return 0;
}