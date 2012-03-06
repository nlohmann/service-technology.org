package lscminer.datastructure;

import java.util.Set;

import com.google.gwt.dev.util.collect.HashSet;

/**
 *
 * @author Anh Cuong Nguyen, dfahland
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
  
  public int getPrechartEventNo(){
    return preChart.length;
  }

  public int getMainchartEventNo(){
    return mainChart.length;
  }


  private int objectsNumber = -1;
  
  public int getDistinctObjectNo() {
    if (objectsNumber != -1) return objectsNumber;
    
    Set<String> objects = new HashSet<String>();
    for (LSCEvent event : preChart) {
      objects.add(event.caller);
      objects.add(event.callee);
    }
    for (LSCEvent event : mainChart) {
      objects.add(event.caller);
      objects.add(event.callee);
    }
    
    objectsNumber = objects.size();
    
    return objectsNumber;
  }
  
  /**
   * a LSC is valid only when its LSCEvents are connected.
   *
   * @return true if all events are connected; false otherwise
   */
  public Set<Integer> getUnConnectedEvents() {
    LSCEvent[] lsc = new LSCEvent[preChart.length + mainChart.length];
    System.arraycopy(preChart, 0, lsc, 0, preChart.length);
    System.arraycopy(mainChart, 0, lsc, preChart.length, mainChart.length);
    Set<Integer> unConnectedEvents = new HashSet<Integer>();
    Set<String> metObjects = new HashSet<String>();

    /* lsc[0] is the first connected events */
    for (int inx = 1; inx < lsc.length; inx++) unConnectedEvents.add(inx);
    metObjects.add(lsc[0].caller);
    metObjects.add(lsc[0].callee);

    /* add those events that are connected with lsc[0] deductively */
    while (true) {
      int metObjectNoBefore = metObjects.size();
      for (int inx = 1; inx < lsc.length; inx++) {
        if (unConnectedEvents.contains(inx)) {
          String caller = lsc[inx].getCaller();
          String callee = lsc[inx].getCallee();
          if (metObjects.contains(caller) || metObjects.contains(callee)) {
            unConnectedEvents.remove(inx);
            metObjects.add(caller);
            metObjects.add(callee);
          }
        }
      }
      int metObjectNoAfter = metObjects.size();
      
      if (metObjectNoAfter == metObjectNoBefore)
        break;
    }
    return unConnectedEvents;
  }

  /**
   * a LSC is valid only when its LSCEvents are connected.
   *
   * @return true if all events are connected; false otherwise
   */
  public boolean isConnected(){
    Set<Integer> unConnectedEvents = getUnConnectedEvents();
    return unConnectedEvents.size() == 0;
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
