#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

// PCB
typedef struct process {
    int pid;
    int priority;
    int arrival_time;
    int burst_time;
    int waiting_time;
    int response_time;
    int turnaround_time;
    int remaining_time;
    int start_time;
} p;

// Check the average of waiting time
float checkWaiting(p process[], int num) {
    int sum = 0;
    for (int i = 0; i < num; i++) {
        sum = sum + process[i].waiting_time;
    }
    return (float)sum / num;
}

// Check the average of response time
float checkResponse(p process[], int num) {
    int sum = 0;
    for (int i = 0; i < num; i++) {
        sum = sum + process[i].response_time;
    }
    return (float)sum / num;
}

// Check the average of turnaround time
float checkTurnaround(p process[], int num) {
    int sum = 0;
    for (int i = 0; i < num; i++) {
        sum = sum + process[i].turnaround_time;
    }
    return (float)sum / num;
}

// Aging function to increment priority of waiting processes
void aging(p process[], int num, int alpha, int currentTime) {
    for (int i = 0; i < num; i++) {
        if (process[i].remaining_time > 0 && process[i].arrival_time <= currentTime && process[i].start_time == -1) {
            process[i].priority -= alpha; // Decrease the priority value to simulate aging
        }
    }
}

// Print time-flow and process execution
void printChart(p process[], int num, int alpha, int* totalRunningTime) {
    int currentTime = 0;
    int completed = 0;
    int n = num;
    int nextArrivalIndex = 0;
    p* current_process = NULL;

    printf("\nScheduling: Preemptive Priority Scheduling with Aging\n");
    printf("==========================================\n");

    while (completed < n) {
        int idle = 1;

        // Aging
        aging(process, num, alpha, currentTime);

        // Check for new process arrival
        for (int i = 0; i < n; i++) {
            if (currentTime == process[i].arrival_time) {
                printf("<time %d> [new arrival] process %d\n", currentTime, process[i].pid);
                idle = 0; // New process arrival, system is not idle

                // Check for preemption
                if (current_process == NULL || process[i].priority < current_process->priority) {
                    if (current_process != NULL) {
                        printf("<time %d> process %d is preempted by process %d\n", currentTime, current_process->pid, process[i].pid);
                    }
                    current_process = &process[i];
                    if (current_process->start_time == -1) {
                        current_process->start_time = currentTime;
                        current_process->response_time = currentTime - current_process->arrival_time;
                    }
                }
            }
        }

        // Execute the current process
        if (current_process != NULL) {
            idle = 0;
            printf("<time %d> process %d is running\n", currentTime, current_process->pid);
            current_process->remaining_time--;
            (*totalRunningTime)++;
            if (current_process->remaining_time == 0) {
                completed++;
                current_process->turnaround_time = currentTime + 1 - current_process->arrival_time;
                current_process->waiting_time = current_process->turnaround_time - current_process->burst_time;
                printf("<time %d> process %d is finished\n", currentTime + 1, current_process->pid);
                current_process = NULL;

                // Check for the next highest priority process
                for (int i = 0; i < n; i++) {
                    if (process[i].remaining_time > 0 && (current_process == NULL || process[i].priority < current_process->priority)) {
                        current_process = &process[i];
                        if (current_process->start_time == -1) {
                            current_process->start_time = currentTime + 1;
                            current_process->response_time = currentTime + 1 - current_process->arrival_time;
                        }
                    }
                }
            }
        }

        if (idle) {
            printf("<time %d> ---- system is idle ----\n", currentTime);
            currentTime++;
        }
        else {
            currentTime++;
        }
    }

    printf("<time %d> all processes finish\n", currentTime);
    printf("==========================================\n");
}

int main() {
    p process[10];
    int process_number = 0;
    int time_quantum;  // This value is not used in priority scheduling
    int alpha;         // Aging factor
    int totalRunningTime = 0;

    FILE* file;
    file = fopen("input.dat", "r");
    if (file == NULL) {
        printf("File Open Error!\n");
        return 1;
    }

    // Get the value
    while (fscanf(file, "%d %d %d %d", &process[process_number].pid, &process[process_number].priority, &process[process_number].arrival_time, &process[process_number].burst_time) != EOF) {
        process[process_number].remaining_time = process[process_number].burst_time;
        process[process_number].start_time = -1;
        process_number++;
    }

    fclose(file);

    // Arrange processes based on arrival time
    for (int i = 0; i < process_number - 1; i++) {
        for (int j = 0; j < process_number - (i + 1); j++) {
            if (process[j].arrival_time > process[j + 1].arrival_time) {
                p temp = process[j];
                process[j] = process[j + 1];
                process[j + 1] = temp;
            }
        }
    }

    // Get time quantum and alpha value from user
    printf("Enter the time quantum: ");
    scanf("%d", &time_quantum); // Not used in this scheduling algorithm
    printf("Enter the aging factor alpha: ");
    scanf("%d", &alpha);

    printChart(process, process_number, alpha, &totalRunningTime);

    // Calculate CPU usage
    int totalTime = 0;
    for (int i = 0; i < process_number; i++) {
        if (process[i].start_time != -1) {
            totalTime = (process[i].start_time + process[i].burst_time > totalTime) ? process[i].start_time + process[i].burst_time : totalTime;
        }
    }

    // Print statistical data
    float cpuUsage = ((float)totalRunningTime / totalTime) * 100;
    float avg_waiting_time = checkWaiting(process, process_number);
    float avg_response_time = checkResponse(process, process_number);
    float avg_turnaround_time = checkTurnaround(process, process_number);

    printf("Average CPU Usage: %.2f%%\n", cpuUsage);
    printf("Average Waiting Time: %.2f\n", avg_waiting_time);
    printf("Average Response Time: %.2f\n", avg_response_time);
    printf("Average Turnaround Time: %.2f\n", avg_turnaround_time);

    return 0;
}
