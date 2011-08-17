package lscminer;

import java.io.*;
import java.util.*;
import java.util.logging.*;
import lscminer.gui.MainFrame;

/**
 * LSC Miner is a tool for mining inter-object scenario-based specifications in the form
 * of a UML2-compliant variant of Live Sequence Charts (LSC). LSC extends the International
 * Telecommunication Union  standard of Message Sequence Charts (MSC) (or UML Sequence Diagram),
 * popular visual languages for systems and software behavioral specifications, mainly with
 * ‘must’ and ‘may’ modalities, and thus allows to specify not only samples of expected behavior
 * but also requirements of the form: `whenever a specified sequence of events occurs, eventually
 * another specified sequence must occur’.
 * 
 * @author Anh Cuong Nguyen
 */
public class Main {
  public static final String sep = System.getProperty("file.separator");
  public static final String usrHome = System.getProperty("user.home");
  public static final String lscLoc = usrHome + sep + ".lscminer";
  public static final String prefName = "preferences.txt";
  static String projLoc = lscLoc + sep + "projects";     // project location
  static String hisLoc = lscLoc + sep + "history.txt";   // history file location
  static String pinLoc;
  static String btraceLoc;

  public static void main(String args[]) {
    setup();

    java.awt.EventQueue.invokeLater(new Runnable() {
      public void run() {
        new MainFrame().setVisible(true);
      }
    });
  }

  private static void setup(){
    try {
      File lscFolder = new File(lscLoc);
      if (!lscFolder.exists()) lscFolder.mkdir(); //create default system folder for lsc miner
      File prefFile = new File(lscLoc + sep + prefName);
      if (!prefFile.exists()) writePref();  // probably first use of system, create preference file
      else readPref();  // read projLoc and hisLoc as stored in preference file
      File projFolder = new File(projLoc);
      if (!projFolder.exists()) projFolder.mkdir();
      File hisFile = new File(hisLoc);
      if (!hisFile.exists()) hisFile.createNewFile();

      String os = System.getProperty("os.name");
      if (os.toLowerCase().startsWith("windows")) {
          pinLoc = System.getProperty("user.dir") + sep + "pin-windows" + sep + "pin.bat";
          btraceLoc = System.getProperty("user.dir") + sep + "pin-windows" + sep + "/BinTracer.dll";
      }
      else {
          pinLoc = System.getProperty("user.dir") + sep + "pin-unix" + sep + "pin";
          btraceLoc = System.getProperty("user.dir") + sep + "pin-unix" + sep + "/BinTracer.so";
      }
    } catch (IOException ex) {
      Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
    }
  }

  public static void writePref(){
    try {
      File prefFile = new File(lscLoc + sep + prefName);
      if (!prefFile.exists()) prefFile.createNewFile();
      PrintWriter pw = new PrintWriter(prefFile.getAbsolutePath());
      pw.println("projLoc=" + projLoc);
      pw.println("hisLoc=" + hisLoc);
      pw.close();
    } catch (IOException ex) {
      Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
    }
  }

  private static void readPref(){
    try {
      File prefFile = new File(lscLoc + sep + prefName);
      if (prefFile.exists()) {
        Scanner sc = new Scanner(prefFile);
        while (sc.hasNext()){
          String pref = sc.nextLine();
          String[] prefSplit = pref.split("=");
          if (prefSplit[0].equals("projLoc")){
            projLoc = prefSplit[1];
          } else if (prefSplit[0].equals("hisLoc")){
            hisLoc = prefSplit[1];
          }
        }
        sc.close();
      }
    } catch (IOException ex) {
      Logger.getLogger(Main.class.getName()).log(Level.SEVERE, null, ex);
    }
  }
  
  public static String getProjLoc(){
    return projLoc;
  }

  public static String getHisLoc(){
    return hisLoc;
  }

  public static String getPinLoc(){
      return pinLoc;
  }

  public static String getBTraceLoc(){
      return btraceLoc;
  }
  
  public static String getBtraceLoc() {
    return btraceLoc;
  }

  public static void setBtraceLoc(String btraceLoc) {
    Main.btraceLoc = btraceLoc;
  }

  public static void setProjLoc(String projLoc) {
    Main.projLoc = projLoc;
  }

  public static void setHisLoc(String hisLoc) {
    Main.hisLoc = hisLoc;
  }

  public static void setPinLoc(String pinLoc) {
    Main.pinLoc = pinLoc;
  }
  
  public static void setHisDisProjNo(int i) {
    // unknown
  }
}
