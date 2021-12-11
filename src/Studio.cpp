#include "../include/Studio.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>


using namespace std;

Studio::Studio(const std::string &configFilePath)
    {
    int trainer_id_counter = 0;
    vector<string> capacity_of_trainers;
    std::ifstream file (configFilePath);
    std::string whole_string;
    std::string line;
    if (file.is_open()) {
        while (line.empty() || line.substr(0, 1) == "#") {  // find # of trainers
            getline(file, line);
        }
        getline(file, line);

        while (line.empty() || line.substr(0, 1) == "#") {  // find trainer capacities
            getline(file, line);
        }
        capacity_of_trainers = split(line);
        for (std::string x: capacity_of_trainers) {
            int cap = std::stoi(x);
            trainers.push_back(new Trainer(cap, trainer_id_counter));
            trainer_id_counter++;
        }
        getline(file, line);
        int workout_id = 0;
        std::vector<std::string> line_input;
        while (!file.eof()) {
            while ((line.empty() || line.substr(0, 1) == "#") && !file.eof()) {
                getline(file, line);
            }
            if (file.eof()) {
                return;
            }
            line_input = split(line);
            std::string wrk_name = line_input[0];
            int line_index = 1;
            if (line_input.at(line_index) != ""){
                for (unsigned int i = 1; i < line_input.size()&&
                        (line_input[i]!= "Anaerobic" && line_input[i]!= "Mixed" && line_input[i]!= "Cardio"); i++){
                    wrk_name.append(" "+ line_input[i]);
                    line_index++;
                }
            }
            WorkoutType type = checkType(line_input[line_index + 1]);
            int price = std::stoi(line_input[line_input.size()-1]);
            workout_options.push_back(Workout(workout_id, wrk_name, price, type));
            workout_id++;

            std::getline(file, line);  // advance line for next iteration
        }
    }
}

void Studio::start() {
  std::stringstream string_stream;
  std::cout << "Studio is now open!" << std::endl;
  open = true;
  std::string user_input;

  while (open) {
    getline(std::cin, user_input);
    if (user_input.find("open") == 0) {
        open_studio(user_input);
    } else if (user_input.find("order") == 0) {
      order(user_input);
    } else if (user_input.find("move") == 0) {
      move(user_input);
    } else if (user_input.find("status") == 0) {
      status(user_input);
    } else if (user_input.find("log") == 0) {
        printActionsLog(user_input);
    }else if (user_input.find("workout_options") == 0) {
        printWorkoutOptions(user_input);
    } else if (user_input.find("backup") == 0) {
      backup(user_input);
    } else if (user_input.find("restore") == 0) {
      restore(user_input);
    } else if (user_input.find("closeall") == 0) {
      closeAll(user_input);
      open = false;
    } else if (user_input.find("close") == 0) {
      close(user_input);
    }
  }
}

void Studio::open_studio(std::string lineIn) {
  vector<string> parsed_line = split(lineIn);
  int trnr_id = std::stoi(parsed_line[1]);
  Trainer *trainer = getTrainer(trnr_id);
  if(trainer == nullptr || trainer->isOpen()){
    std::cout <<"Workout session does not exist or is already open." << std::endl;
    return;
  }
  vector<Customer *> customer_list;
  for (size_t i = 2; i < parsed_line.size() - 1; i+=2) {
    string name = parsed_line[i];
    string type = parsed_line[i + 1];
    if (type == "swt") {
      customer_list.push_back(
          new SweatyCustomer(name, (*this).customer_id_counter));
    }
    if (type == "chp") {
      customer_list.push_back(
          new CheapCustomer(name, (*this).customer_id_counter));
    }
    if (type == "mcl") {
      customer_list.push_back(
          new HeavyMuscleCustomer(name, (*this).customer_id_counter));
    }
    if (type == "fbd") {
      customer_list.push_back(
          new FullBodyCustomer(name, (*this).customer_id_counter));
    }
    ++customer_id_counter;
  }
    OpenTrainer *opener = new OpenTrainer(trnr_id, customer_list);
    opener->act(*this);
}

void Studio::order(std::string lineIn) {
  vector<string> parsed_line = split(lineIn);
  int trnr_id = std::stoi(parsed_line[1]);
  Order *order = new Order(trnr_id);
  order->act(*this);
}

void Studio::move(std::string lineIn) {
  vector<string> parsed_line = split(lineIn);
  int id_src_trainer = std::stoi(parsed_line.at(1));
  int id_dst_trainer = std::stoi(parsed_line.at(2));
  int id_moving_customer = std::stoi(parsed_line.at(3));
  MoveCustomer *move_customer =
      new MoveCustomer(id_src_trainer, id_dst_trainer, id_moving_customer);
  move_customer->act(*this);
}

void Studio::status(std::string lineIn) {
  vector<string> parsed_line = split(lineIn);
  int trnr_id = std::stoi(parsed_line.at(1));
  PrintTrainerStatus *trainer_stat = new PrintTrainerStatus(trnr_id);
  trainer_stat->act(*this);
}


void Studio::backup(std::string lineIn) {
  BackupStudio *backup = new BackupStudio();
  backup->act(*this);
}

void Studio::restore(std::string lineIn) {
  RestoreStudio *restore = new RestoreStudio();
  restore->act(*this);
}

void Studio::printActionsLog(std::string lineIn) {
  PrintActionsLog *p_a_l = new PrintActionsLog();
  p_a_l->act(*this);
}

