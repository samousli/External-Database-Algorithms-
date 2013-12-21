package schedule;

import java.io.File;
import java.util.Collections;
import java.util.List;

/**
 *
 */
public class Main {

    private static Clock clock;
    private static NewProcessTemporaryList newProcesses;
    private static ProcessGenerator processGen;
    private static Statistics stats;
    private static RRScheduler roundRobin;
    private static final int shutdownTime = 100;
    /**
     * Add processes with current arrival time to the ready list of the scheduler.
     */
    public static void addProcessesToReadyList() {
        newProcesses.sortByArrivalTime();
        Process p = newProcesses.getFirst();
        while (p != null && p.getArrivalTime() <= Clock.showTime()) {
            roundRobin.addProcessToReadyList(p);
            p = newProcesses.getFirst();
        }
    }

    public static void runRRSimulation(String inputFile, String outputFile, int quantum) {
        newProcesses = new NewProcessTemporaryList();
        File a = new File(inputFile);
        processGen = new ProcessGenerator(inputFile, true);
        stats = new Statistics(outputFile);
        roundRobin = new RRScheduler(quantum);
        clock = new Clock();

        // Create processes
        //List<Process> pl = processGen.parseProcessFile();
        for (int i = 0; i < 100; i++) {
            newProcesses.addNewProcess(processGen.createProcess());
        }

        while (Clock.showTime() != shutdownTime) {
            addProcessesToReadyList();
            roundRobin.RR();
            clock.timeRun();
        }

    }

    public static void main(String args[]) {
        runRRSimulation(args[0], args[1], 10);
    }

}
