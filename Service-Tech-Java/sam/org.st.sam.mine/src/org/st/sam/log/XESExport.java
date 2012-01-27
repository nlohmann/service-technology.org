package org.st.sam.log;

import java.io.FileOutputStream;
import java.io.IOException;

import org.deckfour.xes.model.XLog;
import org.deckfour.xes.out.XMxmlGZIPSerializer;
import org.deckfour.xes.out.XMxmlSerializer;
import org.deckfour.xes.out.XSerializer;
import org.deckfour.xes.out.XesXmlGZIPSerializer;
import org.deckfour.xes.out.XesXmlSerializer;

public class XESExport {

  public static void writeLog(XLog log, String fileName) throws IOException {
    FileOutputStream out = new FileOutputStream(fileName);
    XSerializer logSerializer;
    if (fileName.endsWith("gz"))
      logSerializer = new XesXmlGZIPSerializer();
    else if (fileName.endsWith("xml.zip"))
      logSerializer = new XMxmlGZIPSerializer();
    else
      logSerializer = new XesXmlSerializer();
    System.out.println("Writing "+fileName);
    logSerializer.serialize(log, out);
    out.close();
  }
}
