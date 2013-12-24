package schedule;

/**
 *
 */
public class SJFScheduler {

    private final boolean isPreemptive;
    private final CPU cpu;
    private final SJFReadyProcessesList processList;

    /**
     *
     * @param isPreemptive
     */
    public SJFScheduler(boolean isPreemptive) {
        this.isPreemptive = isPreemptive;
        this.cpu = new CPU();
        this.processList = new SJFReadyProcessesList();
    }

    /**
     *
     * @param process process to be added
     */
    public void addProcessToReadyList(Process process) {
        this.processList.addProcess(process);
    }

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
            System.out.println("Process Terminated (Time: " + Clock.showTime() + " )");
            currentProcess.printProcess();
            //Main.stats.WriteStatistics2File(processList, currentProcess);
        }
    }
}
