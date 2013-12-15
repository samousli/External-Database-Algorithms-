/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package schedule;

/**
 * 
 * @author avail
 */
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
    private int currentState;
    
    /**
     *  Process ID 
     */
    private final int processID;
    

    public Process(int pid, int arrivalTime, int cpuBurstTime) {
        this.processID = pid;
        this.arrivalTime = arrivalTime;
        this.cpuTotalTime = cpuBurstTime;
        this.cpuRemainingTime = cpuBurstTime;
        this.currentState  = 0; // New
    }

    public void setProcessState(int newState) {
        this.currentState = newState;
    }

    public void changeCpuRemainingTime(int newCpuRemainingTime) {
        this.cpuRemainingTime = newCpuRemainingTime;
    }

}
