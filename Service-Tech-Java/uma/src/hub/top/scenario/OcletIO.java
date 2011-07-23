package hub.top.scenario;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.HashMap;
import java.util.HashSet;

import org.antlr.runtime.ANTLRFileStream;
import org.antlr.runtime.CommonTokenStream;
import org.antlr.runtime.EarlyExitException;
import org.antlr.runtime.MismatchedTokenException;
import org.antlr.runtime.RecognitionException;

import hub.top.petrinet.Arc;
import hub.top.petrinet.LoLALexer;
import hub.top.petrinet.LoLAParser;
import hub.top.petrinet.Node;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;

public class OcletIO extends PetriNetIO {

  public static PetriNet readNetFromFile(String fileName) throws IOException {
    
    String ext = fileName.substring(fileName.lastIndexOf('.')+1);
    
    if ("oclets".equals(ext)) {
      
      LoLALexer lex = new LoLALexer(new ANTLRFileStream(fileName));
      CommonTokenStream tokens = new CommonTokenStream(lex);
  
      LoLAParser parser = new LoLAParser(tokens);
  
      try {
        PetriNet result = parser.net();
        return result;
          
      } catch (RecognitionException e)  {
        if (e instanceof EarlyExitException) {
          EarlyExitException e2 = (EarlyExitException)e;
          System.err.println("failed parsing "+fileName);
          System.err.println("found unexpected '"+e2.token.getText()+"' in "
              +"line "+e2.line+ " at column "+e2.charPositionInLine);
          System.exit(1);
        } else if (e instanceof MismatchedTokenException) {
          MismatchedTokenException e2 = (MismatchedTokenException)e;
          System.err.println("found "+e2.token.getText()+" expected "+LoLAParser.tokenNames[e2.expecting]);
          System.err.println(" in line "+e2.line+" at column "+e2.charPositionInLine);
        } else {
          e.printStackTrace();
        }
      }
    }
    
    return PetriNetIO.readNetFromFile(fileName);
  }
  
  public static void writeFile(String s, String fileName) throws IOException {

    // Create file 
    FileWriter fstream = new FileWriter(fileName);
    BufferedWriter out = new BufferedWriter(fstream);
    
    out.write(s);

    //Close the output stream
    out.close();
  }
  
  public static void writeToFile(OcletSpecification os, String fileName, int format, int parameter) throws IOException {

    // Create file 
    String extFileName = fileName+"."+getFileExtension(format);
    String contents; 
    if (format == FORMAT_DOT) {
      contents = toDot(os);
    } else if (format == FORMAT_LOLA) {
      contents = toLoLA(os);
    } else {
      return;
    }
    writeFile(extFileName, contents);
  }
  
  public static String toDot(Oclet o) {
    StringBuilder b = new StringBuilder();

    String tokenFillString = "fillcolor=black peripheries=2 height=\".2\" width=\".2\" ";
    String preconditionFillString = "fillcolor=gray66";
    String antiFillString = "fillcolor=red";

    String oName = o.name.replace(' ', '_');
    
    HashMap<Node, Integer> nodeIDs = new HashMap<Node, Integer>();
    int nodeID = 0;
      
      b.append("subgraph cluster_"+oName+" {");

      // first print all places
      b.append("\n\n");
      b.append("  node [shape=circle];\n");
      for (Place p : o.getPlaces()) {
        
        nodeID++;
        nodeIDs.put(p, nodeID);
        
        if (p.getTokens() > 0)
          b.append("  "+oName+"_p"+nodeID+" ["+tokenFillString+"]\n");
        else if (o.isInHistory(p))
          b.append("  "+oName+"_p"+nodeID+" ["+preconditionFillString+"]\n");
        else if (o.isAnti && !o.isInHistory(p))
          b.append("  "+oName+"_p"+nodeID+" ["+antiFillString+"]\n");
        else
          b.append("  "+oName+"_p"+nodeID+" []\n");
        
        String auxLabel = "";//"ROLES: "+toString(p.getRoles());
          
        b.append("  "+oName+"_p"+nodeID+"_l [shape=none];\n");
        b.append("  "+oName+"_p"+nodeID+"_l -> "+oName+"_p"+nodeID+" [headlabel=\""+p.getName()+" "+auxLabel+"\"]\n");
      }
  
      // then print all events
      b.append("\n\n");
      b.append("  node [shape=box];\n");
      for (Transition t : o.getTransitions()) {
        
        nodeID++;
        nodeIDs.put(t, nodeID);
        
        if (o.isInHistory(t))
          b.append("  "+oName+"_t"+nodeID+" ["+preconditionFillString+"]\n");
        else if (o.isAnti && !o.isInHistory(t))
          b.append("  "+oName+"_t"+nodeID+" ["+antiFillString+"]\n");
        else
          b.append("  "+oName+"_t"+nodeID+" []\n");
        
        String auxLabel  = "";//"ROLES: "+toString(t.getRoles());
        
        b.append("  "+oName+"_t"+nodeID+"_l [shape=none];\n");
        b.append("  "+oName+"_t"+nodeID+"_l -> "+oName+"_t"+nodeID+" [headlabel=\""+t.getName()+" "+auxLabel+"\"]\n");
      }
      
      // finally print all arcs
      b.append("\n\n");
      b.append("  edge [fontname=\"Helvetica\" fontsize=8 arrowhead=normal color=black];\n");
      for (Arc arc : o.getArcs()) {
        if (arc.getSource() instanceof Transition)
          b.append("  "+oName+"_t"+nodeIDs.get(arc.getSource())+" -> "+oName+"_p"+nodeIDs.get(arc.getTarget())+" [weight=10000.0]\n");
        else
          b.append("  "+oName+"_p"+nodeIDs.get(arc.getSource())+" -> "+oName+"_t"+nodeIDs.get(arc.getTarget())+" [weight=10000.0]\n");
      }
      
      b.append("}\n\n");  //subgraph
      
    return b.toString();

  }
  
