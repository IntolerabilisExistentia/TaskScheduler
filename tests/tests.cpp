#include <gtest/gtest.h>
#include <cmath>
#include "../lib/Sheduler.hpp"

TEST(Task0Test, ExecuteAndGetResult) {
    TTaskScheduler scheduler;
    TaskDescriptor descriptor = scheduler.add([](){ return 42;});

    EXPECT_FALSE(descriptor.isSolved());
    
    scheduler.executeAll();
    
    EXPECT_TRUE(descriptor.isSolved());
    EXPECT_EQ(descriptor.getResult().get<int>(), 42);
}

TEST(Task1Test, ExecuteAndGetResult) {
    TTaskScheduler scheduler;
    TaskDescriptor descriptor = scheduler.add([](int a){ return a * 2;}, 5);

    EXPECT_FALSE(descriptor.isSolved());
    
    scheduler.executeAll();
    
    EXPECT_TRUE(descriptor.isSolved());
    EXPECT_EQ(descriptor.getResult().get<int>(), 10);
}

TEST(Task2Test, ExecuteAndGetResult) {
    TTaskScheduler scheduler;
    TaskDescriptor descriptor = scheduler.add([](int a, int b){ return a + b; }, 10, 20);

    EXPECT_FALSE(descriptor.isSolved());
    
    scheduler.executeAll();
    
    EXPECT_TRUE(descriptor.isSolved());
    EXPECT_EQ(descriptor.getResult().get<int>(), 30);
}

TEST(TTaskSchedulerTest, FutureResult) {
    TTaskScheduler scheduler;
    auto task1 = scheduler.add([](int a){ return a * 2;}, 3);
    auto task2 = scheduler.add([](int a, int b){ return a + b; }, 4, 2); 
    auto task3 = scheduler.add([](int a, int b){ return a + b; }, scheduler.getFutureResult<int>(task1), scheduler.getFutureResult<int>(task2));

    EXPECT_FALSE(task3.isSolved());
    
    scheduler.getResult<int>(task3);
    EXPECT_TRUE(task3.isSolved());
    EXPECT_EQ(task3.getResult().get<int>(), 12);
}

TEST(TaskDescriptorTest, IsSolved) {
    TTaskScheduler scheduler;
    TaskDescriptor descriptor = scheduler.add([](int a){ return a * 2;}, 1);

    EXPECT_FALSE(descriptor.isSolved());
    
    scheduler.executeAll();
    
    EXPECT_TRUE(descriptor.isSolved());
    EXPECT_EQ(descriptor.getResult().get<int>(), 2);
}

class A {
public:
  int foo() {
    return 10;
  }
};

TEST(TaskDescriptorTest, CustomClass) {
    TTaskScheduler scheduler;
    A c;
    TaskDescriptor descriptor = scheduler.add([](A a){ return a.foo();}, c);

    EXPECT_FALSE(descriptor.isSolved());
    
    scheduler.executeAll();
    
    EXPECT_TRUE(descriptor.isSolved());
    EXPECT_EQ(descriptor.getResult().get<int>(), 10);
}

TEST(ExampleTest, Example) {
  float a = 1;
  float b = -2;
  float c = 0;

  TTaskScheduler scheduler;

  auto id1 = scheduler.add([](float a, float c){return -4 * a * c;}, a, c);

  auto id2 = scheduler.add([](float b, float v){return b * b + v;}, b, scheduler.getFutureResult<float>(id1));

  auto id3 = scheduler.add([](float b, float d){return -b + std::sqrt(d);}, b, scheduler.getFutureResult<float>(id2));

  auto id4 = scheduler.add([](float b, float d){return -b - std::sqrt(d);}, b, scheduler.getFutureResult<float>(id2));

  auto id5 = scheduler.add([](float a, float v){return v/(2*a);}, a, scheduler.getFutureResult<float>(id3));

  auto id6 = scheduler.add([](float a, float v){return v/(2*a);},a, scheduler.getFutureResult<float>(id4));

  scheduler.executeAll();

  ASSERT_EQ(scheduler.getResult<float>(id5), 2);
  ASSERT_EQ(scheduler.getResult<float>(id6), 0);
}
