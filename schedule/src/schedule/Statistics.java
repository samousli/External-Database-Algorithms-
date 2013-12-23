package schedule;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
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
     * The time elapsed since the arrival of the process and the first time it
     * got CPU time.
     */
    private int responseTime;

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

        if (!this.outputFile.exists()) {
            try {
                this.outputFile.createNewFile();
            } catch (IOException ex) {
                Logger.getLogger(Statistics.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
    
    public int getResponseTime()
    {
       return 0; 
    }
    
    /**
     * updates the field MaximumListLength
     */
    public void UpdateMaximumListLength() {
        if (this.maximumLengthOfReadyProcessesList < this.totalNumberOfProcesses)
        {
            this.maximumLengthOfReadyProcessesList = this.totalNumberOfProcesses;
        }
    }

    /**
     * 
     * @param ProcessList the list containing the processes waiting to be executed
     * @return the average waiting time calculated as:
     * [sum of every process(current time - (arrival time + CPU time))] / number of processes
     * where CPU time is the total time the process spent in the CPU 
     * and is calculated as: total CPU time - remaining CPU time
     */
    public float CalculateAverageWaitingTime(RRReadyProcessesList ProcessList, Process terminatedProcess) {
        int total_waiting_time = 1;
        int size = ProcessList.getListSize();
        
        if (terminatedProcess != null)
        {
            total_waiting_time += Clock.showTime() - (terminatedProcess.getArrivalTime() + 
                (terminatedProcess.getCpuTotalTime() - terminatedProcess.getCpuRemainingTime()));
            size++;
        }
        for (int i = 0; i < ProcessList.getListSize(); i++)
        {
            Process process = ProcessList.getProcess(i);
            total_waiting_time += Clock.showTime() - (process.getArrivalTime() + 
                    (process.getCpuTotalTime() - process.getCpuRemainingTime()));
        }
        this.totalWaitingTime = total_waiting_time;
        if (size > 0)
        {
            this.averageWaitingTime = total_waiting_time / size;
        }
        return this.averageWaitingTime;
    }

    /**
    * updates every field in Statistics for the current clock time
    * @param ProcessList is the list containing the processes waiting the CPU
    */
    public void updateStatistics(RRReadyProcessesList ProcessList, Process terminatedProcess)
    {
        this.CalculateAverageWaitingTime(ProcessList, terminatedProcess);
        this.totalNumberOfProcesses = ProcessList.getListSize();
        this.UpdateMaximumListLength();
    }

    /**
     * Writes statistics to output file
     */
    public void WriteStatistics2File(RRReadyProcessesList ProcessList, Process terminatedProcess) {
        this.updateStatistics(ProcessList, terminatedProcess);
        try {
            PrintWriter fileWriter;
            fileWriter = new PrintWriter(new BufferedWriter(new FileWriter(this.outputFile, true)));
            fileWriter.println("_________Statistics________");
            fileWriter.println("Total waiting time:        " + this.totalWaitingTime);
            fileWriter.println("Average waiting time:      " + this.averageWaitingTime);
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
}
