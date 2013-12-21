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
        if (cpu.getRunningProcess() == null || 
                cpu.getRunningProcess().getCurrentState() != ProcessState.RUNNING) {
            if (this.processList.getListSize() == 0) {
                System.out.println("CPU idle (clockTime: " + Clock.showTime() + " )");
                return;
            }
            Process nextP = this.processList.getProcessToRunInCPU();
            cpu.addProcess(nextP);
        }
        cpu.execute();
        
        if (cpu.getRunningProcess() != null && 
                cpu.getRunningProcess().getCurrentState() == ProcessState.READY) {
            processList.addProcess(cpu.getRunningProcess());
        } else {
            System.out.println("Process Terminated (clockTime: " + Clock.showTime() + " )");
            cpu.getRunningProcess().printProcess();
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
