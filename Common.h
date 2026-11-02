#ifndef COMMON_H
#define COMMON_H

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

// Helper functions (defined in common.cpp)
string numberToText(int number);
string twoDigits(int number);
string minutesToClock(int minutes);
string moneyToText(int paise);
string cutSpaces(const string &text);
string askText(const string &question);
bool textToNumber(const string &text, int &number);
int askNumber(const string &question);
bool clockToMinutes(const string &text, int &minutes);
int splitByComma(const string &text, string parts[], int maxParts);

#endif
