#include "common.h"

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
