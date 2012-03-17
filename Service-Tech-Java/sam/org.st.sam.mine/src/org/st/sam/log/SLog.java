package org.st.sam.log;

import com.google.gwt.dev.util.collect.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import lscminer.datastructure.LSC;
import lscminer.datastructure.LSCEvent;
import lscminer.datastructure.LSCEventTable;

import org.deckfour.xes.model.XEvent;
import org.deckfour.xes.model.XLog;
import org.deckfour.xes.model.XTrace;

public class SLog {
  
  private short name_id = 0;
  
  public String originalNames[];
  public Map<String, Short> name2id;
  public LSCEventTable lscEventTable;
  
  public Map<SEvent, XEvent> event2event;
  public Map<XEvent, Short> event2id;
  
  public short[][] traces;
  
  public SLog(XLog xLog) {
    translateLog(xLog, false);
  }
  
  public SLog(XLog xLog, boolean backwards) {
    translateLog(xLog, backwards);
  }
  
  private void translateLog(XLog xLog, boolean backwards) {
    List<String> allNamesOrdered = new LinkedList<String>();
    
    name2id = new HashMap<String, Short>();
    event2event = new java.util.HashMap<SEvent, XEvent>();
    event2id = new java.util.HashMap<XEvent, Short>();
    
    traces = new short[xLog.size()][];
    
    int traceNum = 0;
    for (XTrace xTrace : xLog) {
      traces[traceNum] = new short[xTrace.size()];
    
      int eventNum = 0;
      for (XEvent xEvent : xTrace) {
        
        try {
          SEvent event;
          try {
            event = new SEvent(xEvent);
          } catch (Exception e) {

            String name = xEvent.getAttributes().get("concept:name").toString();
            String lifeCycle = xEvent.getAttributes().get("lifecycle:transition").toString();
            //System.out.println(name+"+"+lifeCycle);
            event = new SEvent(name+"+"+lifeCycle, "s", "0", "r", "0", "0");
          }
          String name = event.getCharacter();
          if (!name2id.keySet().contains(name)) {
            allNamesOrdered.add(name);
            name2id.put(name, name_id);
            name_id++;
          }
          
          event2event.put(event, xEvent);
          event2id.put(xEvent, name2id.get(name));
        
          traces[traceNum][eventNum] = name2id.get(name);
        } catch (Exception e) {
          System.err.println("Could not read event #"+eventNum+" in trace #"+traceNum);
          traces[traceNum][eventNum] = -1;
        }
        eventNum++;
      }
      if (backwards) traces[traceNum] = invert(traces[traceNum]);
      traceNum++;
    }
    originalNames = allNamesOrdered.toArray(new String[allNamesOrdered.size()]);
    
    computeLSCEventNames();
    lscEventTable = new LSCEventTable(event_id_to_lsc_events);
  }
  
  public static short[] invert(short[] word) {
    short[] inverted = new short[word.length];
    for (int i=0; i<word.length; i++) {
      inverted[word.length-i-1] = word[i];
    }
    return inverted;
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
  
  public final static int LSC_CALLER = 0;
  public final static int LSC_CALLEE = 1;
  public final static int LSC_METHOD = 2;
  private String[][] event_id_to_lsc_events;
  
  public void computeLSCEventNames() {
    
    event_id_to_lsc_events = new String[originalNames.length][3];
    
    for (short event=0; event<originalNames.length;event++) {
      int i1 = originalNames[event].indexOf('|');
      int i2 = originalNames[event].indexOf('|', i1+1);
      
      event_id_to_lsc_events[event][LSC_CALLER] = originalNames[event].substring(0, i1);
      event_id_to_lsc_events[event][LSC_CALLEE] = originalNames[event].substring(i1+1, i2);
      event_id_to_lsc_events[event][LSC_METHOD] = originalNames[event].substring(i2+1);
    }
  }
  
  public String[] toLSCEventNames(short event) {
    return event_id_to_lsc_events[event];
  }

  public LSCEvent toLSCEvent(short event) {

    return new LSCEvent(
        lscEventTable.name2id_component.get(event_id_to_lsc_events[event][LSC_CALLER]),
        lscEventTable.name2id_component.get(event_id_to_lsc_events[event][LSC_CALLEE]),
        lscEventTable.name2id_method.get(event_id_to_lsc_events[event][LSC_METHOD]),
        lscEventTable);
  }
  
  public LSC toLSC(SScenario s, int support, double confidence) {
    LSCEvent preChart[] = new LSCEvent[s.pre.length];
    for (int i=0; i<s.pre.length; i++) {
      preChart[i] = toLSCEvent(s.pre[i]); 
    }
    LSCEvent mainChart[] = new LSCEvent[s.main.length];
    for (int i=0; i<s.main.length; i++) {
      mainChart[i] = toLSCEvent(s.main[i]); 
    }
    LSC l = new LSC(preChart, mainChart, support, confidence, lscEventTable);
    return l;
  }
  
  public short[] toSScenario_chart(LSCEvent chart[]) throws Exception {
    short s_chart[] = new short[chart.length];
    for (int e=0;e<chart.length;e++) {
      if (!name2id.containsKey(chart[e].toString())) {
        StringBuilder eventlist = new StringBuilder();
        for (String ev : name2id.keySet()) {
          eventlist.append(ev+"\n");
        }
        throw new Exception("Event "+chart[e]+" unknown in list:\n "+eventlist);
      }
      s_chart[e] = name2id.get(chart[e].toString());
    }
    return s_chart;
  }
  
  public SScenario toSScenario(LSC l) throws Exception {
    short pre[] = toSScenario_chart(l.getPreChart());
    short main[] = toSScenario_chart(l.getMainChart());
    return new SScenario(pre, main, l.getSupport());
  }
}
