package schedule;

/**
 *
 */
public class RRScheduler implements Scheduler {

    private final int quantum;
    private final RRReadyProcessesList processList;
    private final TerminatedProcessesList terminatedProcesses;
    private final CPU cpu;
    
    private Process currentProcess = null;

    /**
     *
     * @param quantum
     */
    RRScheduler(int quantum) {
        this.processList = new RRReadyProcessesList();
        this.terminatedProcesses = new TerminatedProcessesList();
        this.cpu = new CPU();
        this.cpu.setTimeToNextContextSwitch(quantum);
        this.quantum = quantum;
    }

    /**
     *
     * @param process
     */
    @Override
    public void addProcessToReadyList(Process process) {
        this.processList.addProcess(process);
    }

    /**
     * @return the quantum
     */
    public int getQuantum() {
        return this.quantum;
    }

    @Override
    public boolean CPUIdle() {
        return (this.processList.getListSize() == 0 && 
                (currentProcess == null 
                || currentProcess.getCurrentState() == ProcessState.TERMINATED)
                );
    }

    /**
     *
     */
    public void RR() {
        
        // Add the previously active process to the end of the queue, 
        // ToDo: refactor properly
        if (currentProcess != null 
                && currentProcess.getCurrentState() == ProcessState.READY) {
            processList.addProcess(currentProcess);
        }
        
        // If the queue is empty just increment the clock.
        if (CPUIdle()) {
            cpu.addProcess(null);
            cpu.execute();
            return;
        }
        
        //Updates the maximum list length in RRStatistics
        this.updateMaximumListLength();

        currentProcess = this.processList.getProcessToRunInCPU();
        
        System.out.println("[CPU] Running P" + currentProcess.getID()
                + " (clock: " + Clock.showTime() + ")");
        
        cpu.addProcess(currentProcess);
        cpu.execute();    
            
        if (currentProcess.getCurrentState() == ProcessState.TERMINATED) {
            
            System.out.println("[CPU] P" + currentProcess.getID()
                    + " Terminated (clock: " + Clock.showTime() + " )");
            
            this.terminatedProcesses.addProcess(currentProcess);
            currentProcess.printProcess();
            this.updateStatistics();
        }
    }

    @Override
    public void updateStatistics() {
        Main.RRstats.updateStatistics(this.processList.getProcessList(), this.terminatedProcesses.getTerminatedProcessesList());
        Main.RRstats.WriteStatistics2File();
    }

    @Override
    public void updateMaximumListLength() {
        Main.RRstats.UpdateMaximumListLength(this.processList.getListSize());
    }
}
