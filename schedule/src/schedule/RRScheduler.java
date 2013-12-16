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
        this.quantum = quantum;
    }

    public void addProcessToReadyList(Process process) {
        processList.addProcess(process);
    }

    public void RR() {
        
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
