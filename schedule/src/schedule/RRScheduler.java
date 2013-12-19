package schedule;

/**
 * 
 */
public class RRScheduler {

    private int quantum;
    private final ReadyProcessesList processList;
    private final CPU cpu; 

    RRScheduler(int quantum) {
        this.processList = new ReadyProcessesList();
        this.cpu = new CPU();
        this.cpu.setTimeToNextContextSwitch(quantum);
        this.quantum = quantum;
    }

    public void addProcessToReadyList(Process process) {
        processList.addProcess(process);
    }

    public void RR() {
        //int currentTime = Clock.showTime();
        
        if (cpu.getRunningProcess().getCurrentState() == ProcessState.RUNNING) {
            cpu.execute();
            return;
        } 
        Process nextP = processList.getProcessToRunInCPU();
        cpu.addProcess(nextP);
        cpu.execute();
        if (cpu.getRunningProcess().getCurrentState() == ProcessState.READY) {
            processList.addProcess(nextP);
        } else {
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
