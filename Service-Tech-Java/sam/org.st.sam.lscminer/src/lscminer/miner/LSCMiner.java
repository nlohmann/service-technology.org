package lscminer.miner;

import java.util.*;
import lscminer.datastructure.*;

/**
 *
 * @author Anh Cuong Nguyen
 */
public class LSCMiner {
  ArrayList<LSC> lscs = new ArrayList<LSC>();

  public LSCMiner(){
    lscs = new ArrayList<LSC>();
  }

  public ArrayList<LSC> getLSCs(){
    return lscs;
  }

  public void mineLSCs(LSCEvent[][] dataSet, int minSupport, double minConfidence, double density){
    /* a hash maps lsc events to integer */
    HashMap<LSCEvent, Integer> lscToInt = new HashMap<LSCEvent, Integer>();
    /* a hash maps integer to lsc events */
    HashMap<Integer, LSCEvent> intToLSC = new HashMap<Integer, LSCEvent>();

    // <editor-fold desc="step 0: get a set of distinct lsc events & create integer data set">
    SingleEvent[][] intDataSet = new SingleEvent[dataSet.length][];
    int lscEventCounter = 0;
    int traceCounter = 0;
    for (LSCEvent[] trace: dataSet){
      SingleEvent[] intTrace = new SingleEvent[trace.length];
      int eventCounter = 0;
      for (LSCEvent lscEvent: trace){
        if (!lscToInt.containsKey(lscEvent)){
          lscToInt.put(lscEvent, lscEventCounter);
          intToLSC.put(lscEventCounter, lscEvent);
          lscEventCounter++;
        }
        intTrace[eventCounter] = new SingleEvent(lscToInt.get(lscEvent));
        eventCounter++;
      }
      intDataSet[traceCounter] = intTrace;
      traceCounter++;
    }
    // </editor-fold>

    // <editor-fold desc="step 1: mine temporal patterns from integer data set">
    TemporalPropertyMiner tpMiner = new TemporalPropertyMiner(false); // mine non-close property; filter non-close lsc later
    tpMiner.mineTemporalProperties(intDataSet, minSupport, minConfidence, density);
    LinkedList<TemporalProperty> properties =  tpMiner.getTemporalProperties();
    // </editor-fold>

    // <editor-fold desc="step 2: generate valid lsc from temporal patterns">
    HashMap<TemporalProperty, LSC> ltlToLSC = new HashMap<TemporalProperty, LSC>();
    int propertyInx = 0;
    while (propertyInx < properties.size()){
      TemporalProperty property = properties.get(propertyInx);
      Integer[] preConditions = property.getPreConditions();
      Integer[] postConditions = property.getPostConditions();
      LSCEvent[] preChart = new LSCEvent[preConditions.length];
      LSCEvent[] postChart = new LSCEvent[postConditions.length];

      /* generate lsc from temporal property */
      for (int inx = 0; inx < preConditions.length; inx++){
        int pre = preConditions[inx];
        preChart[inx] = intToLSC.get(pre);
      }
      for (int inx = 0; inx < postConditions.length; inx++){
        int post = postConditions[inx];
        postChart[inx] = intToLSC.get(post);
      }
      LSC lsc = new LSC(preChart, postChart, property.getSupport(), property.getConfidence());

      /* if lsc is not valid, remove property from properties list */
      if (!lsc.isConnected()){
        properties.remove(propertyInx);
      } else {
        ltlToLSC.put(property, lsc);
        propertyInx++;
      }
    }
    // </editor-fold>

    // <editor-fold desc="step 3: generate close lsc from valid lsc temporal patterns">
    LinkedList<TemporalProperty> closeLSCProperty = TemporalPropertyMiner.getCloseTemporalProperty(properties);
    for (TemporalProperty closeProperty: closeLSCProperty){
      lscs.add(ltlToLSC.get(closeProperty));
    }
    // </editor-fold>
  }
}
