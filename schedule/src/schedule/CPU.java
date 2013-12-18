package schedule;

/**
 *
 */
public class CPU {
    
    /**
     *  Currently running process
     */
    private Process runningProcess;
    /**
     *  Time to interrupt
     */
    private int timeToNextContextSwitch;
    /**
     *  The time the previous process started
     */ 
    private int lastProcessStartTime;

    
    public CPU() {
        this.runningProcess = null;
        this.lastProcessStartTime = 0;
        this.timeToNextContextSwitch = 0;
    }

    /**
     *  Adds a process that's ready to be executed.
     *  @param process the process that's ready to be executed.
     */
    public void addProcess(Process process) {
        this.runningProcess = process;
    }

    /**
     *  Removes the running process
     *  @return The process that got interrupted
     */
    public Process removeProcessFromCpu() {
        Process p = this.runningProcess;
        this.runningProcess = null; 
        return p;
    }

    /**
     *  Executes process and updates it accordingly.
     */
    public void execute() {
        this.lastProcessStartTime = Clock.showTime();
        this.runningProcess.setProcessState(ProcessState.RUNNING);
        if (runningProcess != null) {
           this.runningProcess.changeCpuRemainingTime(
                   this.runningProcess.getCpuRemainingTime() - 
                           this.timeToNextContextSwitch); 
        }
        this.runningProcess.setProcessState(ProcessState.READY);
    }
}
