package hub.top.uma.view;

import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;

import hub.top.petrinet.Arc;
import hub.top.petrinet.Node;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;

public class ViewGeneration3 {

  private PetriNet net;
  public HashMap<Node, Integer> nodeCov;
  public HashMap<Arc, Integer> arcCov; 
  
  public ViewGeneration3 (PetriNet net) {
    this.net = net;
    nodeCov = new HashMap<Node, Integer>();
    arcCov = new HashMap<Arc, Integer>();
    
    net.turnIntoLabeledNet();
    net.makeNormalNet();
    
    /*
    for (Transition t : net.getTransitions()) {
      if (t.getPreSet().isEmpty()) {
        Place p = net.addPlace(t.getName()+"_loop");
        p.setTokens(1);
        net.addArc(p, t);
        net.addArc(t, p);
      }
    }*/

  }
  
  public void markTrace(String[] trace) {
    
    LinkedList<Place> marking = new LinkedList<Place>();
    
    for (Place p : net.getPlaces()) {
      if (p.getTokens() > 0) {
        nodeCov.put(p, 1);
        marking.add(p);
      }
    }
    
    for (int i=0; i<trace.length; i++) {
      Transition t = net.findTransition(trace[i]);
      
      for (Arc a : t.getIncoming()) {
        Place p = (Place)a.getSource();
        if (marking.contains(p)) {
          marking.remove(p);
          if (!arcCov.containsKey(a)) {
            arcCov.put(a, 1);
          } else {
            arcCov.put(a, arcCov.get(a)+1);
          }
          // consumption counts extra
          if (!nodeCov.containsKey(p)) {
            nodeCov.put(p, 1);
          } else {
            nodeCov.put(p, nodeCov.get(p)+1);
          }
        }
      }
      
      if (!nodeCov.containsKey(t)) {
        nodeCov.put(t, 1);
      } else {
        nodeCov.put(t, nodeCov.get(t)+1);
      }
      
      for (Arc a : t.getOutgoing()) {
        Place p = (Place)a.getTarget();
        marking.add(p);
        if (!arcCov.containsKey(a)) {
          arcCov.put(a, 1);
        } else {
          arcCov.put(a, arcCov.get(a)+1);
        }
        if (!nodeCov.containsKey(p)) {
          nodeCov.put(p, 1);
        } else {
          nodeCov.put(p, nodeCov.get(p)+1);
        }
      }
    }
  }
  
  public LinkedList<Arc> getNonRequiredArcs() {
    LinkedList<Arc> toRemove = new LinkedList<Arc>();
    for (Transition t : net.getTransitions()) {
      for (Arc a : t.getIncoming()) {
        if (!arcCov.containsKey(a)) {
          toRemove.add(a);
        }
        if (arcCov.get(a) < nodeCov.get(t)) {
          toRemove.add(a);
        }
      }
      for (Arc a : t.getOutgoing()) {
        if (!arcCov.containsKey(a)) {
          toRemove.add(a);
        }
        if (arcCov.get(a) < nodeCov.get(t)) {
          toRemove.add(a);
        }
      }
    }
    return toRemove;
  }
  
  private LinkedList<Transition> getEnabledTransitions(LinkedList<Place> marking, String name) {
    LinkedList<Transition> ts = new LinkedList<Transition>();
    for (Transition t : net.getTransitions()) {
      if (t.getName().startsWith(name)) {
        
        boolean preSetMarked = true;
        for (Place p : t.getPreSet()) {
          if (!marking.contains(p)) {
            preSetMarked = false;
            break;
          }
        }
        
        if (preSetMarked)
          ts.add(t);
      }
    }

    return ts;
  }
  
  public boolean validateTrace(String[] trace) {
    
    LinkedList<Place> marking = new LinkedList<Place>();
    
    int newLength = trace.length;
    //if (!trace[0].equals("start")) newLength++;
    //if (!trace[trace.length-1].equals("end")) newLength++;
    
    String[] extTrace = new String[newLength];
    int offSet = 0;
    //if (!trace[0].equals("start")) {
    //  extTrace[0] = "start";
    //  offSet++;
    //}
    for (int i=0; i<trace.length; i++)
      extTrace[i+offSet] = trace[i];
    //if (!trace[trace.length-1].equals("end")) {
    //  extTrace[extTrace.length-1] = "end";
    //}
    
    for (Place p : net.getPlaces()) {
      if (p.getTokens() > 0) {
        nodeCov.put(p, 1);
        marking.add(p);
      }
    }

    return validateTrace(extTrace, 0, marking);
  }
  
  public boolean validateTrace(String[] trace, int i, LinkedList<Place> marking) {
    
    if (i == trace.length) return true;
    
    LinkedList<Transition> ts = getEnabledTransitions(marking, trace[i]);
    ts.addAll(getEnabledTransitions(marking, "tau"));
    ts.addAll(getEnabledTransitions(marking, "SILENT"));
    
    for (int j=0;j<i;j++) System.out.print(" ");
    System.out.println(trace[i]+" -> "+ts);
    
    if (ts.isEmpty()) {

      /*
      System.out.print("executed: ");
      for (int j=0;j<i;j++)
        System.out.print(trace[j]+", ");
      System.out.println();
      System.out.println("reached marking: "+marking);
      System.out.println("  "+trace[i]+" is not enabled");
      */
      
      /*
      for (Transition t : net.getTransitions()) {
        if (t.getName().equals(trace[i])) {
          LinkedList<Place> missing = new LinkedList<Place>();
          for (Place p : t.getPreSet()) {
            if (!marking.contains(p)) missing.add(p);
          }
          System.out.println(t+" requires additional tokens on "+missing);
        }
      }*/
      
      return false;
    }
    
    //for (int j=0;j<i;j++) System.out.print(".");
    //System.out.println(ts.size());

    for (Transition t : ts) {
      
      LinkedList<Place> succ_marking = new LinkedList<Place>(marking);
    
      for (Place p : t.getPreSet()) {
        succ_marking.remove(p);
      }
      
      for (Place p : t.getPostSet()) {
        succ_marking.add(p);
      }
      
      int succ_i = (t.getName().equals("tau")) ? i : i+1; 
      
      if (validateTrace(trace, succ_i, succ_marking)) return true;
    }
    
    return false;
  }

  
  public void reduceNet() {
    
    for (Arc a : getNonRequiredArcs()) {
      if (a.getTarget() instanceof Transition
          && a.getTarget().getPreSet().size() == 1) continue;
      
      net.removeArc(a);
    }
  }
  
  public static void main(String[] args) throws Exception {
    
    //String fileName_system_sysPath = "./examples/model_correction/a12f0n05_alpha.lola_refold.lola";
    //String fileName_trace  = "./examples/model_correction/a12f0n05_aligned.log.txt";
    
    //String fileName_system_sysPath = "./examples/model_correction/a22f0n00.lola_refold.lola";
    //String fileName_trace  = "./examples/model_correction/a22f0n05.log_100.txt";
    
    //String fileName_system_sysPath = "./examples/model_correction2/wabo1.lola";
    //String fileName_trace  = "./examples/model_correction2/wabo1.log.txt";
    
    //String fileName_system_sysPath = "./examples/model_correction2/a22f0n00.lola_refold.lola";
    //String fileName_trace  = "./examples/model_correction2/a22f0n00.log_20.txt";
    
    String fileName_system_sysPath = "D:/LinuxShared/Logs/CoSeLoG_WABO/repaired_1.lola";
    String fileName_trace  =  "D:/LinuxShared/Logs/CoSeLoG_WABO/repaired_1.log.txt";
    

    PetriNet sysModel = PetriNetIO.readNetFromFile(fileName_system_sysPath);
    List<String[]> allTraces = ViewGeneration2.readTraces(fileName_trace);
    
    sysModel.makeNormalNet();
    sysModel.turnIntoLabeledNet();
    
    ViewGeneration3 viewGen = new ViewGeneration3(sysModel);
    int numTrace = 0;
    int failedTrace = 0;
    for (String[] trace : allTraces) {
      numTrace++;
      if (!viewGen.validateTrace(trace)) {
        failedTrace++;
        System.out.println("failed "+numTrace);
      } else {
        System.out.println("replayed "+numTrace);
      }
    }
    System.out.println("failed on "+failedTrace+"/"+numTrace);
  }
}
