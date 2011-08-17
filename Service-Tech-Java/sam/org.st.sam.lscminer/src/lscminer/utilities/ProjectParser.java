package lscminer.utilities;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Scanner;
import java.util.logging.Level;
import java.util.logging.Logger;
import lscminer.datastructure.LSC;
import lscminer.datastructure.LSCEvent;
import lscminer.datastructure.Project;

/**
 *
 * @author Anh Cuong Nguyen
 */
public class ProjectParser {
  public String projectName;
  public String taskName;
  public String projectLocation;
  public String support;
  public String confidence;
  public String density;
  public String mineType;
  public String mineTime;
  public String mineRuntime;
  public ArrayList<LSC> lscs;
  public LSCEvent[][] dataset;

  public void parse(Project project, String taskName){
    String projLoc = project.getLocation();
    projLoc = projLoc.replace('\\', '/');
    projLoc += "/" + taskName;

    File logFile = new File(projLoc + "/log.txt");
    File lscFile = new File(projLoc + "/lscs.txt");
    File traceFolder = new File(projLoc + "/traces");
    try {
      // <editor-fold desc="parse mining parameter information">
      Scanner logSc = new Scanner(logFile);
      while (logSc.hasNext()){
        String line = logSc.nextLine();
        String[] split = line.split("=");
        String var = split[0];
        String value = split[1];
        if (var.equals("projectName")) this.projectName = value;
        else if (var.equals("taskName")) this.taskName = value;
        else if (var.equals("projectLocation")) this.projectLocation = value;
        else if (var.equals("support")) this.support = value;
        else if (var.equals("confidence")) this.confidence = value;
        else if (var.equals("density")) this.density = value;
        else if (var.equals("mineType")) this.mineType = value;
        else if (var.equals("mineTime")) this.mineTime = value;
        else if (var.equals("mineRuntime")) this.mineRuntime = value;
      }
      // </editor-fold>

      // <editor-fold desc="parse lsc file">
      lscs = new ArrayList<LSC>();
      Scanner lscSc = new Scanner(lscFile);
      String line;
      String lscStr = "";
      while (lscSc.hasNext()){
        line = lscSc.nextLine().trim();
        if (line.equals("--------------------------------")){
          if (!lscStr.equals("")){
            lscs.add(LSCEventTraceParser.parse(lscStr));
          }
          lscStr = "";
        } else {
          lscStr += line + "\n";
        }
      }
      // </editor-fold>

      // <editor-fold desc="parse trace files">
      ArrayList<LSCEvent[]> dataList = new ArrayList<LSCEvent[]>();
      File[] traceFiles = traceFolder.listFiles();
      for (File traceFile: traceFiles){
        if (traceFile.isFile()){
          ArrayList<LSCEvent[]> trace = LSCEventTraceParser.parse(traceFile);
          if (trace != null) dataList.addAll(trace);
        }
      }
      dataset = dataList.toArray(new LSCEvent[dataList.size()][]);
      // </editor-fold>
    } catch (FileNotFoundException ex) {
      Logger.getLogger(ProjectParser.class.getName()).log(Level.SEVERE, null, ex);
    }
  }
}
