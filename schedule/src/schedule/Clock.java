/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package schedule;

import java.util.Timer;
import java.util.TimerTask;

/**
 *
 * @author avail
 */
public class Clock {

    
    private static int ticks;
    public static final int tickFrequency = 100;
    private static Timer timer;
    private static TimerTickTask tickTask;

    public Clock() {
        Clock.timer = new Timer();
        timer.schedule(new TimerTickTask(this), 0 , tickFrequency);
    }
    
    private void timeRun() {
        ticks++;
    }

    public int showTime() {
        return ticks;
    }

    /**
     * A helper class thats used to recursively call the timeRun method.
     */
    private class TimerTickTask extends TimerTask
    {
        Clock clock;
        public TimerTickTask(Clock c) {
            this.clock = c;
        }

        @Override
        public void run() {
            clock.timeRun();
        }
        
    }
}
