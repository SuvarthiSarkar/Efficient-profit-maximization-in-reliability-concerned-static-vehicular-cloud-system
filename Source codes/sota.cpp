#include <bits/stdc++.h>

using namespace std;

#define RATIO (3)
#define CAR_MTTF (RATIO*10)
#define SYSTEM_MTTR (RATIO*10)
#define CAR_PER_UNIT_TIME_COST 5

class Car;
class Task;

set<Car*> pool_of_cars;

int time_elapsed;
//possible algos = EP, GUS, RV, RPE, EDD, FCFS	
string algo="EP";

class Task{
    public:

    int arrival_time;
    int id;
    int execution_time;
    int deadline;
    int price;
    vector<Car*> executing_cars;
    Car* recruiter;
    int no_of_cars_required;

    Task(int _arrival_time,
         int _id,
         int _execution_time,
         int _deadline,
         int _price,
         int _no_of_cars_required=0){
         arrival_time = _arrival_time;
         id = _id;
         execution_time = _execution_time;
         deadline = _deadline;
         price = _price;
         no_of_cars_required = _no_of_cars_required;;
    }

    int N_calculator(){
        int a = ceil(CAR_MTTF/SYSTEM_MTTR);
        int lower_bound = (double)(2*execution_time/SYSTEM_MTTR)+1;
        no_of_cars_required = 2;
        // // cout<<"task->id="<<id<<" a="<<a<<" right_eq_lower_bound="<<ns_lower_bound<<"\n";

        for(int i=2;i<=50;i++){
            int temp1 = 1;
            for(int j=1;j<=a;j++){
                temp1 = temp1*((i+j));
                temp1 = temp1/(j);
            }
            if(temp1>=(lower_bound)){
                no_of_cars_required = i;
                return i;
            }
        }
        return 2;
    }
};

void on_task_completion_or_failure(Task* task);

class Car{
    public:

    int arrival_time;
    int id;
    int departure_time;
    Task* task;
    bool is_recruiter;
    int task_execution_start_time;
    double initial_work_done;

    Car(int _arrival_time,
        int _id,
        int _departure_time,
        Task* _task=NULL,
        bool _is_recruiter=false,
        double _initial_work_done=0.00,
        int _task_execution_start_time=0){
        arrival_time = _arrival_time;
        id = _id;
        departure_time = _departure_time;
        task = _task;
        is_recruiter = _is_recruiter;
        initial_work_done = _initial_work_done;
        task_execution_start_time = _task_execution_start_time;
    }
    
    void add_in_pool(){
        pool_of_cars.insert(this);
    }

    void car_exit_handler(){
        if(task != NULL) {
            // // // cout<<"exiting car id= "<<id<<" executing_task_id="<<task->id<<"\n";
            vector<Car*> executing_cars = task->executing_cars;
            int car_pos = 0;
            // // // cout<<"sz="<<(task->executing_cars).size()<<"\n";
            for(int i=0; i<(task->executing_cars).size(); i++){
                // // // cout<<"running id="<<task->executing_cars[i]->id<<"\n";
                if(task->executing_cars[i] == this){
                    car_pos=i;  
                    break;  
                }
            }
            auto itr = task->executing_cars.begin() + car_pos;
            task->executing_cars.erase(itr);
            
            vector<Car*> remaining_cars;
            for(auto c:executing_cars){
                if(c->task_execution_start_time <= time_elapsed)    remaining_cars.push_back(c);  
            }

            if(is_recruiter && (remaining_cars).size()>0){
                int remaining_cars_number = remaining_cars.size();
                int index = rand()%remaining_cars_number;
                (remaining_cars[index])->is_recruiter = true;
            }

            // Failure has occured
            if((remaining_cars).size()==0){
                on_task_completion_or_failure(task);
                return;
            }

            if(pool_of_cars.size()==0)    return;
            Car* new_car = *pool_of_cars.begin();
            pool_of_cars.erase(pool_of_cars.begin());
        
            new_car->task = task;
            new_car->task_execution_start_time = time_elapsed + SYSTEM_MTTR;
            new_car->initial_work_done = (initial_work_done) + ((double)((time_elapsed - task_execution_start_time)/(1.0*(task->execution_time))));
            task->executing_cars.push_back(new_car);
            // // // cout<<"new_car_hired yesssssssssssssssssssssssss\n";
            // // // cout<<"sz="<<(task->executing_cars).size()<<"\n";
        }
        pool_of_cars.erase(this);
    }    
};

