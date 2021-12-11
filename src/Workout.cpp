//
// Created by Jack Lewittes on 08/11/2021.
//
#include "../include/Workout.h"
#include <string>

// default constructor
Workout::Workout(int w_id, std::string w_name, int w_price, WorkoutType w_type)
    : id(w_id), name(w_name), price(w_price), type(w_type) {}

int Workout::getId() const { return id; }

std::string Workout::getName() const { return name; }

int Workout::getPrice() const { return price; };

WorkoutType Workout::getType() const { return type; }

std::string Workout::workoutAsString(WorkoutType x) {
  if (x == ANAEROBIC) {
    return "Anaerobic";
  } else if (x == MIXED) {
    return "Mixed";
  } else {
    return "Cardio";
  }
}

