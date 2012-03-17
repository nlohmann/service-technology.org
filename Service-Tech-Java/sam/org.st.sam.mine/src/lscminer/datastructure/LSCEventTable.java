package lscminer.datastructure;

import java.util.LinkedList;
import java.util.Map;

import org.st.sam.log.SLog;
import org.st.sam.util.SAMOutput;

import com.google.gwt.dev.util.collect.HashMap;

/**
 * Auxiliary data structure encoding names of LSC events as shorts with the
 * corresponding translation tables.
 * 
 * @author dfahland
 *
 */
public class LSCEventTable {

  private short componentID = 0;
  private short methodID = 0;
  
  public String method_names[];
  public String component_names[];
  public Map<String, Short> name2id_method;
  public Map<String, Short> name2id_component;
  
  public LSCEventTable(String[][] event_id_to_lsc_events) {
    
    name2id_method = new HashMap<String, Short>();
    name2id_component = new HashMap<String, Short>();
    
    LinkedList<String> allMethodNamesOrdered = new LinkedList<String>();
    LinkedList<String> allComponentNamesOrdered = new LinkedList<String>();
    
    for (String[] event : event_id_to_lsc_events) {

      String methodName = event[SLog.LSC_METHOD];
      
      if (!name2id_method.keySet().contains(methodName)) {
        allMethodNamesOrdered.add(methodName);
        name2id_method.put(methodName, methodID);
        methodID++;
      }
      
      String callerName = event[SLog.LSC_CALLER];
      
      if (!name2id_component.keySet().contains(callerName)) {
        allComponentNamesOrdered.add(callerName);
        name2id_component.put(callerName, componentID);
        componentID++;
      }
      
      String calleeName = event[SLog.LSC_CALLEE];
      
      if (!name2id_component.keySet().contains(calleeName)) {
        allComponentNamesOrdered.add(calleeName);
        name2id_component.put(calleeName, componentID);
        componentID++;
      }
    }
    method_names = allMethodNamesOrdered.toArray(new String[allMethodNamesOrdered.size()]);
    component_names = allComponentNamesOrdered.toArray(new String[allComponentNamesOrdered.size()]);
  }
  
  public void shorteNames() {
    
    for (short i=0; i<method_names.length; i++) {
      method_names[i] = SAMOutput.shortenNames(method_names[i]);
      name2id_method.put(method_names[i], i);
    }
    
    for (short i=0; i<component_names.length; i++) {
      component_names[i] = SAMOutput.shortenNames(component_names[i]);
      name2id_component.put(component_names[i], i);
    }

  }
  
}
