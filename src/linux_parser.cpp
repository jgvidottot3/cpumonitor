#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string linha;
  string chave;
  string valor;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, linha)) {
      std::replace(linha.begin(), linha.end(), ' ', '_');
      std::replace(linha.begin(), linha.end(), '=', ' ');
      std::replace(linha.begin(), linha.end(), '"', ' ');
      std::istringstream linestream(linha);
      while (linestream >> chave >> valor) {
        if (chave == "PRETTY_NAME") {
          std::replace(valor.begin(), valor.end(), '_', ' ');
          return valor;
        }
      }
    }
  }
  return valor;
}

string LinuxParser::Kernel() {
  string os, version, kernel;
  string linha;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, linha);
    std::istringstream linestream(linha);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    if (file->d_type == DT_DIR) {
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  float memoryTotal = 0.0;
  float memFree = 0.0;
  string linha;
  string chave;
  string valor;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, linha)) {

      std::remove(linha.begin(), linha.end(), ' ');
      std::replace(linha.begin(), linha.end(), ':', ' ');
      std::istringstream linestream(linha);
      while (linestream >> chave >> valor) {
        if (chave == "MemTotal") {
          memoryTotal = std::stof(valor);
        }
        else if (chave == "MemFree") {
          memFree = std::stof(valor);
          break;
        }
      }
    }
  }

  return ((memoryTotal - memFree) / memoryTotal);
}

long LinuxParser::UpTime() {
  string linha;
  string wholeTime;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, linha)) {
      std::istringstream linestream(linha);
      while (linestream >> wholeTime) {
        try {
          return std::stol(wholeTime);
        } catch (const std::invalid_argument& arg) {
          return 0;
        }
      }
    }
  }
  return 0;
}

long LinuxParser::Jiffies() { return 0; }


long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

long LinuxParser::ActiveJiffies() { return 0; }

long LinuxParser::IdleJiffies() { return 0; }

vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpuvalors{};
  string linha;
  string chave;
  string ruser, rnice, rsystem, ridle, riowait, rirq, rsoftirq, rsteal, rguest,
      rguest_nice;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, linha)) {
      std::istringstream linestream(linha);
      while (linestream >> chave >> ruser >> rnice >> rsystem >> ridle >>
             riowait >> rirq >> rsoftirq >> rsteal >> rguest >> rguest_nice) {
        if (chave == "cpu") {
          cpuvalors.push_back(ruser);
          cpuvalors.push_back(rnice);
          cpuvalors.push_back(rsystem);
          cpuvalors.push_back(ridle);
          cpuvalors.push_back(riowait);
          cpuvalors.push_back(rirq);
          cpuvalors.push_back(rsoftirq);
          cpuvalors.push_back(rsteal);
          cpuvalors.push_back(rguest);
          cpuvalors.push_back(rguest_nice);
          return cpuvalors;
        }
      }
    }
  }
  return {};
}

int LinuxParser::TotalProcesses() {
  string linha;
  string chave;
  string valor;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, linha)) {
      std::istringstream linestream(linha);
      while (linestream >> chave >> valor) {
        if (chave == "processes") {
          try {
            return std::stol(valor);
          } catch (const std::invalid_argument& arg) {
            return 0;
          }
        }
      }
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  string linha;
  string chave;
  string valor;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, linha)) {
      std::istringstream linestream(linha);
      while (linestream >> chave >> valor) {
        if (chave == "procs_running") {
          try {
            return std::stol(valor);
          } catch (const std::invalid_argument& arg) {
            return 0;
          }
        }
      }
    }
  }
  return 0;
}

string LinuxParser::Command(int pid) {
  string valor = "";
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) +
                           kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, valor);
    return valor;
  }
  return valor;
}


vector<float> LinuxParser::CpuUtilization(int pid) {
  vector<float> cpuvalors{};
  string linha;
  float time = 0;
  string valor;
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, linha)) {
      std::istringstream linestream(linha);
      for (int i = 1; i <= kStarttime_; i++) {
        linestream >> valor;

        if (i == kUtime_ || i == kStime_ || i == kCutime_ || i == kCstime_ ||
            i == kStarttime_) {

          time = std::stof(valor) / sysconf(_SC_CLK_TCK);
          cpuvalors.push_back(time);
        }
      }
    }
  }
  return cpuvalors;
}

string LinuxParser::Ram(int pid) {
  string linha;
  string chave;
  string valor = "";
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, linha)) {
      std::replace(linha.begin(), linha.end(), ':', ' ');
      std::istringstream linestream(linha);
      while (linestream >> chave >> valor) {
        if (chave == "VmSize") {
          return valor;
        }
      }
    }
  }
  return valor;
}

string LinuxParser::Uid(int pid) {
  string linha;
  string chave;
  string valor = "";
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, linha)) {
      std::replace(linha.begin(), linha.end(), ':', ' ');
      std::istringstream linestream(linha);
      while (linestream >> chave >> valor) {
        if (chave == "Uid") {
          return valor;
        }
      }
    }
  }
  return valor;
}

string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  string linha;
  string chave;
  string valor = "";
  string other;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, linha)) {
      std::replace(linha.begin(), linha.end(), ':', ' ');
      std::istringstream linestream(linha);
      while (linestream >> valor >> other >> chave) {
        if (chave == uid) {
          return valor;
        }
      }
    }
  }
  return valor;
}

long LinuxParser::UpTime(int pid) {
  string linha;
  long uptime = 0;
  string valor;
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, linha)) {
      std::istringstream linestream(linha);
      for (int i = 1; i <= kStarttime_; i++) {
        linestream >> valor;
        if (i == kStarttime_) {
          try {
            uptime = std::stol(valor) / sysconf(_SC_CLK_TCK);
            return uptime;
          } catch (const std::invalid_argument& arg) {
            return 0;
          }
        }
      }
    }
  }
  return uptime;
}