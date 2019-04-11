#include"sjf_scheduling.h"

using namespace std;

vector<struct Processor> processor;
vector<struct Task> task;

vector<int> end_t;

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

void scheduling(int p_num, int t_num) {
	
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

	// for(int i = 0; i < task.size(); ++i) {
	// 	cout << task[i].id << " " << task[i].release_t << endl;
	// }

	int cur_time = 0;						// current time
	bool finish[task.size() + 1];					// record the finished task
	memset(finish, false, task.size() + 1 );
	
	struct Task next_served = task[0];			// initialize next served as first task
	int next_served_end_time = 0;
	int remain_task = task.size();

	/* find the earliest arrival as the first task */
	for(int i = 0; i < task.size(); ++i)
		if(task[i].release_t < next_served.release_t)
			next_served = task[i];

	next_served_end_time = next_served.release_t + next_served.exec_t; 
	printf("Processor 1:\n");
	printf("%3d Task%d %3d\n",next_served.release_t,  next_served.id,next_served_end_time); 
	
	/* update the full information */
	cur_time = next_served_end_time;
	--remain_task;
	finish[next_served.id] = true;
	int cur_task_id;
	int wait_time, cpu_exec = 0;

	/* find the next one (the shortest burst time task) */
	while(remain_task) {
		
		// init the next served as the smallest id task
		for(int i = 0; i < task.size() ; ++i) {
			if(!finish[i]){
				next_served = task[i];
				break;
			}
		}
		
		next_served.exec_t = INT_MAX;
		// find the shortest exec_t as the next served task
		for(int i = 0; i < task.size(); ++i) {
			if(finish[i])
				continue;
			if(task[i].exec_t < next_served.exec_t && task[i].release_t <= cur_time) {
				next_served = task[i];
				cur_task_id = i;
			}
		}

		if(next_served.exec_t == INT_MAX) {
			++cur_time;
			continue;
		}

		wait_time += cur_time - next_served.release_t;
		cpu_exec += next_served.exec_t;
		next_served_end_time = cur_time + next_served.exec_t; 
		printf("%3d Task%d %3d\n", cur_time,next_served.id, next_served_end_time); 
		
		// update the full information
		cur_time = next_served_end_time;
		--remain_task;
		finish[cur_task_id] = true;
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
