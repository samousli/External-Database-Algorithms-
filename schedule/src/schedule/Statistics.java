package schedule;

import java.io.File;

/**
 * Collects statistics and logs them to a file.
 * 
 */
public class Statistics {
    /**
     * The average waiting time of the currently waiting processes
     */
    private float averageWaitingTime;
    
    /**
     * The total sum of the times every process has waited so far
     */
    private int totalWaitingTime;
    
    /**
     * The lapse time since the arrival of a process and the first time it 
     * got cpu time 
     */
    private int responseTime;
    
    private int maximumLengthOfReadyProcessesList;
    public int totalNumberOfProcesses;
    
    /**
     * The file for saving the statistics
     */
    private File outputFile;

    /**
     * constructor of the class
     * @param filename the file to write the statistics
     */
    public Statistics(String filename) {
    }

    
    public void UpdateMaximumListLength() {
    }

    /**
     * 
     * @param ProcessList the list containing the processes waiting to be executed
     * @return the average waiting time calculated as:
     * [sum of every process(current time - (arrival time + cpu time))] / number of processes
     * where cpu time is the total time the process spent in the cpu 
     * and is calculated as: total cpu time - remaining cpu time
     */
    public float CalculateAverageWaitingTime(ReadyProcessesList ProcessList) {
        float total_waiting_time = 0;
        
        for (int i = 0; i < ProcessList.getListSize(); i++)
        {
            Process process = ProcessList.getProcess(i);
            total_waiting_time += Clock.showTime() - (process.getArrivalTime() + 
                    (process.getCpuTotalTime() - process.getCpuRemainingTime()));
        }
        float result = total_waiting_time / ProcessList.getListSize();
        return result;
    }

    public void WriteStatistics2File() {
    }
}
