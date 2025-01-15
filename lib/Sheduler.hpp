#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include "any.h"

class Task {
  public:
    virtual ~Task() = default;
    virtual void execute() = 0;
    virtual my::any getResult() = 0;
    virtual bool isSolved() = 0;
  };

  template <class Function>
  class Task0 : public Task {
  public:
    Task0(const Function& function, size_t index)
      : function_(function), is_solved_(false), index_(index) {}

    void execute() override {
      result_ = function_();
      is_solved_ = true;
    }

    bool isSolved() override {
      return is_solved_;
    }

    my::any getResult() override {
      return result_;
    }

  private:
    my::any result_;

    Function function_;
    bool is_solved_;
    size_t index_;
  };

  template <class Function, class Argument>
  class Task1 : public Task {
  public:
    Task1(const Function& function, const Argument& arg, size_t index)
      : function_(function), arg_(arg), is_solved_(false), index_(index) {}

    void execute() override {
      result_ = function_(arg_);
      is_solved_ = true;
    }

    bool isSolved() override {
      return is_solved_;
    }

    my::any getResult() override {
      return result_;
    }

  private:
    my::any result_;

    Function function_;
    Argument arg_;
    bool is_solved_;
    size_t index_;
  };

  template <class Function, class Argument1, class Argument2>
  class Task2 : public Task {
  public:
    Task2(const Function& function, const Argument1& arg1, const Argument2& arg2, size_t index)
      : function_(function), arg1_(arg1), arg2_(arg2), is_solved_(false), index_(index) {}

    void execute() override {
      result_ = function_(arg1_, arg2_);
      is_solved_ = true;
    }

    bool isSolved() override {
      return is_solved_;
    }

    my::any getResult() override {
      return result_;
    }

  private:
    my::any result_;

    Function function_;
    Argument1 arg1_;
    Argument2 arg2_;
    bool is_solved_;
    size_t index_;
  };

class TaskDescriptor {
public:
  TaskDescriptor(size_t id, const std::shared_ptr<Task>& task)
    : id_(id), task_(task) {}

  size_t getId() const { return id_; }
  bool isSolved() const { return task_->isSolved(); }
  my::any getResult() const { return task_->getResult(); }

private:
  size_t id_;
  std::shared_ptr<Task> task_;
};

class TTaskScheduler {
public:

  TTaskScheduler() : sorted_(false), cur_index_(0) {}

  template <class Function>
  TaskDescriptor add(const Function& function) {
    auto task = std::make_shared<Task0<Function>>(function, cur_index_);
    AddTask(task);
    return TaskDescriptor(cur_index_++, task);
  }

  template <class Function, class Argument>
  TaskDescriptor add(const Function& function, Argument arg) {
    auto task = std::make_shared<Task1<Function, Argument>>(function, arg, cur_index_);
    AddTask(task);
    return TaskDescriptor(cur_index_++, task);
  }

  template <class Function, class Argument1, class Argument2>
  TaskDescriptor add(const Function& function, Argument1 arg1, Argument2 arg2) {
    auto task = std::make_shared<Task2<Function, Argument1, Argument2>>(function, arg1, arg2, cur_index_);
    AddTask(task);
    return TaskDescriptor(cur_index_++, task);
  }

  template <class ResultType>
  auto getResult(const TaskDescriptor& taskDescriptor) {
    int index = taskDescriptor.getId();
    if (!tasks_[index]->isSolved()) {
      executeAll();
    }
    return tasks_[index]->getResult().get<ResultType>();
  }

  template <class ResultType>
  auto getFutureResult(const TaskDescriptor& taskDescriptor) {
    int index = taskDescriptor.getId();
    graph_[index].push_back(cur_index_);
    sorted_ = false;

    return FutureResult<ResultType>(tasks_[index]);
  }

  void executeAll() {
    if (!sorted_) {
      TopologicalSort();
    }
    for (int i = 0; i < cur_index_; ++i) {
      if (!tasks_[order_[i]]->isSolved()) {
        tasks_[order_[i]]->execute();
      }
    }
  }
  
private:

  void AddTask(const std::shared_ptr<Task>& task) {
    tasks_.push_back(task);
    graph_.emplace_back();
    order_.push_back(cur_index_);
  }

  void DFS(std::vector<bool>& visited, size_t index) {
    visited[index] = true;
    for (auto to : graph_[index]) {
      if (!visited[to]) {
        DFS(visited, to);
      }
    }
    order_.push_back(index);
  }

  void TopologicalSort() {
    order_.clear();
    std::vector<bool> visited(cur_index_);
    for (size_t i = 0; i < cur_index_; ++i) {
      if (!visited[i]) {
        DFS(visited, i);
      }
    }
    std::reverse(order_.begin(), order_.end());
    sorted_ = true;
  }

  template<class ResultType>
  class FutureResult {
  public:
    FutureResult(const std::shared_ptr<Task>& ptr) : ptr_(ptr.get()) {}

    operator ResultType() {
      return ptr_->getResult().get<ResultType>();
    }

  private:
    Task* ptr_;
  };

  std::vector<std::shared_ptr<Task>> tasks_;
  std::vector<std::vector<size_t>> graph_;
  std::vector<size_t> order_;
  bool sorted_;
  size_t cur_index_;
};



