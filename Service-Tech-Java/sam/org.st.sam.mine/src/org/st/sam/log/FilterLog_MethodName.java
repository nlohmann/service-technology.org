package org.st.sam.log;

import java.io.IOException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;


import org.deckfour.xes.model.XEvent;
import org.deckfour.xes.model.XLog;
import org.deckfour.xes.model.XTrace;
import org.st.sam.mine.datastructure.LSCEvent;
import org.st.sam.util.SAMOutput;

public class FilterLog_MethodName {
  
  public FilterLog_MethodName(String logFile) throws IOException {
    loadLog(logFile);
  }

  private void loadLog(String logFile) throws IOException {
    loadXLog(logFile);
    XLog xlog = getXLog();
    setSLog(new SLog(xlog));
  }
  
  private XLog xlog;
  private SLog slog;
  
  private void setXLog(XLog xlog) {
    this.xlog = xlog;
  }
  
  private XLog getXLog() {
    return xlog;
  }
  
  private XLog loadXLog(String logFile) throws IOException {
    XESImport read = new XESImport();
    setXLog(read.readLog(logFile));
    return getXLog();
  }
  
  private void setSLog(SLog slog) {
    this.slog = slog;
  }
  
  public SLog getSLog() {
    return slog;
  }
  
  private Map<String,Integer> summary_id_count;
  private double summary_average_count;
  
  private void computeLogSummary() {
    summary_id_count = new HashMap<String, Integer>();
    
    summary_average_count = 0;
    
    Map<Short, String> ShortenedNames = new HashMap<Short, String>();
    for (Short i=0; i<getSLog().originalNames.length; i++) {
      String[] e = getSLog().toLSCEventNames(i);
      ShortenedNames.put(i, SAMOutput.shortenNames(e[SLog.LSC_METHOD]));
    }

    for (short[] trace : slog.traces) {
      for (short e : trace) {
        String methodName = ShortenedNames.get(e);
        
        if (!summary_id_count.containsKey(methodName))
          summary_id_count.put(methodName, 0);
        summary_id_count.put(methodName, summary_id_count.get(methodName)+1);
        summary_average_count += 1;
      }
    }
    summary_average_count = summary_average_count / summary_id_count.size();
  }
  
  public String logSummary() {
    StringBuilder s = new StringBuilder();
    
    Map<Short, String> ShortenedNames = new HashMap<Short, String>();
    for (Short i=0; i<getSLog().originalNames.length; i++) {
      String[] e = getSLog().toLSCEventNames(i);
      ShortenedNames.put(i, SAMOutput.shortenNames(e[SLog.LSC_METHOD]));
    }
    
    computeLogSummary();
    
    for (Map.Entry<String,Integer> pair : summary_id_count.entrySet()) {
      System.out.println(pair);
    }
    s.append("average event count: "+summary_average_count+"\n");
    
    return s.toString();
  }
  
  public boolean[] getFrequentEventFilter() {
    
    Map<Short, String> ShortenedNames = new HashMap<Short, String>();
    for (Short i=0; i<getSLog().originalNames.length; i++) {
      String[] e = getSLog().toLSCEventNames(i);
      ShortenedNames.put(i, SAMOutput.shortenNames(e[SLog.LSC_METHOD]));
    }
    
    boolean[] nonFrequent = new boolean[slog.originalNames.length];
  
    double threshold = summary_id_count.get(ShortenedNames.get((short)0));
    
    System.out.println(ShortenedNames);
    
    for (short e=0; e<nonFrequent.length; e++) {
      String name = ShortenedNames.get(e);
      if (summary_id_count.get(name) <= threshold) {
        System.out.println("keep   "+e+" "+summary_id_count.get(name)+"/"+threshold+" "+name);
        nonFrequent[e] = true;
      } else {
        System.out.println("drop   "+e+" "+summary_id_count.get(name)+"/"+threshold+" "+name);
        nonFrequent[e] = false;
      }
    }
    return nonFrequent;
  }
  
  public void filterEventClasses(boolean[] keepClass) {
    for (XTrace trace : xlog) {
      List<XEvent> toRemove = new LinkedList<XEvent>();
      for (XEvent e : trace) {
        short id = slog.event2id.get(e);
        if (!keepClass[id]) toRemove.add(e);
      }
      for (XEvent e : toRemove) {
        trace.remove(e);
      }
    }
  }
  
  public static void main(String[] args) throws IOException {
    
    if (args.length != 1) {
      System.out.println("error. wrong number of arguments. required:");
      System.out.println("   <logfile.xes>");
      return;
    }
    
    FilterLog_MethodName filter = new FilterLog_MethodName(args[0]);
    System.out.print(filter.logSummary());
    boolean keepClass[] = filter.getFrequentEventFilter();
    filter.filterEventClasses(keepClass);
    
    String outputFileName = XESImport.getLogFileName(args[0])+"_filtered2"+XESImport.getLogFileExtension(args[0]);
    XESExport.writeLog(filter.getXLog(), outputFileName);
  }
}
