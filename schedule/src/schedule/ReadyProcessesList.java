/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package schedule;

import java.util.LinkedList;
import java.util.List;

/**
 *  A list(sequence) which contains all the ready processes. 
 *  Implementation depends on the algorithm, 
 *  perhaps a doubly linked list or a priority Queue 
 *  Optimal choices:
 *      RR:  Circular Queue (Linked List with add() and removeFirst() ) 
 *      SJF: Priority Queue
 * @author avail
 */
public class ReadyProcessesList {

    private final List<Process> processList;

    public ReadyProcessesList() {
        this.processList = new LinkedList<>();
    }

    public void addProcess(Process item) {
        this.processList.add(item);
    }
    
    public Process getProcessToRunInCPU() {
        Process p = ((LinkedList<Process>)this.processList).removeFirst();
        return p;
    }

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
