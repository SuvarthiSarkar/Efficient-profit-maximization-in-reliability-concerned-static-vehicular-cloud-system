#include <bits/stdc++.h>
using namespace std;

#define RATIO (1)
#define SMALL_CAR_MTTF (RATIO*10)
#define MEDIUM_CAR_MTTF (RATIO*5*10)
#define LARGE_CAR_MTTF (RATIO*25*10)
#define SYSTEM_MTTR (RATIO*10) // TODO: shouldnt it be only 10
#define CRITICAL_TASK_EXECUTION_TIME_THRESHOLD (500)
#define CRITICAL_TASK_LAXITY_PERCENAGE_THRESHOLD (10)
#define SMALL_CAR_PER_UNIT_TIME_COST (5)
#define MEDIUM_CAR_PER_UNIT_TIME_COST (5)
#define LARGE_CAR_PER_UNIT_TIME_COST (5)
#define FIXED_PACKET_COUNT 5
#define FIXED_PACKET_LENGTH 45
#define PENALTY_CONSTANT 1
#define DEADLINE_INCREMENT_FACTOR 10

class Car;
class Task;

set<Car*> pool_of_small_cars;
set<Car*> pool_of_medium_cars;
set<Car*> pool_of_large_cars;

int total_large_cars = 0;
int time_elapsed;
//possible algos = EP, GUS, RV, RPE, EDD, FCFS
string algo="EP";
string packet_split_algo = "LENGTH";
//possible = LENGTH, COUNT, HYBRID, NONE
int nss=0;
int sum_li = 0;
int sum_ei = 0;

int total_active_hours_of_small_cars = 0;
int total_active_hours_of_medium_cars = 0;
int total_active_hours_of_large_cars = 0;

int total_stay_hours_of_small_cars = 0;
int total_stay_hours_of_medium_cars = 0;
int total_stay_hours_of_large_cars = 0;


class Task{
    public:

    int packet_id;
    int packet_count;
    int arrival_time;
    int id;
    int execution_time;
    int deadline;
    int price;
    vector<Car*> executing_cars;
    Car* recruiter;
    int no_of_small_cars_required;
    int no_of_medium_cars_required;
    int no_of_large_cars_required;
    bool is_critical;

    Task(int _arrival_time,
         int _id,
         int _execution_time,
         int _deadline,
         int _price,
         int _packet_id = 0,
         int _packet_count = 1){
         arrival_time = _arrival_time;
         id = _id;
         execution_time = _execution_time;
         deadline = _deadline;
         price = _price;
         no_of_small_cars_required = 0;
         no_of_medium_cars_required = 0;
         no_of_large_cars_required = 0;
         is_critical = false;
         packet_id = _packet_id;
         packet_count = _packet_count;
    }

    int Ns_calculator(){
        int a = ceil(SMALL_CAR_MTTF*1.0/SYSTEM_MTTR);
        int ns_lower_bound = (double)(2*execution_time/SYSTEM_MTTR)+1;        
        // // cout<<"task->id="<<id<<" a="<<a<<" right_eq_lower_bound="<<ns_lower_bound<<"\n";
        no_of_small_cars_required = 2;
        for(int i=2;i<=50;i++){
            int temp1 = 1;
            for(int j=1;j<=a;j++){
                temp1 = temp1*((i+j));
                temp1 = temp1/(j);
            }
            if(temp1>=(ns_lower_bound)){
                no_of_small_cars_required = i;
                return i;
            }
        }
        return 2;
    }
    
    int Nm_calculator(){
        int i, a = ceil(MEDIUM_CAR_MTTF*1.0/SYSTEM_MTTR);
        int nm_lower_bound = (double)(2*execution_time/SYSTEM_MTTR)+1;
        no_of_medium_cars_required = 2;
        for(i=2;i<=20;i++){
            int temp1 = 1;
            for(int j=1;j<=a;j++){
                temp1 = temp1*((i+j));
                temp1 = temp1/(j);
            }
            if(temp1>=(nm_lower_bound))   break;
        }
        no_of_medium_cars_required = i;
        return i;
    }

    int Nl_calculator(){
        int i, a = ceil(LARGE_CAR_MTTF*1.0/SYSTEM_MTTR);
        int nl_lower_bound = (double)(2*execution_time/SYSTEM_MTTR)+1;
        no_of_large_cars_required = 1;
        for(i=1;i<=5;i++){
            int temp1 = 1;
            for(int j=1;j<=a;j++){
                temp1 = temp1*((i+j));
                temp1 = temp1/(j);
            }
            if(temp1>=(nl_lower_bound))   break;
        }
        no_of_large_cars_required = i;
        return i;
    }

