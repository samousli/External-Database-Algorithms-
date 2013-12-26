package schedule;


public interface Scheduler {

    /**
     *
     * @param process process to be added
     */
    void addProcessToReadyList(Process process);
    
    /**
     * @return true if CPU is idle. (No ready processes are available)  
     */
    boolean isCPUIdle();
    
    /**
     * Updates Statistics according to any new changes done in the processes the scheduler keeps track of
     */
    void updateStatistics();
    
    /**
     * Updates the maximumListLength field in Statistics
     */
    void updateMaximumListLength();
}
