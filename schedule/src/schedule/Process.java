package schedule;

public class Process {
    
    /**
     *  The time at which this process was created. 
     */
    private int arrivalTime;
    /**
     *  Burst time. Time that is required to complete execution of the particular process.
     */
    private int cpuTotalTime;
    /**
     *  Time left, the execution may be interrupted.
     */
    private int cpuRemainingTime;
    
    /**
     *  The current state of the process 
     *  New         - 0
     *  Ready       - 1
     *  Running     - 2
     *  Terminated  - 3
     *  (Why not use an enumeration?)
     */
    private ProcessState currentState;
    
    /**
     *  Process ID 
     */
    private final int processID;
    

    public Process(int pid, int arrivalTime, int cpuBurstTime) {
        this.processID = pid;
        this.arrivalTime = arrivalTime;
        this.cpuTotalTime = cpuBurstTime;
        this.cpuRemainingTime = cpuBurstTime;
        this.currentState  = ProcessState.NEW; 
    }

    public void setProcessState(ProcessState newState) {
        this.currentState = newState;
    }

    public void changeCpuRemainingTime(int newCpuRemainingTime) {
        this.cpuRemainingTime = newCpuRemainingTime;
    }
    
    /*
    Prints the fields of the process
    */
    public void printProcess(){
        System.out.println("Process ID:     " + this.processID);
        System.out.println("Arrival time:   " + this.arrivalTime);
        System.out.println("Burst time:     " + this.cpuTotalTime);
        System.out.println("Remaining time: " + this.cpuRemainingTime);
        System.out.println("Current state:  " + this.currentState);
        System.out.println("=====================================");
    }

}