    int N_hybrid_calculator(int hybrid_car_mttf){
        // cout<<"hybrid_car_mttf="<<hybrid_car_mttf<<" SYSTEM_MTTR="<<SYSTEM_MTTR<<"\n";
        int i, a = ceil(hybrid_car_mttf*1.0/SYSTEM_MTTR);
        int x = (hybrid_car_mttf/SYSTEM_MTTR);
        // cout<<"a just neeche wala="<<a<<" x="<<x<<"\n";
        int nh_lower_bound = (double)(2*execution_time/SYSTEM_MTTR)+1;
        // // cout<<"task->id="<<id<<" a="<<a<<" right_eq_lower_bound="<<nh_lower_bound<<"\n";

        for(i=2;i<=50;i++){
            int temp1 = 1;
            for(int j=1;j<=a;j++){
                temp1 = temp1*((i+j));
                temp1 = temp1/(j);
            }
            if(temp1>=(nh_lower_bound))   break;
        }
        // cout<<"hybrid_car_mttf="<<hybrid_car_mttf<<" a="<<a<<" nh_lower_bound="<<nh_lower_bound<<"  N hybrid="<<i<<"\n";
        return i;
    }

    bool check_if_critical(){
        bool is_critical_task = true;   
        
        if(execution_time > CRITICAL_TASK_EXECUTION_TIME_THRESHOLD){
            is_critical_task = false;   
        }

        int laxity_threshold = (CRITICAL_TASK_LAXITY_PERCENAGE_THRESHOLD*execution_time)/100;
        int task_laxity = deadline - (execution_time + time_elapsed);
        if(task_laxity > laxity_threshold){
            is_critical_task = false;   
        }
        is_critical = is_critical_task;
        return is_critical_task;
    }
};

void on_task_completion_or_failure(Task* task);

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
    int nl;
    int nm;
    int ns;
    int is_critical;
    Task* task;
    int cost;
    int packet_id;
    int packet_count;

    Sorted_Task(int _expected_profit,
                double _gus,
                int _revenue,
                double _revenue_per_unit_time,
                int _deadline,
                int _execution_time,
                int _arrival_time,
                int _laxity,
                int _nl,
                int _nm,
                int _ns,
                int _is_critical,
                Task* _task,
                int _cost,
                int _packet_id=0,
                int _packet_count=1){
                expected_profit = _expected_profit;
                gus = _gus;
                revenue = _revenue;
                revenue_per_unit_time = _revenue_per_unit_time;
                deadline = _deadline;
                execution_time = _execution_time;
                arrival_time = _arrival_time;
                laxity = _laxity;
                nl = _nl;
                nm = _nm;
                ns = _ns;
                is_critical = _is_critical;
                task = _task;
                cost = _cost;
                packet_id = _packet_id;
                packet_count = _packet_count;
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

    if(a.arrival_time != b.arrival_time)
        return (a.arrival_time < b.arrival_time);
    else
        return a.packet_id < b.packet_id;
}

vector<int> cost_spent_to_complete_task(Task* task){
    bool is_critical_task = task->check_if_critical();
    int cost = 0, nl=0, nm=0, ns=0, t=0;
    task->executing_cars.clear();
    if(is_critical_task){
        t=1;
        nl = task->Nl_calculator();
        cost = LARGE_CAR_PER_UNIT_TIME_COST*nl*task->execution_time;  
        // cout<<"exe critical t="<<task->execution_time<<" nl="<<nl<<"\n";
    }
    else{                   // manages hybrid
        t=0;
        int large_car_lower_limit = total_large_cars/10;
        if(pool_of_large_cars.size()>=10 && pool_of_large_cars.size()>large_car_lower_limit){
            t=1;
            nl = task->Nl_calculator();
            cost = LARGE_CAR_PER_UNIT_TIME_COST*nl*task->execution_time;  
            // cout<<"exe critical t="<<task->execution_time<<" nl="<<nl<<"\n";
        }
        else{
            int hybrid_mttf = (double)(((0.8*SMALL_CAR_MTTF) + (0.15*MEDIUM_CAR_MTTF))/0.95);
            int nh = task->N_hybrid_calculator(hybrid_mttf);
            ns = (double)((0.80*nh)/0.95);
            nm = (double)((0.15*nh)/0.95);
            ns = max(ns,1);
            nm = max(nm,1); 

            if(pool_of_medium_cars.size()<nm){
                int no_of_small_cars_required = (nm-(int)pool_of_medium_cars.size())*2;
                nm = (int)pool_of_medium_cars.size();
                ns+=no_of_small_cars_required;
            }

            task->no_of_small_cars_required = ns;
            task->no_of_medium_cars_required = nm;
            // cout<<"task->id="<<task->id<<" nh="<<nh<<" ns="<<ns<<" nm="<<nm<<"\n";
            // ns = task->Ns_calculator();
            cost = (MEDIUM_CAR_PER_UNIT_TIME_COST*nm + SMALL_CAR_PER_UNIT_TIME_COST*ns)*task->execution_time;        
            // cout<<"exe t="<<task->execution_time<<" ns="<<ns<<" nm="<<nm<<"\n";
        }
    }
    return {cost,nl,nm,ns,t};
}

