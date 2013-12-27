package schedule;

import java.io.File;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 */
public class Main {

    //private static Clock clock;
    private static NewProcessTemporaryList newProcesses;
    private static ProcessGenerator processGen;
    static RRStatistics stats;
    private static RRScheduler roundRobin;
    private static SJFScheduler shortestJobFirst;
    private static boolean useInputFile;

    /**
     *
     * @param inputFile
     * @param outputFile
     * @param quantum
     */
    public static void initializeRR(String inputFile, String outputFile,
            int quantum) {
        newProcesses = new NewProcessTemporaryList();
        File a = new File(inputFile);
        useInputFile = (inputFile != null);
        processGen = new ProcessGenerator(inputFile, useInputFile);
        stats = new RRStatistics(outputFile);
        roundRobin = new RRScheduler(quantum);
    }

    /**
     *
     * @param inputFile
     * @param outputFile
     * @param preemptive
     */
    public static void initializeSJF(String inputFile, String outputFile,
            boolean preemptive) {
        newProcesses = new NewProcessTemporaryList();
        File a = new File(inputFile);
        useInputFile = (inputFile != null);
        processGen = new ProcessGenerator(inputFile, useInputFile);
        stats = new RRStatistics(outputFile);
        shortestJobFirst = new SJFScheduler(preemptive);
    }

    /**
     * Add processes with current arrival time to the ready list of the
     * scheduler.
     */
    public static void populateReadyProcessList() {
        newProcesses.sortByArrivalTime();
        Process p = newProcesses.getFirst();
        while (p != null && p.getArrivalTime() <= Clock.showTime()) {
            if (roundRobin != null) {
                roundRobin.addProcessToReadyList(p);
            }
            if (shortestJobFirst != null) {
                shortestJobFirst.addProcessToReadyList(p);
            }
            p = newProcesses.getFirst();
        }
        // an extra process that doesnt fit the description is removed, 
        // we add it back.
        if (p != null && p.getArrivalTime() > Clock.showTime()) {
            newProcesses.addNewProcess(p);
        }
    }

    /**
     * Create processes using the input file or create 10 randomly.
     */
    public static void populateNewProcessList() {
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
    }

    /**
     * Main loop for round robin scheduler, runs until the completion time
     * printing current state also pauses for 100 milliseconds at every
     * iteration.
     */
    public static void runRRSimulation() {
        System.out.println("Running round robin simulation..\n");
        populateNewProcessList();
        // Continue while there are processes or CPU is busy
        while (newProcesses.getListSize() > 0
                || roundRobin.isCPUIdle() == false) {

            populateReadyProcessList();
            roundRobin.RR();
            // Sleep for 100ms after every iteration
            try {
                Thread.sleep(100);
            } catch (InterruptedException ex) {
                Logger.getLogger(Main.class.getName()).log(Level.SEVERE,
                        "Sleep interrupted", ex);
            }
        }
        System.out.println("Simulation complete! ("
                + Clock.showTime() + " steps)");
    }

    /**
     * Main loop for the shortest job first scheduler runs until the completion
     * time, printing current state also pauses for 100 milliseconds at every
     * iteration.
     */
    public static void runSJFSimulation() {
        System.out.printf("Running shortest job first simulation..\n");
        populateNewProcessList();
        // Continue while there are processes or CPU is busy
        while (newProcesses.getListSize() > 0
                || shortestJobFirst.isCPUIdle() == false) {

            populateReadyProcessList();
            shortestJobFirst.SJF();
            // Sleep for 100ms after every iteration
            try {
                Thread.sleep(100);
            } catch (InterruptedException ex) {
                Logger.getLogger(Main.class.getName()).log(Level.SEVERE,
                        "Sleep interrupted", ex);
            }
        }
        System.out.println("Simulation complete! ("
                + Clock.showTime() + " steps)");
    }

    /**
     *
     * @param args
     */
    public static void main(String args[]) {
        boolean useRR  = true;
        boolean useSJF = false;
        
        if (useRR) {
            if (args.length == 1) {
                initializeRR(null, args[0], 1);
            } else if (args.length == 2) {
                initializeRR(args[0], args[1], 1);
            }
            runRRSimulation();
        }
        if (useSJF) {
            if (args.length == 1) {
                initializeSJF(null, args[0], false);
            } else if (args.length == 2) {
                initializeSJF(args[0], args[1], false);
            }
            runSJFSimulation();
        }
    }

}
