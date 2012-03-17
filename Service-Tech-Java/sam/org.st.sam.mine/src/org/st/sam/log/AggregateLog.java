package org.st.sam.log;

import java.io.IOException;
import java.util.LinkedList;
import java.util.List;

import org.deckfour.xes.extension.std.XConceptExtension;
import org.deckfour.xes.factory.XFactory;
import org.deckfour.xes.factory.XFactoryRegistry;
import org.deckfour.xes.model.XAttribute;
import org.deckfour.xes.model.XAttributeLiteral;
import org.deckfour.xes.model.XEvent;
import org.deckfour.xes.model.XLog;
import org.deckfour.xes.model.XTrace;

public class AggregateLog {
  
  public AggregateLog(String logFile) throws IOException {
    xlogFactory = XFactoryRegistry.instance().currentDefault();
    
    loadLog(logFile);
  }

  private void loadLog(String logFile) throws IOException {
    loadXLog(logFile);
    XLog xlog = getXLog();
    setSLog(new SLog(xlog));
  }
  
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
  
  public XLog resample() {
    
    XLog newLog = xlogFactory.createLog();
    
    for (XTrace trace : xlog) {
      
      XTrace currentTrace = xlogFactory.createTrace();
      newLog.add(currentTrace);

      
      int sameEventCount = 1;
      XEvent lastEvent = null;
      
      for (XEvent e : trace) {
        short id = slog.event2id.get(e);
        short lastEventId = (lastEvent != null) ? slog.event2id.get(lastEvent) : -1;
        
        if (id != lastEventId) {
          if (lastEvent != null) {
            if (sameEventCount == 1) {
              currentTrace.add(lastEvent);
            } else {
              
              XEvent eAggregate = xlogFactory.createEvent(lastEvent.getAttributes());
              XAttributeLiteral name = (XAttributeLiteral)eAggregate.getAttributes().get(SEvent.FIELD_METHOD);
              XAttribute name_repeat = xlogFactory.createAttributeLiteral(SEvent.FIELD_METHOD, name.getValue()+"+", XConceptExtension.instance()); 
              eAggregate.getAttributes().put(SEvent.FIELD_METHOD, name_repeat);
              
              currentTrace.add(eAggregate);
              
              sameEventCount = 1;
            }
          }
        } else {
          sameEventCount++;
        }
        lastEvent = e;
      }

      // take care of last event in the trace
      if (sameEventCount == 1) {
        currentTrace.add(lastEvent);
      } else {
        
        XEvent eAggregate = xlogFactory.createEvent(lastEvent.getAttributes());
        XAttributeLiteral name = (XAttributeLiteral)eAggregate.getAttributes().get(SEvent.FIELD_METHOD);
        XAttribute name_repeat = xlogFactory.createAttributeLiteral(SEvent.FIELD_METHOD, name.getValue()+"+", XConceptExtension.instance()); 
        eAggregate.getAttributes().put(SEvent.FIELD_METHOD, name_repeat);
        
        currentTrace.add(eAggregate);
      }
    }
    
    return newLog;
  }
  
  public static void main(String[] args) throws IOException {
    
    if (args.length != 2) {
      System.out.println("error. wrong number of arguments. required:");
      System.out.println("   <logfile.xes.gz> <result.xes.gz>");
      return;
    }
    
    AggregateLog filter = new AggregateLog(args[0]);
    System.out.print(filter.logSummary());
    
    XLog resampled = filter.resample();
    
    String outputFileName = XESImport.getLogFileName(args[1])+XESImport.getLogFileExtension(args[1]);
    XESExport.writeLog(resampled, outputFileName);
  }
}