Sorted_Task create_sorted_task(Task* task){
    
    int expected_profit=0, revenue=0, deadline=0, execution_time=0, arrival_time=0, laxity=task->deadline - task->arrival_time - task->execution_time;
    vector<int> cost_info = cost_spent_to_complete_task(task);
    double revenue_per_unit_time=0.0, gus=0.0;
    int nl = cost_info[1], nm = cost_info[2], ns = cost_info[3], t = cost_info[4];
    
    arrival_time = task->arrival_time;
    execution_time = task->execution_time;
    deadline = task->deadline;
    if(algo == "EDD")   return Sorted_Task(expected_profit, gus, revenue, revenue_per_unit_time, deadline, execution_time, arrival_time, laxity, nl, nm, ns, t, task, cost_info[0], task->packet_id, task->packet_count);

    revenue_per_unit_time = ((task->price)*1.0)/(task->execution_time);
    if(algo == "RPE")   return Sorted_Task(expected_profit, gus, revenue, revenue_per_unit_time, deadline, execution_time, arrival_time, laxity, nl, nm, ns, t, task, cost_info[0],  task->packet_id, task->packet_count);

    revenue = task->price;
    if(algo == "RV")    return Sorted_Task(expected_profit, gus, revenue, revenue_per_unit_time, deadline, execution_time, arrival_time, laxity, nl, nm, ns, t, task, cost_info[0],  task->packet_id, task->packet_count);

    gus = (laxity*1.0)/execution_time;
    if(algo == "GUS")   return Sorted_Task(expected_profit, gus, revenue, revenue_per_unit_time, deadline, execution_time, arrival_time, laxity, nl, nm, ns, t, task, cost_info[0],  task->packet_id, task->packet_count);

    expected_profit = task->price - cost_info[0];
    return Sorted_Task(expected_profit, gus, revenue, revenue_per_unit_time, deadline, execution_time, arrival_time, laxity, nl, nm, ns, t, task, cost_info[0],  task->packet_id, task->packet_count);
}

set<Sorted_Task> sorted_tasks; 
set<Sorted_Task> active_tasks;

