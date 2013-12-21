package schedule;

import java.util.TimerTask;

/**
 *
 */
public class Clock {
    
    /// Incremented by one every <tickPeriod> milliseconds
    /**
     *  Ticks since creation. Incremented manually.
     */
    private static int ticks;
    /**
     *  Time between each tick, in milliseconds.
     */
    //public static final int tickPeriod = 100;
    /**
     *  Timer that calls the timeRun() method every <tickPeriod>
     */
    //private static Timer timer;
    /**
     *  Initializes the object and schedules the timer.
     */
    public Clock() {
        //Clock.timer = new Timer();
        ticks = 0;
    }
    /**
     *  Increments the ticks
     */
    public void timeRun() {
        ticks++;
    }
    
    /**
     *  @return The current Clock time. 
     */
    public static int showTime() {
        return ticks;
    }

    /**
     *  A helper class thats used to recursively call the timeRun method.
     */
    private static class TimerTickTask extends TimerTask
    {
        private static Clock clock;
        public  TimerTickTask(Clock c) {
            clock = c;
        }

        @Override
        public void run() {
            clock.timeRun();
        } 
    }
    
    /**
     * Resets and starts the clock.
     */
    public void reset() {
        Clock.ticks = 0;
        //timer.schedule(new TimerTickTask(this), 0 , tickPeriod);
    }
    
    /**
     * Stops the clock.
     
    public void stop() {
        timer.cancel();
    }*/
}
