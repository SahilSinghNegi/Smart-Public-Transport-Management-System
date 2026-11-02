This project is a console application written in C++ that manages the daily running of a small city bus service. It keeps track of buses, drivers, roads between stops, passengers waiting to travel, trip timings and the tickets that are issued, all from one menu-driven program.

We built it for our college project to practise object-oriented programming and data structures. Instead of using ready-made containers, every data structure in it (hash table, graph, queues and heap) is written by hand, and the work is split into four parts so each team member owns one module.

Features

1.Bus and driver management
Add buses and drivers, assign drivers to buses and look up any bus instantly by ID or number plate.
Mark each bus as Available, Running or under Maintenance and give it a route of ordered stops.

2.Route and navigation
Build a road map with the distance and travel time between stops and view the whole network.
Find the shortest or the fastest route between any two stops using Dijkstra’s algorithm.

3.Passenger and boarding
Register passengers and place them in a bus’s boarding queue for a chosen start and end stop.
Priority passengers, children and seniors board first, and boarding stops once the bus is full.

4.Timetable and tickets
Schedule trips with clash detection and view the timetable sorted by departure time.
Tickets with seat and fare are issued on boarding, then can be viewed, totalled and saved to a file.

Files

bus_management.h -> Buses and drivers

route_navigation.h -> Road map and routes

passenger_boarding.h ->	Passengers and queues

schedule_tickets.h -> Timetable and tickets

transport_system.h -> The menu that joins everything

common.h, common.cpp, person.h ->	Shared helpers

main.cpp -> Starts the program
