package lscminer.miner;

import java.util.*;
import lscminer.datastructure.*;

/**
 * A class to mine close, one effect-one cause
 * and interesting iterative patterns.
 * 
 * @author Anh Cuong Nguyen
 */
public class IterativePatternMiner {
    private LinkedList<Pattern> frequentPatterns;
    boolean close; // indicate whether to mine close or non-close iterative patterns

    public IterativePatternMiner(boolean close){
        frequentPatterns = new LinkedList<Pattern>();
        this.close = close;
    }

    public LinkedList<Pattern> getFrequentPatterns(){
        return frequentPatterns;
    }

    /**
     * The main function to mine iterative patterns.
     * 
     * @param dataSet a data set is a set of sequence of events.
     * @param minSupport the minimum support; a support is the number of occurrence of a pattern in the data set.
     * @param density the minimum density; density represents the density of distinct events inside a pattern
     */
    public void mineIterativePatterns(SingleEvent[][] dataSet, int minSupport, double density){
        assert minSupport > 0 : "minimum support must be a positive integer";
        /* this set captures all distinct events in the data set */
        Integer[] distinctEvents = getDistinctEvent(dataSet);

        // <editor-fold desc="step 1: compute one event frequence">
        /** step 1: compute one event frequent **/
        /* this hash maps a single event with its number of occurreces in the data set */
        HashMap<Integer, Integer> eventOccurrence = new HashMap<Integer, Integer>();
        for (int event: distinctEvents){
            eventOccurrence.put(event, 0);  // now just assume the occurence is 0
        }
        /* this hash maps a single event with its projected database */
        HashMap<Integer, LinkedList<ProjectedPair>> projectedDatabase = new HashMap<Integer, LinkedList<ProjectedPair>>();
        /* count the number of occurrence of events, record frequent events for possible extension */
        for (SingleEvent[] sequence: dataSet){
            for (SingleEvent event: sequence){
                int eventValue = event.getValue();
                int eventOccurr = eventOccurrence.get(eventValue) + 1; // increase event occurrence by 1
                eventOccurrence.put(eventValue, eventOccurr);
                /* there would be at most one time when the occurrence reachs minimum support */
                if (eventOccurr == minSupport){
                    /* as the single event is frequent, add its to the projected database for possible extension */
                    projectedDatabase.put(eventValue, new LinkedList<ProjectedPair>());
                }
            }
        }
        // </editor-fold>
        
        // <editor-fold desc="step 2: create projected database from frequent single events">
        /** step 2: create projected database for frequent single events **/
        for (int sequenceId = 0; sequenceId < dataSet.length; sequenceId++){
            SingleEvent[] sequence = dataSet[sequenceId];
            /* this hash maps a single frequent event with its position in the considering sequence */
            HashMap<Integer, LinkedList<Integer>> eventPosition = new HashMap<Integer, LinkedList<Integer>>();

            /* record the position of the event within the sequence */
            for (int position = 0; position < sequence.length; position++){
                SingleEvent event = sequence[position];
                int eventValue = event.getValue();
                if (projectedDatabase.containsKey(eventValue)) { // the event is frequent
                    if (eventPosition.containsKey(eventValue)){
                        /* add a new remainder index to an already-created remainder list */
                        LinkedList<Integer> sequenceRemainder = eventPosition.get(eventValue);
                        sequenceRemainder.addLast(position+1);
                    } else {
                        /* initialize a remainder list for a first-time met event */
                        LinkedList<Integer> sequenceRemainder = new LinkedList<Integer>();
                        sequenceRemainder.addLast(position+1);
                        eventPosition.put(eventValue, sequenceRemainder);
                    }
                }
            }
            
            /* create the projected database for each single frequent event */
            for (int eventValue : eventPosition.keySet()){
                LinkedList<Integer> eventPos = eventPosition.get(eventValue);
                ProjectedPair pair = new ProjectedPair(sequenceId, eventPos);
                projectedDatabase.get(eventValue).addLast(pair);
            }
        }
        // </editor-fold>

        // <editor-fold desc="step 3: recursively find frequent patterns from each frequent single event">
        /** step 3: recursively find frequent patterns from each frequent single event **/
        for (int eventValue : projectedDatabase.keySet()){
            PatternPrefix patternPrefix = new PatternPrefix();
            patternPrefix.appendSuffixEvent(eventValue);
            LinkedList<ProjectedPair> patternProjectedAll = projectedDatabase.get(eventValue);
            mineRecurse(patternPrefix, dataSet, patternProjectedAll, minSupport, density);
        }
        // </editor-fold>
    }

