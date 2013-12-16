package schedule;

import java.io.File;
import java.util.List;

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
     * The time elapsed since the arrival of the process and the first time it 
     * got CPU time. 
     */
    private int responseTime;
    
    private int maximumLengthOfReadyProcessesList;
    private int totalNumberOfProcesses;
    
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
     * [current time - (arrival time + CPU time)] / number of processes
     * where CPU time is the total time the process spent in the CPU 
     * and is calculated as: total CPU time - remaining CPU time
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

    /**
     * @return the total number of processes
     */
    public int getTotalNumberOfProcesses() {
        return totalNumberOfProcesses;
    }
}
