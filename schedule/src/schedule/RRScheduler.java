package schedule;

/**
 * 
 */
public class RRScheduler {

    private int quantum;
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
        //int currentTime = Clock.showTime();
        
        if (cpu.getRunningProcess() != null &&
            cpu.getRunningProcess().getCurrentState() == ProcessState.RUNNING) {
            cpu.execute();
            return;
        } 
        Process nextP = this.processList.getProcessToRunInCPU();
        if (nextP == null) { return; }
        cpu.addProcess(nextP);
        cpu.execute();
        if (cpu.getRunningProcess().getCurrentState() == ProcessState.READY) {
            processList.addProcess(nextP);
        } else {
            // Process terminated.
            // Run Stats!!
        }
    }

    /**
     * @return the quantum
     */
    public int getQuantum() {
        return quantum;
    }

    /**
     * @param quantum the quantum to set
     */
    public void setQuantum(int quantum) {
        this.quantum = quantum;
    }

}
