
struct Processor {
	int id;
	int ability;
};

struct Task {
	int id;
	int release_t;
	int exec_t;
	int deadline;
	int period;
	int preemption;
	int type;
	int wait_t;
};

struct Precedence {
	int first;
	int follow;
};