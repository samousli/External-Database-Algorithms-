package schedule;

/**
 *
 */
public class RRScheduler implements Scheduler {

    private final int quantum;
    private final RRReadyProcessesList processList;
    private final TerminatedProcessesList terminatedProcesses;
    private final CPU cpu;

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
        return (this.processList.getListSize() == 0);
    }

    /**
     *
     */
    public void RR() {
        // If the queue is empty just increment the clock.
        if (this.CPUIdle()) {
            cpu.addProcess(null);
            cpu.execute();
            return;
        }

        //Updates the maximum list length in RRStatistics
        this.updateMaximumListLength();

        Process currentProcess = this.processList.getProcessToRunInCPU();

        // Execute for at least quantum steps or until it finishes.
        cpu.addProcess(currentProcess);
        int runFor = Math.min(currentProcess.getCpuRemainingTime(), quantum);
        System.out.println("[CPU] Running P" + currentProcess.getID()
                + " (from:" + Clock.showTime() + " to:\t"
                + (Clock.showTime() + runFor) + ")");

        for (int i = 0; i < runFor; i++) {
            cpu.execute();
        }

        if (currentProcess.getCurrentState() == ProcessState.RUNNING) {
            currentProcess.setProcessState(ProcessState.READY);
            processList.addProcess(currentProcess);
        } else {
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