void create_new_task(int task_arrival_time, int task_id ,int task_execution_time, int task_deadline, int task_price){
    if(packet_split_algo == "NONE"){
        Task* new_task = new Task(task_arrival_time, task_id ,task_execution_time, task_deadline, task_price);
        Sorted_Task t = create_sorted_task(new_task);
        sorted_tasks.insert(t);
        sum_li += t.laxity;
        sum_ei += t.execution_time;
        // cout<<"task.id="<<t.task->id<<" task.rv="<<t.revenue<<" task.ep="<<t.expected_profit<<"\n";
        nss+= t.ns + t.nm + t.nl;
    }    
    if(packet_split_algo == "LENGTH"){
        int packet_count = task_execution_time/FIXED_PACKET_LENGTH;
        if(task_execution_time%FIXED_PACKET_LENGTH!=0){
            packet_count++;
        }
        // cout<<"task id "<<task_id<<" task packet count "<< packet_count<<endl;
        for(int i=0;i<packet_count;i++){
            if(i == packet_count-1){
                Task* new_task = new Task(task_arrival_time, task_id ,task_execution_time - (FIXED_PACKET_LENGTH*(packet_count-1)), task_deadline, task_price, i, packet_count);
                Sorted_Task t = create_sorted_task(new_task);
                sum_li += t.laxity;
                sum_ei += t.execution_time;
                // cout<<"task.id="<<t.task->id<<" task.rv="<<t.revenue<<" task.ep="<<t.expected_profit<<"\n";
                nss+= t.ns + t.nm + t.nl;
                sorted_tasks.insert(t);
            }
            else{
                Task* new_task = new Task(task_arrival_time, task_id , FIXED_PACKET_LENGTH, task_deadline, task_price, i, packet_count);
                Sorted_Task t = create_sorted_task(new_task);
                sorted_tasks.insert(t);
            }
        }
    }
    if(packet_split_algo == "COUNT"){
        int packet_length = ceil(task_execution_time*1.0/FIXED_PACKET_COUNT);
        cout<<"packet_length="<<packet_length;
        int total_time_covered = 0; 
        for(int i=0; i<FIXED_PACKET_COUNT; i++){
            if(i == FIXED_PACKET_COUNT-1){   
                Task* new_task = new Task(task_arrival_time, task_id ,task_execution_time - (packet_length*(FIXED_PACKET_COUNT-1)), task_deadline, task_price, i, FIXED_PACKET_COUNT);
                Sorted_Task t = create_sorted_task(new_task);
                sum_li += t.laxity;
                sum_ei += t.execution_time;
                // cout<<"task.id="<<t.task->id<<" task.rv="<<t.revenue<<" task.ep="<<t.expected_profit<<"\n";
                nss+= t.ns + t.nm + t.nl;
                sorted_tasks.insert(t);
            }
            else{                            
                Task* new_task = new Task(task_arrival_time, task_id ,packet_length, task_deadline, task_price, i, FIXED_PACKET_COUNT);
                Sorted_Task t = create_sorted_task(new_task);
                sorted_tasks.insert(t);
            }
        }
    }
    if(packet_split_algo == "HYBRID"){

    }
}


class Car{
    public:

    int arrival_time;
    int id;
    int departure_time;
    char type;
    Task* task;
    bool is_recruiter;
    int task_execution_start_time;
    double initial_work_done;

    Car(int _arrival_time,
        int _id,
        int _departure_time,
        char _type,
        Task* _task=NULL,
        bool _is_recruiter=false,
        double _initial_work_done=0.00,
        int _task_execution_start_time=0){
        arrival_time = _arrival_time;
        id = _id;
        departure_time = _departure_time;
        type = _type;
        task = _task;
        is_recruiter = _is_recruiter;
        initial_work_done = _initial_work_done;
        task_execution_start_time = _task_execution_start_time;
    }
    
    void add_in_pool(){
        if(type=='S')   pool_of_small_cars.insert(this);
        if(type=='M')   pool_of_medium_cars.insert(this);
        if(type=='L')   {
            pool_of_large_cars.insert(this);
            total_large_cars++;
        }
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
                // TODO: Change of arrival time
                on_task_completion_or_failure(task);
                Sorted_Task t = create_sorted_task(task);
                sorted_tasks.insert(t);
                return;
            }

            double max_work_done = 0.0;
            int task_execution_time = task->execution_time;
            for(auto c:executing_cars){
                double work_done = 0.0;
                if(c->task_execution_start_time <= time_elapsed)  
                    work_done = (double)((time_elapsed - c->task_execution_start_time)/(1.0*(task_execution_time)));
                work_done += c->initial_work_done;
                max_work_done = max(max_work_done, work_done);
            }
            // // // cout<<"max_work_done_here="<<max_work_done<<"\n";

            Car* new_car;
            if(type=='S')   {
                if(pool_of_small_cars.size()==0)    return;
                new_car = *pool_of_small_cars.begin();
                pool_of_small_cars.erase(pool_of_small_cars.begin());
            }
            if(type=='M')   {
                if(pool_of_medium_cars.size()==0)    return;
                new_car = *pool_of_medium_cars.begin();
                pool_of_medium_cars.erase(pool_of_medium_cars.begin());
            }
            if(type=='L')   {
                if(pool_of_large_cars.size()==0)    return;
                new_car = *pool_of_large_cars.begin();
                pool_of_large_cars.erase(pool_of_large_cars.begin());
            }
            new_car->task = task;
            new_car->task_execution_start_time = time_elapsed + SYSTEM_MTTR;
            new_car->initial_work_done = max_work_done;
            task->executing_cars.push_back(new_car);
            // // // cout<<"new_car_hired yesssssssssssssssssssssssss\n";
            // // // cout<<"sz="<<(task->executing_cars).size()<<"\n";
        }
        if(type=='S')   pool_of_small_cars.erase(this);
        if(type=='M')   pool_of_medium_cars.erase(this);
        if(type=='L')   {
            pool_of_large_cars.erase(this);
            total_large_cars--;
        }
    }    
};

