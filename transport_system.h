#ifndef TRANSPORT_SYSTEM_H
#define TRANSPORT_SYSTEM_H

#include "common.h"
#include "bus_management.h"
#include "route_navigation.h"
#include "passenger_boarding.h"
#include "schedule_tickets.h"

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


#endif
