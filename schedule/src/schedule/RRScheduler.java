package schedule;

/**
 *
 */
public class RRScheduler implements Scheduler {

    private final int quantum;
    private final RRReadyProcessesList processList;
    private final TerminatedProcessesList terminatedProcesses;
    private final CPU cpu;
    private int contextSwitchCount = 0;
    private Process currentProcess = null;

    /**
     *
     * @param quantum
     */
    RRScheduler(int quantum) {
        this.processList = new RRReadyProcessesList();
        this.terminatedProcesses = new TerminatedProcessesList();
        this.cpu = new CPU();
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
               
        // If the queue is empty just increment the clock.
        if (CPUIdle()) {
            cpu.addProcess(null);
            cpu.execute();
            return;
        }
        
        //Updates the maximum list length in RRStatistics
        this.updateMaximumListLength();

        currentProcess = this.processList.getProcessToRunInCPU();
        // Doesn't remove it from the list.
            Process prevProcess = currentProcess;
            currentProcess = this.processList.getProcessToRunInCPU();
            if ( prevProcess != null && !prevProcess.equals(currentProcess)) {
                
                contextSwitchCount++;
                PrettyPrinter.print("RR", "Context switch! P" + prevProcess.getID() + " -> P" + currentProcess.getID());
            }
        
        // Calculate time till next context switch
        int timeToNextSwitch = Math.min(quantum, 
                currentProcess.getCpuRemainingTime());
        
        cpu.addProcess(currentProcess);
        cpu.setTimeToNextContextSwitch(timeToNextSwitch);
        cpu.execute();    
            
        if (currentProcess.getCurrentState() == ProcessState.TERMINATED) {
            
            this.processList.removeProcess(currentProcess);
            this.terminatedProcesses.addProcess(currentProcess);
            
            this.updateStatistics();
        }
    }
    

    @Override
    public void updateStatistics() {
        Main.RRstats.updateStatistics(this.processList.getProcessList(), 
                this.terminatedProcesses.getTerminatedProcessesList());
        Main.RRstats.WriteStatistics2File();
    }

    @Override
    public void updateMaximumListLength() {
        Main.RRstats.UpdateMaximumListLength(this.processList.getListSize());
    }
}
