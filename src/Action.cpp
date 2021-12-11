//
// Created by Jack Lewittes on 10/11/2021.
//
#include "../include/Action.h"
#include <sstream>
#include <string>

extern Studio* backup;
using namespace std;

// Base Action
BaseAction::BaseAction() {}

ActionStatus BaseAction::getStatus() const { return status; }

std::string BaseAction::statusAsString() const {
  if (status == COMPLETED) {
    return "Completed";
  }
  return errorMsg;
}

void BaseAction::complete() { status = COMPLETED; }

void BaseAction::error(std::string errorMsg) {
  status = ERROR;
  this->errorMsg = errorMsg;
}
std::string BaseAction::getErrorMsg() const { return errorMsg; }

void BaseAction::setErrorMsg(const string &errorMsg) {
    BaseAction::errorMsg = errorMsg;
}

void BaseAction:: setStatus(ActionStatus _status){
    this->status = _status;
}

ActionStatus BaseAction::getStatus(){
    return status;
}

// OPEN_TRAINER
OpenTrainer::OpenTrainer(int trainerId, std::vector<Customer*>& customers)
    : BaseAction(), trainerId(trainerId), customers(customers), stringOutput() {}


void OpenTrainer::act(Studio& studio) {
  Trainer* my_trainer = studio.getTrainer(trainerId);

  if (my_trainer == nullptr || my_trainer->isOpen()){
    error("Error: Workout session does not exist or is already open");
    stringOutput = stringLog();
    // If cannot open Trainer, delete the inserted customer vector
    for(Customer *customer : customers){
        delete customer;
        customer = nullptr;
    }
    return;
  }
  //else: open trainer
  int true_capacity = my_trainer->getCapacity();
  if (true_capacity ==0) {
    error("Error: Trainer capacity too small");
  } else {
    int customers_vector_size = customers.size();
    if (customers_vector_size > true_capacity) {
      int difference = customers_vector_size - true_capacity;
      for (int i = true_capacity; i < customers_vector_size; i++) {
        delete customers.at(i);
        customers.at(i) = nullptr;
      }
      for (int i = 0; i < true_capacity; i++) {
        my_trainer->addCustomer(customers.at(i));
      }
      studio.setCustomerIdCounter(studio.getCustomerIdCounter() - difference);
    } else {
      for (int i = 0; i < customers_vector_size; i++) {
        my_trainer->addCustomer(customers.at(i));
      }
    }
  }
    stringOutput = stringLog();
    my_trainer->setStatus(true);
    complete();
    studio.add_to_actionLog(this);
}

std::string OpenTrainer::toString() const {
    if(getStatus()==COMPLETED){
        return stringOutput;
    }
    else{
        return getErrorMsg();
    }
}

std::string OpenTrainer::stringLog() const {
    std::stringstream ss_output;
    ss_output << "open " + to_string(trainerId) + " ";
    for (Customer *customer: customers) {
        ss_output << customer->getName() + "," + customer->typeAsString() + " ";
    }
    return ss_output.str();
}

OpenTrainer* OpenTrainer::clone() {
    OpenTrainer* ot = new OpenTrainer(trainerId, customers);
    ot->setStatus(this->getStatus());
    ot->setErrorMsg(this->getErrorMsg());
    return ot;
}

// ORDER
Order::Order(int id) : trainerId(id) {}

void Order::act(Studio& studio) {  // set the OrderPair list of this trainer id
  Trainer * my_trainer = studio.getTrainer(trainerId);
  if(my_trainer == nullptr || !my_trainer->isOpen()){
    error("Error: Trainer does not exist or is not open");
  }
  else {
      for (Customer *customer: my_trainer->getCustomers()) {
          if ((customer->getId() != -1)) {
              my_trainer->order(customer->getId(),
                                customer->order(studio.getWorkoutOptions()), studio.getWorkoutOptions());
          }
      }
      complete();
  }
  // print actions
  std::string output{Order::printOrder(studio)};
  std::cout << output << std::endl;
  studio.add_to_actionLog(this);
}

std::string Order::printOrder(Studio& studio) const {
  if (getStatus() == COMPLETED) {
    std::stringstream ss_output;
    Trainer* my_trainer = studio.getTrainer(trainerId);
    for (auto& order : my_trainer->getOrders()) {  // pair or orderpair
      std::string name = my_trainer->getCustomer(order.first)->getName();
      std::string workout_name = order.second.getName();
      ss_output << name + " Is Doing " + workout_name + '\n';
    }
    return ss_output.str();
  } else {
    return getErrorMsg();
  }
}

