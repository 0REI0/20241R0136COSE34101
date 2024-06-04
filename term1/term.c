#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESSES 5
#define TIME_QUANTUM 3
#define MAX_TIME 500

typedef struct {
    int pid;
    int arrival_time;
    int cpu_burst;
    int remain_time;
    int io_enable;
    int io_start;
    int io_burst;
    int priority;
    int completed;
    int waiting_time;
    int turnaround_time;
} Process;

typedef struct {
    Process p[MAX_PROCESSES];
    int size;
} Queue;

typedef struct {
    int pid[MAX_TIME];
    int size;
} Chart;

void create_process(int pid);
void config(Chart c);

void schedule_fcfs();
void schedule_sjf();
void schedule_preemptive_sjf();
void schedule_priority();
void schedule_preemptive_priority();
void schedule_rr();

void evaluation();
void print_chart(Chart c);

int compare_arrival(const void* a, const void* b);
int compare_burst(const void* a, const void* b);
int compare_priority(const void* a, const void* b);

Queue job, new, ready, waiting, finished;
Chart fcfs, sjf, psjf, prior, pprior, rr;

int main() {
    srand(time(0));

    for (int i = 0; i < MAX_PROCESSES; i++) {
        create_process(i);
    }

    config(fcfs);
    printf("FCFS Scheduling\n");
    schedule_fcfs();
    evaluation();

    config(sjf);
    printf("SJF Scheduling\n");
    schedule_sjf();
    evaluation();

    config(psjf);
    printf("preemptive SJF Scheduling\n");
    schedule_preemptive_sjf();
    evaluation();

    config(prior);
    printf("priority Scheduling\n");
    schedule_priority();
    evaluation();

    config(pprior);
    printf("preemptive priority Scheduling\n");
    schedule_preemptive_priority();
    evaluation();

    config(rr);
    printf("RR Scheduling\n");
    schedule_rr();
    evaluation();
    
    printf("FCFS Scheduling\n");
    print_chart(fcfs);
    printf("SJF Scheduling\n");
    print_chart(sjf);
    printf("preemptive SJF Scheduling\n");
    print_chart(psjf);
    printf("priority Scheduling\n");
    print_chart(prior);
    printf("preemptive priority Scheduling\n");
    print_chart(pprior);
    printf("RR Scheduling\n");
    print_chart(rr);
}

void create_process(int i){
    Process p;
    int pid;
    int valid;

    while (1) {
        valid = 1; 
        pid = (rand() % 5000) + 1; // 1~5000
        for (int j = 0; j < job.size; j++) {
            if (pid == job.p[j].pid) {
                valid = 0;
            }
        }
        if (valid == 1) {
            p.pid = pid;
            break;
        }
    }

    p.arrival_time = rand() % 11; // 0~10
    p.cpu_burst = (rand() % 16) + 5; // 5~20
    p.remain_time = p.cpu_burst; 
    if (i == 0) {
        p.io_enable = 1;
    }
    else {
        p.io_enable = (rand() % 2); // 0~1
    }
    if (p.io_enable != 0) {
        p.io_start = (rand() % (p.cpu_burst - 1)) + 1; // 1~(cpuburst-1)
        p.io_burst = (rand() % 4) + 2; // 2~5
    }
    else {
        p.io_start = 0;
        p.io_burst = 0;
    }
    p.priority = rand() % 21; // 0~20
    p.completed = 0;
    p.waiting_time = 0;
    p.turnaround_time = 0;

    printf("process %d\n", i+1);
    printf("pid : %d\n", p.pid);
    printf("arrival_time : %d\n", p.arrival_time);
    printf("cpu_burst : %d\n", p.cpu_burst);
    printf("io_enable : %d\n", p.io_enable);
    printf("io_start : %d\n", p.io_start);
    printf("io_burst : %d\n", p.io_burst);
    printf("priority : %d\n\n", p.priority);

    job.size++;
    job.p[i] = p;
}

