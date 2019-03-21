#include"sjf_context_switch.h"

using namespace std;

vector<struct Processor> processor;
vector<struct Task> task;

int cur_time = 0;
int cur_task_id;			// initialize current served as first task

void scheduling(int p_num, int t_num) {
	
	bool finish[t_num];
	memset(finish, false, sizeof(finish));

	// find the first to do
	cur_task_id = 0;
	int remain_task = t_num;

	// find the current served
	for(int i = 0;i < t_num; ++i)
		if(task[cur_task_id].release_t > task[i].release_t)
			cur_task_id = i;

	// set the current time
	cur_time = task[cur_task_id].release_t;

	int prev = cur_task_id;
	printf("%d Task%d ", cur_time, cur_task_id);
	
	while(remain_task > 0) {
		
		int next_task_id;
		vector<int> job_queue;
		
		//	find the to-do job
		for(int i = 0; i < t_num; ++i) {
			if(!finish[i] && task[i].release_t <= cur_time) {
				job_queue.push_back(i);
			}
		}
		
		if(!job_queue.empty())
			next_task_id = job_queue[0];
		else
			continue;

		// find the shortest time
		for(int i = 0; i < job_queue.size(); ++i) {
			if(task[job_queue[i]].exec_t < task[next_task_id].exec_t) {
				next_task_id = job_queue[i];
			}
		}
		
		//	update current task
		if(task[cur_task_id].start_t == -1)
			task[cur_task_id].start_t = cur_time;
		--task[cur_task_id].exec_t;

		//	print info
		if(prev != cur_task_id)
			printf("%d Task%d ", cur_time, cur_task_id);

		//	increase for 1 second
		++cur_time;
	
		//	check if task is finished
		if(task[cur_task_id].exec_t <= 0) {
			printf("%d\n", cur_time);			
			task[cur_task_id].end_t = cur_time;

			finish[cur_task_id] = true;
			--remain_task;	
		
		}
		else {	
			// context switch
			if(next_task_id != cur_task_id) {

				printf("%d\n", cur_time);
				printf("%d Context switch %d\n", cur_time, cur_time+1);
	
				++cur_time;

			}
		}
	
		//	print the info	
		for(int i = 0;i < t_num;++i) {
			printf("id = %d, exec_t = %d, ", task[i].id, task[i].exec_t);
			if(finish[i])
				printf("finish = true\n");
			else
				printf("finish = false\n");
		}


		prev = cur_task_id;
		cur_task_id = next_task_id;
	}
/*
	//	print the info	
	for(int i = 0;i < t_num;++i) {
		printf("id = %d, exec_t = %d, ", task[i].id, task[i].exec_t);
		if(finish[i])
			printf("finish = true\n");
		else
			printf("finish = false\n");
	}
*/
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
		tmp.start_t = tmp.end_t = -1;
		tmp.wait_t = 0;
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
