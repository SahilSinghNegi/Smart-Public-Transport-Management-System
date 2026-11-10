#ifndef ROUTE_NAVIGATION_H
#define ROUTE_NAVIGATION_H

#include "common.h"

// MEMBER 2 : ROUTE & NAVIGATION MANAGEMENT
class RoadMap {
    string stopNames[Max_Stops];
    int stopCount;
    int roadKm[Max_Stops][Max_Stops];
    int roadMinutes[Max_Stops][Max_Stops];

public:
    RoadMap() : stopCount(0) {
        for (int a = 0; a < Max_Stops; a++) {
            for (int b = 0; b < Max_Stops; b++) {
                roadKm[a][b] = 0;
                roadMinutes[a][b] = 0;
            }
        }
    }

    int getStopCount() const { 
        return stopCount; 
    }
    
    string stopName(int stop) const { 
        return stopNames[stop]; 
    }

    int findStop(const string &name) const {
        for (int i = 0; i < stopCount; i++) {
            if (stopNames[i] == name) {
                return i;
            }
        }
        return -1;
    }

    int addStop(const string &name) {
        int found = findStop(name);
        if (found != -1) {
            return found;
        }
        if (stopCount == Max_Stops) {
            return -1;
        }
        stopNames[stopCount] = name;
        return stopCount++;
    }

    bool addRoad(const string &nameA, const string &nameB, int km, int minutes) {
        if (nameA.empty() || nameB.empty() || nameA == nameB || km <= 0 || minutes <= 0) {
            return false;
        }
        int a = addStop(nameA);
        int b = addStop(nameB);
        
        if (a == -1 || b == -1) {
            return false;
        }
        
        roadKm[a][b] = roadKm[b][a] = km;
        roadMinutes[a][b] = roadMinutes[b][a] = minutes;
        return true;
    }

    int findBestRoute(int start, int end, int path[], int &pathLength, bool useTime) const {
        if (start < 0 || end < 0) {
            return -1;
        }
        
        int bestDistance[Max_Stops];
        int cameFrom[Max_Stops];
        bool alreadyVisited[Max_Stops];
        
        for (int i = 0; i < stopCount; i++) {
            bestDistance[i] = Infinite_Distance;
            cameFrom[i] = -1;
            alreadyVisited[i] = false;
        }
        
        bestDistance[start] = 0;

        for (int round = 0; round < stopCount; round++) {
            int current = -1;
            for (int i = 0; i < stopCount; i++) {
                if (!alreadyVisited[i] && bestDistance[i] < Infinite_Distance &&
                    (current == -1 || bestDistance[i] < bestDistance[current])) {
                    current = i;
                }
            }
            
            if (current == -1 || current == end) {
                break;
            }
            
            alreadyVisited[current] = true;

            for (int next = 0; next < stopCount; next++) {
                int roadCost;
                if (useTime) {
                    roadCost = roadMinutes[current][next];
                } else {
                    roadCost = roadKm[current][next];
                }
                
                if (roadCost > 0 && !alreadyVisited[next] &&
                    bestDistance[current] + roadCost < bestDistance[next]) {
                    bestDistance[next] = bestDistance[current] + roadCost;
                    cameFrom[next] = current;
                }
            }
        }

        if (bestDistance[end] == Infinite_Distance) {
            return -1;
        }

        int backwards[Max_Stops];
        int length = 0;
        
        for (int stop = end; stop != -1; stop = cameFrom[stop]) {
            backwards[length++] = stop;
        }
        
        pathLength = length;
        for (int i = 0; i < length; i++) {
            path[i] = backwards[length - 1 - i];
        }
        
        return bestDistance[end];
    }

    string stopsToText(const int stops[], int length) const {
        string text = "";
        for (int i = 0; i < length; i++) {
            text += stopNames[stops[i]];
            if (i + 1 < length) {
                text += " -> ";
            }
        }
        return text;
    }

    void showMap() const {
        if (stopCount == 0) {
            cout << "  No stops yet.\n";
        }
        
        for (int a = 0; a < stopCount; a++) {
            cout << "  " << stopNames[a] << ": ";
            for (int b = 0; b < stopCount; b++) {
                if (roadKm[a][b] > 0) {
                    cout << stopNames[b] << "(" << roadKm[a][b] << "km," << roadMinutes[a][b] << "min) ";
                }
            }
            cout << "\n";
        }
    }
};
#endif
