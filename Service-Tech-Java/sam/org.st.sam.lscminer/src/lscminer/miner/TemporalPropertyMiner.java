package lscminer.miner;

import java.util.*;
import lscminer.datastructure.*;

/**
 * A class to mine temporal property patterns
 * of the form preconditions imply postconditions.
 * 
 * @author Anh Cuong Nguyen
 */
public class TemporalPropertyMiner {

  boolean close;
  LinkedList<TemporalProperty> temporalProperties;

  public TemporalPropertyMiner(boolean close) {
    this.close = close;
    temporalProperties = new LinkedList<TemporalProperty>();
  }

  public LinkedList<TemporalProperty> getTemporalProperties() {
    return this.temporalProperties;
  }

  /**
   * main function to mine temporal properties with high support and confidence from a set of sequence of events
   *
   * @param dataSet a set of sequence of events
   * @param minSupport the minimum support of the temporal properties, which are the number of occurrences of the property in the data set
   * @param minConfidence the minimum confidence of the temporal properties, which indicates the likelihood of satisfiability of the property among the data set
   * @param density the density of the temporal properties, which is the ratio of distinct events over the total of events in the property
   */
  public void mineTemporalProperties(SingleEvent[][] dataSet, int minSupport, double minConfidence, double density) {
    LinkedList<TemporalProperty> confidentProperties = new LinkedList<TemporalProperty>();
    IterativePatternMiner ipMiner = new IterativePatternMiner(false);
    ipMiner.mineIterativePatterns(dataSet, minSupport, density);
    LinkedList<Pattern> frequentPatterns = ipMiner.getFrequentPatterns();

    /* a hash maps a pattern with its support */
    HashMap<LinkedList<Integer>, Integer> patterns = new HashMap<LinkedList<Integer>, Integer>();
    for (Pattern pattern : frequentPatterns) {
      patterns.put(new LinkedList(Arrays.asList(pattern.getFrequentSequence())), pattern.getSupport());
    }

    /* for each pattern, extract its prefix and postfix to form temporal property */
    for (Pattern pattern : frequentPatterns) {
      Integer[] patternContent = pattern.getFrequentSequence();
      int patternSupport = pattern.getSupport();
      LinkedList<Integer> preFix = new LinkedList<Integer>();
      LinkedList<Integer> postFix = new LinkedList<Integer>(Arrays.asList(patternContent));

      /* extract prefix and postfix for each pattern to from temporal property */
      for (int inx = 0; inx < patternContent.length - 1; inx++) {
        int event = patternContent[inx];
        preFix.add(event);
        postFix.removeFirst();
        int preFixSupport = patterns.get(preFix);
        double confidence = (double) patternSupport / (double) preFixSupport;
//        System.out.println(preFix + " --> " + postFix + "(" + confidence + ")"); // debug

        /* the property satisfies the minimum confidence */
        if (confidence >= minConfidence) {
          TemporalProperty property = new TemporalProperty(
                  preFix.toArray(new Integer[preFix.size()]),
                  postFix.toArray(new Integer[postFix.size()]),
                  patternSupport, confidence);
          confidentProperties.add(property);
        }
      }

      /* process the set of confident properties to filter out those that are weak */
      if (close) {
        temporalProperties = getCloseTemporalProperty(confidentProperties);
      } else {
        temporalProperties = confidentProperties;
      }

    }
  }

  /**
   * Get a strongest set of temporal properties from a given temporal property set.
   *
   * @param confidentProperties an arbitrary temporal property set
   * @return the strongest set of temporal properties
   */
  public static LinkedList<TemporalProperty> getCloseTemporalProperty(LinkedList<TemporalProperty> confidentProperties){
    LinkedList<TemporalProperty> closeProperties = new LinkedList<TemporalProperty>();
    HashMap<TemporalProperty, Boolean> weakProperties = new HashMap<TemporalProperty, Boolean>();

    /* process to the list in two loops to find all weak properties */
    for (int inx = 0; inx < confidentProperties.size(); inx++) {
      TemporalProperty currentProperty = confidentProperties.get(inx);
      if (!weakProperties.containsKey(currentProperty)) {
        for (int inx2 = inx + 1; inx2 < confidentProperties.size(); inx2++) {
          TemporalProperty nextProperty = confidentProperties.get(inx2);
          if (currentProperty.isStronger(nextProperty)) {
            weakProperties.put(nextProperty, Boolean.TRUE);
          } else if (nextProperty.isStronger(currentProperty)) {
            weakProperties.put(currentProperty, Boolean.TRUE);
          }
        }
      }
    }

    /* add only those that are not weak into the final list */
    for (TemporalProperty property : confidentProperties) {
      if (!weakProperties.containsKey(property)) {
        closeProperties.add(property);
      }
    }

    return closeProperties;
  }
}
