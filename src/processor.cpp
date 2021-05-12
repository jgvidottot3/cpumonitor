#include "processor.h"
#include <string>
#include <vector>
#include "linux_parser.h"

using std::string;
using std::vector;

Processor::Processor() : prevTotalCpuTime(0.0), prevIdleCpuTime(0.0){};

float Processor::Utilization() {
  vector<long> cpuValues = convertToLong(LinuxParser::CpuUtilization());
  float totalCpuTime =
      cpuValues[LinuxParser::kUser_] + cpuValues[LinuxParser::kNice_] +
      cpuValues[LinuxParser::kSystem_] + cpuValues[LinuxParser::kIdle_] +
      cpuValues[LinuxParser::kIOwait_] + cpuValues[LinuxParser::kIRQ_] +
      cpuValues[LinuxParser::kSoftIRQ_] + cpuValues[LinuxParser::kSteal_];
  float idleCpuTime =
      cpuValues[LinuxParser::kIdle_] + cpuValues[LinuxParser::kIOwait_];

  float diffIdle = idleCpuTime - prevIdleCpuTime;
  float diffTotal = totalCpuTime - prevTotalCpuTime;
  float diffUsage = (diffTotal - diffIdle) / diffTotal;
  prevIdleCpuTime = idleCpuTime;
  prevTotalCpuTime = totalCpuTime;
  return diffUsage;
}

vector<long> Processor::convertToLong(vector<string> values) {
  vector<long> convertedValues{};

  for (int it = 0; it < (int)values.size(); it++) {
    try {
      convertedValues.push_back(std::stol(values[it]));
    } catch (const std::invalid_argument& arg) {
      convertedValues.push_back((long)0);
    }
  }
  return convertedValues;
}