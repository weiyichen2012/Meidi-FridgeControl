#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <cstdlib>
#include <tuple>
using namespace std;

#define N 3

int globalTime[N + 1] = {0, 0, 0, 0};
void syncGlobalTime();
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
                cout << " ";
                break;
            
            case unitCar:
                cout << "C";
                break;

            case unitContainer:
                cout << "T";
                break;

            case unitFull:
                cout << "F";
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
    vector<tuple<int, string>> moves;

    _layer(){manhattan = -1;}
    _layer(int id){
        manhattan = -1;
        this->id = id;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j){
                unitId[i][j] = id * N * N + i * N + j;
                if (i == 1 && j == 1)
                    unit[i][j] = _unit(unitNull, unitId[i][j]);
                else if (i == 0 && j == ((id) % 2))
                    unit[0][(id) % 2] = _unit(unitCar, unitId[0][(id) % 2]);
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

    void switchUnit(tuple<int, int> thing, tuple<int, int> null, bool ifMove = true){
        // cout << "in switch unit: " << get<0>(thing) << " " << get<1>(thing) << " " << get<0>(null) << " " << get<1>(null) << endl;
        manhattan = -1;
        // moves.push_back("Move (" + to_string(get<0>(thing)) + ", " + to_string(get<1>(thing)) + ") -> (" + to_string(get<0>(null)) + ", " + to_string(get<1>(null)) + ")");
        if (ifMove){
            moves.push_back(make_tuple<int, string>(-1, "Horizontal Move " + to_string(unitId[get<0>(thing)][get<1>(thing)]) + " ... (" + to_string(get<0>(thing)) + ", " + to_string(get<1>(thing)) + ", " + to_string(this->id) + ") -> (" + to_string(get<0>(null)) + ", " + to_string(get<1>(null)) + ", " + to_string(this->id) + ")"));
            // globalTime[this->id]++;
            // cout << "Move " + to_string(unitId[get<0>(thing)][get<1>(thing)]) + " " + to_string(unit[get<0>(thing)][get<1>(thing)].getId()) + " " + "Move (" + to_string(get<0>(thing)) + ", " + to_string(get<1>(thing)) + ") -> (" + to_string(get<0>(null)) + ", " + to_string(get<1>(null)) + ")" << endl;
        }
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
        // if (manhattan != -1)
            // return manhattan;
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

    void spawnTime(){
        for (vector<tuple<int, string>>::iterator it = moves.begin(); it != moves.end(); it++)
            if (get<0>(*it) == -1){
                (*it) = make_tuple(globalTime[this->id], get<1>(*it));
                globalTime[this->id]++;
            }

    }

    void print(){
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j){
                unit[i][j].print();
                if (j == N - 1)
                    cout << endl;
            }
    }

    void printMove(int afterTime = 0){
        int n = 1;
        bool ifPrint = false;
        for (vector<tuple<int, string>>::iterator it = moves.begin(); it != moves.end(); ++it){
            if (get<0>(*it) >= afterTime){
                cout << get<0>(*it) <<  ": " << get<1>(*it) << endl;
                n++;
                ifPrint = true;
            }
            if (it + 2 == moves.end() && (!ifPrint))
                cout << "No moves needed" << endl;
        }
    }
};