    /**
     * The recursive function to mine iterative patterns.
     *
     * @param patternPrefix the pattern has been seen so far.
     * @param dataSet the sequence of events data set
     * @param patternProjectedAll the projected database of the considering pattern
     * @param minSupport the minimum support
     * @param density the minimum density
     */
    private void mineRecurse(PatternPrefix patternPrefix, SingleEvent[][] dataSet, LinkedList<ProjectedPair> patternProjectedAll, int minSupport, double density) {
        int currentSupport = calculateSupport(patternProjectedAll);

        // <editor-fold desc="step 0: pre-filter processing">
        /** step 0: pre-filter processing */
        if (patternPrefix.getMaximalDensity() < density)
            return;
        // </editor-fold>

        // <editor-fold desc="step 1: compute one event frequent">
        /** step 1: compute one event frequent **/
        /* this hash maps a new event extension with its projected database */
        HashMap<Integer, LinkedList<ProjectedPair>> projectedDatabase = new HashMap<Integer, LinkedList<ProjectedPair>>();
        /* this hash maps a new frequent event extension with its projected database */
        HashMap<Integer, LinkedList<ProjectedPair>> frequentProjectedDatabase = new HashMap<Integer, LinkedList<ProjectedPair>>();
        /* this hash maps a new extension event (which represent a new pattern) to the new pattern support */
        HashMap<Integer, Integer> newSupports = new HashMap<Integer, Integer>();

        /* look for all possible single event extensions in each projected pair */
        for (ProjectedPair projectedPair : patternProjectedAll){
            int sequenceId = projectedPair.getSequenceId();
            SingleEvent[] sequence = dataSet[sequenceId];
            LinkedList<Integer> patternExtensions = projectedPair.getPatternExtentions();
//            debug
//            System.out.println("pattern = " + patternPrefix.getEventList() + "; sequenceId = " + sequenceId + "; extension = " + patternExtensions);
            for (int extensionIndex: patternExtensions){
                /* this hash records extension events have met so far; the hashmap is used
                 * to check for the second condition {forall ev in P++e, ev notin px} */
                HashMap<Integer, Boolean> metEvent = new HashMap<Integer, Boolean>();
                for (int position = extensionIndex; position < sequence.length; position++){
//                    debug
//                    System.out.println(position);
                    int extensionEvent = sequence[position].getValue();
                    if (metEvent.containsKey(extensionEvent)){
                        continue; // see this event already, process again will violate the second condition
                    }
                    metEvent.put(extensionEvent, Boolean.TRUE);
                    
                    /* extension event satisfies the first condition of projected-first sequence
                     * second condition has been checked above and below, the extension event is
                     * a valid extension event update support of the extension event */
                    if (isValidErasureBackScan(extensionEvent, extensionIndex, patternPrefix, sequence)){
//                    debug
//                    System.out.println("--- extEvent = " + extensionEvent + "@" + position);
                      // <editor-fold desc="update support of the extension event">
                      /* update support of the extension event */
                      int newSupport;
                      if (newSupports.containsKey(extensionEvent)){
                        newSupport = newSupports.get(extensionEvent)+1;
                      } else {
                        newSupport = 1;
                      }
                      newSupports.put(extensionEvent, newSupport);
                      // </editor-fold>

                      // <editor-fold desc="record candidate extension pattern into projected database">
                      /* add candidate extension events into projected database, this will be added 
                       * into frequent projected database later if the extension events are frequent */
                      if (projectedDatabase.containsKey(extensionEvent)){
                          /* the extension event (which means the pattern P++e) has been met, either append the
                           * extension index into an already created projected pair, or create a new projected pair
                           * with new sequence id and the extension index*/
                          LinkedList<ProjectedPair> pairs = projectedDatabase.get(extensionEvent);
                          ProjectedPair pair = pairs.getLast(); // get the latest pair (pairs are never empty)
                          if (pair.getSequenceId() == sequenceId){
                              /* still process in the same sequence */
                              pair.getPatternExtentions().addLast(position+1);
                          } else {
                              /* process a new sequence already, create a new pair with new sequence id */
                              ProjectedPair newPair = new ProjectedPair(sequenceId, new LinkedList<Integer>());
                              newPair.getPatternExtentions().add(position+1);
                              pairs.add(newPair);
                          }
                      } else {
                          /* first time seeing this extension event pattern, just create a new pair of sequence id and
                           * the extension index and add to the projected database*/
                          ProjectedPair pair = new ProjectedPair(sequenceId, new LinkedList<Integer>());
                          pair.getPatternExtentions().addLast(position+1);
                          LinkedList<ProjectedPair> pairs = new LinkedList<ProjectedPair>();
                          pairs.add(pair);
                          projectedDatabase.put(extensionEvent, pairs);
                      }
                      // </editor-fold>

                      // <editor-fold desc="record frequence extension patetrn">
                      /* frequent single extension events are added into frequent projected database */
                      if (newSupport == minSupport){
                        frequentProjectedDatabase.put(extensionEvent, new LinkedList<ProjectedPair>());
                      }
                      // </editor-fold>
                    }

                    if (patternPrefix.containEvent(extensionEvent)){
                        break;  // the middle string cannot contains elements of pattern prefix, so we can stop extending
                    }
                }
            }
        }
        // </editor-fold>

        // <editor-fold desc="step 2: create frequent projected database">
        /** step 2: create frequent projected database */
        /* just filter out infrequent extension event from the projected database */
        for (int frequentExtension: frequentProjectedDatabase.keySet()){
            frequentProjectedDatabase.put(frequentExtension, projectedDatabase.get(frequentExtension));
        }
        // </editor-fold>

        if (!close || (isEmptySuffixExtension(frequentProjectedDatabase, currentSupport)
              && isEmptyPrefixExtension(patternPrefix, patternProjectedAll, dataSet))) {
          frequentPatterns.add(patternPrefix.toPattern(currentSupport));
        }

        // <editor-fold desc="step 3: recursively find frequent patterns">
        /** step 3: recursively find frequent patterns **/
        for (int extensionEvent: frequentProjectedDatabase.keySet()){
//          System.out.println(patternPrefix.getEventList()); // debug
          patternPrefix.appendSuffixEvent(extensionEvent);
          LinkedList<ProjectedPair> newPatternProjectedAll = frequentProjectedDatabase.get(extensionEvent);
          LinkedList<ProjectedPair> newPatternProjectedAllClone = new LinkedList<ProjectedPair>();
          /* create a projected all clone, as we modify it in isEmptyInfixExtension */
          for (ProjectedPair pair: newPatternProjectedAll){
            ProjectedPair pairClone = new ProjectedPair(pair.getSequenceId(), (LinkedList<Integer>) pair.getPatternExtentions().clone());
            newPatternProjectedAllClone.add(pairClone);
          }
          if (!close || isEmptyInfixExtension(patternPrefix, newPatternProjectedAllClone, dataSet)) {
            mineRecurse(patternPrefix, dataSet, newPatternProjectedAll, minSupport, density);
          }
          patternPrefix.removeLastEvent();
        }
        // </editor-fold>
    }

