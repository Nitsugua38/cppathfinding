#include <iostream>
#include <fstream>
#include <vector>

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
    public:
        void findStart(MAP& m);
        bool dfs(MAP& m, int CurrentX, int CurrentY);
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

    if (m.mapMatrix[CurrentY][CurrentX] == '#' || m.mapMatrix[CurrentY][CurrentX] == '*' || m.mapMatrix[CurrentY][CurrentX] == 'v') {
        return false;
    };

    if (m.mapMatrix[CurrentY][CurrentX] == 'E') {
        return true;
    };


    m.mapMatrix[CurrentY][CurrentX] = 'v';


    if (dfs(m, CurrentX + 1, CurrentY)) return true;
    if (dfs(m, CurrentX - 1, CurrentY)) return true;
    if (dfs(m, CurrentX, CurrentY + 1)) return true;
    if (dfs(m, CurrentX, CurrentY - 1)) return true;

    return false;

};










// ---------------- MAIN -------------------------

int main() {

    MAP workingMap;
    workingMap.initMap("../maps/map1.txt");



    cout << workingMap.NUM_COLUMNS << endl << workingMap.NUM_ROWS << endl << workingMap.NUM_VERTICES << endl;

    for (string i: workingMap.mapMatrix) {
        cout << i << endl;
    }


    ALGOS algos;
    algos.findStart(workingMap);

    cout << endl << "Starting point is located at " << workingMap.StartX << "," << workingMap.StartY << endl << endl;




    algos.dfs(workingMap, workingMap.StartX, workingMap.StartY);

    for (string i: workingMap.mapMatrix) {
        cout << i << endl;
    }


    return 0;
}