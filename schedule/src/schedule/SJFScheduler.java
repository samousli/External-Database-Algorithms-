package schedule;

/**
 *
 */
public class SJFScheduler implements Scheduler {

    private final boolean preemptive;
    private final CPU cpu;
    private final SJFReadyProcessesList processList;
    private final TerminatedProcessesList terminatedProcesses;
    private Process currentProcess;

    /**
     *
     * @param isPreemptive
     */
    public SJFScheduler(boolean isPreemptive) {
        this.preemptive = isPreemptive;
        this.cpu = new CPU();
        this.processList = new SJFReadyProcessesList();
        this.terminatedProcesses = new TerminatedProcessesList();
    }
    
    /**
     *
     * @param process process to be added
     */
    @Override
    public void addProcessToReadyList(Process process) {
        this.processList.addProcess(process);
    }

    @Override
    public boolean CPUIdle() {
        return (this.processList.getListSize() == 0);
    }

    @Override
    public void updateStatistics() {
        Main.SJFstats.updateStatistics(this.processList.getProcessList(), 
                this.terminatedProcesses.getTerminatedProcessesList());
        Main.SJFstats.WriteStatistics2File();
    }

    @Override
    public void updateMaximumListLength() {
        Main.SJFstats.UpdateMaximumListLength(this.processList.getListSize());
    }
    
    /**
     * @return true if the scheduling is preemptive.
     */
    public boolean isPreemptive() {
        return this.preemptive;
    }
    
    public void SJF() {
        // If the queue is empty just increment the clock. 
        // Practically it means that the CPU is idle.
        if (this.CPUIdle()) {
            cpu.addProcess(null);
            cpu.execute();
            return;
        }

        //Update the maximum ready processes list length in statistics
        this.updateMaximumListLength();
        
        if (preemptive || currentProcess == null) {
            currentProcess = this.processList.getProcessToRunInCPU();
        }
        System.out.println("[CPU] Running P" + currentProcess.getID()
                + " (clock: " + Clock.showTime() + ")");
        
        cpu.addProcess(currentProcess);
        cpu.execute();

        if (currentProcess.getCurrentState() == ProcessState.TERMINATED) {
            
            System.out.println("[CPU] P" + currentProcess.getID()
                    + " Terminated (clock: " + Clock.showTime() + ")");
           
            // Earlier we only peeked at the element if it terminates 
            // we have to remove it.
            this.processList.removeProcess(currentProcess);
            
            this.terminatedProcesses.addProcess(currentProcess);
            currentProcess.printProcess();
            this.updateStatistics();
            currentProcess = null;
        }
    } 
}
