package org.st.sam.util;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

import lscminer.datastructure.LSC;
import lscminer.datastructure.LSCEvent;

public class SAMOutput {
  
  public static void shortenLSCnames(LSC l) {
    shortenLSCnames(l.getPreChart());
    shortenLSCnames(l.getMainChart());
  }
  
  public static void shortenLSCnames(LSCEvent[] chart) {
    for (int i=0; i < chart.length; i++) {
      
      String caller = shortenNames(chart[i].getCaller());
      String callee = shortenNames(chart[i].getCallee());
      String method = shortenNames(chart[i].getMethod());
    
      chart[i] = new LSCEvent(caller, callee, method);
    }
  }
  
  public static String shortenNames(String lscString) {
  
    if (lscString.indexOf('(') >= 0) {
      int lastDotBeforePar = lscString.lastIndexOf('.', lscString.indexOf('('));
      lscString = lscString.substring(lastDotBeforePar+1);
      
      if (lscString.length() > 40) {
        lscString = lscString.substring(0, lscString.indexOf('('))+"(...)";
      }
    } else {
      int lastDot = lscString.lastIndexOf('.');
      lscString = lscString.substring(lastDot+1);
    }
    
    lscString = lscString.replace('$', '_');
    return lscString;
  }
  
  public static List<String> getComponents(LSC l) {
    List<String> components = new LinkedList<String>();
    
    for (LSCEvent e  : l.getPreChart()) {
      if (!components.contains(e.getCaller())) components.add(e.getCaller());
      if (!components.contains(e.getCallee())) components.add(e.getCallee());
    }
    for (LSCEvent e  : l.getMainChart()) {
      if (!components.contains(e.getCaller())) components.add(e.getCaller());
      if (!components.contains(e.getCallee())) components.add(e.getCallee());
    }
    return components;
  }
  
  public static int getMSCWitdth(List<String> components) {
    return components.size()*128;
  }
  
  public static String toMSCRenderer(String name, LSC l) {
    StringBuilder sb = new StringBuilder();
    
    sb.append("# LSC\n");
    sb.append("msc {\n");
    
    List<String> components = getComponents(l);
    
    sb.append("  width=\""+getMSCWitdth(components)+"\";\n");

    // list components:
    //  a,b,c;
    sb.append("  ");
    Iterator<String> comp = components.iterator();
    while (comp.hasNext()) {
      sb.append(comp.next());
      if (comp.hasNext())
        sb.append(",");
      else
        sb.append(";\n");
    }
    
    // list pre-chart events
    for (LSCEvent e  : l.getPreChart()) {
      sb.append("  "+e.getCaller()+"=>"+e.getCallee()+" [ label = \""+e.getMethod()+"\", linecolor = \"blue\" ];\n");
    }
    
    // separate pre- and main-chart
    sb.append("  ---;\n");
    
    // list main-chart events
    for (LSCEvent e  : l.getMainChart()) {
      sb.append("  "+e.getCaller()+"=>"+e.getCallee()+" [ label = \""+e.getMethod()+"\", linecolor = \"red\" ];\n");
    }

    // add name of chart
    sb.append("  "+components.get(0)+" note "+components.get(0)+" [label=\""+name+"\"];\n");
    
    sb.append("}\n");

    return sb.toString();
  }
  
  public static void writeToFile(String text, String fileName) throws IOException {
    
    System.out.print("Writing "+fileName+"... ");

    // Create file 
    FileWriter fstream = new FileWriter(fileName);
    BufferedWriter out = new BufferedWriter(fstream);
    out.write(text);

    //Close the output stream
    out.close();
    
    System.out.println(" done.");
  }
  
  public static void systemCall(String command) {
    Runtime r = Runtime.getRuntime();

    try {
      /*
       * Here we are executing the UNIX command ls for directory listing. The
       * format returned is the long format which includes file information and
       * permissions.
       */
      //System.out.println("calling "+command);
      Process p = r.exec(command);
      InputStream in = p.getInputStream();
      BufferedInputStream buf = new BufferedInputStream(in);
      InputStreamReader inread = new InputStreamReader(buf);
      BufferedReader bufferedreader = new BufferedReader(inread);

      // Read the ls output
      String line;
      while ((line = bufferedreader.readLine()) != null) {
        System.out.println(line);
      }
      // Check for ls failure
      try {
        if (p.waitFor() != 0) {
          System.err.println("exit value = " + p.exitValue());
        }
      } catch (InterruptedException e) {
        System.err.println(e);
      } finally {
        // Close the InputStream
        bufferedreader.close();
        inread.close();
        buf.close();
        in.close();
      }
    } catch (IOException e) {
      System.err.println(e.getMessage());
    }
  }

}