void config(Chart c) {
    Process p;

    new.size = MAX_PROCESSES;
    ready.size = 0;
    waiting.size = 0;
    finished.size = 0;

    p.pid = 0;
    p.arrival_time = 0;
    p.cpu_burst = 0;
    p.remain_time = 0;
    p.io_enable = 0;
    p.io_start = 0;
    p.io_burst = 0;
    p.priority = 0;
    p.completed = 0;
    p.waiting_time = 0;
    p.turnaround_time = 0;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        new.p[i] = job.p[i];
        ready.p[i] = p;
        waiting.p[i] = p;
        finished.p[i] = p;
    }

    for (int i = 0; i < MAX_PROCESSES; i++) {
        c.pid[i] = 0;
    }
}

void schedule_fcfs() {
    int time = 0;
    int process_finished = 0;
    Process* p = NULL;
    int minindex = 0;

    while (process_finished < MAX_PROCESSES) {
        for (int i = 0; i < new.size; i++) {
            if (new.p[i].arrival_time == time) {
                ready.p[ready.size] = new.p[i];
                ready.size++;
                new.p[i] = new.p[new.size-1];
                new.size--;
                i--;
            }
        }
        for (int i = 0; i < waiting.size; i++) {
            if (waiting.p[i].io_burst == 0) {
                ready.p[ready.size] = waiting.p[i];
                ready.size++;
                waiting.p[i] = waiting.p[waiting.size-1];
                waiting.size--;
                i--;
            }
        }
        
        if (ready.size == 0) {
            if (waiting.size != 0) {
                for (int i = 0; i < waiting.size; i++) {
                    waiting.p[i].turnaround_time++;
                    waiting.p[i].waiting_time++;
                    waiting.p[i].io_burst--;
                }
            }
            fcfs.pid[time] = -1;
            time++;
            continue;
        }

        if (p == NULL) {
            qsort(ready.p, ready.size, sizeof(Process), compare_arrival);
            p = &ready.p[0];
        }
        
        fcfs.pid[time] = p->pid;
        
        if (ready.size != 0) {
            for (int i = 0; i < ready.size; i++) {
                ready.p[i].turnaround_time++;
                if (ready.p[i].pid != p->pid) {
                    ready.p[i].waiting_time++;
                }
            }
        }
        if (waiting.size != 0) {
            for (int i = 0; i < waiting.size; i++) {
                waiting.p[i].turnaround_time++;
                waiting.p[i].waiting_time++;
                waiting.p[i].io_burst--;
            }
        }

        p->remain_time--;
        if (p->io_enable == 1) {
            p->io_start--;
        }

        if (p->remain_time == 0) {
            p->completed = 1;
            p = NULL;

            finished.p[finished.size] = ready.p[0];
            finished.size++;
            ready.p[0] = ready.p[ready.size - 1];
            ready.size--;

            process_finished++;
        }
        else if (p->io_enable == 1 && p->io_start == 0) {
            p->arrival_time = time + p->io_burst;
            p->io_enable = 0;
            p = NULL;

            waiting.p[waiting.size] = ready.p[0];
            waiting.size++;
            ready.p[0] = ready.p[ready.size - 1];
            ready.size--;
        }

        time++;
    }
}

void schedule_sjf() {
    int time = 0;
    int process_finished = 0;
    Process* p = NULL;

    while (process_finished < MAX_PROCESSES) {
        for (int i = 0; i < new.size; i++) {
            if (new.p[i].arrival_time == time) {
                ready.p[ready.size] = new.p[i];
                ready.size++;
                new.p[i] = new.p[new.size - 1];
                new.size--;
                i--;
            }
        }
        for (int i = 0; i < waiting.size; i++) {
            if (waiting.p[i].io_burst == 0) {
                ready.p[ready.size] = waiting.p[i];
                ready.size++;
                waiting.p[i] = waiting.p[waiting.size - 1];
                waiting.size--;
                i--;
            }
        }

        if (ready.size == 0) {
            if (waiting.size != 0) {
                for (int i = 0; i < waiting.size; i++) {
                    waiting.p[i].turnaround_time++;
                    waiting.p[i].waiting_time++;
                    waiting.p[i].io_burst--;
                }
            }
            sjf.pid[time] = -1;
            time++;
            continue;
        }

        if (p == NULL) {
            qsort(ready.p, ready.size, sizeof(Process), compare_burst);
            p = &ready.p[0];
        }

        sjf.pid[time] = p->pid;

        if (ready.size != 0) {
            for (int i = 0; i < ready.size; i++) {
                ready.p[i].turnaround_time++;
                if (ready.p[i].pid != p->pid) {
                    ready.p[i].waiting_time++;
                }
            }
        }
        if (waiting.size != 0) {
            for (int i = 0; i < waiting.size; i++) {
                waiting.p[i].turnaround_time++;
                waiting.p[i].waiting_time++;
                waiting.p[i].io_burst--;
            }
        }

        p->remain_time--;
        if (p->io_enable == 1) {
            p->io_start--;
        }

        if (p->remain_time == 0) {
            p->completed = 1;
            p = NULL;

            finished.p[finished.size] = ready.p[0];
            finished.size++;
            ready.p[0] = ready.p[ready.size - 1];
            ready.size--;

            process_finished++;
        }
        else if (p->io_enable == 1 && p->io_start == 0) {
            p->arrival_time = time + p->io_burst;
            p->io_enable = 0;
            p = NULL;

            waiting.p[waiting.size] = ready.p[0];
            waiting.size++;
            ready.p[0] = ready.p[ready.size - 1];
            ready.size--;
        }

        time++;
    }
}

