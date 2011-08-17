package lscminer.tracer;

import java.io.*;
import java.util.*;
import java.util.ArrayList;
import lscminer.datastructure.LSCEvent;

/**
 *
 * @author Anh Cuong Nguyen
 */
public class BinaryMethodCallTracer {
  String pinPath;
  String tracerPlugin;
  String failInput;

  public BinaryMethodCallTracer(String pinPath, String tracerPlugin){
    this.pinPath = pinPath;
    this.tracerPlugin = tracerPlugin;
  }

  public String getFailInput(){
    return failInput;
  }

  public LSCEvent[][] trace(String binaryFile, String inputPath){
    String input = "";
    try {
      ArrayList<LSCEvent[]> dataSet = new ArrayList<LSCEvent[]>();
      File inputFile = new File(inputPath);
      Scanner sc = new Scanner(inputFile);
      while (sc.hasNext()){
        input = sc.nextLine();
        String pinCommand = pinPath + " -t " + tracerPlugin + " -- " + binaryFile + " " + input;
        System.out.println(pinPath);  // debug
        Process process = Runtime.getRuntime().exec(pinCommand);
        Scanner pinSc = new Scanner(new InputStreamReader(process.getInputStream()));

        ArrayList<LSCEvent> trace = new ArrayList<LSCEvent>();
        while (pinSc.hasNext()){
          String lscEventStr = pinSc.nextLine();
          System.out.println(lscEventStr);  // debug
          String[] split = lscEventStr.split("\\|");
          if (split.length == 3){ // some are outputs from software itself, not from the tracer
            LSCEvent event = new LSCEvent(split[0], split[1], split[2]);
            trace.add(event);
          }
        }

        dataSet.add(trace.toArray(new LSCEvent[trace.size()]));
      }
      return dataSet.toArray(new LSCEvent[dataSet.size()][]);
    } catch (IOException e){
      failInput = input;
      return null;
    }
  }
}
