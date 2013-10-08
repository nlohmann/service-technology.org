package org.st.sam.mine.datastructure;

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
  
  LSCEvent[] lsc;
  
  private LSCEventTable table;
  
  public LSC(LSCEvent[] preChart, LSCEvent[] mainChart, int support, double confidence, LSCEventTable table){
    this.preChart = preChart;
    this.mainChart = mainChart;
    this.support = support;
    this.confidence = confidence;
    this.table = table;
    
    lsc = new LSCEvent[preChart.length + mainChart.length];
    int j=0;
    int i=0;
    while (i<preChart.length) {
      lsc[j++] = preChart[i++]; 
    }
    i=0;
    while (i<mainChart.length) {
      lsc[j++] = mainChart[i++];
    }
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
          if (event.getMethodName().length() > maxLength)
              maxLength = event.getMethodName().length();
      }
      for (LSCEvent event : mainChart) {
          if (event.getMethodName().length() > maxLength)
              maxLength = event.getMethodName().length();
      }
      return maxLength;
  }

  public int getObjectStringMaxLength(){
      int maxLength = 0;
      for (LSCEvent event : preChart) {
          if (event.getCalleeName().length() > maxLength)
              maxLength = event.getCalleeName().length();
          if (event.getCallerName().length() > maxLength)
              maxLength = event.getCallerName().length();
      }
      for (LSCEvent event : mainChart) {
          if (event.getCalleeName().length() > maxLength)
              maxLength = event.getCalleeName().length();
          if (event.getCallerName().length() > maxLength)
              maxLength = event.getCallerName().length();
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
    
    boolean[] objects = new boolean[table.component_names.length];
    
    for (LSCEvent event : preChart) {
      objects[event.caller] = true;
      objects[event.callee] = true;
    }
    for (LSCEvent event : mainChart) {
      objects[event.caller] = true;
      objects[event.callee] = true;
    }
    
    objectsNumber = 0;
    for (int i=0; i<objects.length; i++)
      if (objects[i]) objectsNumber++;
    
    return objectsNumber;
  }
  
  /**
   * a LSC is valid only when its LSCEvents are connected.
   *
   * @return true if all events are connected; false otherwise
   */
  public boolean[] getUnConnectedEvents() {

    boolean[] unConnectedEvents = new boolean[lsc.length];
    boolean[] metObjects = new boolean[table.component_names.length];

    /* lsc[0] is the first connected events */
    for (int inx = 1; inx < lsc.length; inx++) unConnectedEvents[inx] = true;
    metObjects[lsc[0].caller] = true;
    metObjects[lsc[0].callee] = true;

    
    /* add those events that are connected with lsc[0] deductively */
    boolean added = true;
    while (added) {
      added = false;
      for (int inx = 1; inx < lsc.length; inx++) {
        if (unConnectedEvents[inx]) {
          short caller = lsc[inx].getCaller();
          short callee = lsc[inx].getCallee();
          if (metObjects[caller] || metObjects[callee]) {
            unConnectedEvents[inx] = false;
            if (!metObjects[caller]) added = true;
            if (!metObjects[callee]) added = true;
            
            metObjects[caller] = true;
            metObjects[callee] = true;
          }
        }
      }
    }
    return unConnectedEvents;
  }

  /**
   * a LSC is valid only when its LSCEvents are connected.
   *
   * @return true if all events are connected; false otherwise
   */
  public boolean isConnected(){
    boolean[] unConnectedEvents = getUnConnectedEvents();
    for (int i=0; i<unConnectedEvents.length; i++) if (unConnectedEvents[i]) return false;
    return true;
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
