package lscminer.datastructure;

/**
 * Represent a frequent pattern that occurs 
 * in a sequence database at a support number of time.
 *
 * @author Anh Cuong Nguyen
 */
public class Pattern {
    private Integer[] frequentSequence;
    private int support;

    public Pattern(Integer[] frequentSequence, int support){
        this.frequentSequence = frequentSequence;
        this.support = support;
    }

    public Integer[] getFrequentSequence(){
      return frequentSequence;
    }

    public int getSupport(){
      return support;
    }

    @Override
    public String toString(){
        String sequence = "";

        for (int event: this.frequentSequence){
            sequence += event + " ";
        }
        sequence += ": " + support;

        return sequence;
    }
}
