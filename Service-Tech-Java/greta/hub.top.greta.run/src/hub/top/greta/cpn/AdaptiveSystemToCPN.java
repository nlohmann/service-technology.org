package hub.top.greta.cpn;

import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.Node;
import hub.top.adaptiveSystem.Oclet;

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

import org.cpntools.accesscpn.engine.Packet;
import org.cpntools.accesscpn.engine.Simulator;
import org.cpntools.accesscpn.engine.SimulatorService;
import org.cpntools.accesscpn.engine.highlevel.DeclarationCheckerException;
import org.cpntools.accesscpn.engine.highlevel.HighLevelSimulator;
import org.cpntools.accesscpn.engine.highlevel.PacketGenerator;
import org.cpntools.accesscpn.engine.highlevel.SyntaxCheckerException;
import org.cpntools.accesscpn.engine.highlevel.SyntaxFatalErrorException;
import org.cpntools.accesscpn.engine.highlevel.checker.Checker;
import org.cpntools.accesscpn.engine.highlevel.instance.Binding;
import org.cpntools.accesscpn.engine.highlevel.instance.Instance;
import org.cpntools.accesscpn.engine.highlevel.instance.cpnvalues.CPNValue;
import org.cpntools.accesscpn.model.HLDeclaration;
import org.cpntools.accesscpn.model.Page;
import org.cpntools.accesscpn.model.declaration.VariableDeclaration;
import org.cpntools.accesscpn.model.exporter.DOMGenerator;
import org.cpntools.accesscpn.model.util.BuildCPNUtil;
import org.eclipse.core.internal.localstore.IsSynchronizedVisitor;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.ResourcesPlugin;
import org.eclipse.core.runtime.FileLocator;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.xmi.impl.StringSegment;

public class AdaptiveSystemToCPN {
  
  public AdaptiveSystemToCPN (AdaptiveSystem as) throws Exception {
    build = new BuildCPNUtil();
    initialize(as);
  }
  
  private final BuildCPNUtil build;
  private org.cpntools.accesscpn.model.PetriNet net;
  private org.cpntools.accesscpn.model.Page eventPage;
  
  private HighLevelSimulator sim;
  
  public static boolean isHighLevelSpecification(AdaptiveSystem as) {

    for (Oclet o : as.getOclets()) {
      // check oclet headers for well-formedness: NAME(v1:TYPE1,v2:TYPE2,...)
      if (o.getName().indexOf('(') < 0) {
        System.err.println("No HL-oclet specification: "+o.getName()+" has invalid parameters");
        return false;
      }
      if (o.getName().lastIndexOf(')') <= o.getName().indexOf('(')) {
        System.err.println("No HL-oclet specification: "+o.getName()+" has invalid parameters");
        return false;
      }
      
      String varDeclaration = o.getName().substring(o.getName().indexOf('(')+1,o.getName().lastIndexOf(')'));
      if (varDeclaration.length() > 0) {
        // check well-formedness of non-empty variable declarations
        String varDeclarations[] = varDeclaration.split(",");
        
        for (String v : varDeclarations) {
          String v2[] = v.split(":");
          if (v2.length != 2) {
            System.err.println("No HL-oclet specification: "+o.getName()+" has invalid parameter specification: "+v);
            return false;
          }
        }
      }
      
      // check conditions for well-formedness
      for (Node n : o.getPreNet().getNodes()) {
        if (n instanceof Condition) {
          Condition c = (Condition)n;
          try {
            getPlaceName(c.getName());
            getToken(c.getName());
          } catch (Exception ex) {
            System.err.println("No HL-oclet specification: "+n.getName()+" in "+o.getName()+" (precondition) has invalid format");
            ex.printStackTrace();
            return false;
          }
        }
      }
      for (Node n : o.getDoNet().getNodes()) {
        if (n instanceof Condition && !n.isAbstract()) {
          Condition c = (Condition)n;
          try {
            getPlaceName(c.getName());
            getToken(c.getName());
          } catch (Exception ex) {
            System.err.println("No HL-oclet specification: "+n.getName()+" in "+o.getName()+" (contribution) has invalid format");
            ex.printStackTrace();
            return false;
          }
        }
      }
    }
    
    for (Node n : as.getAdaptiveProcess().getNodes()) {
      if (n instanceof Condition && !n.isAbstract()) {
        Condition c = (Condition)n;
        try {
          getPlaceName(c.getName());
          getToken(c.getName());
        } catch (Exception ex) {
          System.err.println("No HL-oclet specification: "+n.getName()+" in initial run has invalid format");
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
  
  public void initialize(AdaptiveSystem as) throws Exception {
    
    org.eclipse.emf.common.util.URI ml = as.eResource().getURI()
                  .trimFileExtension().appendFileExtension("cpn");
    IPath ws_path = new Path(ml.toPlatformString(true));
    IFile ws_file = ResourcesPlugin.getWorkspace().getRoot().getFile(ws_path);
    
    File file = new File(ws_file.getLocationURI());//getFile("./resource/sim_hl/basic.cpn");
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
  
  public static String getTransitionName(String name) {
    if (name.indexOf('[') == -1) return name;
    String transititionName = name.substring(0, name.indexOf('['));
    return transititionName;
  }
  
  public static String getTransitionGuard(String name) {
    if (name.indexOf('[') == -1) return "";
    String transititionGuard= name.substring(
        name.indexOf('['),
        name.lastIndexOf(']')+1);
    return transititionGuard;
  }
  
  public void loadPlaceTypes(AdaptiveSystem as) {
    org.eclipse.emf.common.util.URI ml = as.eResource().getURI().trimFileExtension().appendFileExtension("types.txt");
    IPath ws_path = new Path(ml.toPlatformString(true));
    IFile ws_file = ResourcesPlugin.getWorkspace().getRoot().getFile(ws_path);
    
    placeTypes = new HashMap<String, String>();
    
    try {
      File sys_file = new File(ws_file.getLocationURI());
      List<String> types = readLinesFromFile(sys_file);
      
      for (String type : types) {
        try {
          String v2[] = type.split(":");
          placeTypes.put(v2[0],v2[1]);
          System.out.println("[Greta] setting type "+v2[0]+":"+v2[1]);
        } catch (Exception e) {
          System.err.println("Failed to read type declaration: "+type);
        }
      }
      
    } catch (FileNotFoundException e) {
      System.out.println("[Greta] Warning, did not find place-type specification file "+ws_file); 
    } catch (Exception e) {
      // TODO Auto-generated catch block
      e.printStackTrace();
    }
  }

  /*
  public void loadFunctionDefinitions(AdaptiveSystem as) {
    org.eclipse.emf.common.util.URI ml = as.eResource().getURI().trimFileExtension().appendFileExtension("ml.txt");
    IPath ws_path = new Path(ml.toPlatformString(true));
    IFile ws_file = ResourcesPlugin.getWorkspace().getRoot().getFile(ws_path);
    
    try {
      File sys_file = new File(ws_file.getLocationURI());
      String def = readFromFile(sys_file);
      
      if (def.length() > 0) {
        build.declareMLFunction(net, def);
      }
      
    } catch (Exception e) {
      // TODO Auto-generated catch block
      e.printStackTrace();
    }
  }*/
  
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
  
  /**
   * Read contents of a text file.
   * 
   * @param fFileName
   * @return
   * @throws FileNotFoundException
   */
  private List<String> readLinesFromFile(final File fFile)
      throws FileNotFoundException {
    List<String> lines = new LinkedList<String>();

    Scanner scanner = new Scanner(new FileInputStream(fFile));
    try {
      while (scanner.hasNextLine()) {
        lines.add(scanner.nextLine());
      }
    } finally {
      scanner.close();
    }
    return lines;
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
  
  private Map<String,String> placeTypes;
  public Map<Event, org.cpntools.accesscpn.model.Transition> eventToTransition;
  public Map<Condition, org.cpntools.accesscpn.model.Place> conditionToPlace;
  
  /**
   * Check if the give variable is already declared. If yes, return the
   * type of the declaration. If not, return {@code null}.
   * @param varName
   * @return type of an existing declaration or {@code null} if not declared
   */
  private String isVariableDeclared(String varName) {
    for (HLDeclaration decl : net.declaration()) {
      if (decl.getStructure() != null
          && decl.getStructure() instanceof VariableDeclaration)
      {
        for (String varNames : ((VariableDeclaration)decl.getStructure()).getVariables()) {
          if (varNames.equals(varName)) {
            return ((VariableDeclaration)decl.getStructure()).getTypeName();
          }
        }
      }
    }
    return null;
  }
  
  public void declareVariables(AdaptiveSystem as) {
    for (Oclet o : as.getOclets()) {
      
      String varDeclaration = o.getName().substring(o.getName().indexOf('(')+1,o.getName().indexOf(')'));
      if (varDeclaration.length() > 0) {
        String varDeclarations[] = varDeclaration.split(",");
        
        for (String v : varDeclarations) {
          String v2[] = v.split(":");
          
          if (isVariableDeclared(v2[0]) == null) {
            build.declareVariable(net, v2[0], v2[1]);
          }
        }
      }
      
    }
  }
  
  public void convertEventsToTransitions(AdaptiveSystem as) {
    
    List<Event> events = new LinkedList<Event>();
    eventToTransition = new HashMap<Event, org.cpntools.accesscpn.model.Transition>();
    conditionToPlace = new HashMap<Condition, org.cpntools.accesscpn.model.Place>();
    
    for (Oclet o : as.getOclets()) {
      for (Node n : o.getDoNet().getNodes()) {
        if (n instanceof Event && !n.isAbstract())
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
  
  public static boolean isConstant(String s) {
    // check for string constants
    if (s.indexOf(0) == '\"' && s.indexOf(s.length()-1) == '\"') return true;
    if (s.indexOf(0) == '\'' && s.indexOf(s.length()-1) == '\'') return true;
    // check for number constants
    boolean allDigits = true;
    for (int i=0;i<s.length();i++) allDigits &= Character.isDigit(s.charAt(i));
    if (allDigits) return true;
    
    return false;
  }
  
  public static String convertToVariableString(String s) {
    
    if (isConstant(s)) return s;
    
    s = s.replace("*", "_MULT_");
    s = s.replace("+", "_PLUS_");
    s = s.replace("-", "_MINUS_");
    s = s.replace("/", "_SLASH_");
    //s = s.replace('(', '$');
    //s = s.replace(')', '$');
    s = s.replace(" ", "_SPACE_");
    
    // strip leading underscores (CPN Tools doesn't like that)
    while (s.charAt(0) == '_' && s.length() > 0) s = s.substring(1);
    
    s = "V_"+s+"_VV";
    
    return s;
  }
  
  
  private static int findNextOccurrenceOfIdentifier(String exp, String subExp, int start) {
    int first = exp.indexOf(subExp, start);
    
    if (first >= 0) {
      // subExp does not occur at i if its neighboring characters are part of an identifier
      if (first > 0) {
        char c = exp.charAt(first-1); 
        if (Character.isJavaIdentifierPart(c)) return findNextOccurrenceOfIdentifier(exp, subExp, first+1);
      }
      int last = first+subExp.length();
      if (last >= 0 && last < exp.length()) {
        char c = exp.charAt(last); 
        if (Character.isJavaIdentifierPart(c)) return findNextOccurrenceOfIdentifier(exp, subExp, first+1);
      }
    }
    return first;
  }

  private static String replaceAll(String str, String pattern, String replace) {
    int s = 0;
    int e = 0;
    StringBuffer result = new StringBuffer();

    //while ((e = str.indexOf(pattern, s)) >= 0) {
    while ((e = findNextOccurrenceOfIdentifier(str, pattern, s)) >= 0) {
        result.append(str.substring(s, e));
        result.append(replace);
        s = e+pattern.length();
    }
    result.append(str.substring(s));
    return result.toString();
  }
  
  private boolean isVariable(String exp) {
    for (HLDeclaration decl : net.declaration()) {
      if (decl.getStructure() instanceof VariableDeclaration) {
        if (((VariableDeclaration)decl.getStructure()).getVariables().get(0).equals(exp)) return true;
      }
    }
    return false;
  }
  
  public void convertEventsToTransitions(Collection<Event> events) {
    
    Map<String, org.cpntools.accesscpn.model.Place> places = new HashMap<String, org.cpntools.accesscpn.model.Place>();
    
    for (Event e : events) {
      
      String ocletName = ((Oclet)e.eContainer().eContainer()).getName();
      ocletName = ocletName.substring(0, ocletName.indexOf('('));
      
      // append a number to transition names to distinguish transitions with the same label in different scenarios
      String transitionName = getTransitionName(e.getName())+"_"+eventToTransition.size();
      String transitionGuard = getTransitionGuard(e.getName());
      org.cpntools.accesscpn.model.Transition t = build.addTransition(eventPage, transitionName, transitionGuard);
      eventToTransition.put(e, t);
      
      // collect everything that is required by this event
      Set<String> requiredExpressions = new HashSet<String>();
      Set<String> providedExpressions = new HashSet<String>();
      for (Condition c : e.getPostConditions()) {
        if (c.isAbstract()) continue;
        if (dependsOn.containsKey(c)) requiredExpressions.addAll(dependsOn.get(c));
      }
      
      Map<String, String> expressionVariable = new HashMap<String, String>();
      
      for (Condition c : e.getPreConditions()) {
        
        if (c.isAbstract()) continue; // do not translate abstract conditions
        
        String type = "INT";
        String placeName = getPlaceName(c.getName());
        String arcExpression = getToken(c.getName());
        
        if (placeTypes.containsKey(placeName)) type = placeTypes.get(placeName);

        if (provides.containsKey(c)) providedExpressions.addAll(provides.get(c));
        
        boolean turnExpressionToVariable = false;

        /* DISABLED: replacing arc expressions by variables
        if (!isConstant(arcExpression) && !isVariable(arcExpression)) {
          turnExpressionToVariable = true;
        }
        */
                
        if (turnExpressionToVariable) {
          
          // for incoming arcs, we can only take simple variables to find the binding, replace
          // complex expressions by variable names, remember the replacement for the post-set
          expressionVariable.put(arcExpression, convertToVariableString(arcExpression));
          
          // all tokens on that place have the same type: variable has this type, declare variable
          // if this has not been done yet and check type consistency
          if (isVariableDeclared(expressionVariable.get(arcExpression)) == null)
              build.declareVariable(net, expressionVariable.get(arcExpression), type);
          else if (!isVariableDeclared(expressionVariable.get(arcExpression)).equals(type)) {
            System.err.println("Arc expression "+arcExpression+" from "+c.getName()+" to "+e.getName()+" has the wrong type ("+isVariableDeclared(expressionVariable.get(arcExpression))+"), expected: "+type);
          }
        }
        
        if (!places.containsKey(placeName)) {
          String marking = getMarkingString(placeName);
          org.cpntools.accesscpn.model.Place p = build.addPlace(eventPage, placeName, type, marking);
          places.put(placeName, p);
        }
        conditionToPlace.put(c, places.get(placeName));
        
        if (turnExpressionToVariable) {
          build.addArc(eventPage, places.get(placeName), t, expressionVariable.get(arcExpression));
        } else {
          build.addArc(eventPage, places.get(placeName), t, arcExpression);
        }
      }
      
      // add extra places for all required but missing expressions 
      for (String arcExpression : requiredExpressions) {
        if (!(providedExpressions.contains(arcExpression))) {
          
          // find a place providing the required expression
          String type = "INT";
          String placeName = null;
          for (Node n2 : e.getAllPredecessors()) {
            if (n2 instanceof Event || n2.isAbstract()) continue;
            Condition c2 = (Condition)n2;
            
            if (provides.containsKey(c2) && provides.get(c2).contains(arcExpression)) {
              placeName = getPlaceName(c2.getName());
            }
          }
          if (placeName == null) continue; // ERROR: could not find place to provide the expression 
          
          if (placeTypes.containsKey(placeName)) type = placeTypes.get(placeName);
          
          Object obj = this.getObjectForID(arcExpression);
          if (obj != null && obj instanceof HLDeclaration) {
            HLDeclaration decl = (HLDeclaration)obj;
            if (decl.getStructure() instanceof VariableDeclaration) {
              arcExpression = ((VariableDeclaration)decl.getStructure()).getVariables().get(0);
            }
          }
          else arcExpression = convertToVariableString(arcExpression);
          
          String specialPlaceName = placeName+"_"+transitionName+"_"+arcExpression;
          if (!places.containsKey(specialPlaceName)) {
            String marking = getMarkingString(placeName);
            org.cpntools.accesscpn.model.Place p = build.addPlace(eventPage, specialPlaceName, type, marking);
            places.put(specialPlaceName, p);
            
            System.out.println("creating place "+specialPlaceName+" for '"+arcExpression+"' at "+e.getName());
          }
          
          build.addArc(eventPage, places.get(specialPlaceName), t, arcExpression);
        }
      }
      
      for (Condition c : e.getPostConditions()) {
        
        if (c.isAbstract()) continue; // do not translate abstract conditions
        
        String type = "INT";
        String placeName = getPlaceName(c.getName());
        String arcExpression = getToken(c.getName());
        
        //System.out.println("old arc expression: "+arcExpression);
        
        // we replaced the expressions at incoming arcs by variable names, now replace
        // at the outgoing arcs all occurrences of the incoming-arc-expressions by the
        // corresponding variable name
        for (String subExp : expressionVariable.keySet()) {
          if (dependsOn.containsKey(c) && dependsOn.get(c).contains(subExp)) {
            // replace subexpressions by variables only if depending on it
            arcExpression = replaceAll(arcExpression, subExp, expressionVariable.get(subExp));
          }
        }
        
        if (placeTypes.containsKey(placeName)) type = placeTypes.get(placeName);
        
        if (!places.containsKey(placeName)) {
          String marking = getMarkingString(placeName);
          org.cpntools.accesscpn.model.Place p = build.addPlace(eventPage, placeName, type, marking);
          places.put(placeName, p);
        }
        conditionToPlace.put(c, places.get(placeName));
        
        build.addArc(eventPage, t, places.get(placeName), arcExpression);
        //System.out.println(t.getName()+" -> "+placeName+" : "+arcExpression);
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

  
  private String[] eval_find_use_parse_result(String result) {
    // result has the following form:
    // val it = ["id3","id5"] : string list
    
    String idList = result.substring(result.indexOf('[')+1,result.indexOf(']'));
    String ids[] = idList.split(",");
    for (int i=0; i<ids.length; i++) {
      // strip quotation marks
      if (ids[i].length() > 1) ids[i] = ids[i].substring(1,ids[i].length()-1);
    }
    return ids;
  }
  
  /**
   * @param id
   * @return the name of the variable defined at this id or {@code null} if this id points to something else but a variable declaration (see {@link #isVariableDeclaration(Object)} and {@link #getObjectForID(String)}
   */
  private String getVariableNameForID(String id) {
    
    Object obj = getObjectForID(id);
    if (isVariableDeclaration(obj)) {
      HLDeclaration decl = (HLDeclaration)obj;
      if (decl.getStructure() instanceof VariableDeclaration) {
        return ((VariableDeclaration)decl.getStructure()).getVariables().get(0);
      }
    }
    return null;
  }
  
  /**
   * @param obj
   * @return {@code true} iff the object is {@link HLDeclaration} containing a {@link VariableDeclaration}
   */
  private boolean isVariableDeclaration(Object obj) {
    return (obj != null
        && obj instanceof HLDeclaration
        && ((HLDeclaration)obj).getStructure() instanceof VariableDeclaration);
  }
  
  private Object getObjectForID(String id) {
    
    for (HLDeclaration d : net.declaration()) {
      if (d.getId().equals(id)) return d;
    }
    
    return null;
  }
  
  /**
   * 
   * @param sim_local
   * @param missingIDs
   * @param expression
   * @param definedExpressions
   * @param requiredExpressions
   * 
   * @return ID of the variable that cannot be resolved, {@code null} iff all variables could be resolved
   */
  private String findExpressionsForMissingIDs(HighLevelSimulator sim_local, Set<String> missingIDs, String expression, Map<String, Set<String>> definedExpressions, Set<String> requiredExpressions) {
    
    if (missingIDs.isEmpty()) return null;
    
    // check for each missing ID whether there is a defined expression
    // that contains this ID and is a subexpression of 'expression'  
    for (String id : missingIDs) {
      for (Map.Entry<String, Set<String>> declares : definedExpressions.entrySet()) {
        
        // subexpression occurs in the current expression
        if (declares.getValue().contains(id) && findNextOccurrenceOfIdentifier(expression, declares.getKey(), 0) >= 0) {
          // replace subexpression it with a dummy
          String replaced = replaceAll(expression, declares.getKey(), "X");
          // re-evaluate all missing ids in the resulting string
          HashSet<String> missingIDs_r = new HashSet<String>();
          String remainingIDs[] = getDeclaringIDs(replaced, sim_local);
          for (String id_r : remainingIDs) {
            if (missingIDs.contains(id_r)) missingIDs_r.add(id_r);
          }
          // and try to resolve the remaining ids
          if (findExpressionsForMissingIDs(sim_local, missingIDs_r, replaced, definedExpressions, requiredExpressions) == null) {
            requiredExpressions.add(declares.getKey());
            return null;
          }
        }
      }
      return id; // could not resolve id
    }
    
    return null;
  }
  
  private Map<Condition,Set<String>> dependsOn;
  private Map<Condition,Set<String>> provides;
  
  /**
   * Extract IDs of the variable declarations of the variables used in the expression.
   *  
   * @param exp
   * @param sim
   * @return
   * @throws Exception
   */
  private String[] getDeclaringIDs(String exp, HighLevelSimulator sim) {
    try {
      String _ml_def_expression = "CPN'Dep.find_use \"fun () = "+exp+"\"";
      String result = sim.evaluate(_ml_def_expression);

      List<String> ids = new LinkedList<String>();
      for (String id : eval_find_use_parse_result(result)) {
        // filter null values
        if (id != null && id.length() > 0) ids.add(id);
      }
      return ids.toArray(new String[ids.size()]);
      
    } catch (Exception exc) {
      System.err.println("Couldn't check expression of "+exp);
      exc.printStackTrace();
    }
    return null;
  }
  
  /**
   * Compute all dependencies of the post-conditions and the guard-label of 
   * event 'e' on the (transitive) pre-conditions of event 'e'.
   * 
   * @param e
   * @param sim_local
   */
  private void buildDependencies(Event e, HighLevelSimulator sim_local) {
    
    Set<String> preConditionSingleIDs = new HashSet<String>();
    Set<String> declaredSingleIDs = new HashSet<String>();
    Set<String> usedIDs = new HashSet<String>();
    
    Map<String, Set<String>> evaluatedCompleteExpressions = new HashMap<String, Set<String>>();
    
    Map<String, List<Condition>> declaringConditions = new HashMap<String, List<Condition>>();
    Map<String, List<Condition>> usingConditions = new HashMap<String, List<Condition>>();
    
    // collect everything that is available: variables (in form of variables IDs) and
    // complete terms which can be bound to evaluated values
    for (Node n : e.getAllPredecessors()) {
      if (n instanceof Event || n.isAbstract()) continue;
      Condition c = (Condition)n;
      
      String placeName = getPlaceName(c.getName());
      String arcExpression = getToken(c.getName());

      String[] ids = getDeclaringIDs(arcExpression, sim_local);
      if (ids != null && ids.length > 0) {
        //System.out.print(c.getName()+" -->* "); for (String id : ids) System.out.print(id+" "); System.out.println();

        if (!provides.containsKey(c)) provides.put(c, new HashSet<String>());
        
        if (ids.length == 1) {
          declaredSingleIDs.add(ids[0]);
          // this id is used in a direct predecessor of e: values can be bound to it
          if (e.getPreSet().contains(c)) preConditionSingleIDs.add(ids[0]);

          if (!declaringConditions.containsKey(ids[0])) declaringConditions.put(ids[0], new LinkedList<Condition>());
          declaringConditions.get(ids[0]).add(c);
          
          provides.get(c).add(ids[0]);
        }
        
        /* DISABLED: re-using entire arc expressions
         
        // this expression is completely evaluated because it is in the event's precondition and
        // hence can be bound to a value in the run
        HashSet<String> idset = new HashSet<String>();
        for (String id : ids) {
          idset.add(id);
        }
        evaluatedCompleteExpressions.put(arcExpression, idset); 
        provides.get(c).add(arcExpression);
        */
      }
    }
    
    // check for each post-condition on what variables and terms it depends on
    for (Condition c : e.getPostConditions()) {
      if (c.isAbstract()) continue;
      
      String placeName = getPlaceName(c.getName());
      String arcExpression = getToken(c.getName());
      
      if (!dependsOn.containsKey(c)) {
        dependsOn.put(c, new HashSet<String>());
      }
      
      String ids[] = getDeclaringIDs(arcExpression, sim_local);
      if (ids != null && ids.length > 0) {
        //System.out.print(c.getName()+" -->* "); for (String id : ids) System.out.print(id+" "); System.out.println();

        boolean singleID = (ids.length == 1);
        
        Set<String> missingIDs = new HashSet<String>();
        for (String id : ids) {
          
          Object o = getObjectForID(id);
          if (!isVariableDeclaration(o)) continue; // only variables can be missing
          
          if (!declaredSingleIDs.contains(id)) {
            System.err.println(c.getName()+" ("+((Oclet)c.eContainer().eContainer()).getName()+") requires variable "+getObjectForID(id));
            missingIDs.add(id);
          }
          dependsOn.get(c).add(id);
        }
        
        // if there is an immediate precondition with the same expression: add it to the dependencies
        for (Condition c2 : e.getPreConditions()) {
          if (!c2.isAbstract() && getToken(c2.getName()).equals(arcExpression)) {
            dependsOn.get(c).add(arcExpression);
          }
        }
        
        // CPN'Dep.find_dependencies ("fun () = amount*price-actual_amount*price","fun () = actual_amount*price")
        
        if (missingIDs.size() > 0) {
          Set<String> requiredExpressions = new HashSet<String>();
          String id_unresolved = findExpressionsForMissingIDs(sim_local, missingIDs, arcExpression, evaluatedCompleteExpressions, requiredExpressions);
          if (id_unresolved == null) {
            System.err.println("can be resolved with "+requiredExpressions);
            for (String exp : requiredExpressions) {
              dependsOn.get(c).add(exp);
            }
            
          } else {
            System.err.println(id_unresolved+" CANNOT be resolved");
            ModelError error = new ModelError(c, c.getName(), "Cannot resolve '"+getVariableNameForID(id_unresolved)+"' for '"+c.getName()+"'");
            errors.add(error);
          }
        }
      }
    }
    
    if (!declaredSingleIDs.containsAll(usedIDs)) {
      System.err.println("Event "+e.getName()+" does not have all data it needs");
      System.err.println("declared: ");
      for (String declared : declaredSingleIDs) {
        System.err.println("  "+getObjectForID(declared));
      }
        
      System.err.println("used:");
      for (String used : usedIDs) {
        System.err.println("  "+getObjectForID(used));
      }
      
    }
    
    for (String id : usedIDs) {
      if (!preConditionSingleIDs.contains(id)) {
        System.err.println("event "+e.getName()+" needs arc from "+id+" "+declaringConditions.get(id));
      }
    }
  }
  
  public static class ModelError {
    
    public ModelError(EObject modelEObject, String location, String error) {
      this.modelObject = modelEObject;
      this.location = location;
      this.error = error;
    }
    
    public EObject modelObject;
    public String location;
    public String error;
  }
  
  /**
   * all model errors found in the last check
   */
  public List<ModelError> errors;
  
  public void buildDependencies(AdaptiveSystem as) {
    
    errors = new LinkedList<AdaptiveSystemToCPN.ModelError>();
    
    try {
      SimulatorService ss = SimulatorService.getInstance();
      Simulator s = ss.getNewSimulator();
      HighLevelSimulator sim_local = HighLevelSimulator.getHighLevelSimulator(s);
  
      try {
  
        
        Checker c = new Checker(net, null, sim_local);
        c.checkEntireModel();

      } catch (Exception e) {
        // catch all exceptions, the only purpose of this check is to compile the model
        // so we can work with the type system
        
        e.printStackTrace();
      }
  
      dependsOn = new HashMap<Condition, Set<String>>();
      provides = new HashMap<Condition, Set<String>>();
      
      for (Oclet o : as.getOclets()) {
        for (Node n : o.getDoNet().getNodes()) {
          if (n instanceof Event && !n.isAbstract()) {
            buildDependencies((Event)n, sim_local);
          }
        }
      }
      
      sim_local.destroy();
      
    } catch (Exception e) {
      e.printStackTrace(); 
    }
  }
  
  public void check() {
    
    try {
      Checker c = new Checker(net, null, sim);
      c.checkEntireModel();
    } catch (SyntaxCheckerException e) {

      ModelError error = new ModelError(null, "CPN model", "The generated CPN model contains errors; see console.");
      
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