    /**
     * Get a set of distinct events from a data set.
     *
     * @param dataSet a dataset is a set of sequence of events.
     * @return a set of distinct events from the data set.
     */
    private Integer[] getDistinctEvent(SingleEvent[][] dataSet){
        ArrayList<Integer> distinctEvents = new ArrayList<Integer>();

        for (SingleEvent[] sequence: dataSet){
            for (SingleEvent event: sequence){
                int eventValue = event.getValue();
                if (!distinctEvents.contains(eventValue)){
                    distinctEvents.add(eventValue);
                }
            }
        }
         
        return distinctEvents.toArray(new Integer[distinctEvents.size()]);
    }

    /**
     * Check for the first condition on erasure in projected-first & sequence.
     * {e is notin erasure(P_instance,P)}
     *
     * @param extensionEvent the extension event e
     * @param extensionIndex the start index of P_instance
     * @param patternPrefix the pattern P so far
     * @param sequence the considering sequence
     * @return true if the condition is satisfied, otherwise return false
     */
    private boolean isValidErasureBackScan(int extensionEvent, int extensionIndex, PatternPrefix patternPrefix, SingleEvent[] sequence) {
        int patternLength = patternPrefix.getLength();
        int remainedEvents = patternLength; // record number of events in the pattern that have not been captured by the instance
        LinkedList<Integer> pattern = patternPrefix.getEventList();

        /* backward scanning to check for erasure condition */
        for (int inx = extensionIndex - 1; inx >= 0; inx--) {
            int currentEvent = sequence[inx].getValue();
            if (remainedEvents != 0 && currentEvent == pattern.get(remainedEvents - 1)) {
                /* capture one more event */
                remainedEvents--;
            } else if (remainedEvents != 0) {
                if (currentEvent == extensionEvent){
                    /* violate erasure condition */
                    return false;
                }
            } else {
                break;
            }
        }

        return true;
    }

