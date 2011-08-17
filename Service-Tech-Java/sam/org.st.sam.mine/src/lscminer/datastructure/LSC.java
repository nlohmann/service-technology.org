package lscminer.datastructure;

import java.util.*;

/**
 *
 * @author Anh Cuong Nguyen
 */
public class LSC {
  LSCEvent[] preChart;
  LSCEvent[] mainChart;
  int support;
  double confidence;

  public LSC(LSCEvent[] preChart, LSCEvent[] mainChart, int support, double confidence){
    this.preChart = preChart;
    this.mainChart = mainChart;
    this.support = support;
    this.confidence = confidence;
  }

  public LSCEvent[] getPreChart(){
    return this.preChart;
  }

  public LSCEvent[] getMainChart(){
    return this.mainChart;
  }

  public int getSupport(){
    return support;
  }

  public double getConfidence(){
    return confidence;
  }

  public int getMethodStringMaxLength(){
      int maxLength = 0;
      for (LSCEvent event : preChart) {
          if (event.method.length() > maxLength)
              maxLength = event.method.length();
      }
      for (LSCEvent event : mainChart) {
          if (event.method.length() > maxLength)
              maxLength = event.method.length();
      }
      return maxLength;
  }

  public int getObjectStringMaxLength(){
      int maxLength = 0;
      for (LSCEvent event : preChart) {
          if (event.callee.length() > maxLength)
              maxLength = event.callee.length();
          if (event.caller.length() > maxLength)
              maxLength = event.callee.length();
      }
      for (LSCEvent event : mainChart) {
          if (event.callee.length() > maxLength)
              maxLength = event.callee.length();
          if (event.caller.length() > maxLength)
              maxLength = event.callee.length();
      }

      return maxLength;
  }

  public int getObjectsNo(){
    HashMap<String, Boolean> objects = new HashMap<String, Boolean>();
    for (LSCEvent event: preChart){
      objects.put(event.caller, Boolean.TRUE);
      objects.put(event.callee, Boolean.TRUE);
    }
    for (LSCEvent event: mainChart){
      objects.put(event.caller, Boolean.TRUE);
      objects.put(event.callee, Boolean.TRUE);
    }

    return objects.size();
  }

  public int getPrechartEventNo(){
    return preChart.length;
  }

  public int getMainchartEventNo(){
    return mainChart.length;
  }

  public int getDistinctObjectNo(){
    HashMap<String, Boolean> objects = new HashMap<String, Boolean>();

    for (LSCEvent event: preChart){
      objects.put(event.callee, Boolean.TRUE);
      objects.put(event.caller, Boolean.TRUE);
    }
    for (LSCEvent event: mainChart){
      objects.put(event.callee, Boolean.TRUE);
      objects.put(event.caller, Boolean.TRUE);
    }

    return objects.size();
  }

  /**
   * a LSC is valid only when its LSCEvents are connected.
   *
   * @return true if all events are connected; false otherwise
   */
  public boolean isConnected(){
    LSCEvent[] lsc = new LSCEvent[preChart.length + mainChart.length];
    System.arraycopy(preChart, 0, lsc, 0, preChart.length);
    System.arraycopy(mainChart, 0, lsc, preChart.length, mainChart.length);
    HashMap<LSCEvent, Boolean> connectedEvents = new HashMap<LSCEvent, Boolean>();
    HashMap<String, Boolean> metObjects = new HashMap<String, Boolean>();

    /* lsc[0] is the first connected events */
    connectedEvents.put(lsc[0], Boolean.TRUE);
    metObjects.put(lsc[0].caller, Boolean.TRUE);
    metObjects.put(lsc[0].callee, Boolean.TRUE);

    /* add those events that are connected with lsc[0] deductively */
    while (true) {
      int metObjectNoBefore = metObjects.size();
      for (int inx = 1; inx < lsc.length; inx++) {
        LSCEvent event = lsc[inx];
        if (!connectedEvents.containsKey(event)) {
          String caller = event.getCaller();
          String callee = event.getCallee();
          if (metObjects.containsKey(caller) || metObjects.containsKey(callee)) {
            connectedEvents.put(event, Boolean.TRUE);
            metObjects.put(caller, Boolean.TRUE);
            metObjects.put(callee, Boolean.TRUE);
          }
        }
      }
      int metObjectNoAfter = metObjects.size();
      
      if (metObjectNoAfter == metObjectNoBefore) {
        if (connectedEvents.size() == lsc.length) {
          return true;
        } else {
          return false;
        }
      }
    }
  }

  @Override
  public String toString(){
    String lsc = "(" + support + " ," + confidence + ")\n";
    lsc += "COLD: \n";
    for (LSCEvent pre: preChart){
      lsc += "\t " + pre + "\n";
    }
    lsc += "HOT: \n";
    for (LSCEvent post: mainChart){
      lsc += "\t " + post + "\n";
    }

    return lsc;
  }
}
