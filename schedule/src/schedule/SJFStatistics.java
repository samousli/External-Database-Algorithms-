package schedule;

import java.util.List;
import java.util.PriorityQueue;
import java.util.Queue;

public class SJFStatistics extends Statistics {

    public SJFStatistics(String filename) {
        super(filename, "SJF");
    }
    
    /**
     * Calculates the average waiting time based on the processes still waiting to be executed and
     * the terminated processes
     * @param processList, a priority queue used in the SJF scheduler
     * @param terminatedProcesses the list with the terminated processes
     * @return the average waiting time 
     */
    public float CalculateAverageWaitingTime(List processList, List terminatedProcesses) {
        int total_waiting_time = 0;
        int size = processList.size() + terminatedProcesses.size();
        Process process = null;

        for (int i = 0; i < terminatedProcesses.size(); i++)
        {
            process = (Process) terminatedProcesses.get(i);
            total_waiting_time += process.getTerminationTime() - 
                    (process.getArrivalTime() + process.getCpuTotalTime());
        }
        
        for (int i = 0; i < processList.size(); i++) {
            process = (Process) processList.get(i);
                total_waiting_time += Clock.showTime() - (process.getArrivalTime()
                        + (process.getCpuTotalTime() - process.getCpuRemainingTime()));
        }
        this.totalWaitingTime = total_waiting_time;
        if (size > 0) {
            this.averageWaitingTime =  (float) total_waiting_time / (float) size;
        }
        return this.averageWaitingTime;
    }
    
    /**
     * Calculates the average response time based on the processes still waiting to be executed and
     * the terminated processes
     * @param processList, a priority queue used in the SJF scheduler
     * @param terminatedProcesses the list with the terminated processes
     * @return the average response time
     */
    public float CalculateAverageResponseTime(List processList, List terminatedProcesses)
    {
        int total_response_time = 0;
        int size = processList.size() + terminatedProcesses.size();
        Process process = null;
        
        for (int i = 0; i < terminatedProcesses.size(); i++)
        {
            process = (Process) terminatedProcesses.get(i);
            total_response_time += process.getResponseTime() - process.getArrivalTime();
        }
        
        for (int i = 0; i < processList.size(); i++) {
            process = (Process) processList.get(i);
            if (process.getResponseTime() >= 0) 
            {
                total_response_time += process.getResponseTime() - process.getArrivalTime();
            }
        }
        this.responseTime = total_response_time;
        if (size > 0) {
            this.responseTime =  (float) total_response_time / (float) size;
        }
        return this.responseTime;
    }
    
    /**
     * updates every field in RRStatistics for the current clock time
     *
     * @param processQueue is the queue containing the processes waiting the CPU
     * @param terminatedProcesses is the list containing the terminated processes
     */
    public void updateStatistics(List processList, List terminatedProcesses) {
        this.CalculateAverageWaitingTime(processList, terminatedProcesses);
        this.CalculateAverageResponseTime(processList, terminatedProcesses);
        this.totalNumberOfProcesses = processList.size() + terminatedProcesses.size();
        this.UpdateMaximumListLength(processList.size());
    }
    
}
