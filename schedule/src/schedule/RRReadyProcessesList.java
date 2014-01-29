package schedule;

import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

/**
 * A list(sequence) which contains all the ready processes. Implementation
 * depends on the algorithm, perhaps a doubly linked list or a priority Queue
 * Optimal choices: RR: Circular Queue (Linked List with add() and removeFirst()
 * ) SJF: Priority Queue
 */
public class RRReadyProcessesList {

    private final List<Process> processList;
    private int nextProcessIndex = 0;

    public RRReadyProcessesList() {
        this.processList = new ArrayList<>();
    }

    public void addProcess(Process item) {
        item.setProcessState(ProcessState.READY);
        this.processList.add(item);
    }

    public Process getProcessToRunInCPU() {
        Process p = null;
        if (!this.processList.isEmpty()) {
            p = processList.get(nextProcessIndex);
            nextProcessIndex = (nextProcessIndex + 1) % processList.size();
        }
        return p;
    }

    public void removeProcess(Process p) {
        try {
            processList.remove(p);
        } catch (NullPointerException e) {}
        
        if (!processList.isEmpty()) {
            nextProcessIndex = (nextProcessIndex - 1) % processList.size();
        }
        
        if (processList.isEmpty() || nextProcessIndex == -1) {
            nextProcessIndex = 0;
        }

    }

    public int getListSize() {
        return this.processList.size();
    }

    public Process getProcess(int i) {
        return this.processList.get(i);
    }

    /**
     * Prints every detail of the processes in the List
     */
    public void printList() {
        System.out.println("Number of processes:   " + this.processList.size());
        System.out.println("________Processes________");
        for (int i = 0; i < this.processList.size(); i++) {
            this.processList.get(i).printProcess();
        }
    }

    public List<Process> getProcessList() {
        return Collections.unmodifiableList(this.processList);
    }

}
