#ifndef BUS_MANAGEMENT_H
#define BUS_MANAGEMENT_H
#include "common.h"
#include "person.h"
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


#endif
