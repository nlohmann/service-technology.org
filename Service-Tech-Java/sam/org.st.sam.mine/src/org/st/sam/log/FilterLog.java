package org.st.sam.log;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import org.deckfour.xes.model.XEvent;
import org.deckfour.xes.model.XLog;
import org.deckfour.xes.model.XTrace;

public class FilterLog {
  
  public FilterLog(String logFile) throws IOException {
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
  
  private short[] summary_id_count;
  private double summary_average_count;
  
  private void computeLogSummary() {
    summary_id_count = new short[slog.originalNames.length];
    
    summary_average_count = 0;

    for (int e=0; e<summary_id_count.length; e++) summary_id_count[e] = 0;
    for (short[] trace : slog.traces) {
      for (short e : trace) {
        summary_id_count[e]++;
        summary_average_count += 1;
      }
    }
    summary_average_count = summary_average_count / summary_id_count.length;
  }
  
  public String logSummary() {
    StringBuilder s = new StringBuilder();
    
    computeLogSummary();
    
    for (int e=0; e<summary_id_count.length; e++) {
      String frequent_event = (summary_id_count[e] > summary_average_count) ? "+" : "-";
      s.append(e+" x "+summary_id_count[e]+frequent_event+": "+slog.originalNames[e]+"\n");
    }
    s.append("average event count: "+summary_average_count+"\n");
    
    return s.toString();
  }
  
  public boolean[] getFrequentEventFilter() {
    boolean[] nonFrequent = new boolean[summary_id_count.length];
  
    for (int e=0; e<summary_id_count.length; e++) {
      if (summary_id_count[e] <= summary_average_count) {
        nonFrequent[e] = true;
      } else {
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
    
    FilterLog filter = new FilterLog(args[0]);
    System.out.print(filter.logSummary());
    boolean keepClass[] = filter.getFrequentEventFilter();
    filter.filterEventClasses(keepClass);
    
    String outputFileName = XESImport.getLogFileName(args[0])+"_filtered"+XESImport.getLogFileExtension(args[0]);
    XESExport.writeLog(filter.getXLog(), outputFileName);
  }
}
