package schedule;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.RandomAccessFile;
import java.util.logging.Level;
import java.util.logging.Logger;

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
     * The time since the arrival of the process and the first time it
     * got CPU time.
     */
    private float responseTime;

    private int maximumLengthOfReadyProcessesList;

    private int totalNumberOfProcesses;

    /**
     * The file for saving the statistics
     */
    private final File outputFile;

    /**
     * constructor of the class
     *
     * @param filename the file to write the statistics
     */
    public Statistics(String filename) {

        this.outputFile = new File(filename);

        if (this.outputFile.exists()) {
            this.outputFile.delete();
        }
        try {
            this.outputFile.createNewFile();
        } catch (IOException ex) {
            Logger.getLogger(Statistics.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    /**
     * updates the field MaximumListLength
     */
    public void UpdateMaximumListLength(int length) {
        if (this.maximumLengthOfReadyProcessesList < length)
        {
            this.maximumLengthOfReadyProcessesList = length;
        }
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
    public float CalculateAverageWaitingTime(RRReadyProcessesList ProcessList, TerminatedProcessesList terminatedProcesses) {
        int total_waiting_time = 0;
        int size = ProcessList.getListSize() + terminatedProcesses.getListSize();
        Process process = null;

        for (int i = 0; i < terminatedProcesses.getListSize(); i++)
        {
            process = terminatedProcesses.getProcess(i);
            total_waiting_time += process.getTerminationTime() - 
                    (process.getArrivalTime() + process.getCpuTotalTime());
        }
        
        for (int i = 0; i < ProcessList.getListSize(); i++) {
            process = ProcessList.getProcess(i);
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
    public float CalculateAverageResponseTime(RRReadyProcessesList ProcessList, TerminatedProcessesList terminatedProcesses)
    {
        int total_response_time = 0;
        int size = ProcessList.getListSize() + terminatedProcesses.getListSize();
        Process process = null;
        
        for (int i = 0; i < terminatedProcesses.getListSize(); i++)
        {
            process = terminatedProcesses.getProcess(i);
            total_response_time += process.getResponseTime() - process.getArrivalTime();
        }
        
        for (int i = 0; i < ProcessList.getListSize(); i++) {
            process = ProcessList.getProcess(i);
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
     * updates every field in Statistics for the current clock time
     *
     * @param ProcessList is the list containing the processes waiting the CPU
     * @param terminatedProcesses is the list containing the terminated processes
     */
    public void updateStatistics(RRReadyProcessesList ProcessList, TerminatedProcessesList terminatedProcesses) {
        this.CalculateAverageWaitingTime(ProcessList, terminatedProcesses);
        this.CalculateAverageResponseTime(ProcessList, terminatedProcesses);
        this.totalNumberOfProcesses = ProcessList.getListSize() + terminatedProcesses.getListSize();
        this.UpdateMaximumListLength(ProcessList.getListSize());
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
            Logger.getLogger(Statistics.class.getName()).log(Level.SEVERE, null, ex);
        }

    }

    /**
     * @return the total number of processes
     */
    public int getTotalNumberOfProcesses() {
        return totalNumberOfProcesses;
    }

    int getTotalRunTime() {
        return Clock.showTime() - this.totalWaitingTime;
    }
}
