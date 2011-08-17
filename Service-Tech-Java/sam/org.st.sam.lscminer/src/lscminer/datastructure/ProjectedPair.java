package lscminer.datastructure;

import java.util.*;

/**
 * Capturing a pair of sequence id and its indices.
 * The sequence indices capture a list of remainder of
 * the list that can still be extended to form a longer
 * pattern. This kind of pair is called a projected pair.
 * A projected database consists of a set of pairs.
 * 
 * @author Anh Cuong Nguyen
 */
public class ProjectedPair {
    private int sequenceId;
    private LinkedList<Integer> patternExtensions;

    public ProjectedPair(int sequenceId, LinkedList<Integer> patternExtensions){
        this.sequenceId = sequenceId;
        this.patternExtensions = patternExtensions;
    }

    public int getSequenceId(){
        return sequenceId;
    }

    public void setSequenceId(int sequenceId){
        this.sequenceId = sequenceId;
    }

    public LinkedList<Integer> getPatternExtentions(){
        return this.patternExtensions;
    }

    public void setPatternExtensions(LinkedList<Integer> patternExtensions){
        this.patternExtensions = patternExtensions;
    }
}
