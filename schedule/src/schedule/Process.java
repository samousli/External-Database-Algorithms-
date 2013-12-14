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

    private int arrivalTime;
    private int cpuTotalTime;
    private int cpuRemainingTime;

    private int currentState;

    public Process(int pid, int arrivalTime, int cpuBurstTime) {
    }

    public int setProcessState(int newState) {
        return 0;
    }

    public void changeCpuRemainingTime(int newCpuRemainingTime) {
        this.cpuRemainingTime = newCpuRemainingTime;
    }

}
