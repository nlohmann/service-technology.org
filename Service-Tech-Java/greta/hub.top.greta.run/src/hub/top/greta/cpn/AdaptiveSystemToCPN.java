package hub.top.greta.cpn;

import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.Node;
import hub.top.adaptiveSystem.Oclet;
import hub.top.greta.run.Activator;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URISyntaxException;
import java.net.URL;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Scanner;
import java.util.Set;

import org.cpntools.accesscpn.engine.Simulator;
import org.cpntools.accesscpn.engine.SimulatorService;
import org.cpntools.accesscpn.engine.highlevel.HighLevelSimulator;
import org.cpntools.accesscpn.engine.highlevel.checker.Checker;
import org.cpntools.accesscpn.engine.highlevel.instance.Binding;
import org.cpntools.accesscpn.engine.highlevel.instance.Instance;
import org.cpntools.accesscpn.engine.highlevel.instance.cpnvalues.CPNValue;
import org.cpntools.accesscpn.model.HLDeclaration;
import org.cpntools.accesscpn.model.declaration.VariableDeclaration;
import org.cpntools.accesscpn.model.exporter.DOMGenerator;
import org.cpntools.accesscpn.model.util.BuildCPNUtil;
import org.eclipse.core.internal.resources.Workspace;
import org.eclipse.core.internal.resources.WorkspaceRoot;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.core.runtime.Platform;
import org.osgi.framework.Bundle;

public class AdaptiveSystemToCPN {
  
  public AdaptiveSystemToCPN () throws Exception {
    build = new BuildCPNUtil();
    initialize();
  }
  
  private final BuildCPNUtil build;
  private org.cpntools.accesscpn.model.PetriNet net;
  private org.cpntools.accesscpn.model.Page eventPage;
  
  private HighLevelSimulator sim;
  
  public static boolean isHighLevelSpecification(AdaptiveSystem as) {

    for (Oclet o : as.getOclets()) {
      // check oclet headers for well-formedness: NAME(v1:TYPE1,v2:TYPE2,...)
      if (o.getName().indexOf('(') == -1) return false;
      if (o.getName().lastIndexOf(')') <= o.getName().indexOf('(')) return false;
      
      String varDeclaration = o.getName().substring(o.getName().indexOf('(')+1,o.getName().lastIndexOf(')'));
      String varDeclarations[] = varDeclaration.split(",");
      
      for (String v : varDeclarations) {
        String v2[] = v.split(":");
        if (v2.length != 2) return false;
      }
      
      // check conditions for well-formedness
      for (Node n : o.getPreNet().getNodes()) {
        if (n instanceof Condition) {
          Condition c = (Condition)n;
          try {
            getPlaceName(c.getName());
            getToken(c.getName());
          } catch (Exception ex) {
            return false;
          }
        }
      }
      for (Node n : o.getDoNet().getNodes()) {
        if (n instanceof Condition) {
          Condition c = (Condition)n;
          try {
            getPlaceName(c.getName());
            getToken(c.getName());
          } catch (Exception ex) {
            return false;
          }
        }
      }
    }
    
    for (Node n : as.getAdaptiveProcess().getNodes()) {
      if (n instanceof Condition) {
        Condition c = (Condition)n;
        try {
          getPlaceName(c.getName());
          getToken(c.getName());
        } catch (Exception ex) {
          return false;
        }
      }
    }
   
    return true;
  }
  
  public static File getFile(String fileName) {
    
    String pluginName = "hub.top.GRETA.run";
    
    File file = null;    

    try {
      URL fileURL = new URL("platform:/plugin/"+pluginName+"/"+fileName);
      file = new File(FileLocator.resolve(fileURL).toURI());
    } catch (MalformedURLException e1) {
      e1.printStackTrace();
    } catch (URISyntaxException e1) {
      e1.printStackTrace();
    } catch (IOException e1) {
      e1.printStackTrace();
    }
    return file;
  }
  
  public void initialize() throws Exception {
    
    File file = getFile("./resource/sim_hl/basic.cpn");
    FileInputStream in = new FileInputStream(file);
    try {
      net = (org.cpntools.accesscpn.model.impl.PetriNetImpl)org.cpntools.accesscpn.model.importer.DOMParser.parse(in, "test");
      eventPage = build.addPage(net, "events");
    } catch (Exception e) {
      System.err.println("Could not parse model in "+file.getAbsolutePath());
      e.printStackTrace();
    }
    
    SimulatorService ss = SimulatorService.getInstance();
    Simulator s = ss.getNewSimulator();
    sim = HighLevelSimulator.getHighLevelSimulator(s);
  }
  
