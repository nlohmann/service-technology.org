package org.st.sam.log;

import java.io.IOException;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import lscminer.datastructure.LSCEvent;

import org.deckfour.xes.model.XEvent;
import org.deckfour.xes.model.XLog;
import org.deckfour.xes.model.XTrace;
import org.st.sam.util.SAMOutput;

public class FilterLog_ByComponents {
  
  public FilterLog_ByComponents(String logFile) throws IOException {
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
  
  public boolean[] getEventFilter(String componentName) {
    
    boolean[] nonFrequent = new boolean[slog.originalNames.length];
  
    for (short e=0; e<nonFrequent.length; e++) {
      
      LSCEvent event = getSLog().toLSCEvent(e);
      
      boolean doFilter = false;
      if (event.getCallee().indexOf(componentName) >= 0 || event.getCaller().indexOf(componentName) >= 0)
        doFilter = true;

      if (!doFilter) {
        System.out.println("keep   "+event);
        nonFrequent[e] = true;
      } else {
        System.out.println("drop   "+event);
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
    
    if (args.length != 2) {
      System.out.println("error. wrong number of arguments. required:");
      System.out.println("   <logfile.xes> <component-substring>");
      return;
    }
    
    FilterLog_ByComponents filter = new FilterLog_ByComponents(args[0]);
    boolean keepClass[] = filter.getEventFilter(args[1]);
    filter.filterEventClasses(keepClass);
    
    String outputFileName = XESImport.getLogFileName(args[0])+"_filtered3"+XESImport.getLogFileExtension(args[0]);
    XESExport.writeLog(filter.getXLog(), outputFileName);
  }
}
