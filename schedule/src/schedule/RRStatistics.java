package schedule;

import java.util.List;

/**
 * Collects statistics for the RR scheduler and logs them to a file.
 *
 */
public class RRStatistics extends Statistics {

    /**
     * constructor of the class
     *
     * @param filename the file to write the statistics
     */
    public RRStatistics(String filename) {

        super(filename);
    }


    /**
     *
     * @param ProcessList the list containing the processes waiting to be
     * executed
     * @param terminatedProcesses the list containing the terminated processes
     * @return the average waiting time calculated as: 
     * 
     * for still waiting processes : [sum of every
     * process(current time - (arrival time + CPU time))] 
     * where CPU time is the total time the process spent in the CPU and is
     * calculated as: total CPU time - remaining CPU time
     * 
     * for terminated processes : sum of every process(termination time - (arrival time + burst time))
     * 
     * finally average waiting time = the sum of these two values / size
     * where size if the sum of the number of the two lists
     */
    public float CalculateAverageWaitingTime(List ProcessList, List terminatedProcesses) {
        int total_waiting_time = 0;
        int size = ProcessList.size() + terminatedProcesses.size();
        Process process = null;

        for (int i = 0; i < terminatedProcesses.size(); i++)
        {
            process = (Process) terminatedProcesses.get(i);
            total_waiting_time += process.getTerminationTime() - 
                    (process.getArrivalTime() + process.getCpuTotalTime());
        }
        
        for (int i = 0; i < ProcessList.size(); i++) {
            process = (Process) ProcessList.get(i);
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
     * Calculates the average response time of a process list and a terminated process
     * @param ProcessList,the list with all the processes waiting
     * @param terminatedProcesses,the list with the terminated processes
     * @return the average response time = sum of response time of every process / number of processes
     */
    public float CalculateAverageResponseTime(List ProcessList, List terminatedProcesses)
    {
        int total_response_time = 0;
        int size = ProcessList.size() + terminatedProcesses.size();
        Process process = null;
        
        for (int i = 0; i < terminatedProcesses.size(); i++)
        {
            process = (Process) terminatedProcesses.get(i);
            total_response_time += process.getResponseTime() - process.getArrivalTime();
        }
        
        for (int i = 0; i < ProcessList.size(); i++) {
            process = (Process) ProcessList.get(i);
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
     * @param ProcessList is the list containing the processes waiting the CPU
     * @param terminatedProcesses is the list containing the terminated processes
     */
    public void updateStatistics(List ProcessList, List terminatedProcesses) {
        this.CalculateAverageWaitingTime(ProcessList, terminatedProcesses);
        this.CalculateAverageResponseTime(ProcessList, terminatedProcesses);
        this.totalNumberOfProcesses = ProcessList.size() + terminatedProcesses.size();
        this.UpdateMaximumListLength(ProcessList.size());
    }

}
