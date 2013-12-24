package schedule;

/**
 *
 */
public class RRScheduler {

    private final int quantum;
    private final RRReadyProcessesList processList;
    private final CPU cpu;

    /**
     * 
     * @param quantum 
     */
    RRScheduler(int quantum) {
        this.processList = new RRReadyProcessesList();
        this.cpu = new CPU();
        this.cpu.setTimeToNextContextSwitch(quantum);
        this.quantum = quantum;
    }

    /**
     * 
     * @param process 
     */
    public void addProcessToReadyList(Process process) {
        this.processList.addProcess(process);
    }

    /**
     * 
     */
    public void RR() {
        // If the queue is empty just increment the clock.
        if(this.processList.getListSize() == 0) {
            cpu.addProcess(null);
            cpu.execute();
            return;
        }
        
        Process currentProcess = this.processList.getProcessToRunInCPU();
        
        // Execute for n steps or until it finishes.
        cpu.addProcess(currentProcess);
        int runFor = Math.min(currentProcess.getCpuRemainingTime(), quantum);
        for (int i = 0; i < runFor; i++) {
            cpu.execute();
        }

        if (currentProcess.getCurrentState() == ProcessState.RUNNING) {
            currentProcess.setProcessState(ProcessState.READY);
            processList.addProcess(currentProcess);
        } else {
            System.out.println("Process Terminated (Time: " + Clock.showTime() + " )");
            currentProcess.printProcess();
            Main.stats.WriteStatistics2File(processList, currentProcess);
        }
    }

    /**
     * @return the quantum
     */
    public int getQuantum() {
        return this.quantum;
    }

    public boolean isCPUIdle() {
        return (this.processList.getListSize() == 0);
    }

}
