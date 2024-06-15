#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PROCESSES 5
#define TIME_QUANTUM 3
#define MAX_TIME 200

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

typedef struct {
    int index;
    double time;
} Eval;

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

int compare_time(const void* a, const void* b);

Queue job, new, ready, waiting, finished;
Chart fcfs, sjf, psjf, prior, pprior, rr;
Eval averageWaitingTime[6], averageTurnaroundTime[6];

int main() {
    srand(time(0));

    job.size = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        create_process(i);
    }
    
    for (int i = 0; i < 6; i++) {
        averageWaitingTime[i].index = i;
        averageTurnaroundTime[i].index = i;
    }
    printf("\n");

    config(fcfs);
    printf("FCFS Scheduling\n");
    schedule_fcfs();

    config(sjf);
    printf("SJF Scheduling\n");
    schedule_sjf();

    config(psjf);
    printf("preemptive SJF Scheduling\n");
    schedule_preemptive_sjf();

    config(prior);
    printf("priority Scheduling\n");
    schedule_priority();

    config(pprior);
    printf("preemptive priority Scheduling\n");
    schedule_preemptive_priority();

    config(rr);
    printf("RR Scheduling\n");
    schedule_rr();

    printf("\n");
    printf("FCFS Scheduling ");
    print_chart(fcfs);
    printf("SJF Scheduling ");
    print_chart(sjf);
    printf("preemptive SJF Scheduling ");
    print_chart(psjf);
    printf("priority Scheduling ");
    print_chart(prior);
    printf("preemptive priority Scheduling ");
    print_chart(pprior);
    printf("RR Scheduling ");
    print_chart(rr);

    printf("\n");
    evaluation();
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

    p.io_enable = (rand() % 2); // 0~1
    if (i == MAX_PROCESSES-1) {
        valid = 0;
        for (int j = 0; j < MAX_PROCESSES-1; j++) {
            if (job.p[j].io_enable != 0) {
                valid = 1;
                break;
            }
        }
        if (valid == 0) {
            p.io_enable = 1;
        }
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

    printf("process %d\t", i+1);
    printf("pid : %d\n", p.pid);
    printf("arrival_time : %d\t", p.arrival_time);
    printf("cpu_burst : %d\n", p.cpu_burst);
    if (p.io_enable != 0){
        printf("io_enable : enabled\t");
        printf("io_start : %d\t", p.io_start);
        printf("io_burst : %d\n", p.io_burst);
    }else{
        printf("io_enable : disabled\n");
    }
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

    c.size = 0;
    
    for (int i = 0; i < MAX_TIME; i++) {
        c.pid[i] = 0;
    }
    
}

