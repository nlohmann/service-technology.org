package org.st.sam.log;

import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import lscminer.datastructure.LSC;
import lscminer.datastructure.LSCEvent;

import org.deckfour.xes.model.XEvent;
import org.deckfour.xes.model.XLog;
import org.deckfour.xes.model.XTrace;

public class SLog {
  
  private short name_id = 0;
  
  public String originalNames[];
  public Map<String, Short> name2id;
  
  public short[][] traces;
  
  public SLog(XLog xLog) {
    translateLog(xLog);
  }
  
  private void translateLog(XLog xLog) {
    List<String> allNamesOrdered = new LinkedList<String>();
    
    name2id = new HashMap<String, Short>();
    
    traces = new short[xLog.size()][];
    
    int traceNum = 0;
    for (XTrace xTrace : xLog) {
      traces[traceNum] = new short[xTrace.size()];
    
      int eventNum = 0;
      for (XEvent xEvent : xTrace) {
        
        try {
          SEvent event = new SEvent(xEvent);
          String name = event.getCharacter();
          if (!name2id.keySet().contains(name)) {
            allNamesOrdered.add(name);
            name2id.put(name, name_id);
            name_id++;
          }
        
          traces[traceNum][eventNum] = name2id.get(name);
        } catch (Exception e) {
          System.err.println("Could not read event #"+eventNum+" in trace #"+traceNum);
          traces[traceNum][eventNum] = -1;
        }
        eventNum++;
      }
      traceNum++;
    }
    originalNames = allNamesOrdered.toArray(new String[allNamesOrdered.size()]);
  }

  public String toString_native() {
    StringBuilder sb = new StringBuilder();
    for (int t = 0; t < traces.length; t++) {
      sb.append(t+": ");
      for (int e = 0; e < traces[t].length; e++) {
        sb.append(traces[t][e]+" ");
      }
      sb.append("\n");
    }
    return sb.toString();
  }

  public LSCEvent toLSCEvent(short event, boolean shortNames) {
    int i1 = originalNames[event].indexOf('|');
    int i2 = originalNames[event].indexOf('|', i1+1);
    String caller = originalNames[event].substring(0, i1);
    String callee = originalNames[event].substring(i1+1, i2);
    String method = originalNames[event].substring(i2+1);
    
    if (shortNames) {
      caller = caller.substring(caller.lastIndexOf('.')+1);
      callee = callee.substring(callee.lastIndexOf('.')+1);
      method = method.substring(method.lastIndexOf('.')+1);
    }
    
    return new LSCEvent(caller, callee, method);
  }
  
  public LSC toLSC(SScenario s, int support, double confidence, boolean shortNames) {
    LSCEvent preChart[] = new LSCEvent[s.pre.length];
    for (int i=0; i<s.pre.length; i++) {
      preChart[i] = toLSCEvent(s.pre[i], shortNames); 
    }
    LSCEvent mainChart[] = new LSCEvent[s.main.length];
    for (int i=0; i<s.main.length; i++) {
      mainChart[i] = toLSCEvent(s.main[i], shortNames); 
    }
    LSC l = new LSC(preChart, mainChart, support, confidence);
    return l;
  }
}
