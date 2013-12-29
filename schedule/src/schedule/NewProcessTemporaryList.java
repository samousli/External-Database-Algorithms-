package schedule;

import java.util.Collections;
import java.util.Comparator;
import java.util.LinkedList;
import java.util.List;

/**
 * Container class for new processes. Currently implemented using a
 * doubly-linked list.
 */
public class NewProcessTemporaryList {

    /**
     * A list that holds the new processes
     */
    private final List<Process> processList;

    public NewProcessTemporaryList() {
        // Using a linked list because of pop support.
        this.processList = new LinkedList<>();
    }

    /**
     * Adds a new process to the list.
     *
     * @param process process to be added.
     */
    public void addNewProcess(Process process) {
        this.processList.add(process);
    }

    /**
     * Remove and return the first element of the list.
     *
     * @return the first process.
     */
    public Process getFirst() {
        Process p = null;
        
        if (!processList.isEmpty()) {
            p = ((LinkedList<Process>) processList).removeFirst();
        }
        return p;
    }

    /**
     * Prints every detail of each process in the list.
     */
    public void printList() {
        System.out.println("Number of processes:   " + this.processList.size());
        System.out.println("________Processes________");
        for (int i = 0; i < this.processList.size(); i++) {
            this.processList.get(i).printProcess();
        }
    }
    
    /**
     * Sorts all elements in an ascending order based on their arrival time.
     */
    public void sortByArrivalTime() { 
        Collections.sort(processList, new Comparator<Process>() {

            @Override
            public int compare(Process p1, Process p2) {
                return new Integer(p1.getArrivalTime()).compareTo(
                       new Integer(p2.getArrivalTime()));
            }
        });
    }
    
    /**
     * 
     * @return the size of the process list.
     */
    public int getListSize() {
        return this.processList.size();
    }
}
