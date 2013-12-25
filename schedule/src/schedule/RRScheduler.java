package schedule;

/**
 *
 */
public class RRScheduler implements Scheduler{

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
    public boolean isCPUIdle() {
        return (this.processList.getListSize() == 0);
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
            System.out.println("[CPU] P" + currentProcess.getID() 
                    + " Terminated (clock: " + Clock.showTime() + " )");
            this.terminatedProcesses.addProcess(currentProcess);
            currentProcess.printProcess();
            Main.stats.WriteStatistics2File(this.processList, this.terminatedProcesses);
            
        }
    }

}
