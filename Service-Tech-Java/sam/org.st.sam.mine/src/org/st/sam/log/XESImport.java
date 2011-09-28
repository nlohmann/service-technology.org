package org.st.sam.log;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.security.InvalidParameterException;
import java.util.Collection;
import java.util.Enumeration;
import java.util.zip.GZIPInputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;

import org.deckfour.xes.extension.std.XConceptExtension;
import org.deckfour.xes.factory.XFactory;
import org.deckfour.xes.factory.XFactoryRegistry;
import org.deckfour.xes.in.XMxmlParser;
import org.deckfour.xes.in.XParser;
import org.deckfour.xes.in.XParserRegistry;
import org.deckfour.xes.in.XesXmlParser;
import org.deckfour.xes.model.XLog;

public class XESImport {

  private final XFactory factory; 
  
  public XESImport() {
    factory = XFactoryRegistry.instance().currentDefault();
  }

  protected InputStream getInputStream(File file) throws IOException {
    FileInputStream stream = new FileInputStream(file);
    if (file.getName().endsWith(".gz") || file.getName().endsWith(".xez")) {
      return new GZIPInputStream(stream);
    }
    if (file.getName().endsWith(".zip")) {
      ZipFile zip = new ZipFile(file);
      Enumeration<? extends ZipEntry> entries = zip.entries();
      ZipEntry zipEntry = entries.nextElement();
      if (entries.hasMoreElements()) {
        throw new InvalidParameterException("Zipped log files should not contain more than one entry.");
      }
      return zip.getInputStream(zipEntry);
    }
    return stream;
  }
  
  public XLog readLog(String xesFileName) throws IOException {
    File logFile = new File(xesFileName);
    return readLog(logFile);
  }

  public XLog readLog(File logFile) throws IOException {
    
    InputStream input = getInputStream(logFile);
    String filename = logFile.getName();
    
    XParser parser;
    if (filename.toLowerCase().endsWith(".xes") || filename.toLowerCase().endsWith(".xez")
        || filename.toLowerCase().endsWith(".xes.gz")) {
      parser = new XesXmlParser(factory);
    } else {
      parser = new XMxmlParser(factory);
    }
    Collection<XLog> logs = null;
    try {
      logs = parser.parse(input);
    } catch (Exception e) {
      logs = null;
    }
    if (logs == null) {
      // try any other parser
      for (XParser p : XParserRegistry.instance().getAvailable()) {
        if (p == parser) {
          continue;
        }
        try {
          logs = p.parse(input);
          if (logs.size() > 0) {
            break;
          }
        } catch (Exception e1) {
          // ignore and move on.
          logs = null;
        }
      }
    }

    // log sanity checks;
    // notify user if the log is awkward / does miss crucial information
    if (logs == null || logs.size() == 0) {
      //throw new Exception("No processes contained in log!");
      System.out.println("Warning! No processes contained in log!");
    }

    XLog log = logs.iterator().next();
    if (XConceptExtension.instance().extractName(log) == null) {
      /*
       * Log name not set. Create a default log name.
       */
      XConceptExtension.instance().assignName(log, "Anonymous log imported from " + filename);
    }

    if (log.isEmpty()) {
      //throw new Exception("No process instances contained in log!");
      System.out.println("No process instances contained in log!");
    }

    return log;
  }
}
