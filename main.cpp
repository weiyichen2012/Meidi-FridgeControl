#include <iostream>
#include <vector>
#include <tuple>
using namespace std;

#define N 3

int intLen(int n){
    int sum = 1;
    while (n >= 10){
        sum++;
        n %= 10;
    }
    return sum;
}
const int printLen = intLen(N * N * N);

//Simplest unit, three stages:
//unitNull: no car, unitCar: car with no container, unitContainer: car with empty container, unitFull: car with full cargo
enum unitStages{unitNull, unitCar, unitContainer, unitFull};
struct _unit{
    int stage;//store unitStage
    int id;
    string cargo;//store cargo info

    _unit(){}
    _unit(int stage, int id, string cargo = ""){
        this->stage = stage;
        this->id = id;
        this->cargo = cargo;
    }

    //store a cargo into container
    void storeCargo(string cargo){
        if (this->stage != unitContainer){
            cout << "Error in unit's storeCargo(), cannot store into a unit that's not unitContainer, its status: " << this->stage << endl;
            return;
        }
        this->stage = unitFull;
        this->cargo = cargo;
    }

    //get a cargo from container
    string getCargo(){
        if (this->stage != unitFull){
            cout << "Error in unit's getCargo(), cannot get a unit that's not unitFull, its status: " << this->stage << endl;
            return "Error";
        }
        this->stage = unitContainer;
        return cargo;
    }

    //store whole container into car
    void storeContainer(_unit* fromContainer){
        if (this->stage != unitCar){
            cout << "Error in unit's storeContainer(), cannot store container into a unit that's not unitCar, its status: " << this->stage << endl;
            return;
        }
        this->stage = fromContainer->stage;
        this->cargo = fromContainer->cargo;
    }

    //get whole container from car
    void getContainer(){
        if (this->stage != unitContainer && this->stage != unitFull){
            cout << "Error in unit's getContainer(), cannot get container from unit that has no container, its status: " << this->stage << endl;
            return;
        }
        this->stage = unitCar;
    }

    void print(){
        switch (this->stage){
            case unitNull:
                // cout << " ";
                break;
            
            case unitCar:
                // cout << "C";
                break;

            case unitContainer:
                // cout << "T";
                break;

            case unitFull:
                // cout << "F";
                break;
        }
        if (this->stage == unitNull)
            for (int i = 0; i < printLen + 1; ++i)
                cout << " ";
        else{
            cout << id;
            for (int i = 0; i < printLen - intLen(id) + 1; ++i)
                cout << " ";
        }
    }
};

int targetId[N][N][N];
tuple<int, int, int> targetPos[N * N * N];
void initTargetPos(){
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            for (int k = 0; k < N; ++k)
                targetPos[targetId[i][j][k]] = make_tuple(i, j, k);
}

struct _layer{
    int id, unitId[N][N], manhattan;
    _unit unit[N][N];

    _layer(){manhattan = -1;}
    _layer(int id){
        this->id = id;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j){
                unitId[i][j] = id * N * N + i * N + j;
                if (i == 0 && j == 0)
                    unit[0][0] = _unit(unitNull, unitId[0][0]);
                else if (i == 0 && j == 1)
                    unit[0][1] = _unit(unitCar, unitId[0][1]);
                else
                    unit[i][j] = _unit(unitContainer, unitId[i][j]);
            }
        manhattan = -1;
    }

    int getManhattan(){
        if (manhattan != -1)
            return manhattan;
    }

    void print(){
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j){
                unit[i][j].print();
                if (j == N - 1)
                    cout << endl;
            }
    }
}layer[N];

struct _fridge{
    _layer layer[N];
    _fridge(){
        for (int i = 0; i < N; ++i)
            layer[i] = _layer(i);
    }

    void print(){
        for (int i = 0; i < N; ++i){
            cout << "layer " << i << ": " << endl;
            layer[i].print();
            cout << endl;
        }
    }
}fridge;

int main(){
    fridge.print();
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            for (int k = 0; k < N; ++k)
                targetId[i][j][k] = i * N * N + j * N + k;
    targetId[0][0][0] = 0;
    targetId[0][0][1] = 1;
    targetId[0][0][2] = 2;
    targetId[0][1][0] = 3;
    targetId[0][1][1] = 4;
    targetId[0][1][2] = 5;
    targetId[0][2][0] = 6;
    targetId[0][2][1] = 7;
    targetId[0][2][2] = 8;
    initTargetPos();
    
}