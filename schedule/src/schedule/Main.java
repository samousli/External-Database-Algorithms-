package schedule;

import java.io.File;
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
    private static boolean useInputFile;

    public static void Initialize(String inputFile, String outputFile, int quantum) {
        newProcesses = new NewProcessTemporaryList();
        File a = new File(inputFile);
        useInputFile = (inputFile != null);
        processGen = new ProcessGenerator(inputFile, useInputFile);
        stats = new Statistics(outputFile);
        roundRobin = new RRScheduler(quantum);
        clock = new Clock();
    }

    /**
     * Add processes with current arrival time to the ready list of the
     * scheduler.
     */
    public static void addProcessesToReadyList() {
        newProcesses.sortByArrivalTime();
        Process p = newProcesses.getFirst();
        while (p != null && p.getArrivalTime() <= Clock.showTime()) {
            roundRobin.addProcessToReadyList(p);
            p = newProcesses.getFirst();
        }
        newProcesses.addNewProcess(p);
    }

    public static void runRRSimulation() {

        // Create processes using the input file or 10 randomly created ones.
        if (useInputFile) {
            List<Process> pl = processGen.parseProcessFile();
            for (Process p : pl) {
                newProcesses.addNewProcess(p);
            }
        } else {
            for (int i = 0; i < 10; i++) {
                newProcesses.addNewProcess(processGen.createProcess());
            }
        }
        while (Clock.showTime() != shutdownTime) {
            addProcessesToReadyList();
            roundRobin.RR();
            clock.timeRun();
        }

    }

    public static void main(String args[]) {
        if (args.length == 1) {
            Initialize(null, args[0], 1);
        } else if (args.length == 2) {
            Initialize(args[0], args[1], 1);
        }
        runRRSimulation();
    }

}
