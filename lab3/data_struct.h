
struct Processor {
	int id;
	int ability;
};

struct Task {
	int id;
	int release_t;
	int exec_t;
	int wait_t;
	int start_t;
	int end_t;
	int deadline;
	int period;
	int preemption;
	int type;
/*
	bool operator< (const struct Task &t1, const struct Task &t2) {
		return t1.exec_t < t2.exec_t;
	}
*/
};
