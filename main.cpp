/*
 * Smart Public Transport Management and Route Optimization System
 * 
 * MEMBER 1 - Bus & Vehicle Management
 *    Role : Look after the buses and their drivers.
 *    Uses : Classes, inheritance, encapsulation, custom Hash Table (chaining) for fast bus search.
 * MEMBER 2 - Route & Navigation Management
 *    Role : Keep the road map and find the best route between two stops.
 *    Uses : Graph (adjacency matrix), Dijkstra's algorithm written with arrays.
 * MEMBER 3 - Passenger & Boarding Management
 *    Role : Register passengers and decide who boards the bus first.
 *    Uses : Custom circular Queue, custom Priority Queue (max-heap), inheritance.
 * MEMBER 4 - Schedule, Ticket & System Integration
 *    Role : Clash-free timetable, tickets and fares, saving data to file.
 *    Uses : Conflict checking, custom sorting, file handling (ofstream), integration.
 */

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#ifdef _WIN32
#include <conio.h>
#endif

using namespace std;

const int Max_Buses = 20;
const int Max_Drivers = 20;
const int Max_Passengers = 50;
const int Max_Stops = 20;
const int Max_Route = 10;
const int Max_Trips = 50;
const int Max_Tickets = 200;
const int Queue_Size = 50;
const int Hash_Size = 11;
const int Infinite_Distance = 1000000000;

string numberToText(int number) {
    if (number == 0) {
        return "0";
    }
    
    string text = "";
    bool isNegative = number < 0;
    
    if (isNegative) {
        number = -number;
    }
    
    while (number > 0) {
        text = char('0' + number % 10) + text;
        number = number / 10;
    }
    
    if (isNegative) {
        return "-" + text;
    }
    
    return text;
}

string twoDigits(int number) {
    string result = numberToText(number);
    if (number < 10) {
        result = "0" + result;
    }
    return result;
}

string minutesToClock(int minutes) {
    int hours = (minutes % 1440) / 60;
    int remainingMinutes = minutes % 60;
    return twoDigits(hours) + ":" + twoDigits(remainingMinutes);
}

string moneyToText(int paise) {
    int rupees = paise / 100;
    int remainingPaise = paise % 100;
    return numberToText(rupees) + "." + twoDigits(remainingPaise);
}

string cutSpaces(const string &text) {
    size_t first = text.find_first_not_of(" \t\r\n");
    if (first == string::npos) {
        return "";
    }
    return text.substr(first, text.find_last_not_of(" \t\r\n") - first + 1);
}

string askText(const string &question) {
    string answer;
    cout << question;
    if (!getline(cin, answer)) { 
        cout << "\n(input finished)\n"; 
        exit(0); 
    }
    return cutSpaces(answer);
}

bool textToNumber(const string &text, int &number) {
    if (text.empty() || text.size() > 9) {
        return false;
    }
    
    number = 0;
    for (size_t i = 0; i < text.size(); i++) {
        if (text[i] < '0' || text[i] > '9') {
            return false;
        }
        number = number * 10 + (text[i] - '0');
    }
    return true;
}

int askNumber(const string &question) {
    while (true) {
        int number;
        if (textToNumber(askText(question), number)) {
            return number;
        }
        cout << "  Please type a valid number.\n";
    }
}

bool clockToMinutes(const string &text, int &minutes) {
    size_t colon = text.find(':');
    int hours, mins;
    
    if (colon == string::npos || !textToNumber(text.substr(0, colon), hours) || !textToNumber(text.substr(colon + 1), mins)) {
        return false;
    }
    
    if (hours > 23 || mins > 59) {
        return false;
    }
    
    minutes = hours * 60 + mins;
    return true;
}

int splitByComma(const string &text, string parts[], int maxParts) {
    int count = 0;
    string current = "";
    
    for (size_t i = 0; i <= text.size(); i++) {
        if (i == text.size() || text[i] == ',') {
            if (count < maxParts) {
                parts[count] = cutSpaces(current);
            }
            count++;
            current = "";
        } else {
            current += text[i];
        }
    }
    return count;
}

// SCREEN HELPERS 
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

string repeatChar(char symbol, int count) {
    string text = "";
    for (int i = 0; i < count; i++) {
        text += symbol;
    }
    return text;
}

void printTitle(const string &title) {
    const int width = 56;
    int space = (width - (int)title.size()) / 2;
    if (space < 0) {
        space = 0;
    }
       cout << "\n  " << repeatChar(' ', space) << title << "\n";
}

void printSection(const string &name) {
    int dashes = 50 - (int)name.size();
    if (dashes < 3) {
        dashes = 3;
    }
    cout << "\n   " << name << " " << repeatChar(' ', dashes) << "\n";
}

void printOption(int number, const string &text) {
    cout << "     [";
    if (number < 10) {
        cout << " ";
    }
    cout << number << "]  " << text << "\n";
}

void pauseScreen() {
    askText("\n  Press Enter to continue...");
}

// Reads a password.
string askPassword(const string &question) {
#ifdef _WIN32
    cout << question;
    string password = "";
    while (true) {
        int key = _getch();
        if (key == '\r' || key == '\n') {
            cout << "\n";
            break;
        }
        if (key == 0 || key == 224) {
            _getch();
            continue;
        }
        if (key == 8) {
            if (!password.empty()) {
                password.erase(password.size() - 1);
                cout << "\b \b";
            }
        } else if (key >= 32 && key <= 126) {
            password += (char)key;
            cout << '*';
        }
    }
    return password;
#else
    return askText(question);
#endif
}


// SHARED PARENT CLASS (Member 1 & Member 3)
class Person {
protected:
    int id;
    string name;

public:
    Person() : id(0), name("") {}
    
    Person(int personId, const string &personName) : id(personId), name(personName) {}
    
    virtual ~Person() {}
    
    int getId() const { 
        return id; 
    }
    
    string getName() const { 
        return name; 
    }
    
    virtual void showDetails() const = 0;
};

// MEMBER 1 : BUS & VEHICLE MANAGEMENT
class Driver : public Person {
    string licenseNumber;
    int busId;

public:
    Driver() : Person(), licenseNumber(""), busId(-1) {}
    
    Driver(int driverId, const string &driverName, const string &license)
        : Person(driverId, driverName), licenseNumber(license), busId(-1) {}
    
    void setBus(int newBusId) { 
        busId = newBusId; 
    }
    
    int getBusId() const { 
        return busId; 
    }

    string getLicenseNumber() const
    {
        return licenseNumber;
    }
    
    void showDetails() const override { 
        cout << "  Driver " << name << " (" << licenseNumber << ")\n"; 
    }
};

enum BusStatus { AVAILABLE, RUNNING, MAINTENANCE };

string statusToText(int status) {
    if (status == AVAILABLE) {
        return "Available";
    }
    if (status == RUNNING) {
        return "Running";
    }
    return "Maintenance";
}

class Bus {
    int busId, totalSeats, seatsTaken, status, driverId, routeLength;
    string busNumber;
    int routeStops[Max_Route];

public:
    Bus() : busId(0), totalSeats(0), seatsTaken(0), status(AVAILABLE), driverId(-1), routeLength(0), busNumber("") {}
    
    Bus(int newBusId, const string &number, int seats)
        : busId(newBusId), totalSeats(seats), seatsTaken(0), status(AVAILABLE), driverId(-1), routeLength(0), busNumber(number) {}
    
    int getId() const { 
        return busId; 
    }
    
    string getNumber() const { 
        return busNumber; 
    }
    
    int getTotalSeats() const { 
        return totalSeats; 
    }
    
    int getSeatsTaken() const { 
        return seatsTaken; 
    }
    
    int seatsFree() const { 
        return totalSeats - seatsTaken; 
    }
    
    int getStatus() const { 
        return status; 
    }
    
    int getDriverId() const { 
        return driverId; 
    }
    
    int getRouteLength() const { 
        return routeLength; 
    }
    int getRouteStop(int position) const { 
        return routeStops[position]; 
    }
    
    void setDriver(int newDriverId) { 
        driverId = newDriverId; 
    }
    
    void setStatus(int newStatus) {
        if (newStatus >= AVAILABLE && newStatus <= MAINTENANCE) {
            status = newStatus;
        } else {
            cout << "Invalid bus status!\n";
        }
    }

    void takeOneSeat() { 
        seatsTaken++; 
    }
    void freeAllSeats() { 
        seatsTaken = 0; 
    }
    
    void setRoute(const int stops[], int length) {
        routeLength = length;
        for (int i = 0; i < length; i++) {
            routeStops[i] = stops[i];
        }
    }

    bool goesFromTo(int fromStop, int toStop) const {
        int fromPosition = -1, toPosition = -1;
        for (int i = 0; i < routeLength; i++) {
            if (routeStops[i] == fromStop && fromPosition == -1) {
                fromPosition = i;
            }
            if (routeStops[i] == toStop) {
                toPosition = i;
            }
        }
        return fromPosition != -1 && toPosition != -1 && fromPosition < toPosition;
    }
};

struct HashNode {
    string busNumber;
    int busId;
    HashNode *next;
};

class BusHashTable {
    HashNode *slots[Hash_Size];

    int hashOf(const string &text) const {
        int value = 0;
        for (size_t i = 0; i < text.size(); i++) {
            value = (value * 31 + (unsigned char)text[i]) % Hash_Size;
        }
        return value;
    }

public:
    BusHashTable() {
        for (int i = 0; i < Hash_Size; i++) {
            slots[i] = nullptr;
        }
    }
    
    BusHashTable(const BusHashTable &) = delete;
    BusHashTable &operator=(const BusHashTable &) = delete;
    
    ~BusHashTable() {
        for (int i = 0; i < Hash_Size; i++) {
            while (slots[i] != nullptr) {
                HashNode *old = slots[i];
                slots[i] = old->next;
                delete old;
            }
        }
    }

    void put(const string &busNumber, int busId) {
        int slot = hashOf(busNumber);
        slots[slot] = new HashNode{busNumber, busId, slots[slot]};
    }

    int get(const string &busNumber) const {
        for (HashNode *node = slots[hashOf(busNumber)]; node != nullptr; node = node->next) {
            if (node->busNumber == busNumber) {
                return node->busId;
            }
        }
        return -1;
    }
};

class BusManager {
    Bus buses[Max_Buses];
    Driver drivers[Max_Drivers];
    int busCount = 0, driverCount = 0;
    BusHashTable plateTable;

public:
    Bus *findBusById(int id) {
        if (id < 1 || id > busCount) {
            return nullptr;
        }
        return &buses[id - 1];
    }

    Bus *findBusByNumber(const string &number) {
        int id = plateTable.get(number);
        if (id < 0) {
            return nullptr;
        }
        return findBusById(id);
    }

    Driver *findDriver(int id) {
        if (id < 1 || id > driverCount) {
            return nullptr;
        }
        return &drivers[id - 1];
    }

    int addBus(const string &number, int seats) {
    if (number.empty()) {
        return -3;
    }

    if (seats <= 0) {
        return -4;
    }

    if (plateTable.get(number) != -1) {
        return -1;
    }

    if (busCount == Max_Buses) {
        return -2;
    }

    buses[busCount] = Bus(busCount + 1, number, seats);
    busCount++;
    plateTable.put(number, busCount);

    return busCount;
}
 int addDriver(const string &driverName, const string &license) {

    if (driverName.empty()) {
        return -2;
    }

    if (license.empty()) {
        return -3;
    }

    for (int i = 0; i < driverCount; i++) {
        if (drivers[i].getLicenseNumber() == license) {
            return -4;
        }
    }

    if (driverCount == Max_Drivers) {
        return -1;
    }

    drivers[driverCount] =
        Driver(driverCount + 1, driverName, license);

    driverCount++;

    return driverCount;
}
    bool assignDriver(int driverId, int busId) {
        Driver *driver = findDriver(driverId);
        Bus *bus = findBusById(busId);
        
        if (driver == nullptr || bus == nullptr) {
            return false;
        }
        
        Bus *oldBus = findBusById(driver->getBusId());
        if (oldBus != nullptr) {
            oldBus->setDriver(-1);
        }
        
        Driver *oldDriver = findDriver(bus->getDriverId());
        if (oldDriver != nullptr) {
            oldDriver->setBus(-1);
        }
        
        driver->setBus(busId);
        bus->setDriver(driverId);
        return true;
    }

