package schedule;

import java.util.LinkedList;
import java.util.List;

/**
 * Container class for new processes. 
 * Currently implemented using a doubly-linked list.
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
     * @param process process to be added.
     */
    public void addNewProcess(Process process) {
        this.processList.add(process);
    }

    /**
     * Remove and return the first element of the list.
     * @return the first process.
     */
    public Process getFirst() {
        Process p = null;
        if (!processList.isEmpty()) {
            p = ((LinkedList<Process>)processList).removeFirst();
        }
        return p;
    }

    /**
     * Prints every detail of the processes in the List
     */
    public void printList() {
        System.out.println("Number of processes:   " + this.processList.size());
        System.out.println("________Processes________");
        for(int i = 0; i < this.processList.size() ; i++){
            this.processList.get(i).printProcess();
        }
    }

}
