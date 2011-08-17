package org.st.sam.log;

import org.deckfour.xes.extension.std.XConceptExtension;
import org.deckfour.xes.extension.std.XLifecycleExtension;
import org.deckfour.xes.factory.XFactory;
import org.deckfour.xes.model.XAttribute;
import org.deckfour.xes.model.XAttributeLiteral;
import org.deckfour.xes.model.XAttributeMap;
import org.deckfour.xes.model.XEvent;

public class SEvent {

  public String method;
  public String sender;
  public String senderID;
  public String receiver;
  public String receiverID;
  public String returnValue;
  
  public static final String FIELD_RESULT = "result";
  public static final String FIELD_METHOD = "concept:name";
  public static final String FIELD_RECEIVERID = "receiverID";
  public static final String FIELD_RECEIVER = "receiver";
  public static final String FIELD_SENDERID = "senderID";
  public static final String FIELD_SENDER = "sender";
  
  public SEvent(String m, String s, String sID, String r, String rID, String val) {
    method = m;
    sender = s;
    senderID = sID;
    receiver = r;
    receiverID = rID;
    returnValue = val;
  }
  
  public SEvent(XEvent e) {
    method = ((XAttributeLiteral)e.getAttributes().get(FIELD_METHOD)).getValue();
    sender = ((XAttributeLiteral)e.getAttributes().get(FIELD_SENDER)).getValue();
    senderID = ((XAttributeLiteral)e.getAttributes().get(FIELD_SENDERID)).getValue();
    receiver = ((XAttributeLiteral)e.getAttributes().get(FIELD_RECEIVER)).getValue();
    receiverID = ((XAttributeLiteral)e.getAttributes().get(FIELD_RECEIVERID)).getValue();
    returnValue = ((XAttributeLiteral)e.getAttributes().get(FIELD_RESULT)).getValue();
  }
  
  public XEvent toXEvent(XFactory factory) {
    XAttributeMap attributes = factory.createAttributeMap();
    XAttribute a_name = factory.createAttributeLiteral(FIELD_METHOD, method, XConceptExtension.instance()); 
    XAttribute a_complete = factory.createAttributeLiteral("lifecycle:transition", "complete",
        XLifecycleExtension.instance());

    XAttribute a_sender = factory.createAttributeLiteral(FIELD_SENDER, sender, null);
    XAttribute a_senderID = factory.createAttributeLiteral(FIELD_SENDERID, senderID, null);
        
    XAttribute a_receiver = factory.createAttributeLiteral(FIELD_RECEIVER, receiver, null);
    XAttribute a_receiverID = factory.createAttributeLiteral(FIELD_RECEIVERID, receiverID, null);
    
    XAttribute a_result = factory.createAttributeLiteral(FIELD_RESULT, returnValue, null);
    
    attributes.put(SEvent.FIELD_METHOD, a_name);
    attributes.put("lifecycle:transition", a_complete);
    attributes.put(SEvent.FIELD_SENDER, a_sender);
    attributes.put(SEvent.FIELD_SENDERID, a_senderID);
    attributes.put(SEvent.FIELD_RECEIVER, a_receiver);
    attributes.put(SEvent.FIELD_RECEIVERID, a_receiverID);
    attributes.put(SEvent.FIELD_RESULT, a_result);

    XEvent e = factory.createEvent(attributes);
    return e;
  }
  
  public String getCharacter() {
    return sender+"|"+receiver+"|"+method;
  }
}
