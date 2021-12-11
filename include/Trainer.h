#ifndef TRAINER_H_
#define TRAINER_H_

#include <vector>
#include "../include/Customer.h"
#include "../include/Workout.h"
#include "../include/Studio.h"
#include "../include/Action.h"

typedef std::pair<int, Workout> OrderPair;

class Trainer{
public:
    Trainer(int t_capacity, int id);
    ~Trainer();
    Trainer(const Trainer &other); //C.C.
    Trainer(Trainer &&other); //M.A.
    Trainer& operator=(const Trainer &other); //C.A.;
    Trainer& operator=(Trainer &&other); //M.A.O

    void copy(const Trainer& other);
    Trainer(int t_capacity);
    int getCapacity() const;
    void addCustomer(Customer* customer);
    void removeCustomer(int id);
    Customer* getCustomer(int id);
    std::vector<Customer*>& getCustomers();
    std::vector<OrderPair>& getOrders();
    void order(const int customer_id, const std::vector<int> workout_ids, const std::vector<Workout>& workout_options);
    void openTrainer();
    void closeTrainer();
    int getSalary() const;
    bool isOpen();
    int getId() const;
    void setStatus(bool input);
    void setSalary();
    void upCapacity();
    void downCapacity();
private:
    int capacity;
    int id{}; //added
    int salary; // we added
    bool open{false};
    std::vector<Customer*> customersList;
    std::vector<OrderPair> orderList;

protected:
    void steal(Trainer& _other);
    void clear();
    void copy();
};


#endif