void assign_cars(Task* task, int is_critical_task, int nl, int nm, int ns){
    task->executing_cars.clear();
    if(is_critical_task){
        for(int i=0;i<nl;i++){
            Car* assigned_car =  *pool_of_large_cars.begin();
            assigned_car->task = task;
            assigned_car->task_execution_start_time = time_elapsed;
            task->executing_cars.push_back(assigned_car);
            pool_of_large_cars.erase(pool_of_large_cars.begin());
        }
    }
    else{                   // manages hybrid
        // // // cout<<"task->id="<<task->id<<" ns="<<ns<<"\n";
        for(int i=0; i<ns; i++){
            // // // cout<<"size="<<pool_of_small_cars.size()<<"\n";
            Car* assigned_car =  *pool_of_small_cars.begin();
            // // // cout<<"assigned_car id="<<assigned_car->id<<"\n";
            // // // cout<<"task_id="<<task->id<<"\n";	
            assigned_car->task = task;	
            assigned_car->task_execution_start_time = time_elapsed;	
            task->executing_cars.push_back(assigned_car);
            pool_of_small_cars.erase(pool_of_small_cars.begin());
        }

        for(int i=0; i<nm; i++){
            // // // cout<<"size="<<pool_of_small_cars.size()<<"\n";
            Car* assigned_car =  *pool_of_medium_cars.begin();
            // // // cout<<"assigned_car id="<<assigned_car->id<<"\n";
            // // // cout<<"task_id="<<task->id<<"\n";
            assigned_car->task = task;
            assigned_car->task_execution_start_time = time_elapsed;
            task->executing_cars.push_back(assigned_car);
            pool_of_medium_cars.erase(pool_of_medium_cars.begin());
        }
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
        
        if(free_car->type == 'S')   pool_of_small_cars.insert(free_car);
        if(free_car->type == 'M')   pool_of_medium_cars.insert(free_car);
        if(free_car->type == 'L')   pool_of_large_cars.insert(free_car);
        
        if(free_car->type == 'S')   total_active_hours_of_small_cars += time_elapsed - free_car->task_execution_start_time;
        if(free_car->type == 'M')   total_active_hours_of_medium_cars += time_elapsed - free_car->task_execution_start_time;
        if(free_car->type == 'L')   total_active_hours_of_large_cars += time_elapsed - free_car->task_execution_start_time;

    }
    task->executing_cars.clear();
}

unordered_map<int, vector<Car*>> car_departure_time_map;

//verified
void add_car_to_pool(int id, int arrival_time, int departure_time, char type){
    Car* new_car = new Car(arrival_time, id, departure_time, type);
    if(car_departure_time_map.find(departure_time)==car_departure_time_map.end()){
        car_departure_time_map[departure_time] = {};    
    }
    car_departure_time_map[departure_time].push_back(new_car);
    new_car->add_in_pool();
} 

