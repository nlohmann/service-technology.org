package org.st.sam.log;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Scanner;
import java.util.regex.Pattern;

import org.deckfour.xes.factory.XFactory;
import org.deckfour.xes.factory.XFactoryRegistry;
import org.deckfour.xes.model.XEvent;
import org.deckfour.xes.model.XLog;
import org.deckfour.xes.model.XTrace;
import org.deckfour.xes.out.XSerializer;
import org.deckfour.xes.out.XesXmlGZIPSerializer;
import org.deckfour.xes.out.XesXmlSerializer;

public class ConvertLog {
  
  private final XFactory xlogFactory;
  private final XLog log;
  
  public ConvertLog() {
    xlogFactory = XFactoryRegistry.instance().currentDefault();
    log = xlogFactory.createLog();
  }
  
  public void read(String dirName) throws FileNotFoundException {
    File logFile = new File(dirName);
    
    if (logFile.isDirectory()) {
      for (File f : logFile.listFiles()) {
        if (f.getName().endsWith("txt")) {
          readTraceFromFile(f);
        }
      }
    } else {
      readTraceFromFile(logFile);
    }
  }
  
  public void writeLog(String fileName) throws IOException {
    FileOutputStream out = new FileOutputStream(fileName);
    XSerializer logSerializer;
    if (fileName.endsWith("gz"))
      logSerializer = new XesXmlGZIPSerializer();
    else
      logSerializer = new XesXmlSerializer();
    logSerializer.serialize(log, out);
    out.close();
  }
  
  public XLog getLog() {
    return log;
  }
  
  public void readTraceFromFile(File traceFile) throws FileNotFoundException {
    
    System.out.println("Reading "+traceFile.getName());
    
    XTrace trace = xlogFactory.createTrace(); 
    
    Scanner scanner = new Scanner(traceFile);
    try {
      // get each line and parse the line to extract the case information
      while ( scanner.hasNextLine() ){
        XEvent e = processEventLine( scanner.nextLine() );
        trace.add(e);
      }
    }
    finally {
      //ensure the underlying stream is always closed
      scanner.close();
    }
    
    log.add(trace);
  }
  
  protected XEvent processEventLine(String caseLine) {
    // use a second Scanner to parse the content of each line
    Scanner scanner = new Scanner(caseLine);
    scanner.useDelimiter(Pattern.compile("\\|"));

    // sender
    String sender = "default_sender";
    // receiver
    String receiver = "default_receiver";
    // method
    String method = "default_method";
    // result
    String result = "default_result";
    
    // sender
    sender = scanner.next();
    // receiver
    receiver = scanner.next();
    // method
    method = scanner.next();
    // result
    if (scanner.hasNext()) {
      result = scanner.next();
    }
    
    scanner.close();
    
    int senderDelim = sender.indexOf('&');
    String senderName = sender.substring(0,senderDelim);
    String senderID = sender.substring(senderDelim+1);
    
    int recvDelim = receiver.indexOf('&');
    String recvName = receiver.substring(0, recvDelim);
    String recvID = receiver.substring(recvDelim+1);
    
    SEvent e = new SEvent(method, senderName, senderID, recvName, recvID, result);
    return e.toXEvent(xlogFactory);
  }
  
  public static void main(String args[]) throws Exception {
    
    if (args.length != 2) {
      System.out.println("error, wrong number of arguments");
      System.out.println("usage: java "+ConvertLog.class.getCanonicalName()+" <tracedir> <output.xes>");
      return;
    }
    
    ConvertLog rl = new ConvertLog();
    rl.read(args[0]);
    System.out.println("Writing "+args[1]);
    rl.writeLog(args[1]);
  }

}
