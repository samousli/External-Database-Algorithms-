
package schedule;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.logging.Level;
import java.util.logging.Logger;


public abstract class Statistics {
    
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
    protected final File outputFile;
    
    public Statistics(String filename)
    {
        
        this.outputFile = new File(filename);

        if (this.outputFile.exists()) {
            this.outputFile.delete();
        }
        try {
            this.outputFile.createNewFile();
        } catch (IOException ex) {
            Logger.getLogger(RRStatistics.class.getName()).log(Level.SEVERE, null, ex);
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
            Logger.getLogger(RRStatistics.class.getName()).log(Level.SEVERE, null, ex);
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
}
