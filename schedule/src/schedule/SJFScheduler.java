package schedule;

import java.util.Comparator;
import java.util.PriorityQueue;

/**
 *
 */
public class SJFScheduler {

    private final boolean isPreemptive;
    private SJFReadyProcessesList processList;
    
    /**
     * Queue to order the processes according to the remaining CPU time
     */
    private 

    SJFScheduler(boolean isPreemptive) {
        this.isPreemptive = isPreemptive;
        
    }

    public void SJF(RRReadyProcessesList list) {
        //this.processQueue.clear();
        for (int i = 0; i < list.getListSize(); i++)
        {
         //   this.processQueue.add(list.getProcess(i));
        }
        
    }

}