std::string Order::toString() const {
  return ("order " + std::to_string(trainerId));
}

Order* Order::clone() {
    Order* od = new Order(trainerId);
    od->setStatus(this->getStatus());
    od->setErrorMsg(this->getErrorMsg());

    return od;
}

// MoveCustomer
MoveCustomer::MoveCustomer(int src, int dst, int customerId)
    : srcTrainer(src), dstTrainer(dst), id(customerId) {}

void MoveCustomer::act(Studio& studio) {
  Trainer* src_trainer = studio.getTrainer(srcTrainer);
  Trainer* dst_trainer = studio.getTrainer(dstTrainer);

  if(src_trainer == nullptr || dst_trainer == nullptr){
    error("Error: Cannot move customer");
      studio.add_to_actionLog(this);
      return;
  }
  Customer* customer = src_trainer->getCustomer(id);
  if(customer == nullptr){
    error("Error: Cannot move customer");
      studio.add_to_actionLog(this);
      return;
  }
  else if (id == (-1) || !(studio.getTrainer(srcTrainer)->isOpen()) ||!(studio.getTrainer(dstTrainer)->isOpen()) ||
      studio.getTrainer(dstTrainer)->getCapacity() == 0) {
          error("Error: Cannot move customer");
          studio.add_to_actionLog(this);
          return;
      }
  else {
    dst_trainer->addCustomer(customer->clone());
    for (OrderPair& order_pair : src_trainer->getOrders()) {
      if (order_pair.first == id) {
        dst_trainer->getOrders().push_back(order_pair);
      }
    }
    src_trainer->removeCustomer(id);
    complete();
    studio.add_to_actionLog(this);
      // check if trainer must close himself
    if(src_trainer->getCustomers().empty()){
        Close* close = new Close(srcTrainer);
        close->act(studio);
    }
  }
}

std::string MoveCustomer::toString() const {
  std::stringstream ss_output;
  ss_output << "move " + std::to_string(srcTrainer) + " " +std::to_string(dstTrainer) + " " + to_string(id);
  return ss_output.str();
}

MoveCustomer* MoveCustomer::clone() {
    MoveCustomer* mc = new MoveCustomer(srcTrainer,dstTrainer,id);
    mc->setStatus(this->getStatus());
    mc->setErrorMsg(this->getErrorMsg());
    return mc;
}

// Close
Close::Close(int id) : trainerId(id) {}

void Close::act(Studio& studio) {
  if ((studio.getTrainer(trainerId))->getId() != trainerId || !((studio.getTrainer(trainerId))->isOpen())) {
    error("Error: Trainer does not exist or is not open");
  }
  Trainer* trainer = studio.getTrainer(trainerId);
  trainer->setStatus(false);
  trainer->setSalary();
  for(Customer *customer: trainer->getCustomers()){
    if(customer != nullptr)
        trainer->removeCustomer(customer->getId());
  }
  complete();
  studio.add_to_actionLog(this);

  std::stringstream ss_output;
  ss_output << "Trainer " + std::to_string(trainerId) + " " + "closed. Salary "
            + std::to_string(studio.getTrainer(trainerId)->getSalary()) + "NIS";
  std::cout << ss_output.str() << std::endl;
}

std::string Close::toString() const {
    std::string output = "close " + (std::to_string(trainerId));
    return output;
}

Close* Close::clone() {
    Close* cl = new Close(trainerId);
    cl->setStatus(this->getStatus());
    cl->setErrorMsg(this->getErrorMsg());
    return cl;
}

// CloseAll
CloseAll::CloseAll() = default;

void CloseAll::act(Studio& studio) {
  for (Trainer* trainer : *(studio.getTrainerVector())) {
    if (trainer == nullptr) continue;

    if (trainer->isOpen()) {
      Close close_trainer = Close(trainer->getId());
      close_trainer.act(studio);
    } else {
      std::stringstream ss_output;
      ss_output << "Trainer " + std::to_string(trainer->getId()) + " " +
                       "closed. Salary "
                << std::to_string(trainer->getSalary()) << "NIS";
      std::cout << ss_output.str() << std::endl;
    }
  }
  complete();
  studio.add_to_actionLog(this);
}

std::string CloseAll::toString() const { return "closeall"; }

CloseAll* CloseAll::clone() {
    CloseAll* cla = new CloseAll();
    cla->setStatus(this->getStatus());
    cla->setErrorMsg(this->getErrorMsg());
    return cla;
}

