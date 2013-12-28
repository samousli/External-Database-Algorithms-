package schedule;

import java.util.List;
import java.util.Queue;

public class SJFStatistics extends Statistics {

    public SJFStatistics(String filename) {
        super(filename, "SJF");
    }
    
    /**
     * Calculates the average waiting time based on the processes still waiting to be executed and
     * the terminated processes
     * @param processQueue, a priority queue used in the SJF scheduler
     * @param terminatedProcesses the list with the terminated processes
     * @return the average waiting time 
     */
    public float CalculateAverageWaitingTime(Queue processQueue, List terminatedProcesses)
    {
        int total_waiting_time = 0;
        int size = terminatedProcesses.size() + processQueue.size();
        Queue queue = processQueue;
        Process process = null;
        
        for (int i = 0; i < terminatedProcesses.size(); i++)
        {
            process = (Process) terminatedProcesses.get(i);
            total_waiting_time += process.getTerminationTime() - 
                    (process.getArrivalTime() + process.getCpuTotalTime());
        }
        
        while(!queue.isEmpty())
        {
            process = (Process) queue.poll();
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
     * @param processQueue, a priority queue used in the SJF scheduler
     * @param terminatedProcesses the list with the terminated processes
     * @return the average response time
     */
    public float CalculateAverageResponseTime(Queue processQueue, List terminatedProcesses)
    {
        int total_response_time = 0;
        int size = terminatedProcesses.size() + processQueue.size();
        Process process = null;
        Queue queue = processQueue;
        
        for (int i = 0; i < terminatedProcesses.size(); i++)
        {
            process = (Process) terminatedProcesses.get(i);
            total_response_time += process.getResponseTime() - process.getArrivalTime();
        }
        
       while(!queue.isEmpty())
        {
            process = (Process) queue.poll();
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
    public void updateStatistics(Queue processQueue, List terminatedProcesses) {
        this.CalculateAverageWaitingTime(processQueue, terminatedProcesses);
        this.CalculateAverageResponseTime(processQueue, terminatedProcesses);
        this.totalNumberOfProcesses = processQueue.size() + terminatedProcesses.size();
        this.UpdateMaximumListLength(processQueue.size());
    }
    
}
