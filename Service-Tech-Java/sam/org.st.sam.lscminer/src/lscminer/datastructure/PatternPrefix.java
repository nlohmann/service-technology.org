package lscminer.datastructure;

import java.util.*;

/**
 * Data structure capturing information pertaining to
 * pattern as we delve deeper into the prefix lattice.
 * 
 * @author Anh Cuong Nguyen
 */
public class PatternPrefix {
    private LinkedList<Integer> pattern;

    public PatternPrefix(){
        this.pattern = new LinkedList<Integer>();
    }

    public PatternPrefix(LinkedList<Integer> pattern){
        this.pattern = pattern;
    }

    public int getLastEvent(){
        return pattern.getLast();
    }

    public LinkedList<Integer> getEventList(){
        return this.pattern;
    }

    /**
     * Get the length of the pattern.
     *
     * @return length of the pattern content.
     */
    public int getLength(){
        return pattern.size();
    }

    /**
     * Append an integer event into the last of the pattern.
     * 
     * @param event a single event represented by an integer.
     */
    public void appendSuffixEvent(int event){
        pattern.addLast(event);
    }

    public void appendPrefixEvent(int event){
        pattern.addFirst(event);
    }

    public boolean containEvent(int event){
        return pattern.contains(event);
    }

    public void removeLastEvent(){
        pattern.removeLast();
    }

    public void removeFirstEvent(){
        pattern.removeFirst();
    }

    /**
     * Get maximal density of a prefix pattern by assuming that the pattern
     * will be appended into max length.
     *
     * @return the maximal density the pattern can get extended to.
     */
    public double getMaximalDensity(){
        HashMap<Integer, Boolean> distinctEvent = new HashMap<Integer, Boolean>(this.getLength());
        for (int eventValue : pattern){
            if (!distinctEvent.containsKey(eventValue))
                distinctEvent.put(eventValue, Boolean.TRUE);
        }
        int maxLength = Integer.MAX_VALUE;
        int maxRemainLength = maxLength - this.getLength();
        double maximalDensity = (double)(maxRemainLength + distinctEvent.size()) / (double)maxLength;

        return maximalDensity;
    }

    /**
     * Return a frequent pattern from the current prefix pattern, given the support number.
     * 
     * @param support the support number
     * @return frequent pattern of the current prefix pattern
     */
    public Pattern toPattern(int support){
        return new Pattern(this.pattern.toArray(new Integer[this.pattern.size()]), support);
    }

    /**
     * Check whether an array of integer is an instance of the current pattern.
     *
     * @param instance the array of integer under testing
     * @return true if the array is an instance of the current pattern; false otherwise
     */
    public boolean isInstance(Integer[] instance){
        /* this hash records distinct events in the pattern */
        HashMap<Integer, Boolean> patternEvents = new HashMap<Integer, Boolean>();
        for (int patternEvent: pattern){
            patternEvents.put(patternEvent, Boolean.TRUE);
        }

        int insInx = 0;
        for (int patternEvent: pattern){
            for (int inx = insInx; inx < instance.length; inx++){
                int instanceEvent = instance[inx];

                /* if instance event is a pattern event, it must be the next expected match; otherwise
                 * the mapping from the pattern into the instance cannot be one to one */
                if (patternEvents.containsKey(instanceEvent) && instanceEvent!=patternEvent)
                    return false;

                /* seeing a match, record the index matched so far of instance */
                if (instanceEvent == patternEvent){
                    insInx = inx+1; 
                    break;
                }

                /* proceed to the end of the instance without seeing any match */
                if (inx == instance.length-1){
                    return false;   
                }
            }
        }

        /* when a match has been identified, the rest of the instance must contain no pattern events */
        for (int inx = insInx; inx < instance.length; inx++){
            int instanceEvent = instance[inx];
            if (patternEvents.containsKey(instanceEvent)){
                return false;
            }
        }

        return true;
    }
    
}
