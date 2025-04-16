#include<bits/stdc++.h>
#define NUM_OF_TASKS 1000
#define OUTLIER_TASKS ((5*NUM_OF_TASKS)/100)
#define ARRIVAL_RATE_OF_TASKS 60
#define EXPECTED_EXECUTION_TIME 0.01
#define LAXITY_PROPORTIONALITY_CONSTANT 0.10
#define PRICE_CONSTANT_K1 3
#define PRICE_CONSTANT_K2 10000 

using namespace std;

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

int main(){
    freopen("task_data.txt", "w", stdout);  
    int i;
    // cout<<"OUTLIER TASK ="<<OUTLIER_TASKS<<"\n";
    default_random_engine generator;
    generator.seed((unsigned) time(NULL));
    exponential_distribution<double> exp_distribution(EXPECTED_EXECUTION_TIME);
    poisson_distribution<int> poisson_distribution(ARRIVAL_RATE_OF_TASKS);

    srand((unsigned) time(NULL));

    Task tasks[NUM_OF_TASKS];

    for(i=0;i<NUM_OF_TASKS-OUTLIER_TASKS;i++){
        int arrival_time = poisson_distribution(generator);
        // float probab = (float(rand())/float((RAND_MAX)));
        int execution_time, deadline, price;
        execution_time = max((int)exp_distribution(generator),1);
        int laxity = (double)LAXITY_PROPORTIONALITY_CONSTANT*execution_time;
        // if(rand()%2){
        //     laxity += rand()%5;
        // }
        // else{
        //     laxity -= rand()%5;
        //     laxity = max(laxity, 0);
        // }
        laxity += rand()%10;
        laxity -= rand()%6;
        laxity = max(laxity,1);
        deadline = arrival_time + execution_time + laxity;
        price = (double)pow(execution_time, 1.5)*PRICE_CONSTANT_K1 + (double)PRICE_CONSTANT_K2/(laxity*laxity);
        
        // cout<<i<<" "<<arrival_time<<" "<<execution_time<<" "<<deadline<<" "<<price<<"\n";
        tasks[i] = Task(i, arrival_time, execution_time, deadline, price);
    }

    for(i=0;i<OUTLIER_TASKS;i++){
        int arrival_time = rand()%50;
        // float probab = (float(rand())/float((RAND_MAX)));
        int execution_time, deadline, price;
        execution_time = 500 + rand()%50;
        int laxity = (double)LAXITY_PROPORTIONALITY_CONSTANT*execution_time;
        // if(rand()%2){
        //     laxity += rand()%5;
        // }
        // else{
        //     laxity -= rand()%5;
        //     laxity = max(laxity, 0);
        // }
        laxity -= rand()%10;
        deadline = arrival_time + execution_time + laxity;
        price = (double)pow(execution_time, 1.5)*PRICE_CONSTANT_K1 + (double)PRICE_CONSTANT_K2/(laxity*laxity);
        
        // cout<<i<<" "<<arrival_time<<" "<<execution_time<<" "<<deadline<<" "<<price<<"\n";
        tasks[i+NUM_OF_TASKS-OUTLIER_TASKS] = Task(i, arrival_time, execution_time, deadline, price);
    }

    sort (tasks, tasks+NUM_OF_TASKS, compareTwoTasks);

    long long job_et = 0;

    for(i=0;i<NUM_OF_TASKS;i++)
    {
        cout<<i+1<<" "<<tasks[i].arrival_time<<" "<<tasks[i].execution_time<<" "<<tasks[i].deadline<<" "<<tasks[i].price<<"\n";
        job_et += tasks[i].execution_time;
    }
    // cout<<"job_et="<<job_et<<"\n";
}
