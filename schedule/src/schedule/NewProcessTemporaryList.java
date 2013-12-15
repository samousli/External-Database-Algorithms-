/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package schedule;

import java.util.LinkedList;
import java.util.List;

/**
 * Container class for new processes. 
 * Currently implemented using a doubly-linked list.
 * @author avail
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
        Process p = ((LinkedList<Process>)this.processList).pop();
        return p;
    }

    /**
     * Prints all elements of the list.
     */
    public void printList() {
        int i = 1;
        System.out.println("List size: " + this.processList.size() 
                + "List elements:");
        for(Process p : this.processList) {
            System.out.println(i + p.toString());
            i++;
        }
    }

}
