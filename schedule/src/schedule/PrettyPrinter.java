package schedule;


import schedule.Clock;



/**
 * A helper class that contains a print method to handle message output in a unified way.
 */
public class PrettyPrinter {
    
    /**
     * Prints the source and message given along with a clock-stamp.
     * @param source
     * @param message 
     */
    public static void print(String source, String message) {
        System.out.println("[" + Clock.showTime()   + "] [" + source + "] " + "  " + message);
                
    } 
    
}
