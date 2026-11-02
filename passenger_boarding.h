#ifndef PASSENGER_BOARDING_H
#define PASSENGER_BOARDING_H

#include "common.h"
#include "person.h"

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


#endif
