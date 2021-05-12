#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : processId_(pid) {
  cpuUsage();
  setCommand();
  setRam();
  setUptime();
  setUser();
}

int Process::Pid() { return processId_; }

float Process::CpuUtilization() const { return cpuUsage_; }

string Process::Command() { return command_; }

string Process::Ram() { return ram_; }

string Process::User() { return user_; }

long int Process::UpTime() { return uptime_; }

void Process::cpuUsage() {
  long uptime = LinuxParser::UpTime();
  vector<float> cpuVal = LinuxParser::CpuUtilization(Pid());
  if (cpuVal.size() == 5) {
    float totaltime =
        cpuVal[kUtime_] + cpuVal[kStime_] + cpuVal[kCutime_] + cpuVal[kCstime_];
    float seconds = uptime - cpuVal[kStarttime_];
    cpuUsage_ = totaltime / seconds;
  } else
    cpuUsage_ = 0;
}

void Process::determineUser() { user_ = LinuxParser::User(Pid()); }
void Process::setCommand() { command_ = LinuxParser::Command(Pid()); }
void Process::setRam() {
  string val = LinuxParser::Ram(Pid());
  try {
    long conv = std::stol(val) / 1000;
    ram_ = std::to_string(conv);
  } catch (const std::invalid_argument& arg) {
    ram_ = "0";
  }
}
void Process::setUptime() {
  uptime_ = LinuxParser::UpTime(Pid());
}