int main(){
    freopen("output.txt", "w", stdout); 
    cout<<"trace_data="<<8<<"\n";
    cout<<"GA weekend";
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
                add_car_to_pool(car_id, car_arrival_time, car_departure_time, car_type);
                if(car_type == 'S') total_stay_hours_of_small_cars += car_departure_time - car_arrival_time;
                if(car_type == 'M') total_stay_hours_of_medium_cars += car_departure_time - car_arrival_time;
                if(car_type == 'L') total_stay_hours_of_large_cars += car_departure_time - car_arrival_time;

                car_id = -1;
            }

            car_data_file>>_car_id>>_car_arrival_time>>_car_departure_time>>_car_type;
            car_id = stoi(_car_id);
            car_arrival_time = stoi(_car_arrival_time);
            car_departure_time = stoi(_car_departure_time);
            // car_type  = _car_type[0];
            car_type = (_car_type == "0")?'S':((_car_type == "1")?'M':'L');

            if(car_arrival_time > time_elapsed) break;

            add_car_to_pool(car_id, car_arrival_time, car_departure_time, car_type);
            if(car_type == 'S') total_stay_hours_of_small_cars += car_departure_time - car_arrival_time;
            if(car_type == 'M') total_stay_hours_of_medium_cars += car_departure_time - car_arrival_time;
            if(car_type == 'L') total_stay_hours_of_large_cars += car_departure_time - car_arrival_time;
            car_id=-1;            
        }    

        while(task_data_file.good()){
            if(task_id!=-1){
                if(task_arrival_time > time_elapsed) break;
                create_new_task(task_arrival_time, task_id ,task_execution_time, task_deadline, task_price);
                task_id = -1;
            }

            task_data_file>>_task_id>>_task_arrival_time>>_task_execution_time>>_task_deadline>> _task_price;
            task_id = stoi(_task_id);
            task_arrival_time = stoi(_task_arrival_time);
            task_execution_time = stoi(_task_execution_time);
            task_deadline = stoi(_task_deadline);
            task_price = stoi(_task_price);
            total_price += task_price;

            if(task_arrival_time > time_elapsed) break;
            create_new_task(task_arrival_time, task_id ,task_execution_time, task_deadline, task_price);
            task_id = -1;

            // // // cout<<"task.is_critical="<<t.is_critical<<" task.nl="<<t.nl<<" task.nm="<<t.nm<<" task.ns="<<t.ns<<"\n";
        }
        
        //Handling Car exit
        for(auto car:car_departure_time_map[time_elapsed]){ 
            if(car->type == 'S')   total_active_hours_of_small_cars += time_elapsed - car->task_execution_start_time;
            if(car->type == 'M')   total_active_hours_of_medium_cars += time_elapsed - car->task_execution_start_time;
            if(car->type == 'L')   total_active_hours_of_large_cars += time_elapsed - car->task_execution_start_time;
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
            if((task.deadline)<=time_elapsed){
                auto itr = new_sorted_tasks.find(task);
                new_sorted_tasks.erase(itr);
                Sorted_Task t = *itr;
                Task *actual_task = t.task;
                actual_task->price = actual_task->price - PENALTY_CONSTANT*(ceil((actual_task->execution_time*DEADLINE_INCREMENT_FACTOR*1.0)/100));
                actual_task->deadline = actual_task->deadline + (ceil((actual_task->execution_time*DEADLINE_INCREMENT_FACTOR*1.0)/100));
                create_new_task(actual_task->arrival_time, actual_task->id ,actual_task->execution_time, actual_task->deadline, actual_task->price);
                continue;    
            }
            int is_critical, nl, nm, ns;
            is_critical = task.is_critical, nl = task.nl, nm = task.nm, ns = task.ns;
            if(is_critical){ 
                if(pool_of_large_cars.size()<nl)    continue;
            }else{
                if(pool_of_small_cars.size()<ns || pool_of_medium_cars.size()<nm)    continue;
            }
            if((task.task->price - task.cost)<0) continue;
            task_accepted++;
            assign_cars(task.task, is_critical, nl, nm, ns);
            active_tasks.insert(task);
            new_sorted_tasks.erase(new_sorted_tasks.find(task));
            // // // cout<<"task.is_critical="<<task.is_critical<<" task.nl="<<task.nl<<" task.nm="<<task.nm<<" task.ns="<<task.ns<<"\n";
        }
        sorted_tasks = new_sorted_tasks;

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
                if((task.task)->packet_id == ((task.task)->packet_count)-1){
                    profit += (long long)((task.task->price) - (task.cost));
                    task_satisfied++;
                }
                on_task_completion_or_failure((task.task));
                new_active_tasks.erase(task);
            }
            else if(work_completed && (task.task)->deadline<time_elapsed){
                if((task.task)->packet_id == ((task.task)->packet_count)-1){
                    long long normal_profit = ((long long)((task.task->price) - (task.cost)));
                    long long penalized_profit = normal_profit - PENALTY_CONSTANT*(time_elapsed-(task.task)->deadline);
                    profit += max((long long)0,penalized_profit);
                    task_satisfied++;
                }
                on_task_completion_or_failure((task.task));
                new_active_tasks.erase(task);
            }
        }
        active_tasks = new_active_tasks;
    }          
    cout<<"\nour total_price="<<total_price<<"\n";
    cout<<"task_satisfied="<<task_satisfied<<"\n";
    cout<<"task_accepted="<<task_accepted<<"\n";
    cout<<"profit="<<profit<<"\n";
    cout<<"profit %="<<((double)(profit*100.0)/total_price)<<"\n\n";
    cout<<"small car active hours %="<<((double)(total_active_hours_of_small_cars*1.0)/total_stay_hours_of_small_cars)<<"\n";
    cout<<"medium car active hours %="<<((double)(total_active_hours_of_medium_cars*1.0)/total_stay_hours_of_medium_cars)<<"\n";
    cout<<"large car active hours %="<<((double)(total_active_hours_of_large_cars*1.0)/total_stay_hours_of_large_cars)<<"\n\n";
}
