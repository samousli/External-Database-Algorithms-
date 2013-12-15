package schedule;

import java.io.File;

/**
 * Collects statistics and logs them to a file.
 * @author avail
 */
public class Statistics {

    private float averageWaitingTime;
    private int totalWaitingTime;

    private int responseTime;

    private int maximumLengthOfReadyProcessesList;
    public int totalNumberOfProcesses;
    private File outputFile;

    public Statistics(String filename) {
    }

    public void UpdateMaximumListLength() {
    }

    public float CalculateAverageWaitingTime() {
        return 0;
    }

    public void WriteStatistics2File() {
    }
}
