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

    public void RR(Statistics stats) {
        if (cpu.getRunningProcess() == null || 
                cpu.getRunningProcess().getCurrentState() != ProcessState.RUNNING) {
            if (this.processList.getListSize() == 0) {
                System.out.println("CPU idle (Time: " + Clock.showTime() + " )");
                return;
            }
            Process nextP = this.processList.getProcessToRunInCPU();
            cpu.addProcess(nextP);
        }
        cpu.execute();
        Process cpuProcess = cpu.getRunningProcess();
        
        if (cpuProcess != null && 
                cpuProcess.getCurrentState() == ProcessState.READY) {
            processList.addProcess(cpuProcess);
        } else {
            System.out.println("Process Terminated (Time: " + Clock.showTime() + " )");
            cpuProcess.printProcess();
            stats.WriteStatistics2File(processList , cpuProcess);
            // Process terminated.
            // Run Stats!!
        }
    }

    /**
     * @return the quantum
     */
    public int getQuantum() {
        return this.quantum;
    }

}