    int getBusCount() const { 
        return busCount; 
    }

    void showBus(const Bus &bus, const string &routeText)
    {
        Driver *driver = findDriver(bus.getDriverId());
        string driverName = "none";
        
        if (driver != nullptr) {
            driverName = driver->getName();
        }

        cout << "  Bus #" << bus.getId() << " | " << bus.getNumber() << " | " << statusToText(bus.getStatus()) << " | seats "
             << bus.getSeatsTaken() << "/" << bus.getTotalSeats() << " | driver: "
             << driverName << "\n    route: " << routeText << "\n";
    }

    void saveBuses() const
    {
        ofstream file("buses.txt");

        if (!file)
        {
            cout << "Unable to save buses.\n";
            return;
        }

        for (int i = 0; i < busCount; i++) 
        {
            file << buses[i].getId() << ","
                << buses[i].getNumber() << ","
                << buses[i].getTotalSeats() << ","
                << buses[i].getStatus() << ","
                << buses[i].getDriverId() << ","
                << buses[i].getRouteLength();

        for (int j = 0; j < buses[i].getRouteLength(); j++)
        {
            file << "," << buses[i].getRouteStop(j);
        }
        file << "\n";
        }
    } 
    void loadBuses()
    {
    ifstream file("buses.txt");

    if (!file)
    {
        return;
    }

    string line;

    while (getline(file, line))
    {
        stringstream ss(line);

        int id, seats, status, driverId, routeLength;
        string number;
        char comma;

        ss >> id >> comma;
        getline(ss, number, ',');

        ss >> seats >> comma
           >> status >> comma
           >> driverId >> comma
           >> routeLength;

        if (busCount == Max_Buses ||
            routeLength < 0 || routeLength > Max_Route ||
            id != busCount + 1)
        {
            continue;
        }

        int stops[Max_Route];

        for (int i = 0; i < routeLength; i++)
        {
            ss >> comma >> stops[i];
        }

        int newId = addBus(number, seats);

        if (newId == -1 || newId == -2)
        {
            continue;
        }

        Bus *bus = findBusById(newId);

        bus->setStatus(status);
        bus->setDriver(driverId);
        bus->setRoute(stops, routeLength);
    }
    }
    void saveDrivers() const
    {
        ofstream file("drivers.txt");

        if (!file)
        {
            cout << "Unable to save drivers.\n";
            return;
        }

        for (int i = 0; i < driverCount; i++)
        {
            file << drivers[i].getId() << ","
                 << drivers[i].getName() << ","
                << drivers[i].getLicenseNumber() << ","
                << drivers[i].getBusId() << "\n";
        }
    }
    void loadDrivers()
    {
        ifstream file("drivers.txt");

        if (!file)
        {
            return;
        }

        string line;

        while (getline(file, line))
        {
            stringstream ss(line);

            int id, busId;
            string driverName, license;

            getline(ss, line, ',');

            if (!textToNumber(line, id))
            {
                continue;
            }

            getline(ss, driverName, ',');
            getline(ss, license, ',');

            if (!(ss >> busId))
            {
                continue;
            }

            if (driverCount == Max_Drivers || id != driverCount + 1)
            {   
                continue;
            }

            int newId = addDriver(driverName, license);

            if (newId != -1)
            {
                drivers[newId - 1].setBus(busId);
            }
        }   
    }
};

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

     bool updateRoad(const string &nameA, const string &nameB, int km, int minutes) {
        int a = findStop(nameA);
        int b = findStop(nameB);
        if (a == -1 || b == -1 || a == b || km <= 0 || minutes <= 0) {
            return false;
        }
        if (roadKm[a][b] == 0) {
            return false;
        }
        roadKm[a][b] = roadKm[b][a] = km;
        roadMinutes[a][b] = roadMinutes[b][a] = minutes;
        return true;
    }
      int countRoads() const {
        int count = 0;

        for (int i = 0; i < stopCount; i++) {
            for (int j = i + 1; j < stopCount; j++) {
                if (roadKm[i][j] > 0) {
                    count++;
                }
            }
        }
        return count;
    }
      void showConnections(const string &name) const {
        int stop = findStop(name);

        if (stop == -1) {
            cout << "  Stop not found.\n";
            return;
        }

        cout << "  Connections of " << name << ":\n";

        bool found = false;

        for (int i = 0; i < stopCount; i++) {
            if (roadKm[stop][i] > 0) {
                cout << "  -> " << stopNames[i]
                     << " | " << roadKm[stop][i] << " km"
                     << " | " << roadMinutes[stop][i] << " minutes\n";

                found = true;
            }
        }

        if (!found) {
            cout << "  No connected roads.\n";
        }
    }
     int getDirectDistance(const string &nameA, const string &nameB) const {
        int a = findStop(nameA);
        int b = findStop(nameB);

        if (a == -1 || b == -1 || a == b || roadKm[a][b] == 0) {
            return -1;
        }

        return roadKm[a][b];
    }
    int getDirectTime(const string &nameA, const string &nameB) const {
        int a = findStop(nameA);
        int b = findStop(nameB);

        if (a == -1 || b == -1 || a == b || roadMinutes[a][b] == 0) {
            return -1;
        }

        return roadMinutes[a][b];
    }
    
     bool removeRoad(const string &nameA, const string &nameB) {
        int a = findStop(nameA);
        int b = findStop(nameB);

        if (a == -1 || b == -1 || a == b) {
            return false;
        }

        if (roadKm[a][b] == 0) {
            return false;
        }
        roadKm[a][b] = roadKm[b][a] = 0;
        roadMinutes[a][b] = roadMinutes[b][a] = 0;

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
    
void showReachableStops(const string &name) const {
    int start = findStop(name);

    if (start == -1) {
        cout << "  Stop not found.\n";
        return;
    }

    bool visited[Max_Stops] = {};
    int queue[Max_Stops];
    int front = 0, rear = 0;

    queue[rear++] = start;
    visited[start] = true;

    cout << "\n  Stops reachable from " << name << ":\n";

    while (front < rear) {
        int current = queue[front++];

        for (int next = 0; next < stopCount; next++) {
            if (roadKm[current][next] > 0 && !visited[next]) {
                visited[next] = true;
                queue[rear++] = next;

                cout << "  -> " << stopNames[next] << "\n";
            }
        }
    }

    if (rear == 1) {
        cout << "  No other stops are reachable.\n";
    }
}


    void saveRoadMap() const
    {
        ofstream file("routes.txt");
        if(!file)
        {
            cout << "Unable to save routes.\n";
            return;
        }
        file << stopCount << "\n";
        for(int i = 0; i < stopCount; i++)
        {
            file << stopNames[i] << "\n";
        }

        for(int i = 0; i < stopCount; i++)
        {
            for(int j = i + 1; j < stopCount; j++)
            {
                if (roadKm[i][j] > 0)
                {
                    file << i << ","
                         << j << ","
                         << roadKm[i][j] << ","
                         << roadMinutes[i][j] << "\n";
                }
            }
        }
    }

    void loadRoadMap()
    {
        ifstream file("routes.txt");

        if(!file)
        {
            return;
        }
        file >> stopCount;
        file.ignore();
        if(stopCount < 0 || stopCount > Max_Stops)
        {
            stopCount = 0;
            return;
        }

        for (int i = 0; i < stopCount; i++)
        {
            getline(file, stopNames[i]);
        } 
        int a, b, km, minutes;
        while(file >> a)
        {
            char comma;
            file >> comma >> b
                 >> comma >> km
                 >> comma >> minutes;
            if (a >= 0 && a < stopCount &&
                b >= 0 && b < stopCount &&
                a != b && km > 0 && minutes > 0)
            {
                roadKm[a][b] = roadKm[b][a] = km;
                roadMinutes[a][b] = roadMinutes[b][a] = minutes;
            }
        }  
    }
};

// MEMBER 3 : PASSENGER & BOARDING MANAGEMENT
class Passenger : public Person {
    int age;
    bool wantsPriority;

public:
    Passenger() : Person(), age(0), wantsPriority(false) {}
    
    Passenger(int passengerId, const string &passengerName, int passengerAge, bool priority)
        : Person(passengerId, passengerName), age(passengerAge), wantsPriority(priority) {}
        
        int getAge() const
    {
        return age;
    }

    bool hasPriority() const
    {
        return wantsPriority;
    }
    
    bool isChildOrSenior() const { 
        return age <= 12 || age >= 60; 
    }
    
    bool getsHalfFare() const { 
        return isChildOrSenior(); 
    }
    
    int priorityLevel() const {
        if (wantsPriority) {
            return 2;
        }
        if (isChildOrSenior()) {
            return 1;
        }
        return 0;
    }

    void showDetails() const override { 
        cout << "  Passenger " << name << ", age " << age << "\n"; 
    }
};

struct BoardingRequest {
    int passengerId, fromStop, toStop;
    int level, arrivalNumber;
};

class NormalQueue {
    BoardingRequest items[Queue_Size];
    int frontPosition = 0;
    int itemCount = 0;

public:
    int size() const { 
        return itemCount; 
    }
    
    bool push(const BoardingRequest &request) {
        if (itemCount == Queue_Size) {
            return false;
        }
        items[(frontPosition + itemCount) % Queue_Size] = request;
        itemCount++;
        return true;
    }

    BoardingRequest pop() {
        BoardingRequest request = items[frontPosition];
        frontPosition = (frontPosition + 1) % Queue_Size;
        itemCount--;
        return request;
    }
bool removePassenger(int passengerId) {
    BoardingRequest temp[Queue_Size];
    int newCount = 0;

    for (int i = 0; i < itemCount; i++) {
        BoardingRequest request =
            items[(frontPosition + i) % Queue_Size];

        if (request.passengerId != passengerId) {
            temp[newCount++] = request;
        }
    }

    if (newCount == itemCount)
        return false;

    frontPosition = 0;
    itemCount = newCount;

    for (int i = 0; i < newCount; i++) {
        items[i] = temp[i];
    }

    return true;
}
};

class PriorityHeap {
    BoardingRequest items[Queue_Size];
    int itemCount = 0;

    static bool boardsBefore(const BoardingRequest &a, const BoardingRequest &b) {
        if (a.level != b.level) {
            return a.level > b.level;
        }
        return a.arrivalNumber < b.arrivalNumber;
    }

    void swapItems(int first, int second) {
        BoardingRequest temporary = items[first];
        items[first] = items[second];
        items[second] = temporary;
    }

public:
    int size() const { 
        return itemCount; 
    }

    bool push(const BoardingRequest &request) {
        if (itemCount == Queue_Size) {
            return false;
        }
        
        int position = itemCount++;
        items[position] = request;
        
        while (position > 0 && boardsBefore(items[position], items[(position - 1) / 2])) {
            swapItems(position, (position - 1) / 2);
            position = (position - 1) / 2;
        }
        return true;
    }