struct Sorted_Task
{
    int expected_profit;
    double gus;
    int revenue;
    double revenue_per_unit_time;
    int deadline;
    int execution_time;
    int arrival_time;
    int laxity;
    int n;
    Task* task;
    int cost;

    Sorted_Task(int _expected_profit,
                double _gus,
                int _revenue,
                double _revenue_per_unit_time,
                int _deadline,
                int _execution_time,
                int _arrival_time,
                int _laxity,
                int _n,
                Task* _task,
                int _cost){
                expected_profit = _expected_profit;
                gus = _gus;
                revenue = _revenue;
                revenue_per_unit_time = _revenue_per_unit_time;
                deadline = _deadline;
                execution_time = _execution_time;
                arrival_time = _arrival_time;
                laxity = _laxity;
                n = _n;
                task = _task;
                cost = _cost;
    }    
};

inline bool operator<(const Sorted_Task &a, const  Sorted_Task &b)	
{	
    if (algo=="FCFS" && a.arrival_time != b.arrival_time)	
        return a.arrival_time < b.arrival_time;	
    if (a.expected_profit != b.expected_profit)	
        return a.expected_profit > b.expected_profit;	
    	
    if (a.gus != b.gus)	
        return a.gus < b.gus;	
    if (a.revenue != b.revenue)	
        return a.revenue > b.revenue;	
    if (a.revenue_per_unit_time != b.revenue_per_unit_time)	
        return a.revenue_per_unit_time > b.revenue_per_unit_time;	
    	
    if (algo=="EDD" && a.deadline != b.deadline)	
        return a.deadline < b.deadline;	
    return (a.arrival_time < b.arrival_time);	
}

set<Sorted_Task> sorted_tasks; 
set<Sorted_Task> active_tasks;

void assign_cars(Task* task, int n){
    task->executing_cars.clear();    
    // // // cout<<"task->id="<<task->id<<" ns="<<ns<<"\n";
    for(int i=0; i<n; i++){
        // // // cout<<"size="<<pool_of_small_cars.size()<<"\n";
        Car* assigned_car =  *pool_of_cars.begin();
        // // // cout<<"assigned_car id="<<assigned_car->id<<"\n";
        // // // cout<<"task_id="<<task->id<<"\n";
        assigned_car->task = task;
        assigned_car->task_execution_start_time = time_elapsed;
        task->executing_cars.push_back(assigned_car);
        pool_of_cars.erase(pool_of_cars.begin());
    }

}

void on_task_completion_or_failure(Task* task){
    for(int i=0; i<task->executing_cars.size(); i++){
        Car* free_car = task->executing_cars[i];
        free_car->task = NULL;
        free_car->is_recruiter = false;
        free_car->task_execution_start_time = 0;
        free_car->initial_work_done = 0.0;
        // // // cout<<"free_car_id="<<free_car->id<<"\n";
        
        pool_of_cars.insert(free_car);
    }
    task->executing_cars.clear();
}

unordered_map<int, vector<Car*>> car_departure_time_map;

//verified
void add_car_to_pool(int id, int arrival_time, int departure_time){
    Car* new_car = new Car(arrival_time, id, departure_time);
    if(car_departure_time_map.find(departure_time)==car_departure_time_map.end()){
        car_departure_time_map[departure_time] = {};    
    }
    car_departure_time_map[departure_time].push_back(new_car);
    new_car->add_in_pool();
} 

