#ifndef ACTION_H_
#define ACTION_H_

#include <string>
#include <iostream>
#include "../include/Customer.h"
#include "../include/Studio.h"


enum ActionStatus{
    COMPLETED, ERROR
};

//Forward declaration
class Studio;

class BaseAction{
public:
    BaseAction();
    ActionStatus getStatus() const;
    virtual void act(Studio& studio)=0;
    virtual std::string toString() const=0;
    virtual ~BaseAction()=default;
    void setStatus(ActionStatus _status);
    ActionStatus getStatus();
    std::string statusAsString() const;
    virtual BaseAction* clone()=0;
    void setErrorMsg(const std::string &errorMsg);

protected:
    void complete();
    void error(std::string errorMsg);
    std::string getErrorMsg() const;
private:
    std::string errorMsg;
    ActionStatus status;
};

class OpenTrainer : public BaseAction {
public:
    OpenTrainer(int id, std::vector<Customer *> &customersList);
    ~OpenTrainer()=default;
    void act(Studio &studio);
    std::string toString() const;
    std::string stringLog() const ;
    OpenTrainer* clone();

private:
    std:: string stringOutput;
    const int trainerId;
	std::vector<Customer *> customers;
};

class Order : public BaseAction {
public:
    Order(int id);
    void act(Studio &studio);
    std::string toString() const;
    std::string printOrder(Studio &studio) const;
    Order* clone();

private:
    const int trainerId;
};


class MoveCustomer : public BaseAction {
public:
    MoveCustomer(int src, int dst, int customerId);
    void act(Studio &studio);
    std::string toString() const;
    MoveCustomer* clone();

private:
    const int srcTrainer;
    const int dstTrainer;
    const int id;
};


class Close : public BaseAction {
public:
    Close(int id);
    void act(Studio &studio);
    std::string toString() const;
    Close* clone();

private:
    const int trainerId;
};


class CloseAll : public BaseAction {
public:
    CloseAll();
    void act(Studio &studio);
    std::string toString() const;
    CloseAll* clone();

private:
};

class PrintWorkoutOptions : public BaseAction {
public:
    PrintWorkoutOptions();
    void act(Studio &studio);
    std::string toString() const;
    PrintWorkoutOptions* clone();

private:
};

class PrintTrainerStatus : public BaseAction {
public:
    PrintTrainerStatus(int id);
    void act(Studio &studio);
    std::string toString() const;
    PrintTrainerStatus* clone();

private:
    const int trainerId;
};


class PrintActionsLog : public BaseAction {
public:
    PrintActionsLog();
    void act(Studio &studio);
    std::string toString() const;
    PrintActionsLog* clone();

private:
};


class BackupStudio : public BaseAction {
public:
    BackupStudio();
    void act(Studio &studio);
    std::string toString() const;
    BackupStudio* clone();
private:
};


class RestoreStudio : public BaseAction {
public:
    RestoreStudio();
    void act(Studio &studio);
    std::string toString() const;
    RestoreStudio* clone();
};


#endif
