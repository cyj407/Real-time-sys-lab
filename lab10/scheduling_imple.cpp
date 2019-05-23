#include"scheduling_imple.h"

using namespace std;

vector<struct Processor> processor;
vector<struct Task> task;
vector<struct Precedence> precedence;

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
	bool allZeros = true;
	for(int i = 0;i < t_num; ++i) {
		if(task[i].period != 0) {
			allZeros = false;
			break;
		}
	}
	if(allZeros)
		return 0;

	int curLCM, ptr;
	for(ptr = 0; task[ptr].period == 0; ++ptr)	;
	curLCM = task[ptr].period;
	for(int i = ptr + 1; i < t_num; ++i) {
		if(task[i].period != 0)
			curLCM = lcm(curLCM, task[i].period);
	}
	return curLCM;
}

void addFutureTask(int t_num, int hyperPeriod) {
	for(int i = 0; i < t_num; ++i) {
		if(task[i].period == 0)
			continue;
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

bool checkConstraint(int i, bool finish[]) {

	for(int j = 0;j < precedence.size(); ++j)
		if(task[i].id == precedence[j].follow && !finish[precedence[j].first])
			return true;
	return false;
}

bool allTaskfinish(int n, bool finish[]) {
	for(int i = 0; i < n; ++i) {
		if(!finish[i])
			return false;
	}
	return true;
}

bool isSchedulable() {
	// check if it is schedulable
	float uti = 0;
	float U = task.size() *( pow(2, 1/(float) task.size()) - 1);
	for (int i = 0;i < task.size(); ++i) {
		uti += (float) task[i].exec_t / (float) task[i].period;
	}
	if(uti > U) {
		printf("is UNSCHEDULABLE !\n");
		return false;
	}	
	else {
		printf("is SCHEDULABLE !\n");
		return true;
	}
}


void rate_monotonic(int p_num, int init_t_num) {

	int hyperPeriod = getHyperPeriod(init_t_num);
	int min_period;			// save the minimum period
	int cur_time = 0;			// current time
	int cur_task_i;				// current doing task
	int pre_task_i = -1;		// previous doing task
	int wait_time = 0;			// total waiting time
	int cpu_exec = 0;			// CPU execution time
	bool no_task = false;		// no task to run now

	if(!isSchedulable())
		return;

	addFutureTask(init_t_num, hyperPeriod);	// add all tasks in the future hyperperiod

	bool finish[task.size()];		// record the finished task
	int exec_time[task.size()];		// save all current execution time
	int slack_time[task.size()];
	memset(exec_time, 0, sizeof(exec_time));
	memset(finish, false, sizeof(finish));
	memset(slack_time, 0, sizeof(slack_time));

	int end_time[task.size()];
	memset(end_time, 0, task.size());
	
	// printf("Hyper period = %d\n", hyperPeriod);
	printf("\nProcessor %d:\n",processor[0].id + 1);			// single processor

	while( (cur_time <= hyperPeriod && hyperPeriod != 0) ||\
			(!allTaskfinish(task.size(), finish))) {

		// find a task initially
		for(int i = 0; i < task.size() ; ++i) {

			// check precedence constraint
			if(checkConstraint(i, finish))
				continue;

			if(!finish[i] && task[i].release_t <= cur_time){
				cur_task_i = i;
				min_period = task[i].period;
				break;
			}

		}
		
		// find the max rate as the next served task
		if(no_task || finish[pre_task_i]) {
			min_period = INT_MAX;
			no_task = false;
		}
		for(int i = 0; i < task.size(); ++i) {
			if(finish[i])
				continue;

			// check precedence constraint
			if(checkConstraint(i, finish))
				continue;

			if(task[i].release_t <= cur_time) {
				if(task[i].period < min_period) {
					min_period = task[i].period;
					cur_task_i = i;
				}
				else if(task[i].period == min_period &&\
						task[i].id < task[cur_task_i].id) {
					//min_slack_time = slack_time[i];
					cur_task_i = i;
				}
			}
		}

		if(cur_time == 0)
			printf("%3d Task%d ", cur_time, task[cur_task_i].id);

		// no task to do now
		if(min_period == INT_MAX) {
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

			task[cur_task_i].wait_t = cur_time - task[cur_task_i].release_t - task[cur_task_i].exec_t;
			end_time[cur_task_i] = cur_time;

			wait_time += cur_time - task[cur_task_i].release_t - task[cur_task_i].exec_t;
			printf("%3d\n", cur_time);
		}
	}

	float mean = (float) wait_time/(float) task.size();
	printf("\nAverage Waiting Time = %.2f\n", mean);
	if(hyperPeriod != 0)
		printf("CPU utilization = %.2f\n", (float) cpu_exec/ (float) hyperPeriod);
}

int main() {
	int processor_num, task_num, precedence_num;
	int test_case = 0;
	
	// read file
	ifstream inFile("input.txt");

	while(inFile >> processor_num >> task_num) {

		if(test_case != 0)
			printf("\n-------------------------------------\n\n");

		if(processor_num > 1) {
			printf("Only handle the single processor !\n");
			return 0;
		}

		for(int i = 0;i < processor_num;++i) {
			struct Processor tmp;
			inFile >> tmp.id >> tmp.ability;
			processor.push_back(tmp);
		}

		for(int j = 0;j < task_num;++j) {
			struct Task tmp;
			inFile >> tmp.id >> tmp.release_t >> tmp.exec_t;
			inFile >> tmp.deadline >> tmp.period;
			inFile >> tmp.preemption >> tmp.type;

			task.push_back(tmp);
		}

		/* precedence constraint */
		inFile >> precedence_num;
		for(int i = 0;i < precedence_num;++i) {
			struct Precedence tmp;
			inFile >> tmp.first >> tmp.follow;
			precedence.push_back(tmp);
		}

		printf("Test case(%d) ", ++test_case);

		// scheduling and print the result
		rate_monotonic(processor_num, task_num);

		// clear data and move on to next test data
		processor.clear();
		task.clear();
		precedence.clear();
		
	}

	inFile.close();

	return 0;
}