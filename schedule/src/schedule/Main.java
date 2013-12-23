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
    private static boolean useInputFile;

    /**
     *
     * @param inputFile
     * @param outputFile
     * @param quantum
     */
    public static void Initialize(String inputFile, String outputFile, int quantum) {
        newProcesses = new NewProcessTemporaryList();
        File a = new File(inputFile);
        useInputFile = (inputFile != null);
        processGen = new ProcessGenerator(inputFile, useInputFile);
        stats = new Statistics(outputFile);
        roundRobin = new RRScheduler(quantum);
        //clock = new Clock();
    }

    /**
     * Add processes with current arrival time to the ready list of the
     * scheduler.
     */
    public static void populateReadyProcessList() {
        newProcesses.sortByArrivalTime();
        Process p = newProcesses.getFirst();
        while (p != null && p.getArrivalTime() <= Clock.showTime()) {
            roundRobin.addProcessToReadyList(p);
            p = newProcesses.getFirst();
        }
        // an extra process that doesnt fit the description is removed, 
        // we add it back.
        if ( p != null && p.getArrivalTime() > Clock.showTime()) {
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
     * Main loop, runs till the completion time, printing current state also
     * pauses for 100 milliseconds at every iteration.
     */
    public static void runRRSimulation() {

        populateNewProcessList();
        // Continue while there are processes or CPU is busy
        while (newProcesses.getListSize() > 0 || roundRobin.isCPUIdle() == false) {
            
            populateReadyProcessList();
            roundRobin.RR();
            // Sleep for 100ms after every iteration
            try {
                Thread.sleep(100);
            } catch (InterruptedException ex) {
                Logger.getLogger(Main.class.getName()).log(Level.SEVERE, "Sleep interrupted", ex);
            }
        }
        System.out.println("Simulation complete! (" + Clock.showTime() + " steps)");
    }

    /**
     *
     * @param args
     */
    public static void main(String args[]) {
        if (args.length == 1) {
            Initialize(null, args[0], 1);
        } else if (args.length == 2) {
            Initialize(args[0], args[1], 3);
        }
        runRRSimulation();
    }

}