void schedule_preemptive_sjf() {
    int time = 0;
    int process_finished = 0;
    Process* p = NULL;
    int arrived;

    while (process_finished < MAX_PROCESSES) {
        arrived = 0;

        for (int i = 0; i < new.size; i++) {
            if (new.p[i].arrival_time == time) {
                ready.p[ready.size] = new.p[i];
                ready.size++;
                new.p[i] = new.p[new.size - 1];
                new.size--;
                i--;
                arrived = 1;
            }
        }
        for (int i = 0; i < waiting.size; i++) {
            if (waiting.p[i].io_burst == 0) {
                ready.p[ready.size] = waiting.p[i];
                ready.size++;
                waiting.p[i] = waiting.p[waiting.size - 1];
                waiting.size--;
                i--;
                arrived = 1;
            }
        }

        if (ready.size == 0) {
            if (waiting.size != 0) {
                for (int i = 0; i < waiting.size; i++) {
                    waiting.p[i].turnaround_time++;
                    waiting.p[i].waiting_time++;
                    waiting.p[i].io_burst--;
                }
            }
            psjf.pid[time] = -1;
            time++;
            continue;
        }

        if (p == NULL || arrived == 1) {
            qsort(ready.p, ready.size, sizeof(Process), compare_burst);
            p = &ready.p[0];
        }


        psjf.pid[time] = p->pid;

        if (ready.size != 0) {
            for (int i = 0; i < ready.size; i++) {
                ready.p[i].turnaround_time++;
                if (ready.p[i].pid != p->pid) {
                    ready.p[i].waiting_time++;
                }
            }
        }
        if (waiting.size != 0) {
            for (int i = 0; i < waiting.size; i++) {
                waiting.p[i].turnaround_time++;
                waiting.p[i].waiting_time++;
                waiting.p[i].io_burst--;
            }
        }

        p->remain_time--;
        if (p->io_enable == 1) {
            p->io_start--;
        }

        if (p->remain_time == 0) {
            p->completed = 1;
            p = NULL;

            finished.p[finished.size] = ready.p[0];
            finished.size++;
            ready.p[0] = ready.p[ready.size - 1];
            ready.size--;

            process_finished++;
        }
        else if (p->io_enable == 1 && p->io_start == 0) {
            p->arrival_time = time + p->io_burst;
            p->io_enable = 0;
            p = NULL;

            waiting.p[waiting.size] = ready.p[0];
            waiting.size++;
            ready.p[0] = ready.p[ready.size - 1];
            ready.size--;
        }

        time++;
    }
}

