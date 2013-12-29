
package schedule;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;


public class Statistics {
    
    /**
     * The average waiting time of the currently waiting processes
     */
    protected float averageWaitingTime;
    
    /**
     * The total sum of the times every process has waited so far
     */
    protected int totalWaitingTime;
    
    /**
     * The time since the arrival of the process and the first time it
     * got CPU time.
     */
    protected float responseTime;
    
    /**
     * Keeps the maximum,up to the point,length of ready process list
     */
    protected int maximumLengthOfReadyProcessesList;
    
    /**
     * Total number of processes up to this point
     */
    protected int totalNumberOfProcesses;
    
    /**
     * The file for saving the statistics
     */
    protected  File outputFile;
    
    public Statistics(String filename, String scheduler)
    {
        filename = scheduler + filename;
        this.outputFile = new File(filename);

        if (this.outputFile.exists()) {
            this.outputFile.delete();
        }
        try {
            this.outputFile.createNewFile();
        } catch (IOException ex) {
            Logger.getLogger(Statistics.class.getName()).log(Level.SEVERE, 
                    "Can't create output file.", ex);
        }
    }
    
    /**
     * updates the field MaximumListLength
     * @param length
     */
    public void UpdateMaximumListLength(int length) {
        if (this.maximumLengthOfReadyProcessesList < length)
        {
            this.maximumLengthOfReadyProcessesList = length;
        }
    }
    
        /**
     * Writes current saved statistics to output file
     */
    public void WriteStatistics2File() {
        try {
            PrintWriter fileWriter;
            fileWriter = new PrintWriter(new BufferedWriter(new FileWriter(this.outputFile, true)));
            fileWriter.println("_________Statistics________");
            fileWriter.println("Maximum list length:       " + this.maximumLengthOfReadyProcessesList);
            fileWriter.println("Total waiting time:        " + this.totalWaitingTime);
            fileWriter.println("Average waiting time:      " + this.averageWaitingTime);
            fileWriter.println("Average response time:     " + this.responseTime);
            fileWriter.println("Time:                      " + Clock.showTime());
            fileWriter.println("___________________________");
            fileWriter.close();

        } catch (IOException ex) {
            Logger.getLogger(Statistics.class.getName()).log(Level.SEVERE, 
                    "Can't write new stats to output file.", ex);
        }

    }
    
    /**
     * @return the total number of processes
     */
    public int getTotalNumberOfProcesses() {
        return totalNumberOfProcesses;
    }
    
    /**
     * 
     * @return the total CPU run time  
     */
    public int getTotalRunTime() {
        return Clock.showTime() - this.totalWaitingTime;
    }

    /**
     * Calculates the average response time of a process list and a terminated process
     * @param ProcessList,the list with all the processes waiting
     * @param terminatedProcesses,the list with the terminated processes
     * @return the average response time = sum of response time of every process / number of processes
     */
    public float CalculateAverageResponseTime(List ProcessList, List terminatedProcesses) {
        int total_response_time = 0;
        int size = ProcessList.size() + terminatedProcesses.size();
        Process process = null;
        for (int i = 0; i < terminatedProcesses.size(); i++) {
            process = (Process) terminatedProcesses.get(i);
            total_response_time += process.getResponseTime() - process.getArrivalTime();
        }
        for (int i = 0; i < ProcessList.size(); i++) {
            process = (Process) ProcessList.get(i);
            if (process.getResponseTime() >= 0) {
                total_response_time += process.getResponseTime() - process.getArrivalTime();
            }
        }
        this.responseTime = total_response_time;
        if (size > 0) {
            this.responseTime = (float) total_response_time / (float) size;
        }
        return this.responseTime;
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
        for (int i = 0; i < terminatedProcesses.size(); i++) {
            process = (Process) terminatedProcesses.get(i);
            total_waiting_time += process.getTerminationTime() - (process.getArrivalTime() + process.getCpuTotalTime());
        }
        for (int i = 0; i < ProcessList.size(); i++) {
            process = (Process) ProcessList.get(i);
            total_waiting_time += Clock.showTime() - (process.getArrivalTime() + (process.getCpuTotalTime() - process.getCpuRemainingTime()));
        }
        this.totalWaitingTime = total_waiting_time;
        if (size > 0) {
            this.averageWaitingTime = (float) total_waiting_time / (float) size;
        }
        return this.averageWaitingTime;
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
