#ifndef PERSON_H
#define PERSON_H

#include "common.h"

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


#endif
