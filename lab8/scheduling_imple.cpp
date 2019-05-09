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

void sjf_non_preem(int p_num, int t_num) {
	
	printf("SJF without preemtion\n");

	int hyperPeriod = getHyperPeriod(t_num);
	printf("Hyper period = %d\n", hyperPeriod);


	// add all tasks in the future
	for(int i = 0; i < t_num; ++i) {
		for(int p = 1; p*task[i].period < hyperPeriod;++p) {
			struct Task tmp = task[i];
			tmp.release_t = p*task[i].period;
			task.push_back(tmp);
		}
	}

	int cur_time = 0;						// current time
	bool finish[task.size() + 1];					// record the finished task
	memset(finish, false, task.size() + 1 );

	int end_time[task.size()];
	memset(end_time, 0, task.size());
	
	struct Task next_served = task[0];			// initialize next served as first task
	int next_served_end_time = 0;
	int remain_task = task.size();
	int cur_task_id;
	int wait_time, cpu_exec = 0;
	
	printf("Processor 1:\n");
	
	/* find the next one (the shortest burst time task) */
	while(remain_task) {
		
		// init the next served as the smallest id task
		for(int i = 0; i < task.size() ; ++i) {
			if(!finish[i]){
				next_served = task[i];
				break;
			}
		}
		
		// find the shortest exec_t as the next served task
		next_served.exec_t = INT_MAX;
		for(int i = 0; i < task.size(); ++i) {
			if(finish[i])
				continue;
			if(task[i].exec_t < next_served.exec_t && task[i].release_t <= cur_time) {
				next_served = task[i];
				cur_task_id = i;
			}
		}

		// no task to do now
		if(next_served.exec_t == INT_MAX) {
			++cur_time;
			continue;
		}

		task[cur_task_id].wait_t = cur_time - next_served.release_t;

		wait_time += cur_time - next_served.release_t;
		cpu_exec += next_served.exec_t;
		next_served_end_time = cur_time + next_served.exec_t; 
		printf("%3d Task%d %3d\n", cur_time,next_served.id, next_served_end_time); 

		// update the full information
		cur_time = next_served_end_time;
		end_time[cur_task_id] = cur_time;
		--remain_task;
		finish[cur_task_id] = true;
	}
	float mean = (float) wait_time/(float) task.size();
	printf("Average Waiting Time = %.2f\n", mean);
	if(hyperPeriod != 0)
		printf("CPU utilization = %.2f\n", (float) cpu_exec/ (float) hyperPeriod);

	// calculate miss rate
	float miss_num = 0.0f;
	for(int i = 0; i < task.size(); ++i) {
		if(end_time[i] > task[i].deadline)
			++miss_num;
	}
	printf("Miss rate = %.2f\n", (float) miss_num/task.size());

	// calculate standard deviation and variance
	float variance = 0.0f;
	for(int i = 0;i < task.size(); ++i) {
		variance += (task[i].wait_t - mean)*(task[i].wait_t - mean);
	}
	variance /= (task.size()-1);
	printf("Variation of waiting time = %.2f\n", variance);
	printf("Standard deviation of waiting time = %.2f\n", sqrt(variance));
}

