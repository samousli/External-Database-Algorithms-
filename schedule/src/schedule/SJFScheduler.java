package schedule;

/**
 *
 */
public class SJFScheduler implements Scheduler {

    private final boolean isPreemptive;
    private final CPU cpu;
    private final SJFReadyProcessesList processList;
    private final TerminatedProcessesList terminatedProcesses;

    /**
     *
     * @param isPreemptive
     */
    public SJFScheduler(boolean isPreemptive) {
        this.isPreemptive = isPreemptive;
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
        Main.stats.updateStatistics(this.processList.getProcessList(), 
                this.terminatedProcesses.getTerminatedProcessesList());
        Main.stats.WriteStatistics2File();
    }

    @Override
    public void updateMaximumListLength() {
        Main.stats.UpdateMaximumListLength(this.processList.getListSize());
    }
    
    /**
     * @return true if the scheduling is preemptive.
     */
    public boolean isPreemptive() {
        return this.isPreemptive;
    }
    
    public void SJF() {
        // If the queue is empty just increment the clock. 
        // Practically it means that the CPU is idle.
        if (this.CPUIdle()) {
            cpu.addProcess(null);
            cpu.execute();
            return;
        }

        //Update the maximum ready processes list length in staatistics
        this.updateMaximumListLength();

        Process currentProcess = this.processList.getProcessToRunInCPU();
        System.out.println("[CPU] Running P" + currentProcess.getID()
                + " (clock: " + Clock.showTime() + ")");
        if (this.isPreemptive) {
            cpu.addProcess(currentProcess);
            cpu.execute();
        } else {
            cpu.addProcess(currentProcess);
            for (int i = 0; i < currentProcess.getCpuTotalTime(); i++) {
                cpu.execute();
            }
        }

        if (currentProcess.getCurrentState() == ProcessState.RUNNING) {
            currentProcess.setProcessState(ProcessState.READY);
        } else {
            this.processList.removeProcess(currentProcess);
            System.out.println("[CPU] P" + currentProcess.getID()
                    + " Terminated (clock: " + Clock.showTime() + ")");
            this.terminatedProcesses.addProcess(currentProcess);
            currentProcess.printProcess();
            this.updateStatistics();
        }
    } 
}
