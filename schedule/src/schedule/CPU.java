package schedule;

/**
 *
 */
public class CPU {

    /**
     * CPU clock
     */
    private Clock clock;

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
    // private int lastProcessStartTime;
    public CPU() {
        this.clock = new Clock();
        this.runningProcess = null;
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
     * Executes the loaded process and updates it's state and CPU clock
     * accordingly.
     */
    public void execute() {
        if (this.runningProcess == null) {
            return;
        }
        this.runningProcess.setProcessState(ProcessState.RUNNING);
        System.out.println("[CPU] Running    P" 
                + this.runningProcess.getID()
                + " at clock: " + Clock.showTime());

        // Update clock.
        clock.timeRun();
        this.runningProcess.changeCpuRemainingTime(
                this.runningProcess.getCpuRemainingTime() - 1);

        if (this.runningProcess.getCpuRemainingTime() == 0) {
            this.runningProcess.setProcessState(ProcessState.TERMINATED);
           // System.out.println("[CPU] Terminated P" 
           //         + this.runningProcess.getID()
           //         + " at clock: " + Clock.showTime());

        } else {
            this.runningProcess.setProcessState(ProcessState.READY);
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
        return this.runningProcess;
    }
}
