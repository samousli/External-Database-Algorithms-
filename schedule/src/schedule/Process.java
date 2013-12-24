package schedule;

public class Process {
    
    /**
     *  The time at which this process was created. 
     */
    private final int arrivalTime;
    /**
     *  Burst time. Time that is required to complete execution of the particular process.
     */
    private final int cpuTotalTime;
    /**
     *  Time left, the execution may be interrupted.
     */
    private int cpuRemainingTime;
    
    /**
     * Response time, the first time the process gets the CPU
     */
    private int responseTime;
    
    /**
     *  The current state of the process 
     */
    private ProcessState currentState;
    
    /**
     *  Process ID 
     */
    private final int processID;
    
    /**
     * 
     * @param pid the process ID.
     * @param arrivalTime current time according to system Clock.
     * @param cpuBurstTime time required for the process to finish.
     */
    public Process(int pid, int arrivalTime, int cpuBurstTime) {
        this.processID = pid;
        this.arrivalTime = arrivalTime;
        this.cpuTotalTime = cpuBurstTime;
        this.cpuRemainingTime = cpuBurstTime;
        this.currentState  = ProcessState.NEW; 
        this.responseTime = -1 ;
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
        System.out.println("Response time:  " + this.responseTime);
        System.out.println("Remaining time: " + this.cpuRemainingTime);
        System.out.println("Current state:  " + this.getCurrentState());
        System.out.println("=====================================");
    }
    
    /**
     * 
     * @return the arrival time
     */
    public int getArrivalTime()
    {
        return this.arrivalTime;
    }
    
    /**
     * 
     * @return the total CPU time the process requires to complete
     */
    public int getCpuTotalTime()
    {
        return this.cpuTotalTime;
    }
    
    /**
     * 
     * @return the remaining time to complete
     */
    public int getCpuRemainingTime()
    {
        return this.cpuRemainingTime;
    }

    /**
     * @return the currentState
     */
    public ProcessState getCurrentState() {
        return currentState;
    }

    /**
     * @return the process ID
     */
    public int getID() {
        return this.processID;
    }
    
    /**
     * 
     * @return the response time of this process
     * It is negative if the process never got the CPU so far
     */
    public int getResponseTime()
    {
        return this.responseTime;
    }
    
    /**
     * Sets the field response time to the parameter time
     * @param time it is the time process got the CPU for the first time
     */
    public void setResponseTime(int time)
    {
        this.responseTime = time;
    }
}
