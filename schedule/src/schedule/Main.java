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
    static Statistics stats;
    private static RRScheduler roundRobin;
    private static SJFScheduler shortestJobFirst;
    private static boolean onlyReadInputFile; // as it is now if it exists it's read.
    private static boolean sleep_between_iterations;

    /**
     *
     * @param inputFile
     * @param outputFile
     * @param quantum
     */
    public static void initializeRR(String inputFile, String outputFile,
            int quantum) {
        newProcesses = new NewProcessTemporaryList();
        onlyReadInputFile = false;//(inputFile != null);
        processGen = new ProcessGenerator(inputFile, onlyReadInputFile);
        stats = new Statistics(outputFile);
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
        onlyReadInputFile = false;//(inputFile != null);
        processGen = new ProcessGenerator(inputFile, onlyReadInputFile);
        stats = new Statistics(outputFile);
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
     * Creates processes using the input file or generates some randomly.
     */
    public static void populateNewProcessList() {
        List<Process> pList;
        if (onlyReadInputFile) {
            pList = processGen.parseProcessFile();
        } else {
            pList = processGen.generateRandomList();
        }

        for (Process p : pList) {
            newProcesses.addNewProcess(p);
        }
    }

    /**
     * Main loop for round robin scheduler, runs until the completion time
     * printing current state also pauses for 100 milliseconds at every
     * iteration.
     */
    public static void runRRSimulation() {
        System.out.println("Running round robin simulation..");
        System.out.println("    Quantum: " + roundRobin.getQuantum());
        populateNewProcessList();
        // Continue while there are processes or CPU is busy
        while (newProcesses.getListSize() > 0
                || roundRobin.CPUIdle() == false) {

            populateReadyProcessList();
            roundRobin.RR();
            if (sleep_between_iterations) {
                try {
                    // Sleep for 100ms after every iteration
                    Thread.sleep(100);
                } catch (InterruptedException ex) {
                    Logger.getLogger(Main.class.getName()).log(Level.SEVERE,
                            "Sleep interrupted.", ex);
                }
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
        System.out.println("Running shortest job first simulation..");
        System.out.println("    Preemptive: " + shortestJobFirst.isPreemptive());
        populateNewProcessList();
        // Continue while there are processes or CPU is busy
        while (newProcesses.getListSize() > 0
                || shortestJobFirst.CPUIdle() == false) {

            populateReadyProcessList();
            shortestJobFirst.SJF();

            if (sleep_between_iterations) {
                try {
                    // Sleep for 100ms after every iteration
                    Thread.sleep(100);
                } catch (InterruptedException ex) {
                    Logger.getLogger(Main.class.getName()).log(Level.SEVERE,
                            "Sleep interrupted.", ex);
                }
            }
        }
        System.out.println("Simulation complete! ("
                + Clock.showTime() + " steps)");
    }

    /**
     * Prints the correct usage in case of bad arguments.
     */
    private static void badArguments() {
        System.err.println("Bad arguments.");
        System.out.println("Correct usage is:");
        System.out.println("schedule [inputfile] outputfile [OPTIONS]");
        System.out.println("Options:");
        System.out.println("    -rr                 Use round robin scheduler");
        System.out.println("    -sjf                Use shortest job first scheduler");
        System.out.println("    --quantum=<val>     Default value is 5");
        System.out.println("    -q=<val>            Same as --quantum");
        System.out.println("    --premptive         Use preemption in SJF");
        System.out.println("    -p                  Same as -preemptive");
        System.out.println("");
        System.out.println("Note: All options are case insensitive");
        System.exit(1);
    }

    /**
     *
     * @param args
     */
    public static void main(String args[]) {

        // Default values
        sleep_between_iterations = true;
        boolean useRR = true;
        int quantum = 5;
        boolean useSJF = false;
        boolean preemptive = false;
        String input = null, output;// = "results.txt";

        // Parse input
        if (args.length < 1) {
            badArguments();
        }

        output = args[0];

        for (int i = 0; i < args.length; i++) {
            String arg = args[i].toLowerCase();

            if (arg.equals("-rr")) {
                useSJF = false;
                useRR = true;
                continue;
            } else if (arg.equals("-sjf")) {
                useRR = false;
                useSJF = true;
                continue;
            } else if (arg.startsWith("--quantum=")
                    || arg.startsWith("-q=")) {
                try {
                    quantum = Integer.parseInt(arg.split("=")[1]);
                } catch (NumberFormatException e) {
                }
                continue;
            } else if (arg.equals("--preemptive")
                    || arg.equals("-p")) {
                preemptive = true;
                continue;
            }
            // If arg1 isn't an option it's the output file, so arg0 is input
            // else arg0 is the output file and processes are generated randomly.
            if (i == 1) {
                output = args[1];
                input = args[0];
            } else {
                output = args[0];
            }
        }

        if (useRR) {
            initializeRR(input, output, quantum);
            runRRSimulation();
        }
        if (useSJF) {
            initializeSJF(input, output, preemptive);
            runSJFSimulation();
        }
    }

}