void schedule_priority() {
    int time = 0;
    int process_finished = 0;
    Process* p = NULL;

    while (process_finished < MAX_PROCESSES) {
        for (int i = 0; i < new.size; i++) {
            if (new.p[i].arrival_time == time) {
                ready.p[ready.size] = new.p[i];
                ready.size++;
                new.p[i] = new.p[new.size - 1];
                new.size--;
                i--;
            }
        }
        for (int i = 0; i < waiting.size; i++) {
            if (waiting.p[i].io_burst == 0) {
                ready.p[ready.size] = waiting.p[i];
                ready.size++;
                waiting.p[i] = waiting.p[waiting.size - 1];
                waiting.size--;
                i--;
            }
        }

        if (ready.size == 0) {
            if (waiting.size != 0) {
                for (int i = 0; i < waiting.size; i++) {
                    waiting.p[i].turnaround_time++;
                    waiting.p[i].waiting_time++;
                    waiting.p[i].io_burst--;
                }
            }
            prior.pid[time] = -1;
            time++;
            continue;
        }

        if (p == NULL) {
            qsort(ready.p, ready.size, sizeof(Process), compare_priority);
            p = &ready.p[0];
        }

        prior.pid[time] = p->pid;

        if (ready.size != 0) {
            for (int i = 0; i < ready.size; i++) {
                ready.p[i].turnaround_time++;
                if (ready.p[i].pid != p->pid) {
                    ready.p[i].waiting_time++;
                }
            }
        }
        if (waiting.size != 0) {
            for (int i = 0; i < waiting.size; i++) {
                waiting.p[i].turnaround_time++;
                waiting.p[i].waiting_time++;
                waiting.p[i].io_burst--;
            }
        }

        p->remain_time--;
        if (p->io_enable == 1) {
            p->io_start--;
        }

        if (p->remain_time == 0) {
            p->completed = 1;
            p = NULL;

            finished.p[finished.size] = ready.p[0];
            finished.size++;
            ready.p[0] = ready.p[ready.size - 1];
            ready.size--;

            process_finished++;
        }
        else if (p->io_enable == 1 && p->io_start == 0) {
            p->arrival_time = time + p->io_burst;
            p->io_enable = 0;
            p = NULL;

            waiting.p[waiting.size] = ready.p[0];
            waiting.size++;
            ready.p[0] = ready.p[ready.size - 1];
            ready.size--;
        }
        time++;
    }
}

void schedule_preemptive_priority() {
    int time = 0;
    int process_finished = 0;
    Process* p = NULL;
    int arrived;

    while (process_finished < MAX_PROCESSES) {
        arrived = 0;

        for (int i = 0; i < new.size; i++) {
            if (new.p[i].arrival_time == time) {
                ready.p[ready.size] = new.p[i];
                ready.size++;
                new.p[i] = new.p[new.size - 1];
                new.size--;
                i--;
                arrived = 1;
            }
        }
        for (int i = 0; i < waiting.size; i++) {
            if (waiting.p[i].io_burst == 0) {
                ready.p[ready.size] = waiting.p[i];
                ready.size++;
                waiting.p[i] = waiting.p[waiting.size - 1];
                waiting.size--;
                i--;
                arrived = 1;
            }
        }

        if (ready.size == 0) {
            if (waiting.size != 0) {
                for (int i = 0; i < waiting.size; i++) {
                    waiting.p[i].turnaround_time++;
                    waiting.p[i].waiting_time++;
                    waiting.p[i].io_burst--;
                }
            }
            pprior.pid[time] = -1;
            time++;
            continue;
        }

        if (p == NULL || arrived == 1) {
            qsort(ready.p, ready.size, sizeof(Process), compare_priority);
            p = &ready.p[0];
        }

        pprior.pid[time] = p->pid;

        if (ready.size != 0) {
            for (int i = 0; i < ready.size; i++) {
                ready.p[i].turnaround_time++;
                if (ready.p[i].pid != p->pid) {
                    ready.p[i].waiting_time++;
                }
            }
        }
        if (waiting.size != 0) {
            for (int i = 0; i < waiting.size; i++) {
                waiting.p[i].turnaround_time++;
                waiting.p[i].waiting_time++;
                waiting.p[i].io_burst--;
            }
        }

        p->remain_time--;
        if (p->io_enable == 1) {
            p->io_start--;
        }

        if (p->remain_time == 0) {
            p->completed = 1;
            p = NULL;

            finished.p[finished.size] = ready.p[0];
            finished.size++;
            ready.p[0] = ready.p[ready.size - 1];
            ready.size--;

            process_finished++;
        }
        else if (p->io_enable == 1 && p->io_start == 0) {
            p->arrival_time = time + p->io_burst;
            p->io_enable = 0;
            p = NULL;

            waiting.p[waiting.size] = ready.p[0];
            waiting.size++;
            ready.p[0] = ready.p[ready.size - 1];
            ready.size--;
        }

        time++;
    }
}

