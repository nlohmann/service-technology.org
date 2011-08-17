package lscminer.utilities;

import java.io.*;
import java.util.*;
import lscminer.datastructure.*;

/**
 *
 * @author Anh Cuong Nguyen
 */
public class LSCEventTraceParser {
  /**
   * A LSC trace file consists of LSC event in each line.
   * A file can represents many LSC trace where two traces are separated by a new line.
   *
   * @param lscTrace the file that contains LSC traces
   * @return a list of LSC traces
   */
  public static ArrayList<LSCEvent[]> parse(File lscTraceFile){
    ArrayList<LSCEvent[]> lscTraces = new ArrayList<LSCEvent[]>();
    try {
      Scanner sc = new Scanner(lscTraceFile);
      ArrayList<LSCEvent> lscTrace = new ArrayList<LSCEvent>();
      while (sc.hasNext()){
        String line = sc.nextLine();
        /* if line is not a new line, parse line and add to current trace */
        if (!line.trim().equals("")){
          LSCEvent lscEvent = parseEvent(line);
          if (lscEvent == null){
            return null;
          } else {
            lscTrace.add(lscEvent);
          }
        } else {
          /* the end of trace, put the trace to lscTraces */
          lscTraces.add(lscTrace.toArray(new LSCEvent[lscTrace.size()]));
          /* start a new trace record */
          lscTrace = new ArrayList<LSCEvent>();
        }
      }

      /* the end of the file, add the latest lscTrace into lscTraces if it is not null */
      if (!lscTrace.isEmpty()){
        lscTraces.add(lscTrace.toArray(new LSCEvent[lscTrace.size()]));
      }
    } catch (FileNotFoundException ex) {
      return null;
    }

    return lscTraces;
  }

  public static LSCEvent parseEvent(String event){
    String[] elements = event.split("\\|");
    if (elements.length == 3){
      String caller = elements[0];
      String callee = elements[1];
      String method = elements[2];
      return new LSCEvent(caller, callee, method);
    } else {
      return null;
    }
  }

  /**
   * Convert a string lsc to lsc object
   * @param lsc
   * @return
   */
  public static LSC parse(String lscStr){
    int support;
    double confidence;
    LSCEvent[] preChart;
    LSCEvent[] mainChart;

    Scanner sc = new Scanner(lscStr);
    String supAndConf = sc.nextLine().trim();  // first line is support and confidence
    supAndConf = supAndConf.substring(1, supAndConf.length()-1);
    String[] supAndConfSplit = supAndConf.split(",");
    support = Integer.parseInt(supAndConfSplit[0].trim());
    confidence = Double.parseDouble(supAndConfSplit[1].trim());

    String cold = sc.nextLine();      // COLD: line
    String line = sc.nextLine().trim();
    ArrayList<LSCEvent> lscEvents = new ArrayList<LSCEvent>();
    while (!line.equals("HOT:")){
      lscEvents.add(LSCEventTraceParser.parseEvent(line));
      line = sc.nextLine().trim();
    }
    preChart = lscEvents.toArray(new LSCEvent[lscEvents.size()]);
    // after HOT: line
    lscEvents = new ArrayList<LSCEvent>();
    while (sc.hasNext()){
      line = sc.nextLine().trim();
      if (!line.equals(""))
        lscEvents.add(LSCEventTraceParser.parseEvent(line));
    }
    mainChart = lscEvents.toArray(new LSCEvent[lscEvents.size()]);

    return new LSC(preChart, mainChart, support, confidence);
  }
}


