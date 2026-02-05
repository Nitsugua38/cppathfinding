#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <queue>
#include <limits>

using namespace std;




class MAP {
    public:
        int NUM_COLUMNS;
        int NUM_ROWS;
        int NUM_VERTICES;

        int StartX;
        int StartY;
        int EndX;
        int EndY;

        vector<string> mapMatrix;
        vector<vector<bool>> visitedMatrix;
        vector<vector<double>> weightMatrix;
        
        void initMap(string mapPath);
        void findStartEnd();
};


class ALGOS {
    private:
        int moveX[4] = {1, -1, 0, 0};
        int moveY[4] = {0, 0, 1, -1};

    public:
        bool dfs(MAP& m, int CurrentX, int CurrentY);
        void bfs(MAP& m, int CurrentX, int CurrentY);
        void dijkstra(MAP& m, int CurrentX, int CurrentY);
        void astar(MAP& m, int CurrentX, int CurrentY);
        double manhattanHeuristic(int x1, int x2, int y1, int y2);
};


class BENCHMARK {
    private:
        chrono::time_point<chrono::high_resolution_clock> start, end;
        chrono::duration<float> duration;

    public:
        int pathLength(MAP m);
        void startTimer();
        float stopTimer();
};


class FLAGMANAGER {
    private:
        int argc;
        char** argv;
        int getFlag(string flag);
    public:
        string getMap();
        void showVisited(MAP& m);

        FLAGMANAGER(int ac, char* av[]) : argc(ac), argv(av) {};
};








// ---------------- IMPORT MAP FROM TXT FILE -------------------------

void MAP::initMap(string mapPath) {

    ifstream readMap(mapPath);

    if (!readMap) {
        throw runtime_error("Error opening map!");
    }
    string fileLine;


    double inf = numeric_limits<double>::infinity();

    while (getline(readMap, fileLine)) {
        mapMatrix.push_back(fileLine);
        visitedMatrix.push_back(vector<bool>(fileLine.size(), false));
        weightMatrix.push_back(vector<double>(fileLine.size(), inf));
    }

    NUM_COLUMNS = mapMatrix[0].size();
    NUM_ROWS = mapMatrix.size();
    NUM_VERTICES = (NUM_COLUMNS - 2) * (NUM_ROWS - 2);


    readMap.close();

};








// ---------------- FIND MAP STARTING/ENDING POINT -------------------------

void MAP::findStartEnd() {
    
    for (int x = 0; x < NUM_COLUMNS; x++) {
        for (int y = 0; y < NUM_ROWS; y++) {
            if (mapMatrix[y][x] == 'S') {
                StartX = x;
                StartY = y;
            };
            if (mapMatrix[y][x] == 'E') {
                EndX = x;
                EndY = y;
            };
        };
    };
    
};












// ---------------- DEPTH-FIRST SEARCH -------------------------

bool ALGOS::dfs(MAP& m, int CurrentX, int CurrentY) {

    // Step 1: if blocked/visited -> abandon

    if (m.mapMatrix[CurrentY][CurrentX] == '#' || m.visitedMatrix[CurrentY][CurrentX] || m.mapMatrix[CurrentY][CurrentX] == '.') {
        return false;
    };


    // Step 2: if arrived -> finish

    if (m.mapMatrix[CurrentY][CurrentX] == 'E') {
        return true;
    };


    // Step 3: mark as visited

    m.visitedMatrix[CurrentY][CurrentX] = true;


    // Step 4: explore neighbors

    if (dfs(m, CurrentX + 1, CurrentY) || dfs(m, CurrentX - 1, CurrentY) || dfs(m, CurrentX, CurrentY + 1) || dfs(m, CurrentX, CurrentY - 1)) {
        if (m.mapMatrix[CurrentY][CurrentX] != 'S') m.mapMatrix[CurrentY][CurrentX] = '.';
        return true;
    }

    return false;

};








// ---------------- BREADTH-FIRST SEARCH -------------------------