    BoardingRequest pop() {
        BoardingRequest top = items[0];
        items[0] = items[--itemCount];
        int position = 0;
        
        while (true) {
            int left = 2 * position + 1;
            int right = left + 1;
            int best = position;
            
            if (left < itemCount && boardsBefore(items[left], items[best])) {
                best = left;
            }
            if (right < itemCount && boardsBefore(items[right], items[best])) {
                best = right;
            }
            if (best == position) {
                break;
            }
            
            swapItems(position, best);
            position = best;
        }
        return top;
    }
bool removePassenger(int passengerId) {
    int position = -1;

    for (int i = 0; i < itemCount; i++) {
        if (items[i].passengerId == passengerId) {
            position = i;
            break;
        }
    }

    if (position == -1)
        return false;

    itemCount--;

    if (position == itemCount)
        return true;

    items[position] = items[itemCount];

    while (position > 0 &&
           boardsBefore(items[position], items[(position - 1) / 2])) {
        swapItems(position, (position - 1) / 2);
        position = (position - 1) / 2;
    }

    while (true) {
        int left = 2 * position + 1;
        int right = left + 1;
        int best = position;

        if (left < itemCount &&
            boardsBefore(items[left], items[best]))
            best = left;

        if (right < itemCount &&
            boardsBefore(items[right], items[best]))
            best = right;

        if (best == position)
            break;

        swapItems(position, best);
        position = best;
    }

    return true;
}
};

struct WaitingList {
    NormalQueue normalQueue;
    PriorityHeap priorityQueue;
};

class PassengerDesk {
    Passenger passengers[Max_Passengers];
    WaitingList waitingLists[Max_Buses];
    int passengerCount = 0;
    int arrivalCounter = 0;
int priorityStreak[Max_Buses] = {};
bool isWaiting[Max_Passengers + 1][Max_Buses + 1] = {};

public:
    int registerPassenger(const string &passengerName, int age, bool priority) {
        if (passengerCount == Max_Passengers) {
            return -1;
        }
        passengers[passengerCount] = Passenger(passengerCount + 1, passengerName, age, priority);
        return ++passengerCount;
    }

    void savePassengers() const
    {
        ofstream file("passengers.txt");

        if(!file)
        {
            cout << "Unable to save passengers.\n";
            return;
        }

        for(int i = 0; i < passengerCount; i++)
        {
            file << passengers[i].getId() << ","
                 << passengers[i].getName() << ","
                << passengers[i].getAge() << ","
                << passengers[i].hasPriority() << "\n";
        }
    }

    void loadPassengers()
    {
        ifstream file("passengers.txt");

        if (!file)
            return;
        string line;
        while(getline(file, line))
        {
            stringstream ss(line);
            string idText, passengerName, ageText, priorityText;
            int id, age, priority;
            if (!getline(ss, idText, ',') ||
                !getline(ss, passengerName, ',') ||
                !getline(ss, ageText, ',') ||
                !getline(ss, priorityText))
                continue;

            if(!textToNumber(idText, id) ||
                !textToNumber(ageText, age) ||
                !textToNumber(priorityText, priority))
                continue;

            if(id != passengerCount + 1 ||
                passengerCount == Max_Passengers ||
                passengerName.empty() ||
                age <= 0 || age > 120 ||
                (priority != 0 && priority != 1))
                continue;

            registerPassenger(passengerName, age, priority == 1);
        }
    }
    int getPassengerCount() const {
        return passengerCount;
    }
    Passenger *findPassenger(int id) {
        if (id < 1 || id > passengerCount) {
            return nullptr;
        }
        return &passengers[id - 1];
    }

    int peopleWaiting(int busId) const {
        return waitingLists[busId - 1].priorityQueue.size() + waitingLists[busId - 1].normalQueue.size();
    }
bool cancelWaitingRequest(int busId, int passengerId) {
    if (busId < 1 || busId > Max_Buses ||
        passengerId < 1 || passengerId > passengerCount)
        return false;

    if (!isWaiting[passengerId][busId])
        return false;

    Passenger &passenger = passengers[passengerId - 1];
    WaitingList &list = waitingLists[busId - 1];

    bool removed;

    if (passenger.priorityLevel() > 0)
        removed = list.priorityQueue.removePassenger(passengerId);
    else
        removed = list.normalQueue.removePassenger(passengerId);

    if (removed)
        isWaiting[passengerId][busId] = false;

    return removed;
}
   int joinWaitingList(int busId, const Passenger &passenger, int fromStop, int toStop) {
    if (busId < 1 || busId > Max_Buses ||
        passenger.getId() < 1 || passenger.getId() > passengerCount) {
        return -1;
        int getWaitingPosition(int busId, int passengerId) {

    if (busId < 1 || busId > Max_Buses ||
        passengerId < 1 || passengerId > passengerCount) {
        return -1;
    }

    if (!isWaiting[passengerId][busId]) {
        return -1;
    }

    WaitingList &list = waitingLists[busId - 1];

    int position = 1;

    // Check priority queue
    for (int i = 0; i < list.priorityQueue.size(); i++) {
        if (list.priorityQueue.getPassengerId(i) == passengerId) {
            return position;
        }
        position++;
    }

    // Check normal queue
    for (int i = 0; i < list.normalQueue.size(); i++) {
        if (list.normalQueue.getPassengerId(i) == passengerId) {
            return position;
        }
        position++;
    }

    return -1;
}
    }

    if (isWaiting[passenger.getId()][busId]) {
        return -2;
    }

    BoardingRequest request = {
        passenger.getId(), fromStop, toStop,
        passenger.priorityLevel(), ++arrivalCounter
    };

    WaitingList &list = waitingLists[busId - 1];
    bool added;

    if (request.level > 0) {
        added = list.priorityQueue.push(request);
    } else {
        added = list.normalQueue.push(request);
    }

    if (!added) {
        return -1;
    }

    isWaiting[passenger.getId()][busId] = true;
    return peopleWaiting(busId);
}