vector<int> cost_spent_to_complete_task(Task* task){
    int cost = 0, n=0;
    task->executing_cars.clear();
    n = task->N_calculator();
    // cout<<"task->id="<<task->id<<" nh="<<nh<<" ns="<<ns<<" nm="<<nm<<"\n";
    // ns = task->Ns_calculator();
    cost = CAR_PER_UNIT_TIME_COST*n*task->execution_time;        
    return {cost,n};
}

Sorted_Task create_sorted_task(Task* task){
    // If algo uses expected price
    int expected_profit=0, revenue=0, deadline=0, execution_time=0, arrival_time=0, laxity=task->deadline - task->arrival_time - task->execution_time;
    vector<int> cost_info = cost_spent_to_complete_task(task);
    double revenue_per_unit_time=0.0, gus=0.0;
    int n = cost_info[1];

    execution_time = task->execution_time;
    arrival_time = task->arrival_time;  
    
    deadline = task->deadline;
    if(algo == "EDD")   return Sorted_Task(expected_profit, gus, revenue, revenue_per_unit_time, deadline, execution_time, arrival_time, laxity, n, task, cost_info[0]);

    revenue_per_unit_time = ((task->price)*1.0)/(task->execution_time);
    if(algo == "RPE")   return Sorted_Task(expected_profit, gus, revenue, revenue_per_unit_time, deadline, execution_time, arrival_time, laxity, n, task, cost_info[0]);

    revenue = task->price;
    if(algo == "RV")    return Sorted_Task(expected_profit, gus, revenue, revenue_per_unit_time, deadline, execution_time, arrival_time, laxity, n, task, cost_info[0]);

    gus = (laxity*1.0)/execution_time;
    if(algo == "GUS")   return Sorted_Task(expected_profit, gus, revenue, revenue_per_unit_time, deadline, execution_time, arrival_time, laxity, n, task, cost_info[0]);

    expected_profit = task->price - cost_info[0];
    return Sorted_Task(expected_profit, gus, revenue, revenue_per_unit_time, deadline, execution_time, arrival_time, laxity, n, task, cost_info[0]);
}

