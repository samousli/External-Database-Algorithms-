/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

package schedule;

/**
 *
 * @author avail
 */
public interface Scheduler {

    /**
     *
     * @param process process to be added
     */
    void addProcessToReadyList(Process process);
    /**
     * @return true if CPU is idle. (No ready processes are available)  
     */
    boolean isCPUIdle();
    
}
