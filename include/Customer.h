#ifndef CUSTOMER_H_
#define CUSTOMER_H_

#include <string>
#include <vector>
#include "../include/Workout.h"

class Customer {
  public:
  Customer(std::string c_name, int c_id);
  virtual std::vector<int> order(const std::vector<Workout> &workout_options) = 0;
  virtual std::string toString() const = 0;
  virtual ~Customer() = default;
  virtual std::string typeAsString() const = 0;
  std::string getName() const;
  int getId() const;
  virtual Customer *clone() const = 0;

private:
  const std::string name;
  const int id;

protected:
  void pointer_swap(Workout *l, Workout *r);
};

class SweatyCustomer : public Customer {
  public:
  SweatyCustomer(std::string cName, int cId);
  std::vector<int> order(const std::vector<Workout> &workout_options);
  std::string toString() const;

  ~SweatyCustomer() = default;
  std::string typeAsString() const;
  SweatyCustomer *clone() const;

  private:
};

class CheapCustomer : public Customer {
  public:
  CheapCustomer(std::string name, int id);
  std::vector<int> order(const std::vector<Workout> &workout_options);
  std::string toString() const;

  ~CheapCustomer() = default;
  std::string typeAsString() const;

  CheapCustomer *clone() const;

  private:
};

class HeavyMuscleCustomer : public Customer {
  public:
  HeavyMuscleCustomer(std::string name, int id);
  // JD added default copy constructor, needed for compilation
  HeavyMuscleCustomer(const HeavyMuscleCustomer &other) = default;

  // move constructor
  HeavyMuscleCustomer(HeavyMuscleCustomer &&other);

  std::vector<int> order(const std::vector<Workout> &workout_options);
  std::string toString() const;

  ~HeavyMuscleCustomer() = default;
  std::string typeAsString() const;
  HeavyMuscleCustomer *clone() const;

  private:
};

class FullBodyCustomer : public Customer {
  public:
  FullBodyCustomer(std::string name, int id);
  ~FullBodyCustomer() = default;
  std::vector<int> order(const std::vector<Workout> &workout_options);
  std::string toString() const;

  std::string typeAsString() const;
  FullBodyCustomer *clone() const;

  private:
};

#endif
