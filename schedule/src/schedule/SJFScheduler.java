package schedule;

import java.util.Comparator;
import java.util.PriorityQueue;

/**
 *
 */
public class SJFScheduler {

    private boolean isPreemptive;
    
    /**
     * Queue to order the processes according to the remaining CPU time
     */
    private PriorityQueue <Process> processQueue = null;

    SJFScheduler(boolean isPreemptive) {
        this.isPreemptive = isPreemptive;
        this.processQueue =  new PriorityQueue(11, new Comparator(){

            /*
            0 if p1.CpuRemainingTime = p2.CpuRemainingTime
            1 if p1.CpuRemainingTime > p2.CpuRemainingTime
            -1 if p1.CpuRemainingTime < p2.CpuRemainingTime
            */
            @Override
            public int compare(Object o1, Object o2) {
                 Process p1 = (Process) o1;
                 Process p2 = (Process) o2;
                 if (p1.getCpuRemainingTime() == p2.getCpuRemainingTime())
                 {
                     return 0;
                 }
                 else if (p1.getCpuRemainingTime() > p2.getCpuRemainingTime())
                 {
                     return 1;
                 }
                 else
                 {
                     return -1;
                 }
            }
        });
    }

    public void SJF(ReadyProcessesList list) {
        this.processQueue.clear();
        for (int i = 0; i < list.getListSize(); i++)
        {
            this.processQueue.add(list.getProcess(i));
        }
        if (this.isPreemptive)
        {
            
        }
        else
        {
            
        }
    }

}
