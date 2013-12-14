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
public class CPU {

    /**
     * Currently running process
     */
    private Process runningProcess;
    /**
     * Time to interrupt
     */
    private int timeToNextContextSwitch;
    /**
     * The time the previous process started
     */ 
    private int lastProcessStartTime;
    /**
     * A waiting queue for new processes.
     */
    private NewProcessTemporaryList processList;

    public CPU() {
        this.runningProcess = null;
        this.lastProcessStartTime = 0;
        this.timeToNextContextSwitch = 0;
    }

    /**
     * Adds a process that's ready to be executed.
     * @param process the process that's ready to be executed.
     */
    public void addProcess(Process process) {
        //this.runningProcess = process;
    }

    /**
     * Removes the running process
     * @return The process that got interrupted
     */
    public Process removeProcessFromCpu() {
        //ToDo: Logs here.
        Process p = this.runningProcess;
        this.runningProcess = null; 
        return p;
    }

    /**
     * Execute process and update clock accordingly.
     */
    public void execute() {
    }
}
