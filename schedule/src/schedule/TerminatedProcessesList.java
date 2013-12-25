package schedule;

import java.util.LinkedList;
import java.util.List;

public class TerminatedProcessesList {
    
    /**
     * The list of terminated processes
     */
    private final List<Process> processList;

    public TerminatedProcessesList() {
        this.processList = new LinkedList<>();
    }
    
    /**
     * Adds the process item to the processList, the clock time the process is added to the list
     * becomes the termination time of the process
     * @param item,a terminated process 
     */
    public void addProcess(Process item) {
        item.setTerminationTime(Clock.showTime());
        this.processList.add(item);
    }
    
    /** 
     * @return the processList size 
     */
    public int getListSize()
    {
        return this.processList.size();
    }
    
    /**
     * 
     * @param i, the position of the process in the processList
     * @return the process in the position i
     */
    public Process getProcess(int i){
        return this.processList.get(i);
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