void edf_non_preem(int p_num, int init_t_num) {

	printf("EDF without preemtion\n");

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
	
	int end_time[task.size()];
	memset(end_time, 0, task.size());

	printf("Hyper period = %d\n", hyperPeriod);
	printf("Processor %d:\n",processor[0].id + 1);			// single processor
	
	while( (cur_time <= hyperPeriod && hyperPeriod != 0) ||\
			(!allTaskfinish(task.size(), finish))) {

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

		exec_time[cur_task_i] = task[cur_task_i].exec_t;
		cur_time += task[cur_task_i].exec_t;
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
	printf("Average Waiting Time = %.2f\n", mean);
	if(hyperPeriod != 0)
		printf("CPU utilization = %.2f\n", (float) cpu_exec/ (float) hyperPeriod);

	// calculate miss rate
	float miss_num = 0.0f;
	for(int i = 0; i < task.size(); ++i) {
		if(end_time[i] > task[i].deadline)
			++miss_num;
	}
	printf("Miss rate = %.2f\n", (float) miss_num/task.size());

	// calculate standard deviation and variance
	float variance = 0.0f;
	for(int i = 0;i < task.size(); ++i) {
		variance += (task[i].wait_t - mean)*(task[i].wait_t - mean);
	}
	variance /= (task.size()-1);
	printf("Variation of waiting time = %.2f\n", variance);
	printf("Standard deviation of waiting time = %.2f\n", sqrt(variance));
}

void lstf_non_preem(int p_num, int init_t_num) {

	printf("LSTF without preemtion\n");

	int hyperPeriod = getHyperPeriod(init_t_num);
	int min_slack_time;			// save the minimum slack time
	int cur_time = 0;			// current time
	int cur_task_i;				// current doing task
	int pre_task_i = -1;		// previous doing task
	int wait_time = 0;			// total waiting time
	int cpu_exec = 0;			// CPU execution time
	bool no_task = false;		// no task to run now

	addFutureTask(init_t_num, hyperPeriod);	// add all tasks in the future hyperperiod

	bool finish[task.size()];		// record the finished task
	int exec_time[task.size()];		// save all current execution time
	int slack_time[task.size()];
	memset(exec_time, 0, sizeof(exec_time));
	memset(finish, false, sizeof(finish));
	memset(slack_time, 0, sizeof(slack_time));

	int end_time[task.size()];
	memset(end_time, 0, task.size());
	
	printf("Hyper period = %d\n", hyperPeriod);
	printf("Processor %d:\n",processor[0].id + 1);			// single processor
	

	while( (cur_time <= hyperPeriod && hyperPeriod != 0) ||\
			(!allTaskfinish(task.size(), finish))) {

		// count slack time
		for(int i = 0; i < task.size(); ++i) {
			int remain_exec_time = task[i].exec_t - exec_time[i];
			slack_time[i] = task[i].deadline - cur_time - remain_exec_time;
		}

		// init the next task as the smallest id task and record the deadline
		for(int i = 0; i < task.size() ; ++i) {

			// check precedence constraint
			if(checkConstraint(i, finish))
				continue;

			if(!finish[i] && task[i].release_t <= cur_time){
				cur_task_i = i;
				min_slack_time = slack_time[cur_task_i];
				break;
			}
		}
		
		// find the least slack time as the next served task
		if(no_task || finish[pre_task_i]) {
			min_slack_time = INT_MAX;
			no_task = false;
		}
		for(int i = 0; i < task.size(); ++i) {
			if(finish[i])
				continue;

			// check precedence constraint
			if(checkConstraint(i, finish))
				continue;

			if(task[i].release_t <= cur_time) {
				if(slack_time[i] < min_slack_time) {
					min_slack_time = slack_time[i];
					cur_task_i = i;
				}
				else if(slack_time[i] == min_slack_time &&\
						task[i].id < task[cur_task_i].id) {
					//min_slack_time = slack_time[i];
					cur_task_i = i;
				}
			}
		}

		if(cur_time == 0)
			printf("%3d Task%d ", cur_time, task[cur_task_i].id);

		// no task to do now
		if(min_slack_time == INT_MAX) {
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

		exec_time[cur_task_i] = task[cur_task_i].exec_t;
		cur_time += task[cur_task_i].exec_t;
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
	printf("Average Waiting Time = %.2f\n", mean);
	if(hyperPeriod != 0)
		printf("CPU utilization = %.2f\n", (float) cpu_exec/ (float) hyperPeriod);

	// calculate miss rate
	float miss_num = 0.0f;
	for(int i = 0; i < task.size(); ++i) {
		if(end_time[i] > task[i].deadline)
			++miss_num;
	}
	printf("Miss rate = %.2f\n", (float) miss_num/task.size());

	// calculate standard deviation and variance
	float variance = 0.0f;
	for(int i = 0;i < task.size(); ++i) {
		variance += (task[i].wait_t - mean)*(task[i].wait_t - mean);
	}
	variance /= (task.size()-1);
	printf("Variation of waiting time = %.2f\n", variance);
	printf("Standard deviation of waiting time = %.2f\n", sqrt(variance));
}

void sjf_preem(int p_num, int init_t_num) {

	printf("SJF with preemtion\n");

	int hyperPeriod = getHyperPeriod(init_t_num);
	int shortest_exec;			// save the shortest job
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

	int end_time[task.size()];
	memset(end_time, 0, task.size());
	
	printf("Hyper period = %d\n", hyperPeriod);
	printf("Processor %d:\n",processor[0].id + 1);			// single processor
	
	while( (cur_time <= hyperPeriod && hyperPeriod != 0) ||\
			(!allTaskfinish(task.size(), finish))) {

		// init the next task as the smallest id task and record the deadline
		for(int i = 0; i < task.size() ; ++i) {
			if(!finish[i] && task[i].release_t <= cur_time){
				cur_task_i = i;
				shortest_exec = task[cur_task_i].exec_t;
				break;
			}
		}
		
		// find the shortest job as the next served task
		if(no_task || finish[pre_task_i]) {
			shortest_exec = INT_MAX;
			no_task = false;
		}
		for(int i = 0; i < task.size(); ++i) {
			if(finish[i])
				continue;
			if(task[i].release_t <= cur_time) {
				if(task[i].exec_t < shortest_exec) {
					shortest_exec = task[i].exec_t;
					cur_task_i = i;
				}
				/*
				else if(task[i].deadline == deadline &&\
						task[i].id < task[cur_task_i].id) {
					deadline = task[i].deadline;
					cur_task_i = i;
				}
				*/
			}
		}

		if(cur_time == 0)
			printf("%3d Task%d ", cur_time, task[cur_task_i].id);

		// no task to do now
		if(shortest_exec == INT_MAX) {
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
	printf("Average Waiting Time = %.2f\n", mean);
	if(hyperPeriod != 0)
		printf("CPU utilization = %.2f\n", (float) cpu_exec/ (float) hyperPeriod);

	// calculate miss rate
	float miss_num = 0.0f;
	for(int i = 0; i < task.size(); ++i) {
		if(end_time[i] > task[i].deadline)
			++miss_num;
	}
	printf("Miss rate = %.2f\n", (float) miss_num/task.size());

	// calculate standard deviation and variance
	float variance = 0.0f;
	for(int i = 0;i < task.size(); ++i) {
		variance += (task[i].wait_t - mean)*(task[i].wait_t - mean);
	}
	variance /= (task.size()-1);
	printf("Variation of waiting time = %.2f\n", variance);
	printf("Standard deviation of waiting time = %.2f\n", sqrt(variance));
}

void edf_preem(int p_num, int init_t_num) {

	printf("EDF with preemtion\n");

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
	
	int end_time[task.size()];
	memset(end_time, 0, task.size());


	printf("Hyper period = %d\n", hyperPeriod);
	printf("Processor %d:\n",processor[0].id + 1);			// single processor
	
	while( (cur_time <= hyperPeriod && hyperPeriod != 0) ||\
			(!allTaskfinish(task.size(), finish))) {

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

			task[cur_task_i].wait_t = cur_time - task[cur_task_i].release_t - task[cur_task_i].exec_t;
			end_time[cur_task_i] = cur_time;

			wait_time += cur_time - task[cur_task_i].release_t - task[cur_task_i].exec_t;
			printf("%3d\n", cur_time);
		}
	}

	float mean = (float) wait_time/(float) task.size();
	printf("Average Waiting Time = %.2f\n", mean);
	if(hyperPeriod != 0)
		printf("CPU utilization = %.2f\n", (float) cpu_exec/ (float) hyperPeriod);

	// calculate miss rate
	float miss_num = 0.0f;
	for(int i = 0; i < task.size(); ++i) {
		if(end_time[i] > task[i].deadline)
			++miss_num;
	}
	printf("Miss rate = %.2f\n", (float) miss_num/task.size());

	// calculate standard deviation and variance
	float variance = 0.0f;
	for(int i = 0;i < task.size(); ++i) {
		variance += (task[i].wait_t - mean)*(task[i].wait_t - mean);
	}
	variance /= (task.size()-1);
	printf("Variation of waiting time = %.2f\n", variance);
	printf("Standard deviation of waiting time = %.2f\n", sqrt(variance));
}

void lstf_preem(int p_num, int init_t_num) {

	printf("LSTF with preemtion\n");

	int hyperPeriod = getHyperPeriod(init_t_num);
	int min_slack_time;			// save the minimum slack time
	int cur_time = 0;			// current time
	int cur_task_i;				// current doing task
	int pre_task_i = -1;		// previous doing task
	int wait_time = 0;			// total waiting time
	int cpu_exec = 0;			// CPU execution time
	bool no_task = false;		// no task to run now

	addFutureTask(init_t_num, hyperPeriod);	// add all tasks in the future hyperperiod

	bool finish[task.size()];		// record the finished task
	int exec_time[task.size()];		// save all current execution time
	int slack_time[task.size()];
	memset(exec_time, 0, sizeof(exec_time));
	memset(finish, false, sizeof(finish));
	memset(slack_time, 0, sizeof(slack_time));

	int end_time[task.size()];
	memset(end_time, 0, task.size());

	
	printf("Hyper period = %d\n", hyperPeriod);
	printf("Processor %d:\n",processor[0].id + 1);			// single processor
	

	while( (cur_time <= hyperPeriod && hyperPeriod != 0) ||\
			(!allTaskfinish(task.size(), finish))) {

		// count slack time
		for(int i = 0; i < task.size(); ++i) {
			int remain_exec_time = task[i].exec_t - exec_time[i];
			slack_time[i] = task[i].deadline - cur_time - remain_exec_time;
		}

		// init the next task as the smallest id task and record the deadline
		for(int i = 0; i < task.size() ; ++i) {

			// check precedence constraint
			if(checkConstraint(i, finish))
				continue;

			if(!finish[i] && task[i].release_t <= cur_time){
				cur_task_i = i;
				min_slack_time = slack_time[cur_task_i];
				break;
			}
		}
		
		// find the least slack time as the next served task
		if(no_task || finish[pre_task_i]) {
			min_slack_time = INT_MAX;
			no_task = false;
		}
		for(int i = 0; i < task.size(); ++i) {
			if(finish[i])
				continue;

			// check precedence constraint
			if(checkConstraint(i, finish))
				continue;

			if(task[i].release_t <= cur_time) {
				if(slack_time[i] < min_slack_time) {
					min_slack_time = slack_time[i];
					cur_task_i = i;
				}
				else if(slack_time[i] == min_slack_time &&\
						task[i].id < task[cur_task_i].id) {
					//min_slack_time = slack_time[i];
					cur_task_i = i;
				}
			}
		}

		if(cur_time == 0)
			printf("%3d Task%d ", cur_time, task[cur_task_i].id);

		// no task to do now
		if(min_slack_time == INT_MAX) {
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
	printf("Average Waiting Time = %.2f\n", mean);
	if(hyperPeriod != 0)
		printf("CPU utilization = %.2f\n", (float) cpu_exec/ (float) hyperPeriod);

	// calculate miss rate
	float miss_num = 0.0f;
	for(int i = 0; i < task.size(); ++i) {
		if(end_time[i] > task[i].deadline)
			++miss_num;
	}
	printf("Miss rate = %.2f\n", (float) miss_num/task.size());

	// calculate standard deviation and variance
	float variance = 0.0f;
	for(int i = 0;i < task.size(); ++i) {
		variance += (task[i].wait_t - mean)*(task[i].wait_t - mean);
	}
	variance /= (task.size()-1);
	printf("Variation of waiting time = %.2f\n", variance);
	printf("Standard deviation of waiting time = %.2f\n", sqrt(variance));
}

int main() {
	int processor_num, task_num, precedence_num;
	
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

	/* precedence constraint */
	scanf("%d", &precedence_num);
	for(int i = 0;i < precedence_num;++i) {
		struct Precedence tmp;
		scanf("%d%d", &tmp.first, &tmp.follow);
		precedence.push_back(tmp);
	}
	
	fclose(stdin);

	/* scheduling and print the result */
	sjf_non_preem(processor_num, task_num);
	printf("=====================================================\n");
	edf_non_preem(processor_num, task_num);

	printf("=====================================================\n");
	lstf_non_preem(processor_num, task_num);

	printf("=====================================================\n");
	sjf_preem(processor_num, task_num);

	printf("=====================================================\n");
	edf_preem(processor_num, task_num);

	printf("=====================================================\n");
	lstf_preem(processor_num, task_num);

	return 0;
}