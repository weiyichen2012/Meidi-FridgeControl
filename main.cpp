#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <cstdlib>
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

    int getId(){
        return id;
    }

    int getStage(){
        return stage;
    }

    string getCargo(){
        return cargo;
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

    //remove a cargo from container
    string removeCargo(){
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

    //remove whole container from car
    void removeContainer(){
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

// the global target
int targetId[N][N][N];
tuple<int, int, int> targetPos[N * N * N];

int deltaMoveX[4] = {-1, 1, 0, 0};
int deltaMoveY[4] = {0, 0, -1, 1};

struct _layer{
    int id, unitId[N][N], manhattan;
    _unit unit[N][N];
    vector<string> moves;

    _layer(){manhattan = -1;}
    _layer(int id){
        manhattan = -1;
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
    }

    void changeId(int* inputId){
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j){
                unitId[i][j] = inputId[i * N + j];
                unit[i][j].id = inputId[i * N + j];
                unit[i][j].stage = unitContainer;
            }
        unit[0][0].stage = unitNull;
    }

    void switchUnit(tuple<int, int> thing, tuple<int, int> null){
        manhattan = -1;
        // moves.push_back("Move (" + to_string(get<0>(thing)) + ", " + to_string(get<1>(thing)) + ") -> (" + to_string(get<0>(null)) + ", " + to_string(get<1>(null)) + ")");
        moves.push_back("Move " + to_string(unitId[get<0>(thing)][get<1>(thing)]));

        _unit tmpUnit = unit[get<0>(thing)][get<1>(thing)];
        unit[get<0>(thing)][get<1>(thing)] = unit[get<0>(null)][get<1>(null)];
        unit[get<0>(null)][get<1>(null)] = tmpUnit;

        int tmpId = unitId[get<0>(thing)][get<1>(thing)];
        unitId[get<0>(thing)][get<1>(thing)] = unitId[get<0>(null)][get<1>(null)];
        unitId[get<0>(null)][get<1>(null)] = tmpId;
    }

    string getHash(){
        string rtnString = "";
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                rtnString += to_string(unitId[i][j]);
        // cout << rtnString << endl;
        return rtnString;
    }

    int getManhattan(){
        if (manhattan != -1)
            return manhattan;
        int sum = 0;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                sum += abs(id - get<0>(targetPos[unit[i][j].getId()])) + abs(i - get<1>(targetPos[unit[i][j].getId()])) + abs(j - get<2>(targetPos[unit[i][j].getId()]));
        manhattan = sum;
        return sum;
    }

    vector<_layer> neighbors(){
        vector<_layer> rtnNeighbors;
        vector<tuple<int, int>> nullPos;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j){
                if (this->unit[i][j].getStage() == unitNull)
                    nullPos.push_back(make_tuple<int, int>(move(i), move(j)));
            }
        for (vector<tuple<int, int>>::iterator it = nullPos.begin(); it != nullPos.end(); it++){
            int posX = get<0>(*it), posY = get<1>(*it);
            for (int i = 0; i < 4; ++i){
                if (0 <= posX + deltaMoveX[i] && posX + deltaMoveX[i] < N && 0 <= posY + deltaMoveY[i] && posY + deltaMoveY[i] < N && this->unit[posX + deltaMoveX[i]][posY + deltaMoveY[i]].getStage() != unitNull){
                    _layer tmpLayer = (*this);
                    tmpLayer.switchUnit(make_tuple<int, int>(posX + deltaMoveX[i], posY + deltaMoveY[i]), make_tuple<int, int>(move(posX), move(posY)));
                    rtnNeighbors.push_back(tmpLayer);
                }
            }
        }
        return rtnNeighbors;
    }

    void print(){
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j){
                unit[i][j].print();
                if (j == N - 1)
                    cout << endl;
            }
    }

    void printMove(){
        int n = 1;
        for (vector<string>::iterator it = moves.begin(); it != moves.end(); ++it){
            cout << n <<  ": " << (*it) << endl;
            n++;
        }
    }
};

struct cmp{
    bool operator()(_layer& l1, _layer& l2){
        return l1.getManhattan() > l2.getManhattan();
    }
};

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

struct _layerSolver{
    int id;
    priority_queue<_layer, vector<_layer>, cmp> solution;
    set<string> alreadySolution;

    _layerSolver(){}

    void initTargetPos(int id){
        this->id = id;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                    targetPos[targetId[id][i][j]] = make_tuple(id, i, j);
    }

    int getSeqNum(int id){
        //todo
    }

    bool solvable(_layer layer){

    }

    void solve(_layer start, int id){
        initTargetPos(id);
        while (!solution.empty()){
            solution.pop();
        }
        solution.push(start);
        alreadySolution.insert(start.getHash());
        while (1){
            // this->printSolver();
            // cin.get();
            _layer topLayer = solution.top();
            if (topLayer.getManhattan() == 0)
                break;
            solution.pop();
            cout << "Solving..." << endl;
            vector<_layer> neighbors = topLayer.neighbors();
            for (vector<_layer>::iterator it = neighbors.begin(); it != neighbors.end(); it++){
                if (alreadySolution.find(it->getHash()) == alreadySolution.end()){
                    solution.push(*it);
                    alreadySolution.insert(it->getHash());
                }
            }

        }
        _layer ans = solution.top();
        ans.printMove();
    }


    void printSolver(){
        cout << "!" << endl;
        priority_queue<_layer, vector<_layer>, cmp> cpSolution = solution;
        cout << "?" << endl;
        int n = 0;
        cout << "Printing Solutions" << endl;
        while (!cpSolution.empty()){
            _layer tmpLayer = cpSolution.top();
            cout << "Solution " << n << " with mahattan = " << tmpLayer.getManhattan() << endl;
            n++;
            tmpLayer.print();
            cpSolution.pop();
            cout << endl;
        }
    }
}layerSolver;

int main(){
    // fridge.print();
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            for (int k = 0; k < N; ++k)
                targetId[i][j][k] = i * N * N + j * N + k;
    int t[N * N];
    t[0 * N + 0] = 0; t[0 * N + 1] = 6; t[0 * N + 2] = 8;
    t[1 * N + 0] = 1; t[1 * N + 1] = 2; t[1 * N + 2] = 5;
    t[2 * N + 0] = 7; t[2 * N + 1] = 4; t[2 * N + 2] = 3;
    _layer testLayer(0);
    testLayer.changeId(t);
    layerSolver.solve(testLayer, 0);
    cin.get();
}