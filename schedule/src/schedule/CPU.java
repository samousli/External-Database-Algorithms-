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
     * Sets the process that's ready to be executed.
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
        Process p = this.runningProcess;
        this.runningProcess = null;
        return p;
    }

    /**
     * Executes the loaded process and updates it's state and also increments
     * CPU clock accordingly.
     */
    public void execute() {

        // Idle
        if (this.runningProcess == null) {
            System.out.println("[CPU] Idle at clock: " + Clock.showTime());
            clock.timeRun();
            return;
        }

        PrettyPrinter.print("CPU", "Running P" + runningProcess.getID());

        // Process running
        this.runningProcess.setProcessState(ProcessState.RUNNING);

        //Update response time 
        if (runningProcess.getCpuTotalTime() == this.runningProcess.getCpuRemainingTime()) {
            this.runningProcess.setResponseTime(Clock.showTime());
        }

        // Run process until the context switch
        //for (int i = 0; i < this.timeToNextContextSwitch; i++) {
        clock.timeRun();
        this.timeToNextContextSwitch--;
        this.runningProcess.changeCpuRemainingTime(
                this.runningProcess.getCpuRemainingTime() - 1);

        
        // Change state
        if (this.runningProcess.getCpuRemainingTime() == 0) {
            this.runningProcess.setProcessState(ProcessState.TERMINATED);
            this.runningProcess.setTerminationTime(Clock.showTime());
            PrettyPrinter.print("CPU", "Terminated P" + runningProcess.getID());
            runningProcess.printProcess();
        } else if (this.timeToNextContextSwitch == 0) {
            this.runningProcess.setProcessState(ProcessState.READY);
        }
        
        // If the process didnt terminate and there is still time until the next context switch
        // Leave it's state as running.
    }

    /**
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