    /**
     * Calculate support of a pattern using its corresponding projected-all in the data set.
     * 
     * @param pairs the projected-all database of a pattern
     * @return the support of the pattern
     */
    private int calculateSupport(LinkedList<ProjectedPair> pairs){
        int support = 0;
        for (ProjectedPair pair: pairs){
            support += pair.getPatternExtentions().size();
        }

        return support;
    }

    /**
     * Check whether an suffix extension pattern has the same support as the old pattern.
     * Input is the projected-all database of the pattern and the old support of the pattern.
     * We can calculate the support of the suffix extension pattern by calculating the size
     * of the associated projection.
     *
     * @param newPatternProjected the projected database of the new suffix extension pattern
     * @param oldPatternSupport the support of the old pattern
     * @return true if these is at lease one projection that has the same old support; false otherwise
     */
    private boolean isEmptySuffixExtension(HashMap<Integer, LinkedList<ProjectedPair>> newPatternProjected, int oldPatternSupport){
        for (LinkedList<ProjectedPair> pairs: newPatternProjected.values()){
            if (calculateSupport(pairs) == oldPatternSupport){
                return false;
            }
        }

        return true;
    }

    /**
     * Check whether an prefix extension has the same support of the old pattern.
     * Input is the pattern, the projected-all database of the pattern and the dataset.
     * Basically, for each instance of the pattern recored by the project-all data base,
     * the instance should be prefix-extended to capture a prefix extension. If no such
     * prefix extension exists, the set of prefix extension is empty.
     *
     * @param patternPrefix the pattern
     * @param patternProjectedAll the projected all database of the pattern
     * @param dataSet the original data set
     * @return true if there exists prefix extension of the pattern; false otherwise
     */
    private boolean isEmptyPrefixExtension(PatternPrefix patternPrefix, LinkedList<ProjectedPair> patternProjectedAll, SingleEvent[][] dataSet){
      LinkedList<Integer> pattern = patternPrefix.getEventList();

      // <editor-fold desc="step 0: get start index of all instances from the pattern projected all">
      LinkedList<ProjectedPair> instanceStartIndices = new LinkedList<ProjectedPair>();
      for (ProjectedPair pair: patternProjectedAll){
        int sequenceId = pair.getSequenceId();
        SingleEvent[] sequence = dataSet[sequenceId];
        LinkedList<Integer> extensions = pair.getPatternExtentions();
        ProjectedPair startIndices = new ProjectedPair(sequenceId, new LinkedList<Integer>());
        for (int extension: extensions){
          /* traversing backward of sequence to obtain the start index of instance */
          int currentInstanceInx = extension-1;
          for (int patInx = pattern.size()-1; patInx >= 0; patInx--) {
            int patValue = pattern.get(patInx);
            for (int insInx = currentInstanceInx;; insInx--) { // there is no need for lower bound condition, as an instance is guaranteed to be found
              int insValue = sequence[insInx].getValue();
              if (insValue == patValue) {
                currentInstanceInx = insInx - 1;
                break;
              }
            }
          } // at the end, currentInstanceInx is the start index of the instance

          startIndices.getPatternExtentions().add(currentInstanceInx);
        }

        instanceStartIndices.add(startIndices);
      }
      // </editor-fold>

      // <editor-fold desc="step 1: get possible prefix extension events">
      HashMap<Integer, Boolean> prefixExtensions = new HashMap<Integer, Boolean>();
      /* obtain minimum extension backward index from the set of instanceStartIndices */
      int extensionSequenceId = 0;
      int extensionIndex = Integer.MAX_VALUE;
      for (ProjectedPair pair : instanceStartIndices) {
        if (pair.getPatternExtentions().get(0) < extensionIndex) {
          extensionSequenceId = pair.getSequenceId();
          extensionIndex = pair.getPatternExtentions().get(0);
        }
      }

      /* traverse backward the sequence to get all possible prefix extensions */
      SingleEvent[] seq = dataSet[extensionSequenceId];
      for (int inx = extensionIndex; inx >=0; inx--){
        prefixExtensions.put(seq[inx].getValue(), Boolean.TRUE);
      }

      if (prefixExtensions.isEmpty()){
        return true;
      }
      // </editor-fold>

      // <editor-fold desc="step 2: travese backward all sequences from its instance start index to find a match for the prefix event">
      /* get distinct events from pattern */
      HashMap<Integer, Boolean> patternEvents = new HashMap<Integer, Boolean>();
      for (int event: pattern){
        patternEvents.put(event, Boolean.TRUE);
      }

      /* this hash records all invalid prefix extensions checked so far */
      HashMap<Integer, Boolean> validPrefixExtensions = new HashMap<Integer, Boolean>();
      /* at the beginning all prefix extensions are assumed valid */
      for (int prefixEvent: prefixExtensions.keySet()){
        validPrefixExtensions.put(prefixEvent, Boolean.TRUE);
      }

      /* for each projected pair, traverse backward and incrementally invalidate prefix extensions */
      for (ProjectedPair pair : instanceStartIndices) {
        int sequenceId = pair.getSequenceId();
        SingleEvent[] sequence = dataSet[sequenceId];
        for (int backwardExtension : pair.getPatternExtentions()) {
          /* travesing backward for each backward extension of the considering sequence */
          // assume all prefix extensions are invalid as we have not met them yet */
          LinkedList<Integer> invalidPrefixExtensions = new LinkedList(validPrefixExtensions.keySet());
          for (int inx = backwardExtension; inx >= 0; inx--) {
            /* basically when traversing backward and we meet an event of the 
             * considering pattern, all prefix events have not met yet are invalid */
            int event = sequence[inx].getValue();
            if (validPrefixExtensions.containsKey(event)) {
              /* this event has been met before any of pattern events are observed, so it is still valid so far */
              invalidPrefixExtensions.remove((Integer)event);
              continue;
            }
            if (patternEvents.containsKey(event)) {
              /* stop traversing backward, all not met prefix events are invalid
               * all invalid prefix events are remove */
              for (int invalidPrefix: invalidPrefixExtensions){
                validPrefixExtensions.remove(invalidPrefix);
              }
              if (validPrefixExtensions.isEmpty()){
                return true;  // the set of prefix extension is empty
              }

              break;
            }
          }
        }
      }
      // </editor-fold>

      return false;
    }