void schedule_fcfs() {
    int time = 0;
    int process_finished = 0;
    Process* p = NULL;
    double totalWaitingTime = 0, totalTurnaroundTime = 0;

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
        if (p->io_enable != 0) {
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
        else if (p->io_enable != 0 && p->io_start == 0) {
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

    for (int i = 0; i < MAX_PROCESSES; i++) {
        totalWaitingTime += finished.p[i].waiting_time;
        totalTurnaroundTime += finished.p[i].turnaround_time;
    }
    averageWaitingTime[0].time = totalWaitingTime / MAX_PROCESSES;
    averageTurnaroundTime[0].time = totalTurnaroundTime / MAX_PROCESSES;
    printf("Average Waiting Time: %.2f\n", averageWaitingTime[0].time);
    printf("Average Turnaround Time: %.2f\n\n", averageTurnaroundTime[0].time);
}

void schedule_sjf() {
    int time = 0;
    int process_finished = 0;
    Process* p = NULL;
    double totalWaitingTime = 0, totalTurnaroundTime = 0;

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
        if (p->io_enable != 0) {
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
        else if (p->io_enable != 0 && p->io_start == 0) {
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

    for (int i = 0; i < MAX_PROCESSES; i++) {
        totalWaitingTime += finished.p[i].waiting_time;
        totalTurnaroundTime += finished.p[i].turnaround_time;
    }
    averageWaitingTime[1].time = totalWaitingTime / MAX_PROCESSES;
    averageTurnaroundTime[1].time = totalTurnaroundTime / MAX_PROCESSES;
    printf("Average Waiting Time: %.2f\n", averageWaitingTime[1].time);
    printf("Average Turnaround Time: %.2f\n\n", averageTurnaroundTime[1].time);
}

void schedule_preemptive_sjf() {
    int time = 0;
    int process_finished = 0;
    Process* p = NULL;
    int arrived;
    double totalWaitingTime = 0, totalTurnaroundTime = 0;

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
        if (p->io_enable != 0) {
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
        else if (p->io_enable != 0 && p->io_start == 0) {
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

    for (int i = 0; i < MAX_PROCESSES; i++) {
        totalWaitingTime += finished.p[i].waiting_time;
        totalTurnaroundTime += finished.p[i].turnaround_time;
    }
    averageWaitingTime[2].time = totalWaitingTime / MAX_PROCESSES;
    averageTurnaroundTime[2].time = totalTurnaroundTime / MAX_PROCESSES;
    printf("Average Waiting Time: %.2f\n", averageWaitingTime[2].time);
    printf("Average Turnaround Time: %.2f\n\n", averageTurnaroundTime[2].time);
}

void schedule_priority() {
    int time = 0;
    int process_finished = 0;
    Process* p = NULL;
    double totalWaitingTime = 0, totalTurnaroundTime = 0;

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
        if (p->io_enable != 0) {
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
        else if (p->io_enable != 0 && p->io_start == 0) {
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
    
    for (int i = 0; i < MAX_PROCESSES; i++) {
        totalWaitingTime += finished.p[i].waiting_time;
        totalTurnaroundTime += finished.p[i].turnaround_time;
    }
    averageWaitingTime[3].time = totalWaitingTime / MAX_PROCESSES;
    averageTurnaroundTime[3].time = totalTurnaroundTime / MAX_PROCESSES;
    printf("Average Waiting Time: %.2f\n", averageWaitingTime[3].time);
    printf("Average Turnaround Time: %.2f\n\n", averageTurnaroundTime[3].time);
}

void schedule_preemptive_priority() {
    int time = 0;
    int process_finished = 0;
    Process* p = NULL;
    int arrived;
    double totalWaitingTime = 0, totalTurnaroundTime = 0;

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
        if (p->io_enable != 0) {
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
        else if (p->io_enable != 0 && p->io_start == 0) {
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
    
    for (int i = 0; i < MAX_PROCESSES; i++) {
        totalWaitingTime += finished.p[i].waiting_time;
        totalTurnaroundTime += finished.p[i].turnaround_time;
    }
    averageWaitingTime[4].time = totalWaitingTime / MAX_PROCESSES;
    averageTurnaroundTime[4].time = totalTurnaroundTime / MAX_PROCESSES;
    printf("Average Waiting Time: %.2f\n", averageWaitingTime[4].time);
    printf("Average Turnaround Time: %.2f\n\n", averageTurnaroundTime[4].time);
}

void schedule_rr() {
    int time = 0;
    int process_finished = 0;
    Process* p = NULL;
    int quantumtime = 0;
    double totalWaitingTime = 0, totalTurnaroundTime = 0;

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
        if (p->io_enable != 0) {
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
        else if (p->io_enable != 0 && p->io_start == 0) {
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
    
    for (int i = 0; i < MAX_PROCESSES; i++) {
        totalWaitingTime += finished.p[i].waiting_time;
        totalTurnaroundTime += finished.p[i].turnaround_time;
    }
    averageWaitingTime[5].time = totalWaitingTime / MAX_PROCESSES;
    averageTurnaroundTime[5].time = totalTurnaroundTime / MAX_PROCESSES;
    printf("Average Waiting Time: %.2f\n", averageWaitingTime[5].time);
    printf("Average Turnaround Time: %.2f\n\n", averageTurnaroundTime[5].time);
}

int compare_arrival(const void* a, const void* b) {
    Process* p1 = (Process*)a;
    Process* p2 = (Process*)b;
    if (p1->arrival_time < p2->arrival_time) return -1;
    if (p1->arrival_time > p2->arrival_time) return 1;
    return 0;
}

int compare_burst(const void* a, const void* b) {
    Process* p1 = (Process*)a;
    Process* p2 = (Process*)b;
    if (p1->remain_time < p2->remain_time) return -1;
    if (p1->remain_time > p2->remain_time) return 1;
    return 0;
}

int compare_priority(const void* a, const void* b) {
    Process* p1 = (Process*)a;
    Process* p2 = (Process*)b;
    if (p1->priority < p2->priority) return -1;
    if (p1->priority > p2->priority) return 1;
    return 0;
}

int compare_time(const void* a, const void* b) {
    Eval* p1 = (Eval*)a;
    Eval* p2 = (Eval*)b;
    if (p1->time < p2->time) return -1;
    if (p1->time > p2->time) return 1;
    return 0;
}

void evaluation() {
    printf("Evaluation\n\n");

    qsort(averageWaitingTime, 6, sizeof(Eval), compare_time);

    printf("averageWaitingTime\n");
    for (int i = 0; i < 6; i++) {
        printf("%d : ", i+1);
        switch (averageWaitingTime[i].index) {
            case 0:
                printf("FCFS = ");
                break;
            case 1:
                printf("SJF = ");
                break;
            case 2:
                printf("preemptive SJF = ");
                break;
            case 3:
                printf("priority = ");
                break;
            case 4:
                printf("preemptive priority = ");
                break;
            case 5:
                printf("RR = ");
                break;
        }
        printf("%.2f\n", averageWaitingTime[i].time);
    }
    printf("\n");
    
    qsort(averageTurnaroundTime, 6, sizeof(Eval), compare_time);

    printf("averageTurnaroundTime\n");
    for (int i = 0; i < 6; i++) {
        printf("%d : ", i+1);
        switch (averageTurnaroundTime[i].index) {
            case 0:
                printf("FCFS = ");
                break;
            case 1:
                printf("SJF = ");
                break;
            case 2:
                printf("preemptive SJF = ");
                break;
            case 3:
                printf("priority = ");
                break;
            case 4:
                printf("preemptive priority = ");
                break;
            case 5:
                printf("RR = ");
                break;
        }
        printf("%.2f\n", averageTurnaroundTime[i].time);
    }
    printf("\n");
}

void print_chart(Chart c) {
    printf("Gantt Chart:");
    for (int i = 0; i < MAX_TIME; i++) {
        if (c.pid[i] == 0) {
            printf("\n\n");
            break;
        }
        if (i % 10 == 0) {
            printf("\n");
        }
        else if (c.pid[i] == -1) {
            printf("IDLE");
        }
        else {
            printf("%04d", c.pid[i]);
        }
        printf("|");
    }
}
