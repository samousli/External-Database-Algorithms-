package schedule;

import java.io.File;

/**
 *
 */
public  class Main {
    
    private static Clock clock;
    private static ReadyProcessesList readyProcesses = new ReadyProcessesList();
    private static NewProcessTemporaryList newProcesses;
    private static ProcessGenerator processGen;
    private static Statistics stats;
    private static CPU cpu;
    private static RRScheduler roundRobin;
    
    public static void runRRSimulation(String inputFile, String outputFile, int quantum) {
        clock = new Clock();
        readyProcesses = new ReadyProcessesList();
        newProcesses = new NewProcessTemporaryList();
        File a  = new File(inputFile);
        processGen = new ProcessGenerator(inputFile, a.isFile());
        stats = new Statistics(outputFile);
        cpu = new CPU();
        roundRobin = new RRScheduler(quantum);
        
        //TODO: Write simulation here! 
    }
    
    public static void main(String args[]) {
        runRRSimulation(args[0], args[1], 100);
    }
    
}
