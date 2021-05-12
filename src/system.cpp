#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
  vector<Process> foundProcesses{};
  vector<int> processIds = LinuxParser::Pids();
  for (int p : processIds) {
    Process pro{p};
    foundProcesses.push_back(pro);
  }

  sort(foundProcesses.begin(), foundProcesses.end(),
       [](const Process& pa, const Process& pb) {
         return (pb.CpuUtilization() < pa.CpuUtilization());
       });
  processes_ = foundProcesses;

  return processes_;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }