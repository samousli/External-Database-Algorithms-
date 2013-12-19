package schedule;

/**
 *
 */
public class CPU {

    /**
     * Currently running process
     */
    private Process runningProcess;
    /**
     * Time to interrupt
     */
    private int timeToNextContextSwitch;
    /**
     * The time the previous process started
     */
    private int lastProcessStartTime;

    public CPU() {
        this.runningProcess = null;
        this.lastProcessStartTime = 0;
        this.timeToNextContextSwitch = 0;
    }

    /**
     * Adds a process that's ready to be executed.
     *
     * @param process the process that's ready to be executed.
     */
    public void addProcess(Process process) {
        this.runningProcess = process;
    }

    /**
     * Removes the running process
     *
     * @return The process that got interrupted
     */
    public Process removeProcessFromCpu() {
        Process p = this.getRunningProcess();
        this.runningProcess = null;
        return p;
    }

    /**
     * Executes process and updates it accordingly.
     */
    public void execute() {
        if (getRunningProcess() == null) {
            return;
        }
        this.getRunningProcess().setProcessState(ProcessState.RUNNING);
        
        this.getRunningProcess().changeCpuRemainingTime(
                this.getRunningProcess().getCpuRemainingTime() - 1 );

        if (this.getRunningProcess().getCpuRemainingTime() == 0) {
            this.getRunningProcess().setProcessState(ProcessState.TERMINATED);
        } else {
            this.getRunningProcess().setProcessState(ProcessState.READY);
        }
    }

    /**
     * @param timeToNextContextSwitch the timeToNextContextSwitch to set
     */
    public void setTimeToNextContextSwitch(int timeToNextContextSwitch) {
        this.timeToNextContextSwitch = timeToNextContextSwitch;
    }

    /**
     * @return the runningProcess
     */
    public Process getRunningProcess() {
        return runningProcess;
    }
}