  public static String toDot(OcletSpecification os) {
    StringBuilder b = new StringBuilder();

    b.append("digraph BP {\n");
    
    // standard style for nodes and edges
    b.append("graph [fontname=\"Helvetica\" nodesep=0.3 ranksep=\"0.2 equally\" fontsize=10];\n");
    b.append("node [fontname=\"Helvetica\" fontsize=8 fixedsize width=\".3\" height=\".3\" label=\"\" style=filled fillcolor=white];\n");
    b.append("edge [fontname=\"Helvetica\" fontsize=8 color=white arrowhead=none weight=\"20.0\"];\n\n");
    
    for (Oclet o : os.getOclets())
      b.append(toDot(o));
    b.append(toDot(os.getInitialRun()));

    b.append("}\n");
    
    return b.toString();
  }
  
  private static String ocletNodeToLoLA(Oclet o, Node n) {
    String antiPrefix = o.isAnti ? "!" : "";
    String histSuffix = o.isInHistory(n) ? "_hist" : "";
    return antiPrefix+o.name+histSuffix+"#"+toLoLA(n.getName())+"_"+n.id;
  }
  
  private static void toLoLA_appendPlaces(StringBuilder b, Oclet o) {
    b.append("  ");
    boolean first = true;
    for (Place p : o.getPlaces()) {
      if (!first) b.append(", ");
      b.append(ocletNodeToLoLA(o, p));
      first = false;
    }
    b.append(";\n");
  }
  
  private static void toLoLA_appendTransitions(StringBuilder b, Oclet o) {
    for (Transition t : o.getTransitions())
    {
      b.append("TRANSITION "+ocletNodeToLoLA(o, t)+"\n");
        b.append("  CONSUME ");
        boolean first = true;
        for (Place p : t.getPreSet()) {
          if (!first) b.append(", ");
          b.append(ocletNodeToLoLA(o, p)+":1");
          first = false;
        }
        b.append(";\n");
        b.append("  PRODUCE ");
        first = true;
        for (Place p : t.getPostSet()) {
          if (!first) b.append(", ");
          b.append(ocletNodeToLoLA(o, p)+":1");
          first = false;
        }
        b.append(";\n\n");
    }
  }
  
  public static String toLoLA(OcletSpecification os) {
    StringBuilder b = new StringBuilder();

    b.append("PLACE\n");
    
    if (os.getInitialRun() != null)
      toLoLA_appendPlaces(b, os.getInitialRun());
    for (Oclet o : os.getOclets())
      toLoLA_appendPlaces(b, o);
    b.append("\n");
    
    b.append("MARKING\n");
    {
      b.append("  ");
      if (os.getInitialRun() != null) {
        boolean first = true;
        for (Place p : os.getInitialRun().getPlaces()) {
          if (!first) b.append(", ");
          if (p.getTokens() > 0)
            b.append(ocletNodeToLoLA(os.getInitialRun(), p)+":"+p.getTokens());
          first = false;
        }
      }
      b.append(";\n");
    }
    b.append("\n");

    if (os.getInitialRun() != null)
      toLoLA_appendTransitions(b, os.getInitialRun());
    for (Oclet o : os.getOclets())
      toLoLA_appendTransitions(b, o);
    
    return b.toString();
  }
}
