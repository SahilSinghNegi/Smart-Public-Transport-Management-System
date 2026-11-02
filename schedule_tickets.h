#ifndef SCHEDULE_TICKETS_H
#define SCHEDULE_TICKETS_H

#include "common.h"
#include "passenger_boarding.h"

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


#endif