void ALGOS::bfs(MAP& m, int CurrentX, int CurrentY) {


    // Step 1: Create a queue and init parent

    queue<pair<int, int>> processQueue;
    processQueue.push({CurrentX, CurrentY});

    bool endingFound = false;
    int parentX[m.NUM_ROWS][m.NUM_COLUMNS];
    int parentY[m.NUM_ROWS][m.NUM_COLUMNS];

    parentX[CurrentY][CurrentX] = CurrentX;
    parentY[CurrentY][CurrentX] = CurrentY;



    // Step 2: Process the queue

    while (!processQueue.empty() && !endingFound) {
        
        CurrentX = processQueue.front().first;
        CurrentY = processQueue.front().second;
        processQueue.pop();


        // For each neighbour

        for (int i = 0; i < 4; i++) {
            int newX = CurrentX + moveX[i];
            int newY = CurrentY + moveY[i];


            if (m.mapMatrix[newY][newX] != '#' && !m.visitedMatrix[newY][newX]) {

                parentX[newY][newX] = CurrentX;
                parentY[newY][newX] = CurrentY;

                

                // Ending found: Stop and rebuild final path from parents
                
                if (m.mapMatrix[newY][newX] == 'E') {
                    endingFound = true;

                    int recX = newX;
                    int recY = newY;

                    while (recX != m.StartX || recY != m.StartY) {
                        if (m.mapMatrix[recY][recX] != 'E') {
                            m.mapMatrix[recY][recX] = '.';
                        };
                        int px = parentX[recY][recX];
                        int py = parentY[recY][recX];
                        recX = px;
                        recY = py;
                    };


                    return;
                }

                

                // Else, mark as visited and add neighbor to queue
                
                if (m.mapMatrix[newY][newX] != 'S' && m.mapMatrix[newY][newX] != 'E') {
                    m.visitedMatrix[newY][newX] = true;
                    processQueue.push({newX, newY});
                };
                
            };
        };
    };

};









// ---------------- DIJKSTRA -------------------------

void ALGOS::dijkstra(MAP& m, int CurrentX, int CurrentY) {


    // Step 1: set start weight to 0 and init Priority Queue

    m.weightMatrix[CurrentY][CurrentX] = 0;

    priority_queue<
        pair<double, pair<int, int>>,
        vector<pair<double, pair<int, int>>>,
        greater<pair<double, pair<int, int>>>
    > priorityQueue;

    priorityQueue.push({0.0, {CurrentX, CurrentY} });

    bool endingFound = false;
    int parentX[m.NUM_ROWS][m.NUM_COLUMNS];
    int parentY[m.NUM_ROWS][m.NUM_COLUMNS];

    parentX[CurrentY][CurrentX] = CurrentX;
    parentY[CurrentY][CurrentX] = CurrentY;




    // Step 2: Process the queue

    while (!priorityQueue.empty() && !endingFound) {
        
        double CurrentWeight = priorityQueue.top().first;
        CurrentX = priorityQueue.top().second.first;
        CurrentY = priorityQueue.top().second.second;
        priorityQueue.pop();


        // Skip if better path already exists

        if (CurrentWeight > m.weightMatrix[CurrentY][CurrentX]) {
            continue;
        };


        // Ending found: Stop and rebuild final path from parents
                
        if (m.mapMatrix[CurrentY][CurrentX] == 'E') {
            endingFound = true;

            int recX = CurrentX;
            int recY = CurrentY;

            while (recX != m.StartX || recY != m.StartY) {
                if (m.mapMatrix[recY][recX] != 'E') {
                    m.mapMatrix[recY][recX] = '.';
                };
                int px = parentX[recY][recX];
                int py = parentY[recY][recX];
                recX = px;
                recY = py;
            };


            return;
        };


        // Mark as visited for flag visualization

        if (m.mapMatrix[CurrentY][CurrentX] != 'S') {
            m.visitedMatrix[CurrentY][CurrentX] = true;
        };



        // For each neighbour

        for (int i = 0; i < 4; i++) {
                
            int newX = CurrentX + moveX[i];
            int newY = CurrentY + moveY[i];

            if (m.mapMatrix[newY][newX] == '#') {
                continue;
            };

            
            // Get new path weight

            double weight;
            switch (m.mapMatrix[newY][newX]) {
                case ' ': weight = 1.0; break;
                case 'E': weight = 1.0; break;
                case ':': weight = 2.0; break;
                case ';': weight = 3.0; break;
            };

            double newWeight = CurrentWeight + weight;


            // If path is better

            if (newWeight < m.weightMatrix[newY][newX]) {
                m.weightMatrix[newY][newX] = newWeight;
                parentX[newY][newX] = CurrentX;
                parentY[newY][newX] = CurrentY;
                priorityQueue.push({newWeight, {newX, newY}});
            };
        };
    };
};









