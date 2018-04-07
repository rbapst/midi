#include <iostream>
#include <thread>
#include <fstream>
#include <signal.h>
#include <unistd.h>

bool done;
static void finish(int ignore) { done = true; }

void task1(std::string msg)
{
  std::cout << "task1: " << msg;
}

void task2()
{
  std::cout << "task2: ";
}

void setup() {
}

void loop() {
}

int main(int argc, char *argv[]) {
  done = false;
  std::thread t1(task1, "Hello");
  std::thread t2(task2);

  setup();
  (void) signal(SIGINT, finish);
  t2.join();

  while(!done) {
    loop();
  }
  t2.join();
  t1.join();
}

