package hub.top.uma;

import java.io.IOException;

import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.unfold.DNodeSys_PetriNet;

public class Uma {
  
  private static String options_inFile = null;
  
  private static void parseCommandLine(String args[]) {
    for (int i=0; i<args.length; i++) {
      if ("-i".equals(args[i])) {
        options_inFile = args[++i];
      }
    }
  }

  /**
   * @param args
   */
  public static void main(String[] args) throws Exception {
    parseCommandLine(args);
    
    if (options_inFile == null) return;
    
    PetriNet net = PetriNetIO.readNetFromFile(options_inFile);
    
    DNodeSys_PetriNet sys = new DNodeSys_PetriNet(net);
    DNodeBP build = new DNodeBP(sys);
    build.configure_buildOnly();
    build.configure_PetriNet();
    build.configure_stopIfUnSafe();
    
    while (build.step() > 0) {
    }

    System.out.println(build.getStatistics());

  }

}
