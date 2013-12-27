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
    public boolean isCPUIdle() {
        return (this.processList.getListSize() == 0);
    }

    public void SJF() {
        // If the queue is empty just increment the clock. 
        // Practically it means that the CPU is idle.
        if (this.processList.getListSize() == 0) {
            cpu.addProcess(null);
            cpu.execute();
            return;
        }
        
        //Update the maximum ready processes list length in staatistics
        this.updateMaximumListLength();
        
        Process currentProcess = this.processList.getProcessToRunInCPU();

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
            System.out.println( currentProcess.getID() + " Terminated (clock: " 
                    + Clock.showTime() + " )");
            this.terminatedProcesses.addProcess(currentProcess);
            currentProcess.printProcess();
            //Main.stats.WriteStatistics2File(processList, currentProcess);
        }
    }
    
    @Override
    public void updateStatistics()
    { 
       // Main.stats.updateStatistics(this.processList.getProcessList(), this.terminatedProcesses.getTerminatedProcessesList());
        Main.stats.WriteStatistics2File();
    }

    @Override
    public void updateMaximumListLength()
    {
        Main.stats.UpdateMaximumListLength(this.processList.getListSize());
    }
}