void Studio::printWorkoutOptions(std::string lineIn) {
    PrintWorkoutOptions *p_w_o = new PrintWorkoutOptions();
    p_w_o->act(*this);
}

void Studio::close(std::string lineIn) {
  vector<string> parsed_line = split(lineIn);
  int trnr_id = std::stoi(parsed_line.at(1));
  Close *close = new Close(trnr_id);
  close->act(*this);
}

void Studio::closeAll(std::string lineIn) {
  CloseAll *close_all = new CloseAll();
  close_all->act(*this);
}

// Copy Constructor
Studio::Studio(const Studio &other)
    : open(other.open),
      customer_id_counter(other.customer_id_counter)
{
  for (unsigned int index = 0; index < other.actionsLog.size(); index++) {
    BaseAction *newAction = other.actionsLog[index]->clone();
    actionsLog.push_back(newAction);
  }
  for (unsigned int index = 0; index < other.trainers.size(); index++) {
    Trainer *newTrainer = new Trainer(*other.trainers[index]);
    trainers.push_back(newTrainer);
  }
  for (unsigned int index = 0; index < other.workout_options.size(); index++) {
      workout_options.push_back(other.workout_options[index]);
    }
}

// Copy Assignment Operator
Studio &Studio::operator=(const Studio &other) {
  if (this != &other) {
    clear();
    copy(other);
  }
  return *this;
}

// Move Constructor
Studio::Studio(Studio &&other)
    : open(other.open),
      customer_id_counter(other.customer_id_counter) ,
      workout_options(other.workout_options){
  steal(other);
  other.workout_options.clear();
}

// Move Assignment Operator
Studio &Studio::operator=(Studio &&other) {
  if (this != &other) {
    open = other.open;
    customer_id_counter = other.customer_id_counter;
    workout_options = other.workout_options;
    // steal pointers
    steal(other);
  }
  return *this;
}

// Destructor
Studio::~Studio() { clear(); 
}

void Studio::clear() {
  for (Trainer *trainer : trainers) {
    if (trainer) {
      delete trainer;
      trainer = nullptr;
    }
  }
  trainers.clear();
  for (BaseAction *action : actionsLog) {
    if (action) {
      delete action;
      action = nullptr;
    }
  }
  actionsLog.clear();
  workout_options.clear();

}
//Steal Pointers
void Studio::steal(Studio &other) {
  for (unsigned int i = 0; i < other.trainers.size(); i++) {
    Trainer *newTrainer = other.trainers[i];
    trainers.push_back(newTrainer);
    other.trainers[i] = nullptr;
  }
  other.trainers.clear();

  for (unsigned int i = 0; i < other.actionsLog.size(); i++) {
    BaseAction *newAction = other.actionsLog[i];
    actionsLog.push_back(newAction);
    other.actionsLog[i] = nullptr;
  }
  other.actionsLog.clear();
}

void Studio::copy(const Studio &other) {
  open = other.open;
  customer_id_counter = other.customer_id_counter;
  workout_options = other.workout_options;

  for (unsigned int i = 0; i < other.trainers.size(); i++) {
    Trainer *newTrainer =  new Trainer(*other.trainers[i]);
    trainers.push_back(newTrainer);
  }

  for (unsigned int i = 0; i < other.actionsLog.size(); i++) {
    BaseAction *newAction = other.actionsLog[i]->clone();
    actionsLog.push_back(newAction);
  }
}

int Studio::getNumOfTrainers() const { return trainers.size(); }

Trainer *Studio::getTrainer(int tid) {
  for (Trainer *x : trainers) {
    if (x->getId() == tid) {
      return x;
    }
  }
  return nullptr;
}

std::vector<Workout> &Studio::getWorkoutOptions() { return workout_options; }

const std::vector<BaseAction *> &Studio::getActionsLog() const {
  return actionsLog;
}

void Studio::add_to_actionLog(BaseAction *action) {
  actionsLog.push_back(action);
}

std::vector<Trainer *> *Studio::getTrainerVector() { return &trainers; }

WorkoutType Studio::checkType(string x) {
  if (x == "Anaerobic"|| x== "ANAEROBIC") {
    return ANAEROBIC;
  } else if (x == "Mixed"|| x== "MIXED") {
    return MIXED;
  } else {
    return CARDIO;
  }
}

vector<string> Studio::split(string line) {
  vector<int> comma;
  vector<string> output;

  std::string reverse_line = line;
  std::reverse(reverse_line.begin(), reverse_line.end());
  line = line + reverse_line; // line- 6,6,8,44,8,6,6

  for (unsigned int i = 0; i < line.size(); i++){
    if (line.at(i) == ',' || line.at(i) == '\n' || line.at(i) == ' ') {
      comma.push_back(i);
    }
  }
  int start = 0;
  int len;
  for (unsigned int i = 0; i < comma.size(); i++) {
      if(comma.size() /2 +1 == i){
            break;
      }
    len = comma[i] - start;
    string sub = line.substr(start, len);
    start = comma[i] + 1;
    output.push_back(sub);
  }
  string tmp=output.at(output.size()-1);
  tmp = tmp.substr(0, tmp.length()/2);
  output.at(output.size()-1) = tmp;
  return output;
}

void Studio::setCustomerIdCounter(int customerIdCounter) {
  customer_id_counter = customerIdCounter;
}

int Studio::getCustomerIdCounter() const { return customer_id_counter; }

