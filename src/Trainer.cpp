#include "../include/Trainer.h"
#include <vector>

Trainer::Trainer(int t_capacity) : capacity(t_capacity) {
  salary = 0;
  open = false;
}

Trainer::Trainer(int t_capacity, int t_id)
    : capacity(t_capacity), id(t_id), open(false) {
  salary = 0;
}

Trainer::~Trainer() {
    clear();
}
// Copy Constructor
Trainer::Trainer(const Trainer& other)
    : capacity(other.getCapacity()),
      id(other.getId()),
      salary(other.getSalary()) {
  copy(other);
}
// Copy Assignment operator
Trainer& Trainer::operator=(const Trainer& other) {
  if (this != &other) {
    clear();
    copy(other);
  }
  return *this;
}
// Move Constructor
Trainer::Trainer(Trainer&& other)
    : capacity(other.getCapacity()),
      id(other.getId()),
      salary(other.getSalary()) {
  steal(other);
}
// Move Assignment
Trainer& Trainer::operator=(Trainer&& other) {
  if (this != &other) {
    clear();
    capacity = other.getCapacity();
    id = other.getId();
    salary = other.getSalary();
    steal(other);
  }
  return *this;
}

void Trainer::copy(const Trainer& other) {
  for (size_t index = 0; index < other.customersList.size(); index++) {
    Customer* add_customer = (other.customersList[index])->clone();
    customersList.push_back(add_customer);
  }

  for (size_t index = 0; index < other.orderList.size(); index++) {
    orderList.push_back(other.orderList[index]);
  }
}

void Trainer::steal(Trainer& other) {
  customersList = std::vector<Customer*>();
  for (size_t index = 0; index < other.customersList.size(); index++) {
    Customer* add_customer = (other.customersList[index]);
    customersList.push_back(add_customer);
    other.customersList[index] = nullptr;
  }
  other.customersList.clear();

  orderList = std::vector<OrderPair>();
  for (size_t index = 0; index < other.orderList.size(); index++) {
    orderList.push_back(other.orderList[index]);
  }
  other.orderList.clear();
}

void Trainer::clear() {
  for (Customer* customer : customersList) {
    if (customer != nullptr) {
      delete customer;
      customer = nullptr;
    }
  }
  customersList.clear();
}

int Trainer::getCapacity() const { return capacity; }

void Trainer::upCapacity() { capacity++; }

void Trainer::downCapacity() { capacity--; }

int Trainer::getId() const { return id; }

int Trainer::getSalary() const { return salary; }

void Trainer::setSalary() {
  int accumulator = 0;
  for (const auto& x : orderList) {
    // deleted customers have id of -1
    if (x.first != -1) {
      accumulator += (x.second.getPrice());
    }
  }
  salary += accumulator;
}

void Trainer::setStatus(bool input) { open = input; }

bool Trainer::isOpen() { return open; }

void Trainer::addCustomer(Customer* customer) {
  if (capacity > 0) {
    customersList.push_back(customer);
    downCapacity();
  }
}

void Trainer::removeCustomer(int id) {
  if (!customersList.empty() && id >= 0) {
    for (unsigned int i = 0; i < customersList.size(); i++) {
      if (customersList.at(i)->getId() == id) {
          delete customersList.at(i);
          customersList.at(i) = nullptr;
          customersList.erase(customersList.begin() + i);
          break;
      }
    }
    for (size_t i = 0; i < orderList.size(); i++) {
      if (orderList.at(i).first == id) {
        orderList.at(i).first = -1;
      }
    }
      upCapacity();
  }
}

Customer* Trainer::getCustomer(int id) {
  if (!(customersList.empty()) && id >= 0) {
    for (size_t i = 0; i < customersList.size(); i++) {
      if (id == customersList[i]->getId()) {
        return customersList[i];
      }
    }
  }
  return nullptr;
}

std::vector<Customer*>& Trainer::getCustomers() { return customersList; }

std::vector<OrderPair>& Trainer::getOrders() { return orderList; }

void Trainer::order(const int customer_id, const std::vector<int> workout_ids,
                    const std::vector<Workout>& workout_options) {
    if (!open) {
        std::cout << "Trainer does not exist or is not open" << std::endl;
    } else {
        for (unsigned int i=0; i<workout_ids.size(); i++) {
            int id = workout_ids.at(i);
            orderList.push_back(OrderPair(customer_id, workout_options.at(id)));
        }
    }
}

void Trainer::openTrainer() { open = true; }

void Trainer::closeTrainer() {
  open = false;
  std::cout << "Trainer " << id << "closed. Salary " << salary << "NIS"
            << std::endl;
}

