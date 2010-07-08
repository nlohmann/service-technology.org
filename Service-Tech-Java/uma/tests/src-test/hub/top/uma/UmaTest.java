package hub.top.uma;

import hub.top.petrinet.PetriNet;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.petrinet.unfold.DNodeSys_PetriNet;
import hub.top.scenario.DNodeSys_OcletSpecification;
import hub.top.scenario.Oclet;
import hub.top.scenario.OcletSpecification;

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
    testPetrinet2Bounded();
    testPetrinet1Bounded_violated();
    
    testOcletPrefix_lexic();
    testOcletPrefix_executable1();
    testOcletPrefix_executable2();
    testOcletPrefix_executable3();
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
  
  private PetriNet getNet2Bounded () {
    PetriNet net = new PetriNet();
    net.addPlace("a"); net.setTokens("a", 2);
    net.addPlace("b"); net.setTokens("b", 1);
    net.addPlace("c"); net.setTokens("c", 1);
    net.addPlace("d");
    net.addPlace("e");
    net.addPlace("f");
    
    net.addTransition("x");
      net.addArc("a", "x"); net.addArc("b", "x");
      net.addArc("x", "a"); net.addArc("x", "d"); net.addArc("x", "e");
      
    net.addTransition("y");
      net.addArc("a", "y"); net.addArc("c", "y");
      net.addArc("y", "a"); net.addArc("y", "d"); net.addArc("y", "f");
      
    net.addTransition("u");
      net.addArc("d", "u"); net.addArc("e", "u");
      net.addArc("u", "b");
      
    net.addTransition("v");
      net.addArc("d", "v"); net.addArc("f", "v");
      net.addArc("v", "c");

    return net;
  }
  
  public void testPetrinet2Bounded () {
    
    lastTest = "Construct prefix of 2-bounded Petri net";
    
    PetriNet net = getNet2Bounded();
      
    try {
      DNodeSys_PetriNet sys = new DNodeSys_PetriNet(net);
      DNodeBP build = new DNodeBP(sys);
      build.configure_buildOnly();
      build.configure_PetriNet();
      build.configure_setBound(2);
      
      while (build.step() > 0) {
      }
      
      build.getStatistics();
      
      assertTrue(build.statistic_eventNum == 28
          && build.statistic_condNum == 48
          && build.statistic_cutOffNum == 20
          && build.statistic_arcNum == 100);
      
    } catch (InvalidModelException e) {
      assertTrue(false);
    }
  }

  public void testPetrinet1Bounded_violated () {
    
    lastTest = "Violate 1-bound during construction";
    
    PetriNet net = getNet2Bounded();
      
    try {
      DNodeSys_PetriNet sys = new DNodeSys_PetriNet(net);
      DNodeBP build = new DNodeBP(sys);
      build.configure_buildOnly();
      build.configure_PetriNet();
      build.configure_stopIfUnSafe();
      
      while (build.step() > 0) {
      }
      
      assertFalse( build.isGloballySafe() );
      
    } catch (InvalidModelException e) {
      assertTrue(false);
    }
  }
  
  private OcletSpecification getOcletSpec() {
    OcletSpecification os = new OcletSpecification();
    
    Oclet init = new Oclet(false);
    init.addPlace("a", false); init.setTokens("a", 1); 
    init.addPlace("b", false); init.setTokens("b", 1);
    
    Oclet o1 = new Oclet(false);
    Place b1 = o1.addPlace("a", true); Place b2 = o1.addPlace("b", true);
    Place b3 = o1.addPlace("a", false); Place b4 = o1.addPlace("b", false);
    
    Transition e = o1.addTransition("X", false);
      o1.addArc(b1, e); o1.addArc(b2, e);
      o1.addArc(e, b3); o1.addArc(e, b4);
      
    Oclet o2 = new Oclet(false);
    b1 = o2.addPlace("a", true); b2 = o2.addPlace("b", true);
    b3 = o2.addPlace("a", false); b4 = o2.addPlace("b", false);
    
    e = o2.addTransition("Y", false);
      o2.addArc(b1, e); o2.addArc(b2, e);
      o2.addArc(e, b3); o2.addArc(e, b4);
      
      
    Oclet o3 = new Oclet(true);
    Place b0 = o3.addPlace("a", true);
    b1 = o3.addPlace("a", true); b2 = o3.addPlace("b", true);
    b3 = o3.addPlace("a", false); b4 = o3.addPlace("b", false);
    
    Transition f = o3.addTransition("X", true);
      o3.addArc(b0, f);
      o3.addArc(f, b1);
    
    e = o3.addTransition("Y", false);
      o3.addArc(b1, e); o3.addArc(b2, e);
      o3.addArc(e, b3); o3.addArc(e, b4);
      
    o3.makeHotNode(e);
    o3.makeHotNode(b3);
    o3.makeHotNode(b4);
      
    os.addOclet(o1);
    os.addOclet(o2);
    os.addOclet(o3);
    os.setInitialRun(init);
    
    return os;
  }
  
  public void testOcletPrefix_lexic() {
    
    lastTest = "Construct prefix of oclet specification";

    OcletSpecification os = getOcletSpec();
    
    DNodeSys_OcletSpecification sys = new DNodeSys_OcletSpecification(os);
    DNodeBP build = new DNodeBP(sys);
    build.configure_buildOnly();
    build.configure_Scenarios();
    build.configure_stopIfUnSafe();
    
    while (build.step() > 0) {
    }
    
    build.getStatistics();
    
    assertTrue(build.statistic_eventNum == 4
        && build.statistic_condNum == 10
        && build.statistic_cutOffNum == 2
        && build.statistic_arcNum == 16);
  }
  
  public void testOcletPrefix_executable1() {
    
    lastTest = "Test for executability of events of an oclet specification 1";

    OcletSpecification os = getOcletSpec();
    
    DNodeSys_OcletSpecification sys = new DNodeSys_OcletSpecification(os);
    short eventToCheck = sys.nameToID.get("X");
    DNodeBP build = new DNodeBP(sys);
    build.configure_buildOnly();
    build.configure_Scenarios();
    build.configure_checkExecutable(eventToCheck);
    
    while (build.step() > 0) {
    }
    
    build.getStatistics();
    
    assertTrue(build.statistic_eventNum == 2
        && build.statistic_condNum == 6
        && build.statistic_cutOffNum == 1
        && build.statistic_arcNum == 8
        && build.canExecuteEvent());
  }
  
  public void testOcletPrefix_executable2() {
    
    lastTest = "Test for executability of events of an oclet specification 2";

    OcletSpecification os = getOcletSpec();
    
    Oclet o4 = new Oclet(false);
    Place b0 = o4.addPlace("a", true);
    Place b1 = o4.addPlace("a", true); Place b2 = o4.addPlace("b", true);
    Place b3 = o4.addPlace("a", true); Place b4 = o4.addPlace("b", true);
    Place b5 = o4.addPlace("a", false); Place b6 = o4.addPlace("b", false);
    
    Transition e1 = o4.addTransition("X", true);
      o4.addArc(b0, e1);
      o4.addArc(e1, b1);
    
    Transition e2 = o4.addTransition("Y", true);
      o4.addArc(b1, e2); o4.addArc(b2, e2);
      o4.addArc(e2, b3); o4.addArc(e2, b4);
      
    Transition e3 = o4.addTransition("Z", false);
      o4.addArc(b3, e3); o4.addArc(b4, e3);
      o4.addArc(e3, b5); o4.addArc(e3, b6);
      
    os.addOclet(o4);
    
    DNodeSys_OcletSpecification sys = new DNodeSys_OcletSpecification(os);
    short eventToCheck = sys.nameToID.get("Z");
    DNodeBP build = new DNodeBP(sys);
    build.configure_buildOnly();
    build.configure_Scenarios();
    build.configure_checkExecutable(eventToCheck);
    
    while (build.step() > 0) {
    }
    
    build.getStatistics();
    
    assertTrue(build.statistic_eventNum == 4
        && build.statistic_condNum == 10
        && build.statistic_cutOffNum == 2
        && build.statistic_arcNum == 16
        && !build.canExecuteEvent());
  }
  
  public void testOcletPrefix_executable3() {
    
    lastTest = "Test for executability of events of an oclet specification 3";

    OcletSpecification os = getOcletSpec();
    
    Oclet o4 = new Oclet(false);
    Place b0 = o4.addPlace("a", true);
    Place b1 = o4.addPlace("a", true); Place b2 = o4.addPlace("b", true);
    Place b3 = o4.addPlace("a", true); Place b4 = o4.addPlace("b", true);
    Place b5 = o4.addPlace("a", false); Place b6 = o4.addPlace("b", false);
    
    Transition e1 = o4.addTransition("X", true);
      o4.addArc(b0, e1);
      o4.addArc(e1, b1);
    
    Transition e2 = o4.addTransition("Y", true);
      o4.addArc(b1, e2); o4.addArc(b2, e2);
      o4.addArc(e2, b3); o4.addArc(e2, b4);
      
    Transition e3 = o4.addTransition("X", false);
      o4.addArc(b3, e3); o4.addArc(b4, e3);
      o4.addArc(e3, b5); o4.addArc(e3, b6);
      
    os.addOclet(o4);

    // first check for executability of event e3
    DNodeSys_OcletSpecification sys = new DNodeSys_OcletSpecification(os);
    DNode eventToCheck = sys.getResultNode(e3);
    
    DNodeBP build = new DNodeBP(sys);
    build.configure_buildOnly();
    build.configure_Scenarios();
    build.configure_checkExecutable(eventToCheck);
    
    while (build.step() > 0) {
    }
    
    build.getStatistics();
    
    assertTrue(build.statistic_eventNum == 4
        && build.statistic_condNum == 10
        && build.statistic_cutOffNum == 2
        && build.statistic_arcNum == 16
        && !build.canExecuteEvent());

    
    lastTest = "Test for executability of events of an oclet specification 4";

    // then check for executability of an event with the same label as e3
    sys = new DNodeSys_OcletSpecification(os);
    short eventLabelToCheck = sys.nameToID.get(e3.getName());    
    
    build = new DNodeBP(sys);
    build.configure_buildOnly();
    build.configure_Scenarios();
    build.configure_checkExecutable(eventLabelToCheck);
    
    while (build.step() > 0) {
    }
    
    build.getStatistics();
    
    assertTrue(build.statistic_eventNum == 2
        && build.statistic_condNum == 6
        && build.statistic_cutOffNum == 1
        && build.statistic_arcNum == 8
        && build.canExecuteEvent());
  }

}
