#include <bits/stdc++.h>
using namespace std;
#define LAXITY_PROPORTIONALITY_CONSTANT 0.10
#define PRICE_CONSTANT_K1 3
#define PRICE_CONSTANT_K2 10000 
#define int long long 

class Task{
    public:
        int id;
        int arrival_time;
        int execution_time;
        int deadline;
        int price;

        Task(){
            id = 0;
            arrival_time = 0;
            execution_time = 0;
            deadline = 0;
            price = 0;
        }

        Task(int _id,
             int _arrival_time,
             int _execution_time,
             int _deadline,
             int _price){
                id = _id;
                arrival_time = _arrival_time;
                execution_time = _execution_time;
                deadline = _deadline;
                price = _price;
        }
};

bool compareTwoTasks(Task &a, Task &b)
{
    return a.arrival_time < b.arrival_time;
}

int32_t main(){
    freopen("trace_data_9.txt", "w", stdout);  
    Task tasks[5000];
    ifstream trace_data_1; 
    int task_id, priority, task_arrival_time, task_execution_time;
    trace_data_1.open("Google_cluster_data/trace_part9.csv");    
    string _a, _b, _c; trace_data_1>>_a>>_b>>_c;
    int i = 0, min_arrival_time = 1e6;
    while(trace_data_1.good()){
        trace_data_1>>task_id>>priority>>task_arrival_time>>task_execution_time;
        task_arrival_time /= 60000000;
        task_execution_time /= 1000000;
        int laxity = (double)LAXITY_PROPORTIONALITY_CONSTANT*task_execution_time;
        laxity += rand()%10+1;
        int deadline = task_arrival_time + task_execution_time + laxity;
        int price = (double)pow(task_execution_time, 1.5)*PRICE_CONSTANT_K1 + (double)PRICE_CONSTANT_K2/(laxity*laxity);
        min_arrival_time = min(min_arrival_time, task_arrival_time);
        tasks[i] = Task(i, task_arrival_time - min_arrival_time, task_execution_time, deadline, price);
        i++;
    }
    sort (tasks, tasks+1116, compareTwoTasks);


    for(i=0;i<1116;i++)
    {
        cout<<i+1<<" "<<tasks[i].arrival_time<<" "<<tasks[i].execution_time<<" "<<tasks[i].deadline<<" "<<tasks[i].price<<"\n";
    }
}