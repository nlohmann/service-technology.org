package hub.top.uma.synthesis;

import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import viptool.algorithm.postprocessing.pnetiplp.ExtendedExhuminator;

public class ImplicitPlaces {

  public static int findImplicitPlaces(PetriNet net) {
    try {
      System.loadLibrary("lpsolve55");
      System.loadLibrary("lpsolve55j");
    } catch (Exception e) {
      System.err.println("Unable to load required libraries for ILP solver.");
      System.err.println("Exception thrown: "+e);
      //                  0123456789012345678901234567890123456789012345678901234567890123456789
      System.err.println("Please obtain a copy of 'lpsolve' and make the libraries available");
      System.err.println("to Uma on the java library path:");
      System.err.println("  java <params> -Djava.library.path=path/to/lpsolve/libs");
      System.err.println("Uma provides other settings for removing implicit places that do not");
      System.err.println("require an ILP solver.");
      return 0;
    }
    
    System.out.println(net.getInfo(false));
    int reduced = ExtendedExhuminator.reduce(net);
    System.out.println(net.getInfo(false));
    return reduced;
  }
  
  public static void main(String args[]) throws Exception {
    PetriNet net = PetriNetIO.readNetFromFile(args[0]);
    
    findImplicitPlaces(net);
  }
}