    /**
     *
     * @param patternPrefix the pattern
     * @param patternProjectedAll the projected all database of the pattern
     * @param dataSet the original data set
     * @return true if there exists infix extension of the pattern; false otherwise
     */
    @SuppressWarnings("unchecked")
    private boolean isEmptyInfixExtension(PatternPrefix patternPrefix, LinkedList<ProjectedPair> patternProjectedAll, SingleEvent[][] dataSet){
//      System.out.println("--" + patternPrefix.getEventList());    // debug

      /* infix events are those in the middle of the instances that are not in the pattern projected all, and not pattern events */
      if (patternPrefix.getLength() < 2) return true;

      // <editor-fold desc="step 0: get invalid infix, which are events that belong to pattern projected all and pattern events">
      HashMap<Integer, Boolean> invalidInfixs = new HashMap<Integer, Boolean>();
      for (ProjectedPair pair: patternProjectedAll){
        SingleEvent[] sequence = dataSet[pair.getSequenceId()];
        int extension = pair.getPatternExtentions().get(0);
        for (int inx = extension; inx < sequence.length; inx++){
          int event = sequence[inx].getValue();
          if (!invalidInfixs.containsKey(event)){
            invalidInfixs.put(event, Boolean.TRUE);
          }
        }
      }
      LinkedList<Integer> events = patternPrefix.getEventList();
      HashMap<Integer, Boolean> patternEvents = new HashMap<Integer, Boolean>();
      for (int event: events){
        invalidInfixs.put(event, Boolean.TRUE);
        patternEvents.put(event, Boolean.TRUE);
      }
      // </editor-fold>

      // <editor-fold desc="step 1: get all middle segment px (string in between two pattern events) in all instances">
      int patternLength = patternPrefix.getLength();
      int middleSegmentNo = calculateSupport(patternProjectedAll);
      /* this hash records all infix candidates, it is empty means that we can return true */
      HashMap<Integer, Boolean> infixCandidates = new HashMap<Integer, Boolean>();
      /* for the first segment, the process is to get infix candidates; while the rest will invalid infix candidates */
      boolean firstSegment = true;
      /* for each event in the pattern, collect all segments between this event and its following events, in all
       * pattern instances; for each set of segments, calculate the infix candidates, which are those events that
       * occurs in all segments, in an equal number of occurrences */
      for (int patternInx = patternLength-2; patternInx >=0; patternInx--){   // ignore the ending element of pattern, as we ignores the end element of instance
        int patternEvent = events.get(patternInx);                            // current proceeded pattern event
        LinkedList[] middleSegments = new LinkedList[middleSegmentNo];        // to record all segments, the number of segments equal to the number of support
        int middleSegmentInx = 0;
        for (ProjectedPair pair: patternProjectedAll){
          SingleEvent[] sequence = dataSet[pair.getSequenceId()];
          LinkedList<Integer> currentSequenceInx = new LinkedList<Integer>(); // record the index proceeded so far
//          System.out.println(pair.getSequenceId() + "**" +  pair.getPatternExtentions()); // debug
          for (int extension: pair.getPatternExtentions()){
            LinkedList<Integer> middleSegment = new LinkedList<Integer>();
            for (int inx = extension-2; ; inx--){
              if (sequence[inx].getValue() == patternEvent){
                currentSequenceInx.add(inx+1);
                break;
              } else {
                middleSegment.add(sequence[inx].getValue());
              }
            }
            middleSegments[middleSegmentInx] = middleSegment;
            middleSegmentInx++;
          }
          pair.setPatternExtensions(currentSequenceInx); // record the index proceeds so far
        }

//        System.out.println("---- segment " + (patternInx - patternLength + 2) + "----" ); // debug
//        for (LinkedList<Integer> segment: middleSegments){  // debug
//          System.out.println("------" + segment);             // debug
//        }                                                   // debug
//        System.out.println("----------" ); // debug
        
        if (firstSegment){
          /* collect infix candidates, initially all events in the first segments are considered as valid */
          LinkedList<Integer> middleSegment0 = middleSegments[0];
          /* for each infix candidates, the number of occurences of the infix in all segments must be equal;
           * otherwise it is an invalid infix; the following use this information to invalid candidates */
          for (int infixCandidate: middleSegment0){
            if (invalidInfixs.containsKey(infixCandidate)){
              int infixCount0 = -1;
              boolean validInfix =  true;
              /* count the number of occurrences of infix candidates in all segments, and compare with infixCount0 */
              for (LinkedList<Integer> middleSegment: middleSegments){
                int infixCount = 0;
                for (int middleEvent: middleSegment){
                  if (middleEvent == infixCandidate){
                    infixCount++;
                  }
                }

                if (infixCount0 == -1){
                  infixCount0 = infixCount; // this iteration counts the infix in segment 0
                } else if (infixCount != infixCount0){
                  /* this is an invalid infix extension */
                  invalidInfixs.put(infixCandidate, Boolean.TRUE);
                  validInfix = false;
                  break;
                }
              }

              if (validInfix){
                infixCandidates.put(infixCandidate, Boolean.TRUE);
              }
            }
          }

          if (infixCandidates.isEmpty()) {
//            System.out.println(" : true"); // debug
            return true;
          }
        } else {
          /* for all infix candidates, each number of occurrences in all segments must be equal; otherwise
           * it is an invalid infix candidate */
          invalidInfixs = new HashMap<Integer, Boolean>();
          for (int infixCandidate: infixCandidates.keySet()){
            int infixCount0 = -1;
            /* count the number of occurrences of infix candidates in all segments, and compare with infixCount0 */
            for (LinkedList<Integer> middleSegment : middleSegments) {
              int infixCount = 0;
              for (int middleEvent : middleSegment) {
                if (middleEvent == infixCandidate) {
                  infixCount++;
                }
              }

              if (infixCount0 == -1) {
                infixCount0 = infixCount; // this iteration counts the infix in segment 0
              } else if (infixCount != infixCount0) {
                /* this is an invalid infix extension */
                invalidInfixs.put(infixCandidate, Boolean.TRUE);
                break;
              }
            }
          }
          /* remove invalid infix */
          for (int invalidInfix: invalidInfixs.keySet()){
            infixCandidates.remove(invalidInfix);
          }

          if (infixCandidates.isEmpty()) {
//            System.out.println(" : true"); // debug
            return true;
          }
        }
      }
      // </editor-fold>

//      System.out.println(" : false"); // debug
      return false;
    }
}
