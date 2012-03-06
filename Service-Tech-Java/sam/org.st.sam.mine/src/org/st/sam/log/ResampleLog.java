package org.st.sam.log;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import org.deckfour.xes.factory.XFactory;
import org.deckfour.xes.factory.XFactoryRegistry;
import org.deckfour.xes.model.XEvent;
import org.deckfour.xes.model.XLog;
import org.deckfour.xes.model.XTrace;

public class ResampleLog {
  
  public ResampleLog(String logFile, int newTraceOn) throws IOException {
    xlogFactory = XFactoryRegistry.instance().currentDefault();
    
    loadLog(logFile, newTraceOn);
  }

  private void loadLog(String logFile, int newTraceOn) throws IOException {
    loadXLog(logFile);
    XLog xlog = getXLog();
    setSLog(new SLog(xlog));
    this.newTraceOn = newTraceOn;
  }
  
  private int newTraceOn;
  
  private final XFactory xlogFactory;
  
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
  
  /**
   * resample the log to start a new trace on the given event
   * @return
   */
  public XLog resample() {
    
    XLog newLog = xlogFactory.createLog();
    
    for (XTrace trace : xlog) {
      
      XTrace currentTrace = null;
      
      for (XEvent e : trace) {
        short id = slog.event2id.get(e);
        if (id == newTraceOn) {
          currentTrace = xlogFactory.createTrace();
          newLog.add(currentTrace);
        }
        
        if (currentTrace != null) {
          currentTrace.add(e);
        }
      }
    }
    
    return newLog;
  }
  
  public static void main(String[] args) throws IOException {
    
    if (args.length != 2) {
      System.out.println("error. wrong number of arguments. required:");
      System.out.println("   <logfile.xes> <event.id-trigger-new-trace>");
      return;
    }
    
    ResampleLog filter = new ResampleLog(args[0], Integer.parseInt(args[1]));
    System.out.print(filter.logSummary());
    
    XLog resampled = filter.resample();
    
    String outputFileName = XESImport.getLogFileName(args[0])+"_resampled"+XESImport.getLogFileExtension(args[0]);
    XESExport.writeLog(resampled, outputFileName);
  }
}
