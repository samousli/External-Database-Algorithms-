package schedule;

/**
 *
 */
public class RRScheduler {

    private final int quantum;
    private final RRReadyProcessesList processList;
    private final CPU cpu;

    RRScheduler(int quantum) {
        this.processList = new RRReadyProcessesList();
        this.cpu = new CPU();
        this.cpu.setTimeToNextContextSwitch(quantum);
        this.quantum = quantum;
    }

    public void addProcessToReadyList(Process process) {
        this.processList.addProcess(process);
    }

    public void RR() {
        
        int runFor = 0;
        if (cpu.getRunningProcess() == null
                || cpu.getRunningProcess().getCurrentState() != ProcessState.RUNNING) {
            if (this.processList.getListSize() == 0) {
                System.out.println("CPU idle (Time: " + Clock.showTime() + " )");
                return;
            }
            Process nextP = this.processList.getProcessToRunInCPU();
            runFor = Math.min(nextP.getCpuRemainingTime(), quantum);
            cpu.addProcess(nextP);
        }
        // Execute for n steps or until it finishes.
        for (int i = 0; i < runFor; i++) {
            cpu.execute();
        }
       
        Process cpuProcess = cpu.getRunningProcess();

        if (cpuProcess != null
                && cpuProcess.getCurrentState() == ProcessState.READY) {
            processList.addProcess(cpuProcess);
        } else {
            System.out.println("Process Terminated (Time: " + Clock.showTime() + " )");
            cpuProcess.printProcess();
            Main.stats.WriteStatistics2File(processList, cpuProcess);
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