  public void destroy() throws Exception {
    sim.destroy();
  }
  
  private Map<String, Set<String>> initialMarking;
  
  public static String getPlaceName(String name) {
    String placeName = name.substring(
        name.indexOf('(')+1,
        name.indexOf(','));
    return placeName;
  }
  
  public static String getToken(String name) {
    
    String token = name.substring(
        name.indexOf(',')+1,
        name.lastIndexOf(')'));
    return token;
  }
  
  public void loadFunctionDefinitions(AdaptiveSystem as) {
    org.eclipse.emf.common.util.URI ml = as.eResource().getURI().trimFileExtension().appendFileExtension("ml.txt");
    IPath ws_path = new Path(ml.toPlatformString(true));
    IFile ws_file = ResourcesPlugin.getWorkspace().getRoot().getFile(ws_path);
    
    try {
      File sys_file = new File(ws_file.getLocationURI());
      String def = readFromFile(sys_file);
      
      build.declareMLFunction(net, def);
      
    } catch (Exception e) {
      // TODO Auto-generated catch block
      e.printStackTrace();
    }
  }
  
  /**
   * Read contents of a text file.
   * 
   * @param fFileName
   * @return
   * @throws FileNotFoundException
   */
  private String readFromFile(final File fFile)
      throws FileNotFoundException {
    StringBuilder text = new StringBuilder();
    String NL = System.getProperty("line.separator");
    Scanner scanner = new Scanner(new FileInputStream(fFile));
    try {
      while (scanner.hasNextLine()) {
        text.append(scanner.nextLine() + NL);
      }
    } finally {
      scanner.close();
    }
    return text.toString();
  }

  
  public void convertInitialRunToMarking(AdaptiveSystem as) {
    
    initialMarking = new HashMap<String, Set<String>>();
    
    for (Condition c : as.getAdaptiveProcess().getMarkedConditions()) {
      
      String placeName = getPlaceName(c.getName());
      String initMarking = getToken(c.getName());
      
      if (!initialMarking.containsKey(placeName)) initialMarking.put(placeName, new HashSet<String>());
      initialMarking.get(placeName).add(initMarking);
    }
  }
  
  public String getMarkingString(String placeName) {
    if (!initialMarking.containsKey(placeName)) return "";
    String mark = null;
    for (String value : initialMarking.get(placeName)) {
      if (mark == null) mark = "1`"+value;
      else {
        mark += " ++ 1`"+value;
      }
    }
      
    return mark;
  }
  
  public void convertEventsToTransitions(AdaptiveSystem as) {
    
    List<Event> events = new LinkedList<Event>();
    for (Oclet o : as.getOclets()) {
      
      String varDeclaration = o.getName().substring(o.getName().indexOf('(')+1,o.getName().indexOf(')'));
      String varDeclarations[] = varDeclaration.split(",");
      
      for (String v : varDeclarations) {
        String v2[] = v.split(":");
        
        boolean alreadyDeclared = false;
        
        for (HLDeclaration decl : net.declaration()) {
          if (decl.getStructure() != null
              && decl.getStructure() instanceof VariableDeclaration)
          {
            for (String varNames : ((VariableDeclaration)decl.getStructure()).getVariables()) {
              if (varNames.equals(v2[0])) alreadyDeclared = true;
            }
          }
        }
        
        if (!alreadyDeclared)
          build.declareVariable(net, v2[0], v2[1]);

      }
      
      for (Node n : o.getDoNet().getNodes()) {
        if (n instanceof Event)
          
          events.add((Event)n);
      }
    }
    convertEventsToTransitions(events);
  }
  
  /*
  public void setMarkingFromRun(AdaptiveSystem as) {
    convertInitialRunToMarking(as);
    
    try {

      for (Instance<org.cpntools.accesscpn.model.PlaceNode> pi : sim.getAllPlaceInstances()) {
        sim.setMarking(pi, getMarkingString(pi.getNode().getName().getText()));
      }
      
      System.out.println(sim.getMarking().toString());
    } catch (Exception e) {
      // TODO Auto-generated catch block
      e.printStackTrace();
    }
  }*/
  
