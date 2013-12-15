package schedule;

import java.util.Timer;
import java.util.TimerTask;

/**
 *
 * @author avail
 */
public class Clock {

    /**
     *  Ticks since creation. Incremented by one every <tickPeriod> milliseconds
     */
    private static int ticks;
    /**
     *  Time between each tick, in milliseconds.
     */
    public static final int tickPeriod = 100;
    /**
     *  Timer that calls the timeRun() method every <tickPeriod>
     */
    private static Timer timer;
    /**
     *  Initializes the object and schedules the timer.
     */
    public Clock() {
        Clock.timer = new Timer();
        timer.schedule(new TimerTickTask(this), 0 , tickPeriod);
    }
    /**
     *  Increments the ticks
     */
    private void timeRun() {
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