int main(){
    freopen("output.txt", "w", stdout); 
    cout<<"sota algo\n";
    cout<<"GA_weekend\n";
    cout<<"trace data="<<8<<"\n";
    int nss=0;
    long long total_price = 0;
    long long profit = 0;
    int task_satisfied = 0;
    int task_accepted = 0;

    srand((unsigned) time(NULL));    // for selecting recruiter randomly
    ifstream car_data_file, task_data_file; 
    car_data_file.open("grand_arcade_weekend.txt");
    task_data_file.open("trace_data_8.txt");

    string _car_id, _car_arrival_time, _car_departure_time, _car_type;
    string _task_id, _task_arrival_time, _task_execution_time, _task_deadline, _task_price;
    int car_id=-1, car_arrival_time, car_departure_time;
    int task_id=-1, task_arrival_time, task_execution_time, task_deadline, task_price;
    char car_type;

    // TODO: divide time into slots
    for(;time_elapsed<=100000; ++time_elapsed){
        // handles the car entry to shopping

        // cout<<"TIME="<<time_elapsed<<"\n";

        while(car_data_file.good()){
            if(car_id!=-1){
                if(car_arrival_time > time_elapsed) break;
                add_car_to_pool(car_id, car_arrival_time, car_departure_time);
                car_id = -1;
            }

            car_data_file>>_car_id>>_car_arrival_time>>_car_departure_time>>_car_type;
            car_id = stoi(_car_id);
            car_arrival_time = stoi(_car_arrival_time);
            car_departure_time = stoi(_car_departure_time);
            car_type  = _car_type[0];

            if(car_arrival_time > time_elapsed) break;

            add_car_to_pool(car_id, car_arrival_time, car_departure_time);
            car_id=-1;            
        }    

        while(task_data_file.good()){
            if(task_id!=-1){
                if(task_arrival_time > time_elapsed) break;
                Task* new_task = new Task(task_arrival_time, task_id ,task_execution_time, task_deadline, task_price);
                task_id = -1;
                Sorted_Task t = create_sorted_task(new_task);
                sorted_tasks.insert(t);
            // // cout<<"task.is_critical="<<t.is_critical<<" task.nl="<<t.nl<<" task.nm="<<t.nm<<" task.ns="<<t.ns<<"\n";
                nss+= t.n;
            }

            task_data_file>>_task_id>>_task_arrival_time>>_task_execution_time>>_task_deadline>> _task_price;
            task_id = stoi(_task_id);
            task_arrival_time = stoi(_task_arrival_time);
            task_execution_time = stoi(_task_execution_time);
            task_deadline = stoi(_task_deadline);
            task_price = stoi(_task_price);
            total_price += task_price;

            if(task_arrival_time > time_elapsed) break;
            Task* new_task = new Task(task_arrival_time, task_id ,task_execution_time, task_deadline, task_price);
            task_id = -1;
            Sorted_Task t = create_sorted_task(new_task);            
            sorted_tasks.insert(t);
            nss+= t.n;
            // // // cout<<"task.is_critical="<<t.is_critical<<" task.nl="<<t.nl<<" task.nm="<<t.nm<<" task.ns="<<t.ns<<"\n";
        }
        
        //Handling Car exit
        for(auto car:car_departure_time_map[time_elapsed]){ 
            car->car_exit_handler();
        }

        // cout<<"\npool of new sorted tasks=\n";	
        // for(auto task:sorted_tasks)  cout<<"task id="<<task.task->id<<" task.ep="<<task.expected_profit<<" task.gus="<<task.gus<<" task.rv="<<task.revenue<<" task.rpe="<<task.revenue_per_unit_time<<" task.deadline="<<task.deadline<<" task.ei="<<task.execution_time<<" task.ai="<<task.arrival_time<<" task.cost="<<task.cost<<"\n";
        // TODO: GUS wale case mai kya laxity humesha change karna?

        // // // cout<<"pool of sorted tasks=\n";
        // for(auto task:sorted_tasks) // // cout<<(task.task)->id<<"\n";

        set<Sorted_Task> new_sorted_tasks = sorted_tasks; 
        for(auto task:sorted_tasks){

            // Removing task if it cannot be completed within deadline
            if((task.deadline-task.execution_time+1)<=time_elapsed){
                new_sorted_tasks.erase(new_sorted_tasks.find(task));
                continue;    
            }
            int n = task.n;
            if(pool_of_cars.size()<n)    continue;
            if((task.task->price - task.cost)<0) continue;
            task_accepted++;
            assign_cars(task.task, n);
            active_tasks.insert(task);
            new_sorted_tasks.erase(new_sorted_tasks.find(task));
            // // // cout<<"task.is_critical="<<task.is_critical<<" task.nl="<<task.nl<<" task.nm="<<task.nm<<" task.ns="<<task.ns<<"\n";
        }
        sorted_tasks = new_sorted_tasks;

        

        // // // cout<<"pool of active tasks=\n";
        // for(auto task:active_tasks) // // cout<<task->id<<"\n";

        //Removing task if its completed
        set<Sorted_Task> new_active_tasks = active_tasks;
        for(auto task:active_tasks) {
            bool work_completed = false;
            for(auto executing_car : (task.task)->executing_cars){
                double work_done = (double)((time_elapsed - executing_car->task_execution_start_time)/(1.0*((task.task)->execution_time)));
                work_done += executing_car->initial_work_done;   
                if(work_done>=1.0){
                    work_completed = true;  
                    break; 
                }
            }
            if(work_completed && (task.task)->deadline>=time_elapsed){
                profit += (long long)(task.task->price - task.cost);
                task_satisfied++;
                on_task_completion_or_failure(task.task);
                new_active_tasks.erase(task);
            }
        }
        active_tasks = new_active_tasks;
    }          
    cout<<"total_price="<<total_price<<"\n";
    cout<<"task_satisfied="<<task_satisfied<<"\n";
    cout<<"task_accepted="<<task_accepted<<"\n";
    cout<<"profit="<<profit<<"\n";
    cout<<"profit %="<<((double)(profit*100.0)/total_price)<<"\n";
}