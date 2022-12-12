#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <format.h>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');          return value;
        }
      }
    }
  }
  return value;
}



// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
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






// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string key, value, kb;
  string line;
  float memTotal;
  float memFree;

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value>>kb) {
        if (key == "MemTotal:") {
          memTotal = (std::stof(value));
        }
        else if (key == "MemFree:"){
          memFree = (std::stof(value));
        }
      }
   }
  }
  return (memTotal - memFree)/memTotal;
}


// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  long uptime;
  string line;

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}






// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return (LinuxParser::UpTime() * sysconf (_SC_CLK_TCK));
 }



// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string line;
  string value;
  string pidNum = to_string(pid);
  long ajiffies{0};

  std::ifstream stream(kProcDirectory + pidNum + kStatFilename);
  if (stream.is_open()) { 
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      for (int i=0; i<= 16; ++i){
        linestream >> value;

        if (i>12){ 
          if (value.empty()) { continue;}
          ajiffies += std::stol(value); }
      }
      return ajiffies;
    }
  }
 }


// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long ajiffies{0};
  string value;
  string line;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      for (int i = 0; i<=10; i++){
          linestream >> value;
          if (i>0) { 
            if (value.empty()) { continue;}
            ajiffies += std::stol(value);}
      }
      return ajiffies;
    }
  }
}


// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  long ijiffies{0};
  string value;
  string line;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      for (int i = 0; i<=5; i++){
          linestream >> value;
          if (i>3) { 
            if (value.empty()) { continue;}
            ijiffies += std::stol(value);}
        }
      return ijiffies;
      }
    }
  }



// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line, cpu, value;
  vector<string> cpu_utilisation;
  
  std::ifstream stream(kProcDirectory + kStatFilename);
  if(stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu;
    while (linestream >> value) {
      cpu_utilisation.emplace_back(value);
    } 
  }
  return cpu_utilisation; 

}



// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string key, value;
  string line;
  int processes;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value ) {
        if (key == "processes") {
          processes = (std::stoi(value));
          return processes;
        }
      }
   }
  }
  return processes;
 }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string key, value;
  string line;
  int processes;

  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while(linestream >> key >> value){
        if (key == "procs_running") {
          processes = (std::stoi(value));
          return processes;
        }
      }
    }
  }
  return processes;
}


// --------------------------------------------------------------------------->

float LinuxParser::CpuUtilization(int pid) { 
  string line, value;
  int i = 0;

  vector<string> cpu_util;

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()){
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> value && i<23) {
        cpu_util.emplace_back(value);
        i +=1;
      }
  }

  float total_time = ( stof(cpu_util[13]) + stof(cpu_util[14]) + stof(cpu_util[15]) + stof(cpu_util[16]) );
  float seconds = LinuxParser::UpTime() - (stof(cpu_util[21]) / sysconf(_SC_CLK_TCK));
  return (total_time/sysconf(_SC_CLK_TCK)/seconds);
  }
}



// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string value, line;
  string pidNum = to_string(pid);

  std::ifstream stream(kProcDirectory + pidNum + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return value.substr(0, 20);
}



// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line;
  string key, value;
  string pidNum = to_string(pid);

  std::ifstream stream(kProcDirectory + pidNum + kStatusFilename);
  if (stream.is_open()) { 
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value ) {
        if (key == "VmSize:") {
          return std::to_string(std::stol(value)/1024);
        }
      }
    }
  }
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line;
  string key, value;
  string pidNum = to_string(pid);

  std::ifstream stream(kProcDirectory + pidNum + kStatusFilename);
  if (stream.is_open()) { 
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  } 
}

// TODO: Read and return the  user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string line;
  string key, value, uid;
  string pidNum = to_string(pid);

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) { 
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value >> uid) {
        if (uid == LinuxParser::Uid(pid)) {
          return key;
        }
      }
    }
  }
}



// TODO: Read and retur n the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string line;
  string value;
  string pidNum = to_string(pid);
  vector<string> values;

  std::ifstream stream(kProcDirectory + pidNum + kStatFilename);
  if (stream.is_open()) { 
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> value){
        values.emplace_back(value);
      }
    }
  }
  return (std::stol(values[21]) / sysconf(_SC_CLK_TCK));
}
