  public void execute(Binding bind) {
    try {
      sim.execute(bind);
    } catch (Exception e) {
      e.printStackTrace();
    }
  }
  
  public void convertEventsToTransitions(Collection<Event> events) {
    
    Map<String, org.cpntools.accesscpn.model.Place> places = new HashMap<String, org.cpntools.accesscpn.model.Place>();
    
    for (Event e : events) {
      
      String ocletName = ((Oclet)e.eContainer().eContainer()).getName();
      ocletName = ocletName.substring(0, ocletName.indexOf('('));
      
      org.cpntools.accesscpn.model.Transition t = build.addTransition(eventPage, e.getName());
      
      for (Condition c : e.getPreConditions()) {
        
        String type = "INT";
        String placeName = getPlaceName(c.getName());
        String arcExpression = getToken(c.getName());
        
        if (!places.containsKey(placeName)) {
          String marking = getMarkingString(placeName);
          org.cpntools.accesscpn.model.Place p = build.addPlace(eventPage, placeName, type, marking);
          places.put(placeName, p);
        }
        
        build.addArc(eventPage, places.get(placeName), t, arcExpression);
      }
      
      for (Condition c : e.getPostConditions()) {
        
        String type = "INT";
        String placeName = getPlaceName(c.getName());
        String arcExpression = getToken(c.getName());
        
        if (!places.containsKey(placeName)) {
          String marking = getMarkingString(placeName);
          org.cpntools.accesscpn.model.Place p = build.addPlace(eventPage, placeName, type, marking);
          places.put(placeName, p);
        }
        
        build.addArc(eventPage, t, places.get(placeName), arcExpression);
      }
    }
  }

  public void exportNet() {
    try {
      System.out.println("writing");
      DOMGenerator.export(net, "D://out.cpn");
      System.out.println("done");
    } catch (Exception e) {
      System.err.println("could not write debug file");
      e.printStackTrace();
    }
  }

  public void check() {
    
    try {
      Checker c = new Checker(net, null, sim);
      c.checkEntireModel();
    } catch (Exception e) {
      e.printStackTrace();
    }
    
    sim.setTarget((org.cpntools.accesscpn.model.impl.PetriNetImpl)net);
  }
  
  public Map<String, String> getResultingTokens(Binding b) {
    Map<String, String> tokens = new HashMap<String, String>();
    
    org.cpntools.accesscpn.model.Transition t = b.getTransitionInstance().getNode();
    for (org.cpntools.accesscpn.model.Arc a : t.getSourceArc()) {
      org.cpntools.accesscpn.model.PlaceNode p = a.getPlaceNode();
      
      List<CPNValue> val = sim.evaluate(a, b);
      if (val.size() == 1) tokens.put(p.getName().getText(), val.get(0).toString());
      else tokens.put(p.getName().getText(), val.toString());
    }
    
    return tokens;
  }
  
  public Map<String, String> getSourceTokens(Binding b) {
    Map<String, String> tokens = new HashMap<String, String>();
    
    org.cpntools.accesscpn.model.Transition t = b.getTransitionInstance().getNode();
    for (org.cpntools.accesscpn.model.Arc a : t.getTargetArc()) {
      org.cpntools.accesscpn.model.PlaceNode p = a.getPlaceNode();
      
      List<CPNValue> val = sim.evaluate(a, b);
      if (val.size() == 1) tokens.put(p.getName().getText(), val.get(0).toString());
      else tokens.put(p.getName().getText(), val.toString());
    }
    return tokens;
  }
  
  public List<Binding> enabledTransitions() {
    
    List<Binding> bindings = new LinkedList<Binding>();
    
    try {
      
      System.out.println(sim.getMarking().toString());
      
      List<Instance<org.cpntools.accesscpn.model.Transition>> tis = sim.getAllTransitionInstances();
      for (Instance<org.cpntools.accesscpn.model.Transition> ti : tis) {
        if (sim.isEnabled(ti))
          bindings.addAll(sim.getBindings(ti));
      }
    } catch (Exception e) {
      e.printStackTrace();
    }
    
    System.out.println(bindings);
    
    return bindings;
  }
}
