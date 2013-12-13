/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package schedule;

import java.io.File;

/**
 *
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
