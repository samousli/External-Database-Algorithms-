package schedule;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Creates processes given an input file or randomly, can also store these
 * processes in the input file.
 */
public class ProcessGenerator {

    private final File inputFile;
    private final List<Process> processList;
    private final boolean writeToFile;
    private final Random rnd;
    private int nextPID = 0;
    private Process lastProcess;

    /**
     *
     * @param filename input/output file name as defined by readFile parameter
     * @param readFile if it's set to true, the file is only readable, otherwise
     * random processes will be generated
     */
    public ProcessGenerator(String filename, boolean readFile) {
        this.writeToFile = !readFile;
        this.inputFile = new File(filename);
        this.processList = new ArrayList<>();
        this.rnd = new Random();
    }

    /**
     * @return a process from the input file if it exists, a random process
     * otherwise.
     */
    public Process createProcess() {
        if (this.writeToFile) {
            createRandomProcess();
            StoreProcessToFile();
        } else {
            if (this.processList.isEmpty()) {
                createRandomProcess();
            } else {
                this.lastProcess = processList.remove(0);
            }
        }
        return this.lastProcess;
    }

    /**
     * Creates a random process with burstTime ranging from 1 to 100 and arrival
     * time as current system clock time.
     */
    private void createRandomProcess() {
        // range: (now, now+10)
        int arrivalTime = rnd.nextInt(10) + Clock.showTime();
        // range: (1-100)
        int burstTime = rnd.nextInt(100) + 1;

        this.lastProcess = new Process(nextPID, arrivalTime, burstTime);
        nextPID++;
    }

    /**
     * Save the last created process to file.
     */
    public void StoreProcessToFile() {

        try (Writer writer = new BufferedWriter(
                new OutputStreamWriter(
                        new FileOutputStream(this.inputFile)))) {

                    writer.write(this.lastProcess.getArrivalTime() + " "
                            + this.lastProcess.getCpuTotalTime());
                } catch (IOException ex) {
                    Logger.getLogger(ProcessGenerator.class.getName()).log(Level.SEVERE,
                            "Unable to store process to file.", ex);
                }
    }

    /**
     * Parses the input file it's associated with and returns a list of processes
     * from the given data. Can ignore badly formatted files 
     * @return the processes defined in the input file.
     */
    public List<Process> parseProcessFile() {
        if (this.writeToFile) {
            return null;
        }

        try (BufferedReader input = new BufferedReader(
                new FileReader(this.inputFile))) {
            String text = input.readLine();
            int vals[];
            while (text != null) {
                
                vals = tokenizeStrToInt(text);
                // Ignore processes with invalid values.
                if (vals == null) {
                    System.out.println("Ignoring " + text);
                } else {
                this.processList.add(new Process(this.nextPID, vals[0], vals[1]));
                this.nextPID++;
                }
                text = input.readLine();
            }
        } catch (FileNotFoundException ex) {
            Logger.getLogger(ProcessGenerator.class.getName()).log(Level.SEVERE,
                    "Input file doesn't exist.", ex);
        } catch (IOException ex) {
            Logger.getLogger(ProcessGenerator.class.getName()).log(Level.SEVERE,
                    "File IO exception.", ex);
        }
        return new ArrayList<>(processList);
    }
    
    /**
     * Tokenizes the input string into 2 integers.
     * @param str input string
     * @return an array of 2 integers or null if there was an error
     */
    private int[] tokenizeStrToInt(String str) {
        String[] tokens = str.split(" ");
        
        if (tokens.length != 2 || tokens[0] == null || tokens[1].equals("") || 
                tokens[1] == null || tokens[1].equals("")) {
            return null;
        }
        int vals[] = new int[2];
        vals[0] = Integer.parseInt(tokens[0]);
        vals[1] = Integer.parseInt(tokens[1]);
        if (vals[0] < 0 || vals[1] < 0) {
            return null;
        } 
        return vals;
    }

}