// PrintWorkoutOptions
PrintWorkoutOptions::PrintWorkoutOptions() = default;

void PrintWorkoutOptions::act(Studio& studio) {
  std::vector<Workout>::iterator workout_it;
  for (workout_it = studio.getWorkoutOptions().begin();
       workout_it < studio.getWorkoutOptions().end(); workout_it++)
    std::cout << workout_it->getName() + ", " +
                     std::to_string(workout_it->getType()) + ", " +
                     std::to_string(workout_it->getPrice())+"\n";
  complete();
  studio.add_to_actionLog(this);

}
std::string PrintWorkoutOptions::toString() const {return "workout_options";}

PrintWorkoutOptions* PrintWorkoutOptions::clone() {
    PrintWorkoutOptions* pwo = new PrintWorkoutOptions();
    pwo->setStatus(this->getStatus());
    pwo->setErrorMsg(this->getErrorMsg());
    return pwo;
}

// PrintTrainerStatus
PrintTrainerStatus::PrintTrainerStatus(int id) : trainerId(id) {}

void PrintTrainerStatus::act(Studio& studio) {
  std::stringstream str;
  if (studio.getTrainer(trainerId)->isOpen()) {
    str << "Trainer" <<" "<< trainerId << " status: open"
        << "\n";
    str << "Customers:"
        << "\n";
    for (Customer* customer : studio.getTrainer(trainerId)->getCustomers()) {
      if (customer->getId() != -1) {
        str << customer->getId() << " " << customer->getName() << "\n";
      }
    }
    str << "Orders:"
        << "\n";
    int currentSalary = 0;
    for (unsigned int i = 0; i < studio.getTrainer(trainerId)->getOrders().size(); i++) {
        std::vector<OrderPair> order_list = studio.getTrainer(trainerId)->getOrders();
        if (order_list[i].first != -1) {
            str << order_list[i].second.getName() << " "
                << order_list[i].second.getPrice() << "NIS " << (order_list[i].first)
                << "\n";
            currentSalary += order_list[i].second.getPrice();
        }
    }
    str << "Current Trainer's Salary: "
        << currentSalary << "NIS";
  } else {
    str << "Trainer " << trainerId << " status: closed" << endl;
  }
  complete();
  studio.add_to_actionLog(this);
  std::cout << str.str() << std::endl;
 }

std::string PrintTrainerStatus::toString() const {return "status "+std::to_string(trainerId);}

PrintTrainerStatus* PrintTrainerStatus::clone() {
    PrintTrainerStatus* pts = new PrintTrainerStatus(trainerId);
    pts->setStatus(this->getStatus());
    pts->setErrorMsg(this->getErrorMsg());
    return pts;
}

// PrintActionsLog
PrintActionsLog::PrintActionsLog() {}

void PrintActionsLog::act(Studio& studio) {
  for (BaseAction* action : studio.getActionsLog()) {
    std::cout << action->toString() + " " + action->statusAsString() + "\n";
  }
    complete();
  //current log print will be included in NEXT log print
    studio.add_to_actionLog(this);
}

std::string PrintActionsLog::toString() const {return "log";}

PrintActionsLog* PrintActionsLog::clone() {
    PrintActionsLog* pal = new PrintActionsLog();
    pal->setStatus(this->getStatus());
    pal->setErrorMsg(this->getErrorMsg());
    return pal;
}

// BackupStudio
BackupStudio::BackupStudio() {}

void BackupStudio::act(Studio& studio) {
  if (backup == nullptr) {
   backup = new Studio(studio);
  } else {
      delete backup;
      backup = new Studio(studio);
  }
  complete();
  studio.add_to_actionLog(this);
}

std::string BackupStudio::toString() const { return "backup"; }

BackupStudio* BackupStudio::clone() {
    BackupStudio* bs = new BackupStudio();
    bs->setStatus(this->getStatus());
    bs->setErrorMsg(this->getErrorMsg());
    return bs;
}

// RestoreStudio
RestoreStudio::RestoreStudio() {}

void RestoreStudio::act(Studio& studio) {
    if (backup == nullptr){
        error("Error: No backup available");
        std:: cout<< getErrorMsg()<< endl;
    }
    else{
        studio = *backup;
        complete();
    }
    studio.add_to_actionLog(this);
}

std::string RestoreStudio::toString() const { return "restore"; }

RestoreStudio* RestoreStudio::clone() {
    RestoreStudio* rs = new RestoreStudio();
    rs->setStatus(this->getStatus());
    rs->setErrorMsg(this->getErrorMsg());
    return rs;
}
