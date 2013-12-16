package schedule;

import java.io.File;

/**
 *
 */
public  class Main {
    
    private static Clock clock;
    private static NewProcessTemporaryList newProcesses;
    private static ProcessGenerator processGen;
    private static Statistics stats;
    private static RRScheduler roundRobin;
    
    public static void runRRSimulation(String inputFile, String outputFile, int quantum) {
        newProcesses = new NewProcessTemporaryList();
        File a  = new File(inputFile);
        processGen = new ProcessGenerator(inputFile, a.isFile());
        stats = new Statistics(outputFile);
        roundRobin = new RRScheduler(quantum);
        
        // Create processes
        Process p;
        for (int i = 0; i < 100; i++) {
            p = processGen.createProcess();
            newProcesses.addNewProcess(p);
        }
        // Start clock
        clock = new Clock();
        
    }
    
    public static void main(String args[]) {
        runRRSimulation(args[0], args[1], 100);
    }
    
}