void schedule_rr() {
    int time = 0;
    int process_finished = 0;
    Process* p = NULL;
    int quantumtime = 0;

    while (process_finished < MAX_PROCESSES) {
        for (int i = 0; i < new.size; i++) {
            if (new.p[i].arrival_time == time) {
                ready.p[ready.size] = new.p[i];
                ready.size++;
                new.p[i] = new.p[new.size-1];
                new.size--;
                i--;
            }
        }
        for (int i = 0; i < waiting.size; i++) {
            if (waiting.p[i].io_burst == 0) {
                ready.p[ready.size] = waiting.p[i];
                ready.size++;
                waiting.p[i] = waiting.p[waiting.size-1];
                waiting.size--;
                i--;
            }
        }
        
        if (ready.size == 0) {
            if (waiting.size != 0) {
                for (int i = 0; i < waiting.size; i++) {
                    waiting.p[i].turnaround_time++;
                    waiting.p[i].waiting_time++;
                    waiting.p[i].io_burst--;
                }
            }
            rr.pid[time] = -1;
            quantumtime = 0;
            time++;
            continue;
        }

        if (p == NULL) {
            qsort(ready.p, ready.size, sizeof(Process), compare_arrival);
            p = &ready.p[0];
        }

        rr.pid[time] = p->pid;

        if (ready.size != 0) {
            for (int i = 0; i < ready.size; i++) {
                ready.p[i].turnaround_time++;
                if (ready.p[i].pid != p->pid) {
                    ready.p[i].waiting_time++;
                }
            }
        }
        if (waiting.size != 0) {
            for (int i = 0; i < waiting.size; i++) {
                waiting.p[i].turnaround_time++;
                waiting.p[i].waiting_time++;
                waiting.p[i].io_burst--;
            }
        }

        p->remain_time--;
        if (p->io_enable == 1) {
            p->io_start--;
        }
        quantumtime++;

        if (quantumtime == TIME_QUANTUM) {
            p->arrival_time = time;
        }

        if (p->remain_time == 0) {
            p->completed = 1;
            p = NULL;
            quantumtime = 0;

            finished.p[finished.size] = ready.p[0];
            finished.size++;
            ready.p[0] = ready.p[ready.size-1];
            ready.size--;

            process_finished++;
        }
        else if (p->io_enable == 1 && p->io_start == 0) {
            p->arrival_time = time + p->io_burst;
            p->io_enable = 0;
            p = NULL;
            quantumtime = 0;

            waiting.p[waiting.size] = ready.p[0];
            waiting.size++;
            ready.p[0] = ready.p[ready.size-1];
            ready.size--;
        }

        if (quantumtime == TIME_QUANTUM) {
            p = NULL;
            quantumtime = 0;
        }

        time++;
    }
}

int compare_arrival(const void* a, const void* b) {
    Process* p1 = (Process*)a;
    Process* p2 = (Process*)b;
    return p1->arrival_time - p2->arrival_time;
}

int compare_burst(const void* a, const void* b) {
    Process* p1 = (Process*)a;
    Process* p2 = (Process*)b;
    return p1->remain_time - p2->remain_time;
}

int compare_priority(const void* a, const void* b) {
    Process* p1 = (Process*)a;
    Process* p2 = (Process*)b;
    return p1->priority - p2->priority;
}

void evaluation() {
    double totalWaitingTime = 0, totalTurnaroundTime = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        totalWaitingTime += finished.p[i].waiting_time;
        totalTurnaroundTime += finished.p[i].turnaround_time;
    }
    printf("Average Waiting Time: %.2f\n", totalWaitingTime / MAX_PROCESSES);
    printf("Average Turnaround Time: %.2f\n\n", totalTurnaroundTime / MAX_PROCESSES);
}

void print_chart(Chart c) {
    printf("Gantt Chart:\n");
    for (int i = 0; i < MAX_TIME; i++) {
        if (c.pid[i] == 0) {
            printf("\n\n");
            break;
        }
        else if (c.pid[i] == -1) {
            printf("IDLE");
        }
        else {
            printf("%04d", c.pid[i]);
        }
        printf("|");
        if (i % 10 == 9) {
            printf("\n");
        }
    }
}