// ---------------- A* -------------------------

void ALGOS::astar(MAP& m, int CurrentX, int CurrentY) {


    // Step 1: set start weight to 0 and init Priority Queue

    m.weightMatrix[CurrentY][CurrentX] = 0;

    priority_queue<
        pair<double, pair<int, int>>,
        vector<pair<double, pair<int, int>>>,
        greater<pair<double, pair<int, int>>>
    > priorityQueue;


    // Queue is now sorted by Manhattan Distance Heuristic + terrain weight
    
    double initialMH = manhattanHeuristic(m.EndX, CurrentX, m.EndY, CurrentY);
    priorityQueue.push({initialMH, {CurrentX, CurrentY} });


    bool endingFound = false;
    int parentX[m.NUM_ROWS][m.NUM_COLUMNS];
    int parentY[m.NUM_ROWS][m.NUM_COLUMNS];

    parentX[CurrentY][CurrentX] = CurrentX;
    parentY[CurrentY][CurrentX] = CurrentY;




    // Step 2: Process the queue

    while (!priorityQueue.empty() && !endingFound) {
        
        CurrentX = priorityQueue.top().second.first;
        CurrentY = priorityQueue.top().second.second;
        double CurrentWeight = m.weightMatrix[CurrentY][CurrentX];
        priorityQueue.pop();


        // Skip if better path already exists

        if (CurrentWeight > m.weightMatrix[CurrentY][CurrentX]) {
            continue;
        };


        // Ending found: Stop and rebuild final path from parents
                
        if (m.mapMatrix[CurrentY][CurrentX] == 'E') {
            endingFound = true;

            int recX = CurrentX;
            int recY = CurrentY;

            while (recX != m.StartX || recY != m.StartY) {
                if (m.mapMatrix[recY][recX] != 'E') {
                    m.mapMatrix[recY][recX] = '.';
                };
                int px = parentX[recY][recX];
                int py = parentY[recY][recX];
                recX = px;
                recY = py;
            };


            return;
        };


        // Mark as visited for flag visualization

        if (m.mapMatrix[CurrentY][CurrentX] != 'S') {
            m.visitedMatrix[CurrentY][CurrentX] = true;
        };



        // For each neighbour

        for (int i = 0; i < 4; i++) {
                
            int newX = CurrentX + moveX[i];
            int newY = CurrentY + moveY[i];

            if (m.mapMatrix[newY][newX] == '#') {
                continue;
            };

            
            // Get new path weight

            double weight;
            switch (m.mapMatrix[newY][newX]) {
                case ' ': weight = 1.0; break;
                case 'E': weight = 1.0; break;
                case ':': weight = 2.0; break;
                case ';': weight = 3.0; break;
            };

            double newWeight = CurrentWeight + weight;


            // If path is better

            if (newWeight < m.weightMatrix[newY][newX]) {
                m.weightMatrix[newY][newX] = newWeight;
                parentX[newY][newX] = CurrentX;
                parentY[newY][newX] = CurrentY;


                // Manhattan Distance Heuristic + terrain weight for queue prioritization 

                double newMH = manhattanHeuristic(m.EndX, newX, m.EndY, newY);
                priorityQueue.push({(newWeight + newMH), {newX, newY}});
            };
        };
    };
};









double ALGOS::manhattanHeuristic(int x1, int x2, int y1, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
};












// ---------------- BENCHMARK -------------------------

int BENCHMARK::pathLength(MAP m) {
    
    int pl = 0;

    for (int x = 0; x < m.NUM_COLUMNS; x++) {
        for (int y = 0; y < m.NUM_ROWS; y++) {
            if (m.mapMatrix[y][x] == '.') {
                pl++;
            };
        }
    }

    return pl;

};


void BENCHMARK::startTimer() {
    start = chrono::high_resolution_clock::now();
};


float BENCHMARK::stopTimer() {
    end = chrono::high_resolution_clock::now();
    duration = end - start;
    float ms = duration.count() * 1000.0f;
    return ms;
}














