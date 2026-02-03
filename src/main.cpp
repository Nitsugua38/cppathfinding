#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <queue>

using namespace std;




class MAP {
    public:
        int NUM_COLUMNS;
        int NUM_ROWS;
        int NUM_VERTICES;

        int StartX;
        int StartY;

        vector<string> mapMatrix;
        void initMap(string mapPath);
};


class ALGOS {
    private:
        int moveX[4] = {1, -1, 0, 0};
        int moveY[4] = {0, 0, 1, -1};

    public:
        void findStart(MAP& m);
        bool dfs(MAP& m, int CurrentX, int CurrentY);
        void bfs(MAP& m, int CurrentX, int CurrentY);
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








// ---------------- IMPORT MAP FROM TXT FILE -------------------------

void MAP::initMap(string mapPath) {

    ifstream readMap(mapPath);

    if (!readMap) {
        throw runtime_error("Error opening map!");
    }
    string fileLine;


    while (getline(readMap, fileLine)) {
        mapMatrix.push_back(fileLine);
    }

    NUM_COLUMNS = mapMatrix[0].size();
    NUM_ROWS = mapMatrix.size();
    NUM_VERTICES = (NUM_COLUMNS - 2) * (NUM_ROWS - 2);


    readMap.close();

};











// ---------------- FIND MAP STARTING POINT -------------------------

void ALGOS::findStart(MAP& m) {
    
    for (int x = 0; x < m.NUM_COLUMNS; x++) {
        for (int y = 0; y < m.NUM_ROWS; y++) {
            if (m.mapMatrix[y][x] == 'S') {
                m.StartX = x;
                m.StartY = y;
                return;
            };
        }
    }
    
};




// ---------------- DEPTH-FIRST SEARCH -------------------------

bool ALGOS::dfs(MAP& m, int CurrentX, int CurrentY) {

    // Step 1: if blocked/visited -> abandon

    if (m.mapMatrix[CurrentY][CurrentX] == '#' || m.mapMatrix[CurrentY][CurrentX] == '*' || m.mapMatrix[CurrentY][CurrentX] == 'v' || m.mapMatrix[CurrentY][CurrentX] == '.') {
        return false;
    };


    // Step 2: if arrived -> finish

    if (m.mapMatrix[CurrentY][CurrentX] == 'E') {
        return true;
    };


    // Step 3: mark as visited

    if (m.mapMatrix[CurrentY][CurrentX] != 'S') m.mapMatrix[CurrentY][CurrentX] = 'v';


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


            if (m.mapMatrix[newY][newX] != '#' && m.mapMatrix[newY][newX] != '*' && m.mapMatrix[newY][newX] != 'v') {

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
                    m.mapMatrix[newY][newX] = 'v';
                    processQueue.push({newX, newY});
                };
                
            };
        };
    };

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











// ---------------- MAIN -------------------------

int main() {

    MAP workingMap;
    ALGOS algos;

    
    workingMap.initMap("../maps/map11.txt");
    cout << "Imported map has " << workingMap.NUM_COLUMNS << " columns, " << workingMap.NUM_ROWS << " rows and " << workingMap.NUM_VERTICES << " vertices." << endl;


    
    algos.findStart(workingMap);
    cout << "Starting point is located at " << workingMap.StartX << "," << workingMap.StartY << endl << endl << "DFS Result:" << endl << endl;






    // Execute DFS

    MAP DFSworkingMap = workingMap;
    BENCHMARK DFSbenchmark;

    DFSbenchmark.startTimer();
    algos.dfs(DFSworkingMap, DFSworkingMap.StartX, DFSworkingMap.StartY);
    float DFStime = DFSbenchmark.stopTimer();
    
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

    for (string i: BFSworkingMap.mapMatrix) {
        cout << i << endl;
    }

    cout << endl << "Benchmark:" << endl << "DFS Path length is " << DFSbenchmark.pathLength(DFSworkingMap) << " and took " << DFStime << " ms to run." << endl
        << "BFS Path length is " << BFSbenchmark.pathLength(BFSworkingMap) << " and took " << BFStime << " ms to run." << endl;


    cout << endl;
    return 0;
}