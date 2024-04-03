#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <unistd.h>
#include <sstream>
#include <string>

using namespace std;

// program struct
struct Program {
    std::string name;
    int timeRequirement;
    int memoryRequirement;
    std::pair<int, int> ioOperations; // pairs of timeTodoIO and AmountOfIOTime
    
};

// job struct
struct Job {
    Program program;
    int timeLeft;
    int memoryUsage;
    int timeToFinish;
};

// RATCHSimulator class
class RATCHSimulator {

// private variables
private:
    int memory;
    int burstTime;
    std::vector<Program> programs;
    std::queue<Job> jobQueue;
    std::queue<Job> ioQueue;
    std::vector<Job> finishedJobs;
    Job currentJob;
    int currentTime = 0;
    
// public functions
public:
    // constructor
    RATCHSimulator() {
        memory = 0;
        burstTime = 0;
    }

    void addProgram(const Program& program);
    void setMemory(int memorySize);
    int  getMemory();
    void setBurst(int burst); // set the burst time which is the time quantum for the round robin scheduler meaning the maximum time a process can run before being interrupted
    void start(const std::string& programName);
    void step();
    void run();
    void printSystemState();
    int getBurst();
    int cburstTime = 0;

};


// addProgram function takes a program and adds it to the programs vector returning void
void RATCHSimulator::addProgram(const Program& program) {
    // Ensure that ioOperations are sorted in ascending order of timeTodoIO
   // std::sort(program.ioOperations.begin(), program.ioOperations.end());
    programs.push_back(program);
}

// setMemory function takes an integer memorySize and sets the memory to the memorySize returning void
void RATCHSimulator::setMemory(int memorySize) {
    memory = memorySize;
}

int RATCHSimulator::getMemory() {
    return memory;
}

// setBurst function takes an integer burst and sets the burstTime to the burst returning void
void RATCHSimulator::setBurst(int burst) {
    burstTime = burst;
}

// getBurst function returns the burstTime
int RATCHSimulator::getBurst() {
    return burstTime;
}

// start function takes a string programName and starts the program returning void
void RATCHSimulator::start(const std::string& programName) {
    

    for (auto& program : programs) {
        if (program.name == programName) { // if program is found
            if (program.memoryRequirement > memory) { // if memory is not enough
                std::cout << "Not enough memory to start program " << program.name << std::endl; // print error message
                return;
            }
            Job job;
            job.program = program; // set the program to the job
            job.timeLeft = program.timeRequirement; // time left to finish the program
            job.memoryUsage = program.memoryRequirement; // memory usage of the program
            jobQueue.push(job); // push the job to the job queue
            memory -= program.memoryRequirement; // subtract the memory usage of the program from the memory
            return;
        }
    }
    std::cout << "Program not found" << std::endl;
}

// step function runs the simulation for one step returning void
void RATCHSimulator::step() {
    if (currentTime == 0) {
        currentJob = jobQueue.front();
        jobQueue.pop();
        std::cout << "Advancing the system until all jobs finished\n";
        printSystemState();
    }
     
    if (!jobQueue.empty() || currentJob.timeLeft > 0) {
        // if current job is empty then get the next job from the job queue
        if (currentJob.timeLeft == 0) {
            currentJob = jobQueue.front();
            jobQueue.pop();
        }

        cburstTime += 1;
        currentJob.timeLeft -= 1;
        // if the current job has io operations then check if the timeTodoIO is equal to the cburstTime 
        // and if it is then push the job to the io queue and get the next job from the job queue
        if (currentJob.program.ioOperations.first == cburstTime) {
            ioQueue.push(currentJob);
            currentJob = jobQueue.front();
            jobQueue.pop();
            cburstTime = 0;
            currentTime += 1;
            printSystemState();
            return;
            
        }
        currentTime += 1;
        
        /*
        
        */
        

        // go through the io queue and check and subtract the amountofiotime by 1 and if it reaches 0 then send the job back to the job queue
        if (!ioQueue.empty()) {
            std::queue<Job> ioQueueCopy = ioQueue; // Create a copy of the queue
            while (!ioQueueCopy.empty()) {
                Job job = ioQueueCopy.front(); // get the job at the front of the queue
                job.program.ioOperations.second -= 1;
                job.program.ioOperations.first = 0;
                if (job.program.ioOperations.second == 0) { // if the timeTodoIO is 0
                    jobQueue.push(job); // send the job back to the job queue
                    ioQueue.pop(); // remove the job from the io queue
                }else {
                    ioQueue.pop(); // if the timeTodoIO is not 0 then remove the job from the copy of the io queue
                    ioQueue.push(job); // add the job back to the io queue
                }
                ioQueueCopy.pop(); // remove the job from the copy of the io queue
            }
        } 


        // if the current job timeleft is 0 then set the timeToFinish to the currentTime and subtract the memory usage of the program from the memory 
        //and push the job to the finished jobs vector
        if (currentJob.timeLeft == 0) {
            currentJob.timeToFinish = currentTime;
            finishedJobs.push_back(currentJob);
            currentJob = jobQueue.front();
            if (jobQueue.empty()) {
                cburstTime = 0;
            }else{
                jobQueue.pop();
                cburstTime = 0;
            }
            printSystemState();
    
        } 

        // if the cburstTime is equal to the burstTime then push the current job to the job queue and get the next job from the job queue
        if (cburstTime == burstTime) {
            jobQueue.push(currentJob);
            currentJob = jobQueue.front();
            jobQueue.pop();
            cburstTime = 0;
        }
    }
   
}
 

