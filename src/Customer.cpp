//
// Created by Jack Lewittes on 08/11/2021.
//
#include <algorithm>
#include <string>
#include <vector>
#include "../include/Customer.h"

using namespace std;

// Customer
Customer::Customer(std::string c_name, int c_id) : name(c_name), id(c_id) {}
int Customer::getId() const { return id; }
std::string Customer::getName() const { return name; }

void Customer::pointer_swap(Workout *l, Workout *r) {
  Workout *tmp = r;
  r = l;
  l = tmp;
}

// Sweaty Customer
SweatyCustomer::SweatyCustomer(std::string cName, int cId)
    : Customer(cName, cId) {}

std::vector<int> SweatyCustomer::order(
    const std::vector<Workout> &workout_options) {
  std::vector<int> output;
  for (Workout x : workout_options) {
    if (x.getType() == CARDIO) {
      output.push_back(x.getId());
    }
  }
  return output;
}

std::string SweatyCustomer::toString() const {
  return std::to_string(SweatyCustomer::getId()) + " " +
         SweatyCustomer::getName();
}
std::string SweatyCustomer::typeAsString() const { return "swt"; }

SweatyCustomer *SweatyCustomer::clone() const { return new SweatyCustomer(*this); }

// Cheap Customer
CheapCustomer::CheapCustomer(std::string cName, int cId)
    : Customer(cName, cId) {}

std::vector<int> CheapCustomer::order(
    const std::vector<Workout> &workout_options) {
  // min price and min ID
  std::vector<int> output;
  int minPrice = workout_options[0].getPrice();
  for (size_t i = 1; i < workout_options.size(); ++i) {
    if (workout_options[i].getPrice() < minPrice) {
      minPrice = workout_options[i].getPrice();
    }
  }
  for (Workout x : workout_options) {
    if (x.getPrice() == minPrice) {
      output.push_back(x.getId());
      break;
    }
  }
  // vector with single workout
  return output;
}

std::string CheapCustomer::toString() const {
  return std::to_string(CheapCustomer::getId()) + " " +
         CheapCustomer::getName();
}

std::string CheapCustomer::typeAsString() const { return "chp"; }

CheapCustomer *CheapCustomer::clone() const { return new CheapCustomer(*this); }

// HeavyMuscle Customer
HeavyMuscleCustomer::HeavyMuscleCustomer(std::string cName, int cId)
    : Customer(cName, cId) {}

std::vector<int> HeavyMuscleCustomer::order(
    const std::vector<Workout>
        &workout_options) {  // anaerobic, price max, id min

  std::vector<Workout> vector_order;
  std::vector<int> output;
  for (Workout x : workout_options) {
    if (x.getType() == ANAEROBIC) {
      vector_order.push_back(x);
    }
  }

  struct workoutStruct {
    bool operator()(const Workout &a, const Workout &b) const {
      return (b.getPrice() < a.getPrice() ||
              (b.getPrice() == a.getPrice() && b.getId() < a.getId()));
    }
  } workout_cmp;

  std::sort(vector_order.begin(), vector_order.end(), workout_cmp);

  for (auto x : vector_order) {
    output.push_back(x.getId());
  }
  return output;
}

std::string HeavyMuscleCustomer::toString() const {
  return std::to_string(HeavyMuscleCustomer::getId()) + " " +
         HeavyMuscleCustomer::getName();
}

std::string HeavyMuscleCustomer::typeAsString() const { return "mcl"; }

HeavyMuscleCustomer *HeavyMuscleCustomer::clone() const {
  return new HeavyMuscleCustomer(*this);
}

// FullBody Customer
FullBodyCustomer::FullBodyCustomer(std::string cName, int cId)
    : Customer(cName, cId) {}

std::vector<int> FullBodyCustomer::order(
    const std::vector<Workout> &workout_options) {
  // cheap cardio, most expensive mix, cheapest anaerobic
  std::vector<int> output;
  std::vector<Workout> cardio_workouts;
  std::vector<Workout> mixed_workouts;
  std::vector<Workout> anaerobic_workouts;
  // find cardio+cheap+min id
  for (Workout x : workout_options) {
    if (x.getType() == CARDIO) {
      cardio_workouts.push_back(x);
    }
  }
  std::sort(cardio_workouts.begin(), cardio_workouts.end(),
            [](Workout const &a, Workout const &b) {
              return a.getPrice() < b.getPrice();
            });
  // ascending order
  for (size_t i = 0; i < cardio_workouts.size() - 1; i++) {
    if (cardio_workouts[i].getPrice() == cardio_workouts[i + 1].getPrice()) {
      if (cardio_workouts[i + 1].getId() < cardio_workouts[i].getId()) {
        Customer::pointer_swap(&cardio_workouts[i + 1], &cardio_workouts[i]);
      }
    }
  }
  output.push_back(cardio_workouts.at(0).getId());

  for (Workout x : workout_options) {
    if (x.getType() == MIXED) {
      mixed_workouts.push_back(x);
    }
  }
  std::sort(mixed_workouts.begin(), mixed_workouts.end(),
            [](Workout const &a, Workout const &b) {
              return b.getPrice() < a.getPrice();
            });
  // descending order
  for (size_t i = 0; i < mixed_workouts.size() - 1; i++) {
    if (mixed_workouts[i].getPrice() == mixed_workouts[i + 1].getPrice()) {
      if (mixed_workouts[i + 1].getId() < mixed_workouts[i].getId()) {
        Customer::pointer_swap(&mixed_workouts[i + 1], &mixed_workouts[i]);
      }
    }
  }
  output.push_back(mixed_workouts.at(0).getId());

  for (Workout x : workout_options) {
    if (x.getType() == ANAEROBIC) {
      anaerobic_workouts.push_back(x);
    }
  }
  std::sort(anaerobic_workouts.begin(), anaerobic_workouts.end(),
            [](Workout const &a, Workout const &b) {
              return a.getPrice() < b.getPrice();
            });  // ascending order
  for (size_t i = 0; i < anaerobic_workouts.size() - 1; i++) {
    if (anaerobic_workouts[i].getPrice() ==
        anaerobic_workouts[i + 1].getPrice()) {
      if (anaerobic_workouts[i + 1].getId() < anaerobic_workouts[i].getId()) {
        Customer::pointer_swap(&anaerobic_workouts[i + 1],
                               &anaerobic_workouts[i]);
      }
    }
  }
  output.push_back(anaerobic_workouts.at(0).getId());

  return output;
}

std::string FullBodyCustomer::toString() const {
  return std::to_string(FullBodyCustomer::getId()) + " " +
         FullBodyCustomer::getName();
}
std::string FullBodyCustomer::typeAsString() const { return "fbd"; }

FullBodyCustomer *FullBodyCustomer::clone() const {
  return new FullBodyCustomer(*this);
}