   BoardingRequest callNextPassenger(int busId) {
    WaitingList &list = waitingLists[busId - 1];

    BoardingRequest request;

    if (list.priorityQueue.size() > 0) {
        request = list.priorityQueue.pop();
    } else {
        request = list.normalQueue.pop();
    }

    isWaiting[request.passengerId][busId] = false;
    return request;
}
void displayQueueStatus(int busId) {
    if (busId < 1 || busId > Max_Buses) {
        cout << "Invalid Bus ID!\n";
        return;
    }

    int normal = waitingLists[busId - 1].normalQueue.size();
    int priority = waitingLists[busId - 1].priorityQueue.size();

    cout << "\n========== QUEUE STATUS ==========\n";
    cout << "Bus ID: " << busId << "\n";
    cout << "Normal Queue: " << normal << "\n";
    cout << "Priority Queue: " << priority << "\n";
    cout << "Total Waiting: " << normal + priority << "\n";
    cout << "==================================\n";

void generatePassengerReport() {
    if (passengerCount == 0) {
        cout << "\nNo passengers registered yet!\n";
        return;
    }

    int children = 0;
    int seniorCitizens = 0;
    int adults = 0;
    int priorityPassengers = 0;
    int normalPassengers = 0;
    int halfFarePassengers = 0;

    cout << "\n========== PASSENGER ANALYTICS REPORT ==========\n";

    cout << left
         << setw(12) << "ID"
         << setw(20) << "NAME"
         << setw(10) << "AGE"
         << setw(15) << "CATEGORY"
         << setw(15) << "PRIORITY"
         << "FARE\n";

    cout << string(85, '-') << "\n";

    for (int i = 0; i < passengerCount; i++) {
        int age = passengers[i].getAge();
        bool priority = passengers[i].hasPriority();

        string category;
        string fare;

        if (age <= 12) {
            category = "Child";
            children++;
            halfFarePassengers++;
            fare = "Half Fare";
        }
        else if (age >= 60) {
            category = "Senior";
            seniorCitizens++;
            halfFarePassengers++;
            fare = "Half Fare";
        }
        else {
            category = "Adult";
            adults++;
            fare = "Full Fare";
        }

        if (priority) {
            priorityPassengers++;
        }
        else {
            normalPassengers++;
        }

        cout << left
             << setw(12) << passengers[i].getId()
             << setw(20) << passengers[i].getName()
             << setw(10) << age
             << setw(15) << category
             << setw(15) << (priority ? "Priority" : "Normal")
             << fare << "\n";
    }

    cout << "\n========== SUMMARY ==========\n";
    cout << "Total Passengers: " << passengerCount << "\n";
    cout << "Children: " << children << "\n";
    cout << "Senior Citizens: " << seniorCitizens << "\n";
    cout << "Adults: " << adults << "\n";
    cout << "Priority Passengers: " << priorityPassengers << "\n";
    cout << "Normal Passengers: " << normalPassengers << "\n";
    cout << "Half Fare Eligible: " << halfFarePassengers << "\n";
    cout << "=============================\n";
}
    int getWaitingPosition(int busId, int passengerId) {

    if (busId < 1 || busId > Max_Buses ||
        passengerId < 1 || passengerId > passengerCount) {
        return -1;
    }

    if (!isWaiting[passengerId][busId]) {
        return -1;
    }

    WaitingList &list = waitingLists[busId - 1];

    Passenger *currentPassenger = findPassenger(passengerId);

    if (currentPassenger == nullptr) {
        return -1;
    }

    int position = 1;

    // Check priority queue
    if (currentPassenger->priorityLevel() > 0) {

        for (int i = 0; i < list.priorityQueue.size(); i++) {

            if (list.priorityQueue.getPassengerId(i) == passengerId) {
                return position;
            }

            position++;
        }
    }

    // Check normal queue
    else {

        for (int i = 0; i < list.normalQueue.size(); i++) {

            if (list.normalQueue.getPassengerId(i) == passengerId) {
                return position;
            }

            position++;
        }
    }

    return -1;
}
    int getTotalWaitingPassengers(int busId) {

    if (busId < 1 || busId > Max_Buses) {
        return -1;
    }

    WaitingList &list = waitingLists[busId - 1];

    int priorityCount = list.priorityQueue.size();
    int normalCount = list.normalQueue.size();

    int totalPassengers = priorityCount + normalCount;

    return totalPassengers;
}
    void displayAllWaitingSummary() {
    cout << "\n========== COMPLETE WAITING REPORT ==========\n";

    int grandTotal = 0;
    int busesWithPassengers = 0;
    int emptyBuses = 0;

    for (int busId = 1; busId <= Max_Buses; busId++) {

        WaitingList &list = waitingLists[busId - 1];

        int priorityCount = list.priorityQueue.size();
        int normalCount = list.normalQueue.size();

        int totalCount = priorityCount + normalCount;

        cout << "\n-----------------------------------\n";
        cout << "Bus ID: " << busId << "\n";

        cout << "Priority Queue: " << priorityCount << "\n";
        cout << "Normal Queue  : " << normalCount << "\n";
        cout << "Total Waiting : " << totalCount << "\n";

        if (totalCount > 0) {
            cout << "Status: Passengers Waiting\n";
            busesWithPassengers++;
        }
        else {
            cout << "Status: No Passengers Waiting\n";
            emptyBuses++;
        }

        if (totalCount >= Queue_Size) {
            cout << "Warning: Waiting Queue is Full!\n";
        }

        grandTotal += totalCount;
    }

    cout << "\n========== FINAL SUMMARY ==========\n";
    cout << "Total Waiting Passengers : " << grandTotal << "\n";
    cout << "Buses With Passengers    : " << busesWithPassengers << "\n";
    cout << "Empty Buses              : " << emptyBuses << "\n";
    cout << "===================================\n";
}
    void displayAllWaitingSummary() {
    cout << "\n========== COMPLETE WAITING REPORT ==========\n";

    int grandTotal = 0;
    int busesWithPassengers = 0;
    int emptyBuses = 0;

    for (int busId = 1; busId <= Max_Buses; busId++) {

        WaitingList &list = waitingLists[busId - 1];

        int priorityCount = list.priorityQueue.size();
        int normalCount = list.normalQueue.size();

        int totalCount = priorityCount + normalCount;

        cout << "\n-----------------------------------\n";
        cout << "Bus ID: " << busId << "\n";

        cout << "Priority Queue: " << priorityCount << "\n";
        cout << "Normal Queue  : " << normalCount << "\n";
        cout << "Total Waiting : " << totalCount << "\n";

        if (totalCount > 0) {
            cout << "Status: Passengers Waiting\n";
            busesWithPassengers++;
        }
        else {
            cout << "Status: No Passengers Waiting\n";
            emptyBuses++;
        }

        if (totalCount >= Queue_Size) {
            cout << "Warning: Waiting Queue is Full!\n";
        }

        grandTotal += totalCount;
    }

    cout << "\n========== FINAL SUMMARY ==========\n";
    cout << "Total Waiting Passengers : " << grandTotal << "\n";
    cout << "Buses With Passengers    : " << busesWithPassengers << "\n";
    cout << "Empty Buses              : " << emptyBuses << "\n";
    cout << "===================================\n";
}

// ============================================================
// MEMBER 3 - ADVANCED PASSENGER MANAGEMENT MODULE
// ============================================================

class AdvancedPassengerModule {
private:
    PassengerDesk &desk;

    struct Feedback {
        int passengerId;
        int rating;
        string message;
    };

    struct Activity {
        int passengerId;
        string activityName;
        string details;
    };

    Feedback feedbackList[200];
    Activity activityList[500];

    int feedbackCount = 0;
    int activityCount = 0;

    void addActivity(int id, const string &name,
                     const string &details) {
        if (activityCount >= 500)
            return;

        activityList[activityCount].passengerId = id;
        activityList[activityCount].activityName = name;
        activityList[activityCount].details = details;
        activityCount++;
    }

    bool validPassenger(int id) {
        return id >= 1 && id <= desk.getPassengerCount();
    }

    void printLine() {
        cout << "============================================\n";
    }

public:
    AdvancedPassengerModule(PassengerDesk &passengerDesk)
        : desk(passengerDesk) {}

    // 1. Search passenger using ID
    void searchPassenger() {
        printLine();
        cout << "          PASSENGER SEARCH\n";
        printLine();

        int id = askNumber("Enter Passenger ID: ");

        Passenger *p = desk.findPassenger(id);

        if (p == nullptr) {
            cout << "Passenger not found!\n";
            return;
        }

        cout << "\nPassenger Found!\n";
        cout << "ID: " << p->getId() << "\n";
        cout << "Name: " << p->getName() << "\n";
        cout << "Age: " << p->getAge() << "\n";

        addActivity(id, "Passenger Search",
                    "Passenger profile searched");

        cout << "Search completed successfully.\n";
    }

    // 2. Display passenger profile
    void displayProfile() {
        printLine();
        cout << "          PASSENGER PROFILE\n";
        printLine();

        int id = askNumber("Enter Passenger ID: ");

        if (!validPassenger(id)) {
            cout << "Invalid passenger ID!\n";
            return;
        }

        Passenger *p = desk.findPassenger(id);

        cout << "\nPassenger ID: " << p->getId();
        cout << "\nPassenger Name: " << p->getName();
        cout << "\nPassenger Age: " << p->getAge();

        cout << "\nPassenger Category: ";

        if (p->hasPriority())
            cout << "Priority Passenger";
        else if (p->isChildOrSenior())
            cout << "Child/Senior Passenger";
        else
            cout << "Normal Passenger";

        cout << "\nPriority Level: "
             << p->priorityLevel() << "\n";

        addActivity(id, "Profile Viewed",
                    "Passenger profile displayed");
    }

    // 3. Display all passenger profiles
    void displayAllPassengers() {
        printLine();
        cout << "          ALL PASSENGERS\n";
        printLine();

        int total = desk.getPassengerCount();

        if (total == 0) {
            cout << "No passengers registered.\n";
            return;
        }

        for (int i = 1; i <= total; i++) {
            Passenger *p = desk.findPassenger(i);

            if (p == nullptr)
                continue;

            cout << "\nPassenger ID: " << p->getId();
            cout << "\nName: " << p->getName();
            cout << "\nAge: " << p->getAge();

            cout << "\nCategory: ";

            if (p->hasPriority())
                cout << "Priority";
            else if (p->isChildOrSenior())
                cout << "Child/Senior";
            else
                cout << "Normal";

            cout << "\n--------------------------------------------\n";
        }

        cout << "\nTotal Registered Passengers: " << total << "\n";
    }

    // 4. Passenger statistics
    void passengerStatistics() {
        printLine();
        cout << "       PASSENGER STATISTICS\n";
        printLine();

        int total = desk.getPassengerCount();
        int priority = 0;
        int normal = 0;
        int children = 0;
        int senior = 0;

        for (int i = 1; i <= total; i++) {
            Passenger *p = desk.findPassenger(i);

            if (p == nullptr)
                continue;

            if (p->hasPriority())
                priority++;

            if (p->getAge() <= 12)
                children++;

            if (p->getAge() >= 60)
                senior++;

            if (p->priorityLevel() == 0)
                normal++;
        }

        cout << "Total Passengers: " << total << "\n";
        cout << "Priority Passengers: " << priority << "\n";
        cout << "Normal Passengers: " << normal << "\n";
        cout << "Children: " << children << "\n";
        cout << "Senior Citizens: " << senior << "\n";
    }

    // 5. Bus-wise waiting report
    void waitingReport() {
        printLine();
        cout << "          WAITING REPORT\n";
        printLine();

        int grandTotal = 0;
        int activeBuses = 0;
        int emptyBuses = 0;

        for (int bus = 1; bus <= Max_Buses; bus++) {
            int count = desk.peopleWaiting(bus);

            cout << "\nBus ID: " << bus;
            cout << "\nWaiting Passengers: " << count;

            if (count > 0) {
                cout << "\nStatus: Active Waiting List";
                activeBuses++;
            } else {
                cout << "\nStatus: Empty";
                emptyBuses++;
            }

            grandTotal += count;
            cout << "\n--------------------------------------------\n";
        }

        cout << "\nTotal Waiting Passengers: " << grandTotal;
        cout << "\nBuses With Passengers: " << activeBuses;
        cout << "\nEmpty Buses: " << emptyBuses << "\n";
    }

    // 6. Add passenger feedback
    void addFeedback() {
        printLine();
        cout << "          PASSENGER FEEDBACK\n";
        printLine();

        if (feedbackCount >= 200) {
            cout << "Feedback storage is full!\n";
            return;
        }

        int id = askNumber("Enter Passenger ID: ");

        if (!validPassenger(id)) {
            cout << "Invalid passenger ID!\n";
            return;
        }

        int rating = askNumber("Enter Rating (1-5): ");

        if (rating < 1 || rating > 5) {
            cout << "Rating must be between 1 and 5.\n";
            return;
        }

        string message = askText("Enter Feedback: ");

        if (message.empty()) {
            cout << "Feedback cannot be empty.\n";
            return;
        }

        feedbackList[feedbackCount].passengerId = id;
        feedbackList[feedbackCount].rating = rating;
        feedbackList[feedbackCount].message = message;

        feedbackCount++;

        addActivity(id, "Feedback Added",
                    "Passenger submitted feedback");

        cout << "\nFeedback submitted successfully!\n";
    }

    // 7. Display all feedback
    void displayFeedback() {
        printLine();
        cout << "          FEEDBACK RECORDS\n";
        printLine();

        if (feedbackCount == 0) {
            cout << "No feedback available.\n";
            return;
        }

        for (int i = 0; i < feedbackCount; i++) {
            cout << "\nFeedback Number: " << i + 1;
            cout << "\nPassenger ID: "
                 << feedbackList[i].passengerId;
            cout << "\nRating: "
                 << feedbackList[i].rating << "/5";
            cout << "\nMessage: "
                 << feedbackList[i].message;

            cout << "\n--------------------------------------------\n";
        }
    }

    // 8. Calculate average feedback rating
    void feedbackStatistics() {
        printLine();
        cout << "       FEEDBACK STATISTICS\n";
        printLine();

        if (feedbackCount == 0) {
            cout << "No feedback records available.\n";
            return;
        }

        int totalRating = 0;
        int excellent = 0;
        int average = 0;
        int poor = 0;

        for (int i = 0; i < feedbackCount; i++) {
            totalRating += feedbackList[i].rating;

            if (feedbackList[i].rating >= 4)
                excellent++;
            else if (feedbackList[i].rating == 3)
                average++;
            else
                poor++;
        }

        double avg = (double)totalRating / feedbackCount;

        cout << "Total Feedback: " << feedbackCount << "\n";
        cout << "Average Rating: " << avg << "/5\n";
        cout << "Positive Feedback: " << excellent << "\n";
        cout << "Average Feedback: " << average << "\n";
        cout << "Low Ratings: " << poor << "\n";
    }

    // 9. Passenger activity history
    void displayActivityHistory() {
        printLine();
        cout << "       PASSENGER ACTIVITY HISTORY\n";
        printLine();

        int id = askNumber("Enter Passenger ID: ");

        if (!validPassenger(id)) {
            cout << "Invalid passenger ID!\n";
            return;
        }

        bool found = false;

        for (int i = 0; i < activityCount; i++) {
            if (activityList[i].passengerId == id) {
                cout << "\nActivity: "
                     << activityList[i].activityName;

                cout << "\nDetails: "
                     << activityList[i].details;

                cout << "\n--------------------------------------------\n";

                found = true;
            }
        }

        if (!found)
            cout << "No activity history available.\n";
    }

    // 10. Save feedback to file
    void saveFeedback() {
        ofstream file("passenger_feedback.txt");

        if (!file) {
            cout << "Unable to open feedback file.\n";
            return;
        }

        for (int i = 0; i < feedbackCount; i++) {
            file << feedbackList[i].passengerId << ","
                 << feedbackList[i].rating << ","
                 << feedbackList[i].message << "\n";
        }

        file.close();

        cout << "Feedback saved successfully!\n";
    }

    // 11. Export passenger report
    void exportPassengerReport() {
        ofstream file("passenger_report.txt");

        if (!file) {
            cout << "Unable to create report file.\n";
            return;
        }

        file << "SMART PUBLIC TRANSPORT SYSTEM\n";
        file << "PASSENGER MANAGEMENT REPORT\n";
        file << "====================================\n";

        for (int i = 1; i <= desk.getPassengerCount(); i++) {
            Passenger *p = desk.findPassenger(i);

            if (p == nullptr)
                continue;

            file << "\nPassenger ID: " << p->getId();
            file << "\nName: " << p->getName();
            file << "\nAge: " << p->getAge();

            file << "\nCategory: ";

            if (p->hasPriority())
                file << "Priority";
            else if (p->isChildOrSenior())
                file << "Child/Senior";
            else
                file << "Normal";

            file << "\n------------------------------------\n";
        }

        file.close();

        cout << "Passenger report exported successfully!\n";
    }

    // 12. Display module dashboard
    void dashboard() {
        printLine();
        cout << "       PASSENGER MANAGEMENT DASHBOARD\n";
        printLine();

        cout << "Registered Passengers: "
             << desk.getPassengerCount() << "\n";

        cout << "Stored Feedback: "
             << feedbackCount << "\n";

        cout << "Recorded Activities: "
             << activityCount << "\n";

        int waiting = 0;

        for (int bus = 1; bus <= Max_Buses; bus++)
            waiting += desk.peopleWaiting(bus);

        cout << "Total Waiting Passengers: "
             << waiting << "\n";

        printLine();
    }

    // 13. Main module menu
    void run() {
        int choice;

        do {
            printLine();
            cout << "     ADVANCED PASSENGER MANAGEMENT\n";
            printLine();

            cout << "1. Search Passenger\n";
            cout << "2. View Passenger Profile\n";
            cout << "3. Display All Passengers\n";
            cout << "4. Passenger Statistics\n";
            cout << "5. Bus Waiting Report\n";
            cout << "6. Submit Passenger Feedback\n";
            cout << "7. View All Feedback\n";
            cout << "8. Feedback Statistics\n";
            cout << "9. Passenger Activity History\n";
            cout << "10. Save Feedback\n";
            cout << "11. Export Passenger Report\n";
            cout << "12. Dashboard\n";
            cout << "0. Return to Main Menu\n";

            printLine();

            choice = askNumber("Enter your choice: ");

            switch (choice) {
                case 1:
                    searchPassenger();
                    break;

                case 2:
                    displayProfile();
                    break;

                case 3:
                    displayAllPassengers();
                    break;

                case 4:
                    passengerStatistics();
                    break;

                case 5:
                    waitingReport();
                    break;

                case 6:
                    addFeedback();
                    break;

                case 7:
                    displayFeedback();
                    break;

                case 8:
                    feedbackStatistics();
                    break;

                case 9:
                    displayActivityHistory();
                    break;

                case 10:
                    saveFeedback();
                    break;

                case 11:
                    exportPassengerReport();
                    break;

                case 12:
                    dashboard();
                    break;

                case 0:
                    cout << "Returning to main menu...\n";
                    break;

                default:
                    cout << "Invalid choice! Try again.\n";
            }

            if (choice != 0)
                pauseScreen();

        } while (choice != 0);
    }

// ============================================================
// MEMBER 3 - ADVANCED PASSENGER MANAGEMENT
// PART 1: PASSENGER PROFILE AND RECORD MANAGEMENT
// ============================================================

class AdvancedPassengerManager {
private:

    PassengerDesk &desk;

    struct PassengerLog {
        int passengerId;
        string action;
        string description;
    };

    PassengerLog logs[500];
    int logCount = 0;

    void addLog(int id, const string &action,
                const string &description) {

        if (logCount >= 500)
            return;

        logs[logCount].passengerId = id;
        logs[logCount].action = action;
        logs[logCount].description = description;

        logCount++;
    }

    bool validId(int id) const {
        return id >= 1 &&
               id <= desk.getPassengerCount();
    }

    void heading(const string &title) {
        cout << "\n";
        cout << "============================================\n";
        cout << "       " << title << "\n";
        cout << "============================================\n";
    }

    void showCategory(const Passenger &p) {
        if (p.hasPriority())
            cout << "Priority Passenger";
        else if (p.getAge() <= 12)
            cout << "Child Passenger";
        else if (p.getAge() >= 60)
            cout << "Senior Citizen";
        else
            cout << "Normal Passenger";
    }

public:

    AdvancedPassengerManager(PassengerDesk &passengerDesk)
        : desk(passengerDesk) {}

    // --------------------------------------------------------
    // 1. SEARCH PASSENGER
    // --------------------------------------------------------

    void searchPassenger() {

        heading("SEARCH PASSENGER");

        int id = askNumber("Enter Passenger ID: ");

        if (!validId(id)) {
            cout << "Passenger does not exist!\n";
            return;
        }

        Passenger *p = desk.findPassenger(id);

        if (p == nullptr) {
            cout << "Passenger record not found!\n";
            return;
        }

        cout << "\nPassenger Found Successfully!\n";

        cout << "Passenger ID   : " << p->getId() << "\n";
        cout << "Passenger Name : " << p->getName() << "\n";
        cout << "Passenger Age  : " << p->getAge() << "\n";

        cout << "Category       : ";
        showCategory(*p);

        cout << "\nPriority Level : "
             << p->priorityLevel() << "\n";

        addLog(id, "SEARCH",
               "Passenger searched using ID");

    }

    // --------------------------------------------------------
    // 2. DISPLAY PASSENGER PROFILE
    // --------------------------------------------------------

    void displayPassengerProfile() {

        heading("PASSENGER PROFILE");

        int id = askNumber("Enter Passenger ID: ");

        if (!validId(id)) {
            cout << "Invalid Passenger ID!\n";
            return;
        }

        Passenger *p = desk.findPassenger(id);

        if (p == nullptr)
            return;

        cout << "\n";
        cout << "Passenger ID       : " << p->getId() << "\n";
        cout << "Passenger Name     : " << p->getName() << "\n";
        cout << "Passenger Age      : " << p->getAge() << "\n";

        cout << "Passenger Category : ";
        showCategory(*p);

        cout << "\n";
        cout << "Priority Level     : "
             << p->priorityLevel() << "\n";

        cout << "Half Fare Eligible : ";

        if (p->getsHalfFare())
            cout << "Yes\n";
        else
            cout << "No\n";

        addLog(id, "PROFILE",
               "Passenger profile viewed");

    }

    // --------------------------------------------------------
    // 3. DISPLAY ALL PASSENGERS
    // --------------------------------------------------------

    void displayAllPassengers() {

        heading("ALL REGISTERED PASSENGERS");

        int total = desk.getPassengerCount();

        if (total == 0) {
            cout << "No passengers registered yet.\n";
            return;
        }

        for (int i = 1; i <= total; i++) {

            Passenger *p = desk.findPassenger(i);

            if (p == nullptr)
                continue;

            cout << "\nPassenger ID : " << p->getId();
            cout << "\nName         : " << p->getName();
            cout << "\nAge          : " << p->getAge();

            cout << "\nCategory     : ";
            showCategory(*p);

            cout << "\nPriority     : "
                 << p->priorityLevel();

            cout << "\n--------------------------------------------\n";
        }

        cout << "\nTotal Passengers: " << total << "\n";

    }

    // --------------------------------------------------------
    // 4. PASSENGER STATISTICS
    // --------------------------------------------------------

    void passengerStatistics() {

        heading("PASSENGER STATISTICS");

        int total = desk.getPassengerCount();

        int priority = 0;
        int normal = 0;
        int children = 0;
        int senior = 0;
        int halfFare = 0;

        for (int i = 1; i <= total; i++) {

            Passenger *p = desk.findPassenger(i);

            if (p == nullptr)
                continue;

            if (p->hasPriority())
                priority++;

            if (p->getAge() <= 12)
                children++;

            if (p->getAge() >= 60)
                senior++;

            if (p->priorityLevel() == 0)
                normal++;

            if (p->getsHalfFare())
                halfFare++;
        }

        cout << "Total Passengers       : " << total << "\n";
        cout << "Priority Passengers    : " << priority << "\n";
        cout << "Normal Passengers      : " << normal << "\n";
        cout << "Children               : " << children << "\n";
        cout << "Senior Citizens        : " << senior << "\n";
        cout << "Half Fare Eligible     : " << halfFare << "\n";

        if (total > 0) {
            double percentage =
                (double)priority / total * 100;

            cout << "Priority Percentage    : "
                 << percentage << "%\n";
        }

    }

    // --------------------------------------------------------
    // 5. AGE CATEGORY REPORT
    // --------------------------------------------------------

    void ageCategoryReport() {

        heading("AGE CATEGORY REPORT");

        int children = 0;
        int teenagers = 0;
        int adults = 0;
        int senior = 0;

        for (int i = 1;
             i <= desk.getPassengerCount(); i++) {

            Passenger *p = desk.findPassenger(i);

            if (p == nullptr)
                continue;

            int age = p->getAge();

            if (age <= 12)
                children++;
            else if (age <= 19)
                teenagers++;
            else if (age < 60)
                adults++;
            else
                senior++;
        }

        cout << "Children (0-12)       : " << children << "\n";
        cout << "Teenagers (13-19)     : " << teenagers << "\n";
        cout << "Adults (20-59)        : " << adults << "\n";
        cout << "Senior Citizens (60+) : " << senior << "\n";

    }

    // --------------------------------------------------------
    // 6. PRIORITY PASSENGER REPORT
    // --------------------------------------------------------

    void priorityPassengerReport() {

        heading("PRIORITY PASSENGER REPORT");

        int count = 0;

        for (int i = 1;
             i <= desk.getPassengerCount(); i++) {

            Passenger *p = desk.findPassenger(i);

            if (p == nullptr)
                continue;

            if (p->priorityLevel() > 0) {

                cout << "\nPassenger ID: "
                     << p->getId();

                cout << "\nName: "
                     << p->getName();

                cout << "\nAge: "
                     << p->getAge();

                cout << "\nPriority Level: "
                     << p->priorityLevel();

                cout << "\n-----------------------------------\n";

                count++;
            }
        }

        cout << "\nTotal Priority Category Passengers: "
             << count << "\n";

    }

    // --------------------------------------------------------
    // 7. PASSENGER NAME SEARCH
    // --------------------------------------------------------

    void searchPassengerByName() {

        heading("SEARCH PASSENGER BY NAME");

        string searchName =
            askText("Enter Passenger Name: ");

        if (searchName.empty()) {
            cout << "Name cannot be empty!\n";
            return;
        }

        bool found = false;

        for (int i = 1;
             i <= desk.getPassengerCount(); i++) {

            Passenger *p = desk.findPassenger(i);

            if (p == nullptr)
                continue;

            if (p->getName().find(searchName)
                != string::npos) {

                cout << "\nPassenger ID: "
                     << p->getId();

                cout << "\nName: "
                     << p->getName();

                cout << "\nAge: "
                     << p->getAge();

                cout << "\nCategory: ";
                showCategory(*p);

                cout << "\n-----------------------------------\n";

                found = true;

                addLog(p->getId(), "NAME SEARCH",
                       "Passenger found by name");
            }
        }

        if (!found)
            cout << "No matching passenger found.\n";

    }

    // --------------------------------------------------------
    // 8. DISPLAY PASSENGER ACTIVITY
    // --------------------------------------------------------

    void displayPassengerActivity() {

        heading("PASSENGER ACTIVITY HISTORY");

        int id = askNumber("Enter Passenger ID: ");

        if (!validId(id)) {
            cout << "Invalid Passenger ID!\n";
            return;
        }

        bool found = false;

        for (int i = 0; i < logCount; i++) {

            if (logs[i].passengerId == id) {

                cout << "\nActivity: "
                     << logs[i].action;

                cout << "\nDescription: "
                     << logs[i].description;

                cout << "\n-----------------------------------\n";

                found = true;
            }
        }

        if (!found)
            cout << "No activity recorded for this passenger.\n";

    }

    // --------------------------------------------------------
    // 9. DISPLAY ALL ACTIVITY
    // --------------------------------------------------------

    void displayAllActivity() {

        heading("COMPLETE ACTIVITY LOG");

        if (logCount == 0) {
            cout << "No activity available.\n";
            return;
        }

        for (int i = 0; i < logCount; i++) {

            cout << "\nRecord Number: " << i + 1;

            cout << "\nPassenger ID: "
                 << logs[i].passengerId;

            cout << "\nAction: "
                 << logs[i].action;

            cout << "\nDescription: "
                 << logs[i].description;

            cout << "\n-----------------------------------\n";
        }

    }

    // --------------------------------------------------------
    // 10. EXPORT PASSENGER REPORT
    // --------------------------------------------------------

    void exportPassengerReport() {

        heading("EXPORT PASSENGER REPORT");

        ofstream file("advanced_passenger_report.txt");

        if (!file) {
            cout << "Unable to create report file!\n";
            return;
        }

        file << "SMART PUBLIC TRANSPORT SYSTEM\n";
        file << "ADVANCED PASSENGER REPORT\n";
        file << "====================================\n";

        for (int i = 1;
             i <= desk.getPassengerCount(); i++) {

            Passenger *p = desk.findPassenger(i);

            if (p == nullptr)
                continue;

            file << "\nPassenger ID: "
                 << p->getId();

            file << "\nName: "
                 << p->getName();

            file << "\nAge: "
                 << p->getAge();

            file << "\nPriority Level: "
                 << p->priorityLevel();

            file << "\nHalf Fare Eligible: "
                 << (p->getsHalfFare() ? "Yes" : "No");

            file << "\n------------------------------------\n";
        }

        file.close();

        cout << "Report exported successfully!\n";
        cout << "File: advanced_passenger_report.txt\n";

    }

    // --------------------------------------------------------
    // 11. PASSENGER DASHBOARD
    // --------------------------------------------------------

    void passengerDashboard() {

        heading("PASSENGER MANAGEMENT DASHBOARD");

        int total = desk.getPassengerCount();
        int priority = 0;
        int halfFare = 0;

        for (int i = 1; i <= total; i++) {

            Passenger *p = desk.findPassenger(i);

            if (p == nullptr)
                continue;

            if (p->hasPriority())
                priority++;

            if (p->getsHalfFare())
                halfFare++;
        }

        int waiting = 0;

        for (int bus = 1; bus <= Max_Buses; bus++) {
            waiting += desk.peopleWaiting(bus);
        }

        cout << "\nRegistered Passengers : " << total;
        cout << "\nPriority Passengers   : " << priority;
        cout << "\nHalf Fare Passengers  : " << halfFare;
        cout << "\nWaiting Requests      : " << waiting;
        cout << "\nActivity Records      : " << logCount;

        cout << "\n\nDashboard generated successfully.\n";

    }

    // --------------------------------------------------------
    // 12. PASSENGER MANAGEMENT MENU
    // --------------------------------------------------------

    void run() {

        int choice;

        do {

            heading("ADVANCED PASSENGER MANAGEMENT");

            cout << "1. Search Passenger by ID\n";
            cout << "2. Display Passenger Profile\n";
            cout << "3. Display All Passengers\n";
            cout << "4. Passenger Statistics\n";
            cout << "5. Age Category Report\n";
            cout << "6. Priority Passenger Report\n";
            cout << "7. Search Passenger by Name\n";
            cout << "8. Passenger Activity History\n";
            cout << "9. Display All Activities\n";
            cout << "10. Export Passenger Report\n";
            cout << "11. Passenger Dashboard\n";
            cout << "0. Return to Main Menu\n";

            cout << "\nEnter Choice: ";

            choice = askNumber("");

            switch (choice) {

                case 1:
                    searchPassenger();
                    break;

                case 2:
                    displayPassengerProfile();
                    break;

                case 3:
                    displayAllPassengers();
                    break;

                case 4:
                    passengerStatistics();
                    break;

                case 5:
                    ageCategoryReport();
                    break;

                case 6:
                    priorityPassengerReport();
                    break;

                case 7:
                    searchPassengerByName();
                    break;

                case 8:
                    displayPassengerActivity();
                    break;

                case 9:
                    displayAllActivity();
                    break;

                case 10:
                    exportPassengerReport();
                    break;

                case 11:
                    passengerDashboard();
                    break;

                case 0:
                    cout << "Returning to main menu...\n";
                    break;

                default:
                    cout << "Invalid choice!\n";
            }

            if (choice != 0)
                pauseScreen();

        } while (choice != 0);

    }
};

};


// MEMBER 4 : SCHEDULE, TICKET & SYSTEM INTEGRATION
struct Trip {
    int busId, departMinutes, tripMinutes;
};

struct Ticket {
    int ticketId, passengerId, busId, seatNumber, fromStop, toStop, km;
    int farePaise;
};

class Timetable {
    Trip trips[Max_Trips];
    int tripCount = 0;

public:
    int getTripCount() const { 
        return tripCount; 
    }
    
    const Trip &getTrip(int position) const { 
        return trips[position]; 
    }

    int findClash(int busId, int departMinutes, int tripMinutes) const {
        for (int i = 0; i < tripCount; i++) {
            if (trips[i].busId == busId && departMinutes < trips[i].departMinutes + trips[i].tripMinutes &&
                trips[i].departMinutes < departMinutes + tripMinutes) {
                return i;
            }
        }
        return -1;
    }

    bool addTrip(int busId, int departMinutes, int tripMinutes) {
        if (tripCount == Max_Trips) {
            return false;
        }
        trips[tripCount].busId = busId;
        trips[tripCount].departMinutes = departMinutes;
        trips[tripCount].tripMinutes = tripMinutes;
        tripCount++;
        return true;
    }

    void saveTimetable() const
    {
        ofstream file("timetable.txt");

        if(!file)
        {
            cout << "Unable to save timetable.\n";
            return;
        }

        for(int i = 0; i < tripCount; i++)
        {
            file << trips[i].busId << ","
                << trips[i].departMinutes << ","
                << trips[i].tripMinutes << "\n";
        }
    }

    void loadTimetable()
    {
        ifstream file("timetable.txt");

        if(!file)
            return;

        int busId, departure, duration;

        while(file >> busId)
        {
            char comma1, comma2;

            if(!(file >> comma1 >> departure
                    >> comma2 >> duration))
                break;

            if(comma1 != ',' || comma2 != ',' ||
                busId <= 0 ||
                departure < 0 || departure >= 1440 ||
                duration <= 0 ||
                tripCount == Max_Trips)
                continue;

            if(findClash(busId, departure, duration) == -1)
            {
                addTrip(busId, departure, duration);
            }
        }
    }

    void sortedOrder(int order[]) const {
        for (int i = 0; i < tripCount; i++) {
            order[i] = i;
        }
        for (int pass = 0; pass < tripCount - 1; pass++) {
            for (int i = 0; i < tripCount - 1 - pass; i++) {
                if (trips[order[i]].departMinutes > trips[order[i + 1]].departMinutes) {
                    int temporary = order[i];
                    order[i] = order[i + 1];
                    order[i + 1] = temporary;
                }
            }
        }
    }
};

class TicketOffice {
    Ticket tickets[Max_Tickets];
    int ticketCount = 0;
    int nextTicketId = 1001;

public:
    int getTicketCount() const { 
        return ticketCount; 
    }
    
    const Ticket &getTicket(int position) const { 
        return tickets[position]; 
    }
    
    static int normalFarePaise(int km) { 
        return km * 250; 
    }

    int issueTicket(const Passenger &passenger, int busId, int seat, int fromStop, int toStop, int km) {
        if (ticketCount == Max_Tickets) {
            return -1;
        }
        int fare = normalFarePaise(km);
        if (passenger.getsHalfFare()) {
            fare = fare / 2;
        }
        Ticket ticket = {nextTicketId++, passenger.getId(), busId, seat, fromStop, toStop, km, fare};
        tickets[ticketCount] = ticket;
        return ticketCount++;
    }
    void saveTickets() const
    {
        ofstream file("tickets.txt");

        if(!file)
        {
            cout << "Unable to save tickets.\n";
            return;
        }

        for(int i = 0; i < ticketCount; i++)
        {
            file << tickets[i].ticketId << ","
                 << tickets[i].passengerId << ","
                << tickets[i].busId << ","
                << tickets[i].seatNumber << ","
                << tickets[i].fromStop << ","
                << tickets[i].toStop << ","
                << tickets[i].km << ","
                << tickets[i].farePaise << "\n";
        }
    }

    void loadTickets(int passengerCount, int busCount, int stopCount)
    {
        ifstream file("tickets.txt");

        if(!file)
        return;

        string line;

        while(getline(file, line))
        {
            stringstream ss(line);

            int id, passengerId, busId, seat;
            int from, to, km, fare;
            char c1, c2, c3, c4, c5, c6, c7;

            if(!(ss >> id >> c1 >> passengerId >> c2
                    >> busId >> c3 >> seat >> c4
                    >> from >> c5 >> to >> c6
                    >> km >> c7 >> fare))
                continue;

            if(c1 != ',' || c2 != ',' || c3 != ',' ||
            c4 != ',' || c5 != ',' || c6 != ',' || c7 != ',')
                continue;

            if(ticketCount == Max_Tickets ||
                id < 1001 ||
                passengerId < 1 || passengerId > passengerCount ||
                busId < 1 || busId > busCount ||
                seat <= 0 ||
                from < 0 || from >= stopCount ||
                to < 0 || to >= stopCount ||
                from == to || km <= 0 || fare < 0)
                continue;

            bool duplicate = false;
            for (int i = 0; i < ticketCount; i++)
            {
                if (tickets[i].ticketId == id)
                    duplicate = true;
            }

            if (duplicate)
                continue;

            tickets[ticketCount++] = {
                id, passengerId, busId, seat,
                from, to, km, fare
        };

        if(id >= nextTicketId)
            nextTicketId = id + 1;
        }
}
};

// ADMIN LOGIN 
class AdminAccount {
    string username;
    unsigned int passwordHash;

    static unsigned int hashText(const string &text) {
        unsigned int hash = 5381;
        for (size_t i = 0; i < text.size(); i++) {
            hash = hash * 33 + (unsigned char)text[i];
        }
        return hash;
    }

public:
    AdminAccount() : username("admin"), passwordHash(hashText("admin123")) {}

    bool check(const string &typedName, const string &typedPassword) const {
        return typedName == username && hashText(typedPassword) == passwordHash;
    }

    bool checkPassword(const string &typedPassword) const {
        return hashText(typedPassword) == passwordHash;
    }

    void changePassword(const string &newPassword) {
        passwordHash = hashText(newPassword);
    }

    void save() const {
        ofstream file("admin.txt");
        if (!file) {
            cout << "Unable to save admin account.\n";
            return;
        }
        file << passwordHash << "\n";
    }

    void load() {
        ifstream file("admin.txt");
        if (!file) {
            return;
        }
        unsigned int savedHash;
        if (file >> savedHash) {
            passwordHash = savedHash;
        }
    }
};

class TransportSystem {
    BusManager busManager;
    RoadMap roadMap;
    PassengerDesk passengerDesk;
    Timetable timetable;
    TicketOffice ticketOffice;
    AdminAccount admin;

    string routeTextOf(const Bus &bus) const {
        if (bus.getRouteLength() == 0) {
            return "not set";
        }
        int stops[Max_Route];
        for (int i = 0; i < bus.getRouteLength(); i++) {
            stops[i] = bus.getRouteStop(i);
        }
        return roadMap.stopsToText(stops, bus.getRouteLength());
    }

    int tripTimeOfBus(const Bus &bus) const {
        if (bus.getRouteLength() < 2) {
            return -1;
        }
        int totalMinutes = 0;
        for (int i = 0; i + 1 < bus.getRouteLength(); i++) {
            int path[Max_Stops];
            int pathLength;
            int minutes = roadMap.findBestRoute(bus.getRouteStop(i), bus.getRouteStop(i + 1), path, pathLength, true);
            if (minutes < 0) {
                return -1;
            }
            totalMinutes += minutes;
        }
        return totalMinutes;
    }
    
    void restoreSeatsFromTickets() {
        for (int i = 0; i < ticketOffice.getTicketCount(); i++) {
            Bus *bus = busManager.findBusById(ticketOffice.getTicket(i).busId);
            if (bus != nullptr && bus->seatsFree() > 0) {
                bus->takeOneSeat();
            }
        }
    }

public:
    bool loadAll() {
        roadMap.loadRoadMap();
        busManager.loadBuses();
        busManager.loadDrivers();
        passengerDesk.loadPassengers();
        timetable.loadTimetable();
        ticketOffice.loadTickets(passengerDesk.getPassengerCount(),
                                 busManager.getBusCount(),
                                 roadMap.getStopCount());
        admin.load();
        restoreSeatsFromTickets();
        return busManager.getBusCount() > 0;
    }   

    void saveAll() const {
        roadMap.saveRoadMap();
        busManager.saveBuses();
        busManager.saveDrivers();
        passengerDesk.savePassengers();
        timetable.saveTimetable();
        ticketOffice.saveTickets();
         admin.save();
    }

        bool adminLogin() {
        printTitle("ADMIN LOGIN");
        for (int attempt = 1; attempt <= 3; attempt++) {
            string name = askText("  Username: ");
            string password = askPassword("  Password: ");
            if (admin.check(name, password)) {
                cout << "\n  Login successful. Welcome, " << name << "!\n";
                pauseScreen();
                return true;
            }
            cout << "  Wrong username or password (" << (3 - attempt) << " attempt(s) left).\n\n";
        }
        return false;
    }

    void changePasswordAction() {
        string oldPassword = askPassword("  Current password: ");
        if (!admin.checkPassword(oldPassword)) {
            cout << "  Wrong current password.\n";
            return;
        }
        string newPassword = askPassword("  New password (min 4 characters): ");
        if (newPassword.size() < 4) {
            cout << "  Password is too short.\n";
            return;
        }
        if (newPassword != askPassword("  Type the new password again: ")) {
            cout << "  The two passwords do not match.\n";
            return;
        }
        admin.changePassword(newPassword);
        cout << "  Password changed successfully.\n";
    }

    void loadDemoData() {
        roadMap.addRoad("Station", "Market", 4, 10);
        roadMap.addRoad("Market", "College", 3, 8);
        roadMap.addRoad("Station", "Hospital", 6, 12);
        roadMap.addRoad("Hospital", "College", 2, 5);
        roadMap.addRoad("College", "Airport", 10, 15);
        
        int firstBus = busManager.addBus("UK07-101", 3);
        int secondBus = busManager.addBus("UK07-202", 40);
        
        int firstRoute[4] = {roadMap.findStop("Station"), roadMap.findStop("Market"), roadMap.findStop("College"), roadMap.findStop("Airport")};
        int secondRoute[3] = {roadMap.findStop("Station"), roadMap.findStop("Hospital"), roadMap.findStop("College")};
        
        busManager.findBusById(firstBus)->setRoute(firstRoute, 4);
        busManager.findBusById(secondBus)->setRoute(secondRoute, 3);
        
        busManager.assignDriver(busManager.addDriver("Ramesh Kumar", "DL-1001"), firstBus);
        busManager.assignDriver(busManager.addDriver("Suresh Singh", "DL-1002"), secondBus);
        
        passengerDesk.registerPassenger("Aman", 20, false);
        passengerDesk.registerPassenger("Riya", 10, false);
        passengerDesk.registerPassenger("Mr. Sharma", 65, false);
        passengerDesk.registerPassenger("Neha", 22, true);
        
        timetable.addTrip(firstBus, 9 * 60 + 30, tripTimeOfBus(*busManager.findBusById(firstBus)));
    }

    void addBusAction() {
        string number = askText("Bus number plate: ");
        int seats = askNumber("Number of seats: ");
        
        if (number.empty() || seats <= 0 || seats > 200) {
            cout << "  Invalid plate or seat count (1-200).\n";
            return;
        }
  int id = busManager.addBus(number, seats);

if (id == -1) {
    cout << "  This number plate already exists.\n";
}
else if (id == -2) {
    cout << "  No space for more buses.\n";
}
else if (id < 0) {
    cout << "  Invalid bus number or seating capacity.\n";
}
else {
    cout << "  Bus added with ID " << id
         << " (use option 5 to give it a route).\n";
}
    }
    void searchBusAction() {
        string typed = askText("Type a bus ID or a number plate: ");
        int typedNumber;
        Bus *bus = nullptr;
        
        if (textToNumber(typed, typedNumber)) {
            bus = busManager.findBusById(typedNumber);
        } else {
            bus = busManager.findBusByNumber(typed);
        }
        
        if (bus == nullptr) {
            cout << "  Bus not found.\n";
        } else {
            busManager.showBus(*bus, routeTextOf(*bus));
        }
    }

    void changeStatusAction() {
        Bus *bus = busManager.findBusById(askNumber("Bus ID: "));
        if (bus == nullptr) {
            cout << "  Bus not found.\n";
            return;
        }
        
        int choice = askNumber("New status 1) Available  2) Running  3) Maintenance : ");
        if (choice == 1) { 
            bus->setStatus(AVAILABLE); 
            bus->freeAllSeats(); 
            cout << "  Available again, all seats free.\n"; 
        } else if (choice == 2 && bus->getDriverId() == -1) {
            cout << "  A bus cannot run without a driver.\n";
        } else if (choice == 2) { 
            bus->setStatus(RUNNING); 
            cout << "  Bus is now Running.\n"; 
        } else if (choice == 3) { 
            bus->setStatus(MAINTENANCE); 
            cout << "  Bus is now in Maintenance.\n"; 
        } else {
            cout << "  Invalid choice.\n";
        }
    }

    void addDriverAction() {
        string driverName = askText("Driver name: ");
        string license = askText("License number: ");
        
        if (driverName.empty() || license.empty()) {
            cout << "  Name and license are needed.\n";
            return;
        }
        
        int driverId = busManager.addDriver(driverName, license);
        if (driverId == -1) { 
            cout << "  No space for more drivers.\n"; 
            return; 
        }
        
        int busId = askNumber("Bus ID to assign (0 = assign later): ");
        if (busId != 0 && !busManager.assignDriver(driverId, busId)) {
            cout << "  Driver added (ID " << driverId << ") but the bus ID was wrong.\n";
        } else {
            cout << "  Driver added with ID " << driverId;
            if (busId != 0) {
                cout << " and assigned to the bus.\n";
            } else {
                cout << ".\n";
            }
        }
    }

    void assignRouteAction() {
        Bus*bus = busManager.findBusById(askNumber("Bus ID: "));
        if (bus == nullptr) {
            cout << "  Bus not found.\n";
            return;
        }
        
        string typedStops[Max_Route];
        int count = splitByComma(askText("Stops in order, separated by commas: "), typedStops, Max_Route);
        
        if (count < 2 || count > Max_Route) {
            cout << "  A route needs 2 to " << Max_Route << " stops.\n";
            return;
        }
        
        int stops[Max_Route];
        for (int i = 0; i < count; i++) {
            stops[i] = roadMap.findStop(typedStops[i]);
            if (stops[i] == -1) { 
                cout << "  '" << typedStops[i] << "' is not on the road map.\n"; 
                return; 
            }
        }
        
        bus->setRoute(stops, count);
        cout << "  Route saved: " << roadMap.stopsToText(stops, count) << "\n";
    }

    void showBusesAction() {
        if (busManager.getBusCount() == 0) {
            cout << "  No buses yet.\n";
        }
        for (int id = 1; id <= busManager.getBusCount(); id++) {
            Bus *bus = busManager.findBusById(id);
            if (bus != nullptr) {
                busManager.showBus(*bus, routeTextOf(*bus));
            }
        }
    }

    void addRoadAction() {
        string stopA = askText("First stop: ");
        string stopB = askText("Second stop: ");
        int km = askNumber("Distance in km: ");
        int minutes = askNumber("Travel time in minutes: ");
        
        bool roadAdded = roadMap.addRoad(stopA, stopB, km, minutes);
        if (roadAdded) {
            cout << "  Road added.\n";
        } else {
            cout << "  Invalid road details (or map is full).\n";
        }
    }
    void updateRoadAction() {
        string a = askText("First stop: ");
        string b = askText("Second stop: ");

        int km = askNumber("New distance in km: ");
        int minutes = askNumber("New travel time in minutes: ");

        if (roadMap.updateRoad(a, b, km, minutes)) {
            cout << "  Road details updated successfully.\n";
        } else {
            cout << "  Road not found or invalid details.\n";
        }
    }
    void removeRoadAction() {
        string a = askText("First stop: ");
        string b = askText("Second stop: ");

        if (roadMap.removeRoad(a, b)) {
            cout << "  Road removed successfully.\n";
        } else {
            cout << "  Road not found.\n";
        }
    }
    void showConnectionsAction() {
        string name = askText("Enter stop name: ");
        roadMap.showConnections(name);
    }
    void showDirectRoadAction() {
        string a = askText("First stop: ");
        string b = askText("Second stop: ");

        int km = roadMap.getDirectDistance(a, b);
        int minutes = roadMap.getDirectTime(a, b);

        if (km == -1) {
            cout << "  No direct road exists between these stops.\n";
            return;
        }
        cout << "  Direct road: " << a << " <-> " << b << " | " << km << " km | " << minutes << " min\n";
    }
    void showRoadCountAction() {
        cout << "  Total stops: " << roadMap.getStopCount() << "\n";
        cout << "  Total roads: " << roadMap.countRoads() << "\n";
    }
    void findRouteAction() {
        int start = roadMap.findStop(askText("From: "));
        int end = roadMap.findStop(askText("To: "));
        bool fastest = askNumber("Find 1) shortest route (km)  2) fastest route (minutes) : ") == 2;
        
        int path[Max_Stops];
        int pathLength;
        int total = roadMap.findBestRoute(start, end, path, pathLength, fastest);
        
        if (total < 0) {
            cout << "  No route found (check the stop names).\n";
        } else {
            if (fastest) {
                cout << "  Fastest route: ";
            } else {
                cout << "  Shortest route: ";
            }
            cout << roadMap.stopsToText(path, pathLength) << "\n  Total: " << total;
            if (fastest) {
                cout << " minutes\n";
            } else {
                cout << " km\n";
            }
        }
    }
    void showMapAction() { 
        roadMap.showMap(); 
    }

    void registerPassengerAction() {
        string passengerName = askText("Passenger name: ");
        int age = askNumber("Age: ");
        
        if (passengerName.empty() || age <= 0 || age > 120) {
            cout << "  Invalid name or age.\n";
            return;
        }
        
        bool priority = askNumber("Needs priority boarding, e.g. pregnant/injured? (1 = yes, 0 = no): ") == 1;
        int id = passengerDesk.registerPassenger(passengerName, age, priority);
        
        if (id == -1) {
            cout << "  No space for more passengers.\n";
        } else {
            cout << "  Passenger registered with ID " << id << "\n";
        }
    }

    void joinQueueAction() {
        Passenger *passenger = passengerDesk.findPassenger(askNumber("Passenger ID: "));
        Bus *bus = busManager.findBusById(askNumber("Bus ID: "));
        
        if (passenger == nullptr || bus == nullptr) { 
            cout << "  Wrong passenger ID or bus ID.\n"; 
            return; 
        }
        if (bus->getStatus() != AVAILABLE) { 
            cout << "  This bus is " << statusToText(bus->getStatus()) << ".\n"; 
            return; 
        }
        
        string fromName = askText("Boarding stop: ");
        string toName = askText("Destination stop: ");
        int fromStop = roadMap.findStop(fromName);
        int toStop = roadMap.findStop(toName);
        
        if (!bus->goesFromTo(fromStop, toStop)) { 
            cout << "  This bus does not go from " << fromName << " to " << toName << ".\n"; 
            return; 
        }
        
        int waiting = passengerDesk.joinWaitingList(bus->getId(), *passenger, fromStop, toStop);
        if (waiting == -2) {
    cout << "  Passenger is already waiting for this bus.\n";
    return;
}
        if (waiting == -1) { 
            cout << "  The waiting list is full.\n"; 
            return; 
        }
        
        cout << "  " << passenger->getName() << " joined the ";
        if (passenger->priorityLevel() > 0) {
            cout << "Priority";
        } else {
            cout << "Normal";
        }
        cout << " queue (" << waiting << " waiting).\n";
    }
void cancelQueueAction() {
    int passengerId = askNumber("Passenger ID: ");
    int busId = askNumber("Bus ID: ");

    if (passengerDesk.cancelWaitingRequest(busId, passengerId)) {
        cout << "  Queue request cancelled successfully.\n";
        cout << "  Remaining passengers: "
             << passengerDesk.peopleWaiting(busId) << "\n";
    } else {
        cout << "  Passenger is not waiting in this bus queue.\n";
    }
}

    void boardPassengersAction() {
        int busId = askNumber("Bus ID to start boarding: ");
        Bus *bus = busManager.findBusById(busId);
        
        if (bus == nullptr) {
            cout << "  Bus not found.\n";
            return;
        }
        if (bus->getStatus() != AVAILABLE || bus->getDriverId() == -1) { 
            cout << "  Bus must be Available and have a driver.\n"; 
            return; 
        }
        if (passengerDesk.peopleWaiting(busId) == 0) { 
            cout << "  Nobody is waiting.\n"; 
            return; 
        }
        
        while (passengerDesk.peopleWaiting(busId) > 0) {
            if (bus->seatsFree() == 0) { 
                cout << "  Bus is FULL. " << passengerDesk.peopleWaiting(busId) << " still waiting.\n"; 
                break; 
            }
            
            BoardingRequest request = passengerDesk.callNextPassenger(busId);
            Passenger *passenger = passengerDesk.findPassenger(request.passengerId);
            
            int path[Max_Stops];
            int pathLength;
            int km = roadMap.findBestRoute(request.fromStop, request.toStop, path, pathLength, false);
            
            if (passenger == nullptr || km < 0) {
                continue;
            }
            
            bus->takeOneSeat();
            int position = ticketOffice.issueTicket(*passenger, busId, bus->getSeatsTaken(), request.fromStop, request.toStop, km);
            
            if (position == -1) { 
                cout << "  The ticket book is full.\n"; 
                return; 
            }
            
            const Ticket &ticket = ticketOffice.getTicket(position);
            cout << "  Boarded";
            if (request.level > 0) {
                cout << " [PRIORITY]";
            }
            cout << ": " << passenger->getName() << " | seat " << ticket.seatNumber
                 << " | ticket #" << ticket.ticketId << " | Rs " << moneyToText(ticket.farePaise) << "\n";
        }
    }

    void addTripAction() {
        int busId = askNumber("Bus ID: ");
        Bus *bus = busManager.findBusById(busId);
        
        if (bus == nullptr) {
            cout << "  Bus not found.\n";
            return;
        }
        
        int tripMinutes = tripTimeOfBus(*bus);
        int departMinutes;
        
        if (tripMinutes < 0) { 
            cout << "  Give this bus a valid route first (option 5).\n"; 
            return; 
        }
        if (!clockToMinutes(askText("Departure time (HH:MM, 24 hour): "), departMinutes)) { 
            cout << "  Invalid time.\n"; 
            return; 
        }
        
        int clash = timetable.findClash(busId, departMinutes, tripMinutes);
        if (clash != -1) {
            cout << "  CLASH: this bus already runs " << minutesToClock(timetable.getTrip(clash).departMinutes) << " - "
                 << minutesToClock(timetable.getTrip(clash).departMinutes + timetable.getTrip(clash).tripMinutes) << ". Trip not added.\n";
            return;
        }
        
        if (!timetable.addTrip(busId, departMinutes, tripMinutes)) { 
            cout << "  The timetable is full.\n"; 
            return; 
        }
        
        cout << "  Trip added: " << minutesToClock(departMinutes) << " - " << minutesToClock(departMinutes + tripMinutes) << "\n";
    }

    void showTimetableAction() {
        int order[Max_Trips];
        timetable.sortedOrder(order);
        
        if (timetable.getTripCount() == 0) {
            cout << "  No trips yet.\n";
        }
        
        for (int i = 0; i < timetable.getTripCount(); i++) {
            const Trip &trip = timetable.getTrip(order[i]);
            Bus *bus = busManager.findBusById(trip.busId);
            
            cout << "  " << minutesToClock(trip.departMinutes) << " - " << minutesToClock(trip.departMinutes + trip.tripMinutes)
                 << "  Bus ";
            if (bus != nullptr) {
                cout << bus->getNumber();
            } else {
                cout << "?";
            }
            cout << "\n";
        }
    }

    void showTicketsAction() {
        int totalPaise = 0;
        
        if (ticketOffice.getTicketCount() == 0) {
            cout << "  No tickets yet.\n";
        }
        
        for (int i = 0; i < ticketOffice.getTicketCount(); i++) {
            const Ticket &ticket = ticketOffice.getTicket(i);
            cout << "  #" << ticket.ticketId << " | passenger " << ticket.passengerId << " | bus " << ticket.busId << " seat "
                 << ticket.seatNumber << " | " << roadMap.stopName(ticket.fromStop) << " -> " << roadMap.stopName(ticket.toStop)
                 << " | Rs " << moneyToText(ticket.farePaise) << "\n";
            totalPaise += ticket.farePaise;
        }
        
        if (ticketOffice.getTicketCount() > 0) {
            cout << "  Total collected: Rs " << moneyToText(totalPaise) << "\n";
        }
    }

        void saveTicketsAction() {
        saveAll();
        cout << "  All data saved.\n";
        ofstream ticketFile("tickets_report.txt");
        if (!ticketFile) { 
            cout << "  Could not open tickets_report.txt.\n";
            return; 
        }
        
        ticketFile << "TicketID,PassengerID,BusID,Seat,From,To,Km,Fare\n";
        for (int i = 0; i < ticketOffice.getTicketCount(); i++) {
            const Ticket &ticket = ticketOffice.getTicket(i);
            ticketFile << ticket.ticketId << "," << ticket.passengerId << "," << ticket.busId << "," << ticket.seatNumber << ","
                       << roadMap.stopName(ticket.fromStop) << "," << roadMap.stopName(ticket.toStop) << "," << ticket.km << ","
                       << moneyToText(ticket.farePaise) << "\n";
        }
        cout << "  Saved " << ticketOffice.getTicketCount() << " ticket(s) to tickets_report.txt\n";
    }
};

//These are the options which guest can use
bool guestCanUse(int choice) {
    return choice == 0 || choice == 2 || choice == 6 || choice == 8 || choice == 9 ||
           choice == 10 || choice == 11 || choice == 14 || choice == 19 ;
}

// Shows one menu line, but only if this user is allowed to use it.
void menuItem(bool isAdmin, int number, const string &text) {
    if (isAdmin || guestCanUse(number)) {
        printOption(number, text);
    }
}

int showStartScreen(const string &note) {
    
    printTitle("SMART PUBLIC TRANSPORT SYSTEM");
    cout << "  " << note << "\n";
    printSection("WELCOME");
    printOption(1, "Admin login");
    printOption(2, "Continue as guest");
    printOption(0, "Exit");
    cout << "\n";
    return askNumber("  Enter your choice: ");
}

void showMenu(bool isAdmin) {
    printTitle("SMART PUBLIC TRANSPORT SYSTEM");
    if (isAdmin) {
        cout << "  Logged in as: ADMIN\n";
    } else {
        cout << "  Mode: GUEST (limited options)\n";
    }

    printSection("BUS & VEHICLE");
    menuItem(isAdmin, 1, "Add bus");
    menuItem(isAdmin, 2, "Search bus");
    menuItem(isAdmin, 3, "Change bus status");
    menuItem(isAdmin, 4, "Add driver");
    menuItem(isAdmin, 5, "Assign route to bus");
    menuItem(isAdmin, 6, "Show all buses");

    printSection("ROUTES & NAVIGATION");
    menuItem(isAdmin, 7, "Add road");
    menuItem(isAdmin, 17, "Update road");
    menuItem(isAdmin, 18, "Remove road");
    menuItem(isAdmin, 8, "Find best route");
    menuItem(isAdmin, 9, "Show road map");
    menuItem(isAdmin, 19, "Show stop connections");
    menuItem(isAdmin, 20, "Direct road details");
    menuItem(isAdmin, 21, "Count total roads");

    printSection("PASSENGERS & BOARDING");
    menuItem(isAdmin, 10, "Register passenger");
    menuItem(isAdmin, 11, "Join boarding queue");
    menuItem(isAdmin, 12, "Board passengers");
    menuItem(isAdmin, 22, "Cancel queue request");

    printSection("SCHEDULE & TICKETS");
    menuItem(isAdmin, 13, "Add trip");
    menuItem(isAdmin, 14, "Show timetable");
    menuItem(isAdmin, 15, "Show tickets");
    menuItem(isAdmin, 16, "Save data + ticket report");

    if (isAdmin) {
        printSection("ADMIN");
        printOption(23, "Change admin password");
    }

    printSection("EXIT");
    printOption(0, "Exit (data is saved)");
    cout << "\n";
}

int main() {
        TransportSystem transportsystem;
    string loadNote;

    if (transportsystem.loadAll()) {
        loadNote = "Saved data loaded from files.";
    } else {
        transportsystem.loadDemoData();
        loadNote = "No saved data found. Demo data loaded.";
    }

    bool isAdmin = false;
    while (true) {
        int mode = showStartScreen(loadNote);
        if (mode == 0) {
            transportsystem.saveAll();
            cout << "\n  Goodbye!\n";
            return 0;
        }
        if (mode == 2) {
            break;
        }
        if (mode == 1 && transportsystem.adminLogin()) {
            isAdmin = true;
            break;
        }
    }

    while (true) {
        showMenu(isAdmin);
        int choice = askNumber("  Enter your choice: ");
        cout << "\n";

        if (!isAdmin && !guestCanUse(choice)) {
            cout << "  This option is not available in guest mode.\n";
            continue;
        }
        
        switch (choice) {
            case 1: 
                transportsystem.addBusAction(); 
                break;
            case 2: 
                transportsystem.searchBusAction(); 
                break;
            case 3: 
                transportsystem.changeStatusAction(); 
                break;
            case 4: 
                transportsystem.addDriverAction(); 
                break;
            case 5: 
                transportsystem.assignRouteAction(); 
                break;
            case 6: 
                transportsystem.showBusesAction(); 
                break;
            case 7: 
                transportsystem.addRoadAction(); 
                break;
            case 8: 
                transportsystem.findRouteAction(); 
                break;
            case 9: 
                transportsystem.showMapAction(); 
                break;
            case 10: 
                transportsystem.registerPassengerAction(); 
                break;
            case 11: 
                transportsystem.joinQueueAction(); 
                break;
            case 12: 
                transportsystem.boardPassengersAction(); 
                break;
            case 13: 
                transportsystem.addTripAction(); 
                break;
            case 14: 
                transportsystem.showTimetableAction(); 
                break;
            case 15: 
                transportsystem.showTicketsAction(); 
                break;
            case 16: 
                transportsystem.saveTicketsAction(); 
                break;
             case 17:
                transportsystem.updateRoadAction();
                break;
            case 18:
                transportsystem.removeRoadAction();
                break;
            case 19:
                transportsystem.showConnectionsAction();
                break;
            case 20:
                transportsystem.showDirectRoadAction();
                break;
            case 21:
                transportsystem.showRoadCountAction();
                break;
            case 22:
                transportsystem.cancelQueueAction();
                break;
            case 23:
                transportsystem.changePasswordAction();
                break;
            case 0: 
                transportsystem.saveAll();
                cout << "  Data saved.\n"; 
                return 0;
            default: 
                cout << "  Please choose a number from the menu.\n";
        }
        transportsystem.saveAll();
        
    }
}
