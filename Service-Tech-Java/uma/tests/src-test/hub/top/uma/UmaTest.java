package hub.top.uma;

import hub.top.petrinet.PetriNet;
import hub.top.petrinet.unfold.DNodeSys_PetriNet;

public class UmaTest {
  public String lastTest = "";
  public int testNum = 0;
  public int testFail = 0;
  
  public UmaTest(String name) {
    super();
  }
 
  private static final String RESULT_FAIL = "[failed]";
  private static final String RESULT_OK = "[ok]";
  
  public void assertTrue(boolean result) {
    System.out.println(lastTest+": "+result+", expected: true "+(result ? RESULT_OK : RESULT_FAIL));
    if (!result) testFail++;
  }
  
  public void assertFalse(boolean result) {
    System.out.println(lastTest+": "+result+", expected: false "+(!result ? RESULT_OK : RESULT_FAIL));
    if (result) testFail++;
  }
  
  public void run () {
    testPetrinetPrefix_lexic();
  }

  public static void main(String[] args) {
    UmaTest t = new UmaTest("Uma unfolding");
    t.run();
  }
  
  public void testPetrinetPrefix_lexic () {
    
    lastTest = "Construct prefix of Petri net with lexicographic order";
    
    PetriNet net = new PetriNet();
    net.addPlace("p1"); net.setTokens("p1", 1);
    net.addPlace("p2"); net.setTokens("p2", 1);
    net.addPlace("p3");
    net.addPlace("p4");
    net.addPlace("p5");
    net.addPlace("p6");
    
    net.addTransition("u");
      net.addArc("p1", "u");
      net.addArc("u", "p3");
      
    net.addTransition("v");
      net.addArc("p1", "v");
      net.addArc("v", "p3");
      
    net.addTransition("w");
      net.addArc("p2", "w");
      net.addArc("w", "p4");
      
    net.addTransition("x");
      net.addArc("p3", "x"); net.addArc("p4", "x");
      net.addArc("x", "p5"); net.addArc("x", "p6");
      
    net.addTransition("y");
      net.addArc("p5", "y");
      net.addArc("y", "p1");
      
    net.addTransition("z");
      net.addArc("p6", "z");
      net.addArc("z", "p2");
      
    try {
      DNodeSys_PetriNet sys = new DNodeSys_PetriNet(net);
      DNodeBP build = new DNodeBP(sys);
      build.configure_buildOnly();
      build.configure_PetriNet();
      build.configure_stopIfUnSafe();
      
      while (build.step() > 0) {
      }
      
      build.getStatistics();
      
      assertTrue(build.statistic_eventNum == 10
          && build.statistic_condNum == 14
          && build.statistic_cutOffNum == 3
          && build.statistic_arcNum == 24);
      
    } catch (InvalidModelException e) {
      assertTrue(false);
    }
  }
}
