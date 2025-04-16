#include<bits/stdc++.h>
#define RATIO 5
#define NUM_OF_CARS 4000
#define ARRIVAL_RATE_OF_CARS 60
using namespace std;

class Car{
    public:
        int id;
        int arrival_time;
        int departure_time;
        char type;

        Car(){
            id = 0;
            arrival_time = 0;
            departure_time = 0;
            type = 'S';
        }

        Car(int _id,
                int _arrival_time,
                int _departure_time,
                char _type){
                id = _id;
                arrival_time = _arrival_time;
                departure_time = _departure_time;
                type = _type;
        }
};

bool compareTwoCars(Car &a, Car &b)
{
    return a.arrival_time < b.arrival_time;
}

int main(){
    freopen("car_data.txt", "w", stdout);  
    int i;

    default_random_engine generator;
    generator.seed((unsigned) time(NULL));
    poisson_distribution<int> distribution(ARRIVAL_RATE_OF_CARS);

    srand((unsigned) time(NULL));

    Car cars[NUM_OF_CARS];

    for(i=0;i<NUM_OF_CARS;i++){
        int arrival_time = distribution(generator);
        float probab = (float(rand())/float((RAND_MAX)));
        int residency_time, departure_time;
        char ch;
        if(probab<=0.80){
            ch = '0';
            residency_time = rand()%(RATIO*2) + (RATIO*9); 
        }else if(probab>0.80 && probab<=0.95){
            ch = '1';
            residency_time = rand()%(RATIO*3*2) + (RATIO*3*9); 
        }
        else{
            ch = '2';
            residency_time = rand()%(RATIO*10*2) + (RATIO*9*10); 
        }
        departure_time = arrival_time + residency_time;

        
        cars[i] = Car(i, arrival_time, departure_time, ch);
    }

    sort (cars, cars+NUM_OF_CARS, compareTwoCars);

    for(i=0;i<NUM_OF_CARS;i++)
    {
        cout<<i+1<<" "<<cars[i].arrival_time<<" "<<cars[i].departure_time<<" "<<cars[i].type<<"\n";
    }
}