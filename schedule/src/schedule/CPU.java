package schedule;

/**
 *
 */
public class CPU {

    /**
     * CPU clock
     */
    private final Clock clock;

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
     * Adds a process that's ready to be executed and updates the previous
     * process's state.
     *
     * @param process the process that's ready to be executed.
     */
    public void addProcess(Process process) {
        if (this.runningProcess != null
                && this.runningProcess.getCurrentState() == ProcessState.RUNNING) {
            this.runningProcess.setProcessState(ProcessState.READY);
        }
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

        // Idle
        if (this.runningProcess == null) {
            System.out.println("[CPU] Idle at clock: " + Clock.showTime());
            clock.timeRun();
            return;
        }
        // Process running
        this.runningProcess.setProcessState(ProcessState.RUNNING);
        System.out.println("[CPU] Running P" + this.runningProcess.getID()
                + " (clock: " + Clock.showTime() + ")");

        //Update response time 
        if (runningProcess.getCpuTotalTime() == this.runningProcess.getCpuRemainingTime()) {
            this.runningProcess.setResponseTime(Clock.showTime());
        }

        // Update clock.
        clock.timeRun();
        this.runningProcess.changeCpuRemainingTime(
                this.runningProcess.getCpuRemainingTime() - 1);

        // Change state
        if (this.runningProcess.getCpuRemainingTime() == 0) {
            this.runningProcess.setProcessState(ProcessState.TERMINATED);
        }
    }

    /**
     * This is useless and makes no sense to me whatsoever. Why should the CPU
     * know about the future?
     *
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
