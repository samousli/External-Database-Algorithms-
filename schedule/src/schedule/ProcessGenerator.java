package schedule;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.io.Writer;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Collections;
import java.util.Date;
import java.util.GregorianCalendar;
import java.util.List;
import java.util.Random;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Creates processes given an input file or randomly, can also store these
 * processes in the input file.
 */
public class ProcessGenerator {

    private File inputFile = null;
    private final List<Process> processList;
    private final boolean readOnly;
    private final Random rnd;
    private int nextPID = 0;
    private Process lastProcess;

    /**
     * This class handles all process creation requests.
     *
     * @param filename input/output file name as defined by readFile parameter
     * @param readOnly if it's set to true, the file is only readable, otherwise
     * random processes will be generated
     */
    public ProcessGenerator(String filename, boolean readOnly) {
        this.readOnly = readOnly;
        this.rnd = new Random();
        this.processList = new ArrayList<>();

        // If file name is null and file is not read-only, 
        // create random filename to save randomly created processes.
        String _filename = filename;
        if (_filename == null && !this.readOnly) {
            Date now = new Date();

            _filename = "Input " + now.toLocaleString() + ".txt";
        }

        this.inputFile = new File(_filename);

        if (!this.readOnly) {
            try {
                this.inputFile.createNewFile();
            } catch (IOException ex) {
                Logger.getLogger(ProcessGenerator.class.getName()).log(
                        Level.SEVERE, "Unable to create new input file", ex);
            }
        }

    }

    /**
     * @return a process from the input file if it exists, a random process
     * otherwise.
     */
    public Process createProcess() {
        Process p;

        if (!this.processList.isEmpty()) {
            p = processList.remove(0);
        } else {
            p = createRandomProcess();
        }

        if (!this.readOnly) {
            StoreProcessToFile(p);
        }
        return p;
    }

    /**
     * Creates a random process with burstTime ranging from 1 to 100 and arrival
     * time as current system clock time.
     */
    private Process createRandomProcess() {
        // range: (now, now+10)
        int arrivalTime = rnd.nextInt(10) + Clock.showTime();
        // range: (1-100)
        int burstTime = rnd.nextInt(10) + 1;
        nextPID++;
        return new Process(nextPID, arrivalTime, burstTime);
    }

    /**
     * @return generates a list of random processes
     */
    public List<Process> generateRandomList(int count) {
        //int count = rnd.nextInt(20) + 1;
        List<Process> pList = new ArrayList<>();
        Process p;
        for (int i = 0; i < count; i++) {
            p = this.createRandomProcess();
            StoreProcessToFile(p);
            pList.add(p);
        }
        return pList;
    }

    /**
     * Save a process to file.
     *
     * @param process process to be stored in file.
     */
    public void StoreProcessToFile(Process process) {
        if (this.readOnly) {
            return;
        }

        try (PrintWriter writer = new PrintWriter(
                new BufferedWriter(new FileWriter(inputFile, true)))) {
            
            writer.println(process.getArrivalTime() + " "
                    + process.getCpuTotalTime());

        } catch (IOException ex) {
            Logger.getLogger(ProcessGenerator.class.getName()).log(
                    Level.SEVERE, "Can't write process to file.", ex);
        }
    }

    /**
     * Parses the input file it's associated with and returns an unmodifiable
     * list of processes from the given data. Ignores badly formatted lines.
     *
     * @return the processes defined in the input file.
     */
    public List<Process> parseProcessFile() {

        try (BufferedReader input = new BufferedReader(
                new FileReader(this.inputFile))) {
            String text = input.readLine();
            int vals[];
            int lineCount = 1;
            while (text != null) {

                vals = tokenizeStrToInt(text);
                // Ignore processes with invalid values.
                if (vals == null) {
                    // Check for comment lines
                    if (!text.startsWith("//")) {
                        System.out.println("Ignoring input: line "
                                + lineCount + " (\"" + text + "\")");
                    }
                } else {
                    this.processList.add(new Process(
                            this.nextPID, vals[0], vals[1]));
                    this.nextPID++;
                }
                text = input.readLine();
                lineCount++;
            }
        } catch (FileNotFoundException ex) {
            Logger.getLogger(ProcessGenerator.class.getName()).log(Level.SEVERE,
                    "Input file doesn't exist.", ex);
        } catch (IOException ex) {
            Logger.getLogger(ProcessGenerator.class.getName()).log(Level.SEVERE,
                    "File IO exception.", ex);
        }
        return Collections.unmodifiableList(processList);
    }

    /**
     * Tokenizes the input string into 2 integers.
     *
     * @param str input string
     * @return an array of 2 integers or null if there was an error
     */
    private int[] tokenizeStrToInt(String str) {
        String[] tokens = str.split(" ");

        if (tokens.length != 2 || tokens[0] == null || tokens[1].equals("")
                || tokens[1] == null || tokens[1].equals("")) {
            return null;
        }
        int vals[] = new int[2];
        try {
            vals[0] = Integer.parseInt(tokens[0]);
            vals[1] = Integer.parseInt(tokens[1]);
        } catch (NumberFormatException e) {
            return null;
        }
        if (vals[0] < 0 || vals[1] <= 0) {
            return null;
        }
        return vals;
    }

}
