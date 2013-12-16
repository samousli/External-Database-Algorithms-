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
 *
 */
public class ProcessGenerator {

    private final File inputFile;
    private final List<Process> processList;
    private final boolean writeToFile;
    private final Random rnd;
    private int nextPID = 0;
    private Process lastProcess;

    public ProcessGenerator(String filename, boolean readFile) {
        this.writeToFile = !readFile;
        this.inputFile = new File(filename);
        this.processList = new ArrayList<>();
        this.rnd = new Random();
    }

    public Process createProcess() {
        if (this.writeToFile) {
            int burstTime = rnd.nextInt(100) + 1; // (1-100)
            this.lastProcess = new Process(nextPID,Clock.showTime(),burstTime);
            nextPID++;
            StoreProcessToFile();
        } else {
            this.lastProcess = processList.remove(0);
        }
        return this.lastProcess;
    }

    public void StoreProcessToFile() {
        
        try (Writer writer = new BufferedWriter(
                             new OutputStreamWriter(
                             new FileOutputStream(this.inputFile)))) {
            
            writer.write(this.lastProcess.getArrivalTime() + " " + this.lastProcess.getCpuTotalTime());
        } catch (IOException ex) {
            Logger.getLogger(ProcessGenerator.class.getName()).log(Level.SEVERE, null, ex);
        } 
    }

    public List<Process> parseProcessFile() {

        try (BufferedReader input = new BufferedReader(new FileReader(this.inputFile))) {
            String text = input.readLine();

            while ( text != null) {
                text = input.readLine();
                String[] tokens = text.split(" ");
                this.processList.add(new Process(this.nextPID,
                        Integer.parseInt(tokens[0]),
                        Integer.parseInt(tokens[1])));
                this.nextPID++;
            }
        } catch (FileNotFoundException ex) {
            Logger.getLogger(ProcessGenerator.class.getName()).log(Level.SEVERE, "Input file not found.", ex);
        } catch (IOException ex) {
            Logger.getLogger(ProcessGenerator.class.getName()).log(Level.SEVERE, "File IO exception.", ex);
        }
        return new ArrayList<>(processList);
    }

}