// ---------------- FLAG MANAGER -------------------------

int FLAGMANAGER::getFlag(string flag) {
    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == flag) {
            return i;
        }
    }
    return -1;
};

string FLAGMANAGER::getMap() {
    int flagIndex = getFlag("-map");
    if (flagIndex != -1) {
        string chosenMap = string(argv[flagIndex + 1]);
        return chosenMap;
    };
    return "../maps/map4.txt";
};

void FLAGMANAGER::showVisited(MAP& m) {
    if (getFlag("--show-visited") != -1) {
        for (int x = 0; x < m.NUM_COLUMNS; x++) {
            for (int y = 0; y < m.NUM_ROWS; y++) {
                if (m.visitedMatrix[y][x] && m.mapMatrix[y][x] != '.') {
                    m.mapMatrix[y][x] = 'v';
                };
            }
        }
    };
};













// ---------------- MAIN -------------------------

int main(int argc, char* argv[]) {    

    MAP workingMap;
    ALGOS algos;
    FLAGMANAGER flagManager(argc, argv);


    workingMap.initMap(flagManager.getMap());
    cout << "Map " << "has " << workingMap.NUM_COLUMNS << " columns, " << workingMap.NUM_ROWS << " rows and " << workingMap.NUM_VERTICES << " vertices." << endl;


    
    workingMap.findStartEnd();
    cout << "Starting point is located at " << workingMap.StartX << "," << workingMap.StartY << endl << endl << "DFS Result:" << endl << endl;






    // Execute DFS

    MAP DFSworkingMap = workingMap;
    BENCHMARK DFSbenchmark;

    DFSbenchmark.startTimer();
    algos.dfs(DFSworkingMap, DFSworkingMap.StartX, DFSworkingMap.StartY);
    float DFStime = DFSbenchmark.stopTimer();
    
    flagManager.showVisited(DFSworkingMap);
    for (string i: DFSworkingMap.mapMatrix) {
        cout << i << endl;
    }





    // Execute BFS

    MAP BFSworkingMap = workingMap;
    BENCHMARK BFSbenchmark;

    BFSbenchmark.startTimer();
    algos.bfs(BFSworkingMap, BFSworkingMap.StartX, BFSworkingMap.StartY);
    float BFStime = BFSbenchmark.stopTimer();

    cout << endl << "BFS Result:" << endl << endl;

    flagManager.showVisited(BFSworkingMap);
    for (string i: BFSworkingMap.mapMatrix) {
        cout << i << endl;
    }





    // Execute Dijkstra

    MAP DIJworkingMap = workingMap;
    BENCHMARK DIJbenchmark;

    DIJbenchmark.startTimer();
    algos.dijkstra(DIJworkingMap, DIJworkingMap.StartX, DIJworkingMap.StartY);
    float DIJtime = DIJbenchmark.stopTimer();

    cout << endl << "Dijkstra Result:" << endl << endl;

    flagManager.showVisited(DIJworkingMap);
    for (string i: DIJworkingMap.mapMatrix) {
        cout << i << endl;
    };




    // Execute A*

    MAP AworkingMap = workingMap;
    BENCHMARK Abenchmark;

    Abenchmark.startTimer();
    algos.astar(AworkingMap, AworkingMap.StartX, AworkingMap.StartY);
    float Atime = Abenchmark.stopTimer();

    cout << endl << "A* Result:" << endl << endl;

    flagManager.showVisited(AworkingMap);
    for (string i: AworkingMap.mapMatrix) {
        cout << i << endl;
    }

    cout << endl << "Benchmark:" << endl << endl << "DFS Path length is " << DFSbenchmark.pathLength(DFSworkingMap) << " and took " << DFStime << " ms to run." << endl
        << "BFS Path length is " << BFSbenchmark.pathLength(BFSworkingMap) << " and took " << BFStime << " ms to run." << endl
        << "Dijkstra Path length is " << DIJbenchmark.pathLength(DIJworkingMap) << " and took " << DIJtime << " ms to run." << endl
        << "A* Path length is " << Abenchmark.pathLength(AworkingMap) << " and took " << Atime << " ms to run." << endl;


    cout << endl;
    return 0;
}