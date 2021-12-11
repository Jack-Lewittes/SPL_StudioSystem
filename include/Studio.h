#ifndef STUDIO_H_
#define STUDIO_H_

#include <string>
#include <vector>
#include "../include/Customer.h"
#include "../include/Trainer.h"
#include "../include/Workout.h"

// Forward declaration , needed for compilation
class Trainer;
class BaseAction;

class Studio {
  public:
  Studio() = default;
  Studio(const std::string& configFilePath);
  // Move Assignment operator
  Studio(Studio&& other);
  // Copy Constructor
  Studio(const Studio& other);
  // Destructor
  ~Studio();
  // Move Assignment
  Studio& operator=(Studio&& other);
  // Copy Assignment operator
  Studio& operator=(const Studio& other);
  void steal(Studio& other);
  void clear();
  void copy(const Studio& other);
  void start();
  int getNumOfTrainers() const;
  Trainer* getTrainer(int tid);
  const std::vector<BaseAction*>& getActionsLog() const;
  std::vector<Workout>& getWorkoutOptions();
  std::vector<Trainer*>* getTrainerVector();

  void open_studio(std::string lineIn);
  void order(std::string lineIn);
  void move(std::string lineIn);
  void status(std::string lineIn);
  void backup(std::string lineIn);
  void restore(std::string lineIn);
  void printActionsLog(std::string lineIn);
  void printWorkoutOptions(std::string lineIn);
  void close(std::string lineIn);
  void closeAll(std::string lineIn);


  void add_to_actionLog(BaseAction* action);
  WorkoutType checkType(std::string x);

  private:
  std::vector<std::string> split(std::string s);

  protected:
  bool open{false};
  int customer_id_counter = 0;
  std::vector<BaseAction*> actionsLog;
  std::vector<Trainer*> trainers;
  std::vector<Workout> workout_options;

  public:
  int getCustomerIdCounter() const;

  public:
  void setCustomerIdCounter(int customerIdCounter);
};

#endif
