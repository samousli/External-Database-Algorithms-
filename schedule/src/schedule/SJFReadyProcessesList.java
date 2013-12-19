package schedule;

import java.util.Comparator;
import java.util.LinkedList;
import java.util.List;
import java.util.PriorityQueue;

/**
 * A list(sequence) which contains all the ready processes. Implementation
 * depends on the algorithm, perhaps a doubly linked list or a priority Queue
 * Optimal choices: RR: Circular Queue (Linked List with add() and removeFirst()
 * ) SJF: Priority Queue
 */
public class SJFReadyProcessesList {

    private final PriorityQueue<Process> processQueue;

    @SuppressWarnings("unchecked")
    public SJFReadyProcessesList() {
        this.processQueue = new PriorityQueue<>(11, new Comparator<Process>() {

            /*
             0 if p1.CpuRemainingTime = p2.CpuRemainingTime
             1 if p1.CpuRemainingTime > p2.CpuRemainingTime
             -1 if p1.CpuRemainingTime < p2.CpuRemainingTime
             */
            @Override
            public int compare(Process o1, Process o2) {
                if (o1.getCpuRemainingTime() == o2.getCpuRemainingTime()) {
                    return 0;
                } else if (o1.getCpuRemainingTime() > o2.getCpuRemainingTime()) {
                    return 1;
                } else {
                    return -1;
                }
            }
        });
    }

    public void addProcess(Process item) {
        item.setProcessState(ProcessState.READY);
        this.processQueue.add(item);
    }

    public Process getProcessToRunInCPU() {
        return processQueue.poll();
    }

    public int getListSize() {
        return this.processQueue.size();
    }


    /**
     * Prints every detail of the processes in the List
     */
    public void printList() {
        System.out.println("Number of processes:   " + this.processQueue.size());
        System.out.println("________Processes________");
        PriorityQueue<Process> temp = new PriorityQueue<>(this.processQueue);
        for (int i = 0; i < this.processQueue.size(); i++) {
            temp.poll().printProcess();
        }
    }

}
