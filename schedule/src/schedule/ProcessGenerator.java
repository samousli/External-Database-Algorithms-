package schedule;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author avail
 */
public class ProcessGenerator {

    private File inputFile;
    private final List<Process> processList;

    public ProcessGenerator(String filename, boolean readFile) {
        this.processList = new ArrayList<>();
        Path p = Paths.get(filename);
        if (Files.exists((p))) {
            this.inputFile = new File(filename);

            try (BufferedReader input = new BufferedReader(new FileReader(filename))) {
                String text = input.readLine();
                int lineCount = Integer.parseInt(text);

                for (int i = 0; i < lineCount; i++) {
                    text = input.readLine();
                    String[] tokens = text.split(" ");
                    this.processList.add(new Process(i,
                            Integer.parseInt(tokens[0]),
                            Integer.parseInt(tokens[1])));
                }
            } catch (FileNotFoundException ex) {
                Logger.getLogger(ProcessGenerator.class.getName()).log(Level.SEVERE, "Input file not found.", ex);
            } catch (IOException ex) {
                Logger.getLogger(ProcessGenerator.class.getName()).log(Level.SEVERE, "File IO exception.", ex);
            }

        }
    }

    public Process createProcess() {
        return processList.remove(0);
    }

    public void StoreProcessToFile() {
    }

    public List<Process> parseProcessFile() {
        return null;
    }
}