struct cmp{
    bool operator()(_layer& l1, _layer& l2){
        return l1.getManhattan() > l2.getManhattan();
    }
};

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

    int getSeqNum(int num){
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                if (targetId[this->id][i][j] == num)
                    return (i * N + j);
        return -1;
    }

    bool solvable(_layer layer, int id){
        this->id = id;
        int reverseSum = 0;
        for (int i = 0; i < N * N; ++i)
            for (int j = i + 1; j < N * N; ++j)
                if (layer.unit[i / 3][i % 3].getStage() != unitNull && layer.unit[j / 3][j % 3].getStage() != unitNull && getSeqNum(layer.unitId[j / 3][j % 3]) < getSeqNum(layer.unitId[i / 3][i % 3])){
                    // cout << i << j << endl;
                    reverseSum++;
                }
        // cout << "in solvable: " << reverseSum << endl;
        // layer.print();
        return (reverseSum % 2 == 0);
    }

    _layer solve(_layer start, int id){
        initTargetPos(id);
        this->id = id;
        if (!this->solvable(start, id)){
            cout << "In solve, meet an unsolvable puzzle" << endl;
            return _layer();
        }
        alreadySolution.clear();
        while (!solution.empty()){
            solution.pop();
        }
        solution.push(start);
        alreadySolution.insert(start.getHash());
        while (1){
            // this->printSolver();
            // cin.get();
            _layer topLayer = solution.top();
            // topLayer.print();cout << endl << topLayer.getManhattan() << endl;
            // cin.get();
            if (topLayer.getManhattan() == 0)
                break;
            solution.pop();
            // cout << "Solving..." << endl;
            vector<_layer> neighbors = topLayer.neighbors();
            for (vector<_layer>::iterator it = neighbors.begin(); it != neighbors.end(); it++){
                if (alreadySolution.find(it->getHash()) == alreadySolution.end()){
                    solution.push(*it);
                    alreadySolution.insert(it->getHash());
                }
            }

        }
        _layer ans = solution.top();
        // ans.printMove();
        return ans;
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

int storeX[6] = {2, 2, 2, 2, 2, 2};
int storeY[6] = {0, 1, 2, 0, 1, 2};
int storeZ[6] = {0, 0, 0, 1, 1, 1};

int liftCarY[2] = {0, 1};
int liftCargoY[2] = {1, 0};

struct _fridge{
    _layer layer[N];
    vector<tuple<int, string>> moves;
    _fridge(){
        for (int i = 0; i < N; ++i)
            layer[i] = _layer(i);
    }

    void prepare(){
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                targetId[2][i][j] = layer[2].unitId[i][j];

        _layer proposeLayer[2] = {this->layer[0], this->layer[1]};
        int startTime = globalTime[0];
        for (int z = 0; z < 2; ++z){
            vector<tuple<int, int>> containerLayer;
            for (int i = 0; i < N - 1; ++i)
                for (int j = 0; j < N; ++j)
                    if (proposeLayer[z].unit[i][j].getStage() == unitContainer)
                        containerLayer.push_back(make_tuple<int, int>(move(i), move(j)));

            for (int i = 0; i < N; ++i)
                if (proposeLayer[z].unit[2][i].getStage() != unitContainer){
                    int minManhattan = N * N;
                    tuple<int, int> minPos;
                    vector<tuple<int, int>>::iterator minIt;
                    for (vector<tuple<int, int>>::iterator it = containerLayer.begin(); it != containerLayer.end(); ++it){
                        if (abs(get<0>(*it) - 2) + abs(get<1>(*it) - i) < minManhattan){
                            minManhattan = abs(get<0>(*it) - 2) + abs(get<1>(*it) - i);
                            minPos = make_tuple<int, int>(move(get<0>(*it)), move(get<1>(*it)));
                            minIt = it;
                        }
                    }
                    containerLayer.erase(minIt);
                    proposeLayer[z].switchUnit(make_tuple<int, int>(2, move(i)), minPos, false);
                }

            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    targetId[z][i][j] = proposeLayer[z].unitId[i][j];
            
            // cout << "initial proposeLayer" << endl;
            // proposeLayer[z].print();

            if (!layerSolver.solvable(layer[z], z)){
                cout << "Detect unsolvable" << endl;
                proposeLayer[z].switchUnit(make_tuple<int, int>(2, 0), make_tuple<int, int>(2, 1), false);
                targetId[z][2][0] = proposeLayer[z].unitId[2][0];
                targetId[z][2][1] = proposeLayer[z].unitId[2][1];
            }
            cout << endl << "Begin solving Prepare layer" << z << ": " << endl << endl;
            cout << "Current layer: " << endl;
            layer[z].print();
            cout << endl << "Proposed layer: " << endl; 
            proposeLayer[z].print();
            cout << endl;
            layer[z] = layerSolver.solve(layer[z], z);
            layer[z].moves.push_back(make_tuple<int, string>(-1, "----------------Finish prepare----------------"));
            layer[z].spawnTime();
            // cout << "Printing moves..." << endl;
            // layer[z].printMove(startTime);
        }
        vector<tuple<int, string>>::iterator it[N] = {layer[0].moves.begin(), layer[1].moves.begin()};
        for (int z = 0; z < 2; ++z){
            while (get<0>(*(it[z])) < startTime)
                it[z]++;
        }
        while (it[0] != layer[0].moves.end() || it[1] != layer[1].moves.end()){
            cout << startTime << ": " << endl;
            for (int z = 0; z < 2; ++z)
                if (it[z] != layer[z].moves.end() && get<0>(*(it[z])) >= startTime){
                    // if (get<1>(*(it[z]))[0] != '-')
                    cout << "    " << (get<1>(*(it[z]))) << endl;
                    it[z]++;
                }
            startTime++;
        }
        syncGlobalTime();
    }

    void storeCargo(int n, int* cargoId, string* cargo){
        for (int i = 0; i < n; ++i){
            int id = cargoId[i];
            layer[storeZ[id]].unit[storeX[id]][storeY[id]].storeCargo(cargo[i]);
        }
    }

    tuple<int, int, int> findCargo(string cargo){
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                for (int k = 0; k < N; ++k)
                    if (layer[i].unit[j][k].getCargo() == cargo)
                        return make_tuple<int, int, int>(move(i), move(j), move(k));
        cout << "Error in findCargo, cargo not found" << endl;
        return make_tuple<int, int, int>(0, 0, 0);
    }

    void getCargo(int n, string* cargo){
        bool used[2][3];
        int startTime = globalTime[0];
        _layer proposeLayer[2] = {layer[0], layer[1]};
        for (int i = 0; i < 2; ++i)
            for (int j = 0; j < N; ++j)
                used[i][j] = false;
        for (int i = 0; i < n; ++i){
            tuple<int, int, int> cargoIndex = findCargo(cargo[i]);
            // cout << cargo[i] << " cargoIndex: " << get<0>(cargoIndex) << get<1>(cargoIndex) << get<2>(cargoIndex) << endl;
            int unitX = get<1>(cargoIndex), unitY = get<2>(cargoIndex), unitZ = get<0>(cargoIndex);
            proposeLayer[unitZ].unit[unitX][unitY].removeCargo();
            for (int j = 0; j < n; ++j)
                if (!used[unitZ][j]){
                    proposeLayer[unitZ].switchUnit(make_tuple<int, int>(2, move(j)), make_tuple<int, int>(move(unitX), move(unitY)), false);
                    used[unitZ][j] = true;
                    break;
                }
        }
        for (int z = 0; z < 2; ++z){
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    targetId[z][i][j] = proposeLayer[z].unitId[i][j];
            
            if (!layerSolver.solvable(layer[z], z)){
                proposeLayer[z].switchUnit(make_tuple<int, int>(2, 0), make_tuple<int, int>(2, 1), false);
                targetId[z][2][0] = proposeLayer[z].unitId[2][0];
                targetId[z][2][1] = proposeLayer[z].unitId[2][1];
            }
            cout << endl << "Begin solving getCargo layer" << z << ": " << endl << endl;
            cout << "Current layer: " << endl;
            layer[z].print();
            cout << endl << "Proposed layer: " << endl; 
            proposeLayer[z].print();
            cout << endl;
            layer[z] = layerSolver.solve(layer[z], z);
            layer[z].moves.push_back(make_tuple<int, string>(-1, "----------------Finish getCargo----------------"));
            int startTime = globalTime[z];
            layer[z].spawnTime();
            // cout << "----------------Printing moves----------------" << endl;
            // layer[z].printMove(startTime);
        }
        vector<tuple<int, string>>::iterator it[N] = {layer[0].moves.begin(), layer[1].moves.begin()};
        for (int z = 0; z < 2; ++z){
            while (get<0>(*(it[z])) < startTime)
                it[z]++;
        }
        while (it[0] != layer[0].moves.end() || it[1] != layer[1].moves.end()){
            cout << startTime << ": " << endl;
            for (int z = 0; z < 2; ++z)
                if (get<0>(*(it[z])) >= startTime){
                    if (get<1>(*(it[z]))[0] != '-')
                        cout << "    " << (get<1>(*(it[z]))) << endl;
                    it[z]++;
                }
            startTime++;
        }
        syncGlobalTime();
    }

    void exchangeFloorWithUp(int layerId){
        if (liftCarY[0] == 0){
            liftCarY[0] = 1; liftCarY[1] = 0;
            liftCargoY[0] = 0; liftCargoY[1] = 1;
        }
        else{
            liftCarY[0] = 0; liftCarY[1] = 1;
            liftCargoY[0] = 1; liftCargoY[1] = 0;
        }
        _unit tmpUnit = layer[layerId].unit[0][0];
        layer[layerId].unit[0][0] = layer[layerId + 1].unit[0][0];
        layer[layerId + 1].unit[0][0] = tmpUnit;

        tmpUnit = layer[layerId].unit[0][1];
        layer[layerId].unit[0][1] = layer[layerId + 1].unit[0][1];
        layer[layerId + 1].unit[0][1] = tmpUnit;

        for (int z = layerId; z < layerId + 2; ++z)
            for (int j = 0; j < 2; ++j)
                layer[z].unitId[0][j] = layer[z].unit[0][j].id;
        
        moves.push_back(make_tuple<int, string>(move(globalTime[N]), "    Vertical Exchange Layer " + to_string(layerId) + " with Layer " + to_string(layerId + 1)));
        syncGlobalTime();
    }

    //dir : 1 -> up, -1 -> down
    void liftCargo(int dir, string cargo){
        tuple<int, int, int> cargoIndex = findCargo(cargo);
        int unitX = get<1>(cargoIndex), unitY = get<2>(cargoIndex), unitZ[2] = {get<0>(cargoIndex), get<0>(cargoIndex) + dir};
        cout << unitX << " " << unitY << " " << unitZ[0] << " " << unitZ[1] << endl;
        int startTime = globalTime[0];
        _layer proposeLayer[2] = {layer[unitZ[0]], layer[unitZ[1]]};
        
        for (int z = 0; z < 2; ++z){
            bool ifFound = false;
            for (int i = 0; i < N; ++i){
                for (int j = 0; j < N; ++j)
                    if (proposeLayer[z].unit[i][j].getStage() == unitCar){
                        cout << "liftCarY " << z << " " << i << " " << j << endl;
                        cout << "Move to " << 0 << " " << liftCarY[unitZ[z] % 2] << endl;
                        ifFound = true;
                        proposeLayer[z].switchUnit(make_tuple<int, int>(move(i), move(j)), make_tuple<int, int>(0, move(liftCarY[unitZ[z] % 2])));
                        break;
                    }
                if (ifFound)
                    break;
            }
        }

        proposeLayer[1].print();

        proposeLayer[0].switchUnit(make_tuple<int, int>(move(unitX), move(unitY)), make_tuple<int, int>(0, move(liftCargoY[unitZ[0] % 2])));
        if (proposeLayer[1].unit[0][liftCargoY[unitZ[1] % 2]].getStage() != unitContainer){
            bool ifFound = false;
            for (int i = 0; i < N; ++i){
                if (ifFound)
                    break;
                for (int j = 0; j < N; ++j)
                    if (proposeLayer[1].unit[i][j].getStage() == unitContainer){
                        ifFound = true;
                        proposeLayer[1].switchUnit(make_tuple<int, int>(move(i), move(j)), make_tuple<int, int>(0, move(liftCargoY[unitZ[1] % 2])));
                        break;
                    }
            }
        }


        for (int z = 0; z < 2; ++z){
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    for (int k = 0; k < N; ++k)
                        targetId[k][i][j] = layer[k].unitId[i][j];
            
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    targetId[unitZ[z]][i][j] = proposeLayer[z].unitId[i][j];

            #define try1X1 2
            #define try1Y1 0
            #define try1X2 2
            #define try1Y2 1
            if (!layerSolver.solvable(layer[unitZ[z]], unitZ[z])){
                if (proposeLayer[z].unit[try1X1][try1Y1].getStage() != unitNull && proposeLayer[z].unit[try1X2][try1Y2].getStage() != unitNull){
                    proposeLayer[z].switchUnit(make_tuple<int, int>(try1X1, try1Y1), make_tuple<int, int>(try1X2, try1Y2), false);
                    targetId[unitZ[z]][try1X1][try1Y1] = proposeLayer[z].unitId[try1X1][try1Y1];
                    targetId[unitZ[z]][try1X2][try1Y2] = proposeLayer[z].unitId[try1X2][try1Y2];
                }
                else{
                    cout << "using backup!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
                    proposeLayer[z].switchUnit(make_tuple<int, int>(1, 2), make_tuple<int, int>(0, 2), false);
                    targetId[unitZ[z]][1][2] = proposeLayer[z].unitId[1][2];
                    targetId[unitZ[z]][0][2] = proposeLayer[z].unitId[0][2];
                }
            }
            cout << endl << "Begin solving liftCargo's prepare layer" << unitZ[z] << ": " << endl << endl;
            cout << "Current layer: " << endl;
            layer[unitZ[z]].print();
            cout << endl << "Proposed layer: " << endl; 
            proposeLayer[z].print();
            cout << endl;
            layer[unitZ[z]] = layerSolver.solve(layer[unitZ[z]], unitZ[z]);
            layer[unitZ[z]].moves.push_back(make_tuple<int, string>(-1, "------------Finish liftCargo Prepare------------"));
            layer[unitZ[z]].spawnTime();
            // cout << "----------------Printing moves----------------" << endl;
            // layer[z].printMove(startTime);
        }
        vector<tuple<int, string>>::iterator it[N] = {layer[unitZ[0]].moves.begin(), layer[unitZ[1]].moves.begin()};
        for (int z = 0; z < 2; ++z){
            while (get<0>(*(it[z])) < startTime)
                it[z]++;
        }
        while (it[0] != layer[unitZ[0]].moves.end() || it[1] != layer[unitZ[1]].moves.end()){
            cout << startTime << ": " << endl;
            for (int z = 0; z < 2; ++z)
                if (it[z] != layer[unitZ[z]].moves.end() && get<0>(*(it[unitZ[z]])) >= startTime){
                    if (get<1>(*(it[z]))[0] != '-')
                        cout << "    " << (get<1>(*(it[z]))) << endl;
                    it[z]++;
                }
            startTime++;
        }
        syncGlobalTime();
        if (dir == 1)
            exchangeFloorWithUp(unitZ[0]);
        else
            exchangeFloorWithUp(unitZ[1]);
        vector<tuple<int, string>>::iterator fridgeIt = moves.begin();
        for (;fridgeIt != moves.end(); ++fridgeIt){
            if (get<0>(*fridgeIt) >= startTime){
                cout << get<0>(*fridgeIt) << ": " << endl << "    " << (get<1>(*(fridgeIt))) << endl;
            }
            else
                cout << "opps" << get<0>(*fridgeIt);
        }
    }

    void print(){
        for (int i = 0; i < N; ++i){
            cout << "layer " << i << ": " << endl;
            layer[i].print();
            cout << endl;
        }
    }

}fridge;

void syncGlobalTime(){
    int maxTime = -1;
    for (int i = 0; i < N + 1; ++i)
        maxTime = max(maxTime, globalTime[i]);
    
    for (int i = 0; i < N + 1; ++i)
        globalTime[i] = maxTime;
}

int main(){
    cout << "Welcome to fridge controlSystem" << endl;
    cout << "Input help/h for helps" << endl;
    string inputString;
    while (getline(cin, inputString)){
        cout << inputString << endl;
        string splitMsg[30];
        int splitMsgId = 0;
        for (int i = 0; i < inputString.size(); ++i)
            if (inputString[i] != ' ')
                splitMsg[splitMsgId] += inputString[i];
            else
                splitMsgId++;
        if (splitMsg[0] == "help" || splitMsg[0] == "h"){
            cout << "There are four commands in total to help you control the fridge" << endl;
            cout << "prepare: prepare the fridge so that you can put items into it" << endl;
            cout << "store N id[0] id[1] ... id[N - 1] name[0] name[1] ... name[N - 1]: put N items whose names are name[0], name[1]..., name[N - 1] and positions are at id[0], id[1], ... id[N - 1] into the fidge" << endl;
            cout << "get N name[0] name[1] ... name[N - 1]: get N items from the fridge" << endl;
            cout << endl;
            cout << "Other commands help you see the status of the fridge" << endl;
            cout << "status: show the status of the whole fridge" << endl;
        }
        else if (splitMsg[0] == "prepare"){
            fridge.prepare();
        }
        else if (splitMsg[0] == "store"){
            int n = atoi(splitMsg[1].c_str());
            int itemId[10];
            for (int i = 0; i < n; ++i)
                itemId[i] = atoi(splitMsg[2 + i].c_str());
            fridge.storeCargo(n, itemId, splitMsg + n + 2);
            cout << "finish store" << endl;
        }
        else if (splitMsg[0] == "get"){
            int n = atoi(splitMsg[1].c_str());
            fridge.getCargo(n, splitMsg + 2);
        }
        else if (splitMsg[0] == "lift"){
            int dir = atoi(splitMsg[1].c_str());
            fridge.liftCargo(dir, splitMsg[2]);
            // fridge.print();
        }
        else
            cout << "Command not known" << endl;
    }

    // fridge.print();
    // string cargo[6] = {"Apple", "Banana", "Can", "Deer", "Elephant", "Fan"};
    // int cargoId[6] = {4, 2, 0, 3, 5, 1};
    // fridge.storeCargo(2, cargoId, cargo);
    // fridge.prepare();
    // fridge.storeCargo(2, cargoId + 2, cargo + 2);
    // fridge.prepare();
    // fridge.getCargo(2, cargo);

    // string cargo1[6] = {"App", "Bana", "Ca", "De", "Eleph", "Fa"};
    // int cargoId1[6] = {2, 0, 4, 3, 5, 1};
    // fridge.storeCargo(2, cargoId1, cargo1);
    // fridge.prepare();
    // fridge.getCargo(3, cargo);
    // fridge.getCargo(3, cargo + 3);

    // fridge.storeCargo(2, cargoId1, cargo1);
    // fridge.prepare();
    // fridge.getCargo(1, cargo);

    // for (int i = 0; i < N; ++i)
    //     for (int j = 0; j < N; ++j)
    //         for (int k = 0; k < N; ++k)
    //             targetId[i][j][k] = i * N * N + j * N + k;
    // int t[N * N];
    // t[0 * N + 0] = 0; t[0 * N + 1] = 6; t[0 * N + 2] = 8;
    // t[1 * N + 0] = 1; t[1 * N + 1] = 2; t[1 * N + 2] = 5;
    // t[2 * N + 0] = 7; t[2 * N + 1] = 3; t[2 * N + 2] = 4;
    // _layer testLayer(0);
    // testLayer.changeId(t);
    // layerSolver.solve(testLayer, 0);
    cin.get();
}