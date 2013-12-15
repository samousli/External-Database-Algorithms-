package schedule;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;

/**
 *
 * @author avail
 */
public class ProcessGenerator {

    private File inputFile;

    public ProcessGenerator(String filename, boolean readFile) {
        Path p = Paths.get(filename);
        if (Files.exists((p))) {
            this.inputFile = new File(filename);

            try {
                //Initializing the IO
                BufferedReader input = new BufferedReader(new FileReader(filename));
                //Reading all the data.
                String text = input.readLine();
                //Closing input file.
                input.close();
                //Initializing the StringTokenizer which will be used to parse the input.
                StringTokenizer tokenizer = new StringTokenizer(text);

                //storing the numbers in an array list instead of a simple array 
                //because its size is unknown during initializion.
                ArrayList<Integer> temp = new ArrayList<>();
                //Parsing to array.
                while (tokenizer.hasMoreTokens()) {
                    temp.add(Integer.valueOf(Integer.parseInt(tokenizer.nextToken())));
                }
            } catch (IOException e) {
                System.err.println("Error: " + e);
            }
        }

    }

    public Process createProcess() {
        return null;
    }

    public void StoreProcessToFile() {
    }

    public List<Process> parseProcessFile() {
        return null;
    }
}
