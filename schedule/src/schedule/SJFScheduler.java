package schedule;

/**
 *
 */
public class SJFScheduler implements Scheduler {

    // The burst of time the cpu runs before checking for a process with shorter remaining time. 
    private final int PREEMPTIVE_CHECK_INTERVAL = 1;

    private final boolean preemptive;
    private final CPU cpu;
    private final SJFReadyProcessesList processList;
    private final TerminatedProcessesList terminatedProcesses;
    private Process currentProcess;
    private int contextSwitchCount = 0;

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
        if (this.CPUIdle()) {
            cpu.addProcess(null);
            cpu.execute();
            return;
        }
        //Update the maximum ready processes list length in statistics.
        this.updateMaximumListLength();

        // Get the new process if the current process terminated or we run in preemptive mode.
        if (preemptive || currentProcess == null
                || currentProcess.getCurrentState() == ProcessState.TERMINATED) {
            // Doesn't remove it from the list.
            Process prevProcess = currentProcess;
            currentProcess = this.processList.getProcessToRunInCPU();
            if (preemptive && prevProcess != null && prevProcess.equals(currentProcess)) {
                
                contextSwitchCount++;
                PrettyPrinter.print("SJF", "Context switch! P" + prevProcess.getID() + " -> P" + currentProcess.getID());
            }
        }
        
        cpu.addProcess(currentProcess);

        // If it's preemptive we have to run checks every set amount of time to see if there are any new processes
        // that match the criteria for a context switch
        if (preemptive) {
            cpu.setTimeToNextContextSwitch(PREEMPTIVE_CHECK_INTERVAL);
        } else {
            cpu.setTimeToNextContextSwitch(currentProcess.getCpuTotalTime());
        }
        cpu.execute();

        if (currentProcess.getCurrentState() == ProcessState.TERMINATED) {

            // With getProcessToRunInCPU we only peek at the element if it terminates 
            // we have to remove it.
            this.processList.removeProcess(currentProcess);
            this.terminatedProcesses.addProcess(currentProcess);

            this.updateStatistics();
        }
    }
}