// run function runs the step function until the job queue is empty and the current job timeleft is 0 returning void
void RATCHSimulator::run() {
    while (!jobQueue.empty() || currentJob.timeLeft > 0) {
        step(); // run the step function
    } 
}

// printSystemState function prints the current time, the running job, the job queue, the finished jobs and the io queue
void RATCHSimulator::printSystemState() {
    std::cout << "Current time <" << currentTime << ">\n";
    if (!jobQueue.empty() || currentJob.timeLeft > 0) {
        std::cout << "Running job " << currentJob.program.name << " has " << currentJob.timeLeft << " time left and is using " << currentJob.program.memoryRequirement << " memory resources.\n";
    }
    if (jobQueue.empty() && currentJob.timeLeft == 0) {
        std::cout << "Running job is empty\n";
    }

    if (jobQueue.empty()) {
        std::cout << "The queue is: empty\n";

   }else {
        std::cout << "The queue is:\n";
        std::queue<Job> jobQueueCopy = jobQueue; // Create a copy of the queue
        int position = 1;
        while (!jobQueueCopy.empty()) {
            Job job = jobQueueCopy.front();
            std::cout << "  Position " << position << ": job " << job.program.name << " has " << job.timeLeft << " units left and is using " << job.program.memoryRequirement << " memory resources.\n";
            jobQueueCopy.pop();
            position++;
        }
    }
    

    if (!finishedJobs.empty()) {
        std::cout << "Finished jobs are:\n";
        for (const auto& job : finishedJobs) {
            std::cout << "  " << job.program.name << " " << job.program.timeRequirement << " " << job.timeToFinish << "\n";
        }
    }
    // The process first is obtaining IO and will be back in 1 unit.
    if (!ioQueue.empty()) {
        std::queue<Job> ioQueueCopy = ioQueue; // Create a copy of the queue
        while (!ioQueueCopy.empty()) {
            Job job = ioQueueCopy.front();
            std::cout << "The process " << job.program.name << " is obtaining IO and will be back in " << job.program.ioOperations.second << " units.\n";
            ioQueueCopy.pop();
        }
    }
    
}


// trim function takes a string str and trims the string returning the trimmed string
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

int main() {
    
   RATCHSimulator sim;

   
    while (true) {

      if (isatty(STDIN_FILENO)) {
        cout << "EnterCommand>";
        }

        string line;
        getline(cin, line);  

        if (line.empty()) {
        continue;  // Ignore empty lines
        }

        std::stringstream ss(line); // Use the fully qualified name for stringstream
        string command;
        getline(ss, command, ' '); // Parse command up to the first space


        string argument;
        getline(ss, argument); // Parse the rest of the line for the argument

        
        if (command == "getMemory") {
            if (argument.empty()) {
                cout << sim.getMemory() << endl; // print the memory
            } else {
                cout << "Invalid command. Please try again." << endl;
            }
        } else if (command == "setMemory") {
            int memorySize;
            argument = trim(argument);
            memorySize = stoi(argument);
            sim.setMemory(memorySize); // set the memory
        } else if (command == "addProgram") {
            std::string name = argument.substr(0, argument.find(" "));
            argument = argument.substr(argument.find(" ") + 1);

            int timeRequirement = stoi(argument.substr(0, argument.find(" ")));
            argument = argument.substr(argument.find(" ") + 1);

            int memoryRequirement = stoi(argument.substr(0, argument.find(" ")));
            argument = argument.substr(argument.find(" ") + 1);

            int timeTodoIO = 0;
            int AmountOfIOTime = 0;

            
            if (argument.find(" ") != std::string::npos) {
                timeTodoIO = stoi(argument.substr(0, argument.find(" ")));
                argument = argument.substr(argument.find(" ") + 1);
                AmountOfIOTime = stoi(argument);
            }

            

            Program program;
            program.name = name;
            program.timeRequirement = timeRequirement;
            program.memoryRequirement = memoryRequirement;
            program.ioOperations = {timeTodoIO, AmountOfIOTime};
            sim.addProgram(program); // add the program
        } else if (command == "setBurst") {
            int burst;
            argument = trim(argument);
            burst = stoi(argument); // convert the argument to an integer
            sim.setBurst(burst); // set the burst
        } else if (command == "start") {
            argument = trim(argument);
            sim.start(argument); // start the program
        } else if (command == "step") {
            argument = trim(argument);
            int steps = stoi(argument); // convert the argument to an integer
            for (int i = 0; i < steps; i++) {
                sim.step(); // run the simulation for the number of steps
            }
        } else if (command == "run") {
            if (argument.empty()) {
                sim.run(); // run the simulation
            } else{
                cout << "Invalid command. Please try again." << endl;
            }
        }  else if (command == "quit") {
            break; // exit the program
        }else {
            cout << "Invalid command. Please try again." << endl;
        }
    }
    
    return 0;
    
}

