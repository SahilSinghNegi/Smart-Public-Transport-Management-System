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
    
    void setStatus(int newStatus) { 
        status = newStatus; 
    }
    
    void setDriver(int newDriverId) { 
        driverId = newDriverId; 
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
        if (driverCount == Max_Drivers) {
            return -1;
        }
        drivers[driverCount] = Driver(driverCount + 1, driverName, license);
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

    void showBus(const Bus &bus, const string &routeText) {
        Driver *driver = findDriver(bus.getDriverId());
        string driverName = "none";
        
        if (driver != nullptr) {
            driverName = driver->getName();
        }

        cout << "  Bus #" << bus.getId() << " | " << bus.getNumber() << " | " << statusToText(bus.getStatus()) << " | seats "
             << bus.getSeatsTaken() << "/" << bus.getTotalSeats() << " | driver: "
             << driverName << "\n    route: " << routeText << "\n";
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

// MEMBER 3 : PASSENGER & BOARDING MANAGEMENT
class Passenger : public Person {
    int age;
    bool wantsPriority;

public:
    Passenger() : Person(), age(0), wantsPriority(false) {}
    
    Passenger(int passengerId, const string &passengerName, int passengerAge, bool priority)
        : Person(passengerId, passengerName), age(passengerAge), wantsPriority(priority) {}
    
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

public:
    int registerPassenger(const string &passengerName, int age, bool priority) {
        if (passengerCount == Max_Passengers) {
            return -1;
        }
        passengers[passengerCount] = Passenger(passengerCount + 1, passengerName, age, priority);
        return ++passengerCount;
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

    int joinWaitingList(int busId, const Passenger &passenger, int fromStop, int toStop) {
        BoardingRequest request = {passenger.getId(), fromStop, toStop, passenger.priorityLevel(), ++arrivalCounter};
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
        return peopleWaiting(busId);
    }

    BoardingRequest callNextPassenger(int busId) {
        WaitingList &list = waitingLists[busId - 1];
        if (list.priorityQueue.size() > 0) {
            return list.priorityQueue.pop();
        }
        return list.normalQueue.pop();
    }
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
};

class TransportSystem {
    BusManager busManager;
    RoadMap roadMap;
    PassengerDesk passengerDesk;
    Timetable timetable;
    TicketOffice ticketOffice;

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

public:
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
        } else if (id == -2) {
            cout << "  No space for more buses.\n";
        } else {
            cout << "  Bus added with ID " << id << " (use option 5 to give it a route).\n";
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
        Bus *bus = busManager.findBusById(askNumber("Bus ID: "));
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
        ofstream ticketFile("tickets.txt");
        if (!ticketFile) { 
            cout << "  Could not open tickets.txt.\n"; 
            return; 
        }
        
        ticketFile << "TicketID,PassengerID,BusID,Seat,From,To,Km,Fare\n";
        for (int i = 0; i < ticketOffice.getTicketCount(); i++) {
            const Ticket &ticket = ticketOffice.getTicket(i);
            ticketFile << ticket.ticketId << "," << ticket.passengerId << "," << ticket.busId << "," << ticket.seatNumber << ","
                       << roadMap.stopName(ticket.fromStop) << "," << roadMap.stopName(ticket.toStop) << "," << ticket.km << ","
                       << moneyToText(ticket.farePaise) << "\n";
        }
        cout << "  Saved " << ticketOffice.getTicketCount() << " ticket(s) to tickets.txt\n";
    }
};

void showMenu() {
    cout << "\n SMART PUBLIC TRANSPORT SYSTEM \n"
         << "  BUS & VEHICLE (Member 1) \n"
         << "  1. Add bus             3. Change bus status   5. Assign route to bus\n"
         << "  2. Search bus          4. Add driver          6. Show all buses\n"
         << "  ROUTE & NAVIGATION (Member 2) \n"
         << "  7. Add road            8. Find route          9. Show road map\n"
         << "  PASSENGER & BOARDING (Member 3) \n"
         << " 10. Register passenger 11. Join boarding queue 12. Board passengers\n"
         << " SCHEDULE & TICKETS (Member 4) \n"
         << " 13. Add trip           14. Show timetable     15. Show tickets\n"
         << " 16. Save tickets to file                       0. Exit\n"
         << endl;
}

int main() {
    TransportSystem transportsystem;
    transportsystem.loadDemoData();
    
    cout << "Demo data loaded (stops: Station, Market, College, Hospital, Airport).\n";
    
    while (true) {
        showMenu();
        int choice = askNumber("Enter your choice: ");
        cout << "\n";
        
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
            case 0: 
                cout << "\n"; 
                return 0;
            default: 
                cout << "  Please choose a number from the menu.\n";
        }
    }
}