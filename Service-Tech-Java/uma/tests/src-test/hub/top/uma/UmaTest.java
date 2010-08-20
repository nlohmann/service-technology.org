package hub.top.uma;

import java.io.IOException;

import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.petrinet.unfold.DNodeSys_PetriNet;
import hub.top.scenario.DNodeSys_OcletSpecification;
import hub.top.scenario.Oclet;
import hub.top.scenario.OcletIO;
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
    //System.out.println(System.getProperty("user.dir"));
    t.run();
  }
  
  public void testPetrinetPrefix_lexic () {
    
    lastTest = "Construct prefix of Petri net with lexicographic order";
    
    try {
      PetriNet net = PetriNetIO.readNetFromFile("./testfiles/net_lexik.lola");
      
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
    } catch (IOException e) {
      System.err.println("Couldn't read test file: "+e);
      assertTrue(false);
    }
  }
  
  public void testPetrinet2Bounded () {
    
    lastTest = "Construct prefix of 2-bounded Petri net";
    
    try {
      PetriNet net = PetriNetIO.readNetFromFile("./testfiles/net_2bounded.lola");
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
    } catch (IOException e) {
      System.err.println("Couldn't read test file: "+e);
      assertTrue(false);
    }
  }

  public void testPetrinet1Bounded_violated () {
    
    lastTest = "Violate 1-bound during construction";
    
    try {
      PetriNet net = PetriNetIO.readNetFromFile("./testfiles/net_2bounded.lola");
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
    } catch (IOException e) {
      System.err.println("Couldn't read test file: "+e);
      assertTrue(false);
    }

  }
  
  public void testOcletPrefix_lexic() {
    
    lastTest = "Construct prefix of oclet specification";

    try {
      PetriNet net = OcletIO.readNetFromFile("./testfiles/spec1.oclets");
      OcletSpecification os = new OcletSpecification(net);
      
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
      
    } catch (Exception e) {
      assertTrue(false);
    }
  }
  
  public void testOcletPrefix_executable1() {
    
    lastTest = "Test for executability of events of an oclet specification 1";

    try {
      PetriNet net = OcletIO.readNetFromFile("./testfiles/spec1.oclets");
      OcletSpecification os = new OcletSpecification(net);
      
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
      
    } catch (Exception e) {
      assertTrue(false);
    }
  }
  
  public void testOcletPrefix_executable2() {
    
    lastTest = "Test for executability of events of an oclet specification 2";

    try {
      PetriNet net = OcletIO.readNetFromFile("./testfiles/spec2.oclets");
      OcletSpecification os = new OcletSpecification(net);
      
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
          && build.statistic_arcNum == 16);
      assertFalse(build.canExecuteEvent());
      
    } catch (Exception e) {
      assertTrue(false);
    }
  }
  
  public void testOcletPrefix_executable3() {
    
    lastTest = "Test for executability of events of an oclet specification 3";
    
    try {

      PetriNet net = OcletIO.readNetFromFile("./testfiles/spec1.oclets");
      OcletSpecification os = new OcletSpecification(net);
      
      Oclet o4 = new Oclet("o5", false);
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
          && build.statistic_arcNum == 16);
      assertFalse(build.canExecuteEvent());
  
      
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
    } catch (Exception e) {
      assertTrue(false);
    }
  }

}
