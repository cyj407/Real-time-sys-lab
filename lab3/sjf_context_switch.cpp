#include"sjf_context_switch.h"

using namespace std;

vector<struct Processor> processor;
vector<struct Task> task;

vector<int> end_t, exec_t;

int cur_time = 0;
int cur_task_id = -1;			// initialize current served as first task

void scheduling(int p_num, int t_num) {
	
	bool finish[t_num];
	memset(finish, false, sizeof(finish));
	int remain_task = t_num;
	int prev_task_id;

	for(int i = 0; i < t_num ;++i) {
		end_t.push_back(-1);
		exec_t.push_back(0);
	}

	while(remain_task > 0) {

		prev_task_id = cur_task_id;

		//  find job queue
		vector<int> job_queue;
		for(int i = 0;i < t_num; ++i) {
			if(task[i].release_t <= cur_time && !finish[i]) {
				job_queue.push_back(i);
			}
		}

		//	find the next to do
		int min = INT_MAX;
		for(int i = 0;i < job_queue.size(); ++i) {
			int job_id = job_queue[i];
			int remain_t = task[job_id].exec_t - exec_t[job_id];

			if(remain_t < min) {
				cur_task_id = job_id;
				min = remain_t;
			}
		}

		// print first task
		if(cur_time == 0) {
			printf("%d Task%d ", cur_time, cur_task_id);
		}

		//	finish the last one
		if(finish[prev_task_id]) {
			// check if current task has started executing 
			if(exec_t[cur_task_id] != 0) {

				//	deal with CONTEXT SWITCH
				printf("%d context switch %d\n", cur_time, cur_time+1);
				printf("%d Task%d ", cur_time+1, cur_task_id);
				++cur_time;
			}
			else
				printf("%d Task%d ", cur_time, cur_task_id);
		}
		
		//	check the three conditions and do context switch
		//  1. not first task ->  current time != 0
		//  2. have to change the current task
		//  3. previous task has finished
		if(cur_time != 0 && prev_task_id != cur_task_id && !finish[prev_task_id]) {
			
			//	deal with CONTEXT SWITCH
			printf("%d\n", cur_time);
			printf("%d context switch %d\n", cur_time, cur_time+1);
			
			printf("%d Task%d ", cur_time+1, cur_task_id);
			++cur_time;
		}

		//	 update info
		++exec_t[cur_task_id];
		++cur_time;
		
		//	check if finish
		if(exec_t[cur_task_id] >= task[cur_task_id].exec_t) {
			finish[cur_task_id] = true;
			--remain_task;
			
			end_t[cur_task_id] = cur_time;
			
			printf("%d\n", cur_time);
		}

	}

	// count waiting time
	int wait_t[t_num];
	float hit_num = 0.0f, avr_wait_t = 0.0f;
	for(int i = 0; i < t_num ;++i) {

		wait_t[i] = end_t[i] - task[i].release_t - task[i].exec_t;
		avr_wait_t += wait_t[i];

		if(task[i].deadline > end_t[i])
			++hit_num;	
	}
	avr_wait_t = avr_wait_t / t_num;
	printf("--------------------\n");
	printf("Average waiting time = %.2f\n", avr_wait_t);
	printf("Hit rate = %.2f\n",(float) hit_num/t_num);

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
