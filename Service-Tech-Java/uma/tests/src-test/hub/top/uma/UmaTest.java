package hub.top.uma;

import hub.top.petrinet.ISystemModel;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.petrinet.unfold.DNodeSys_PetriNet;
import hub.top.scenario.DNodeSys_OcletSpecification;
import hub.top.scenario.Oclet;
import hub.top.scenario.OcletIO;
import hub.top.scenario.OcletSpecification;
import hub.top.uma.synthesis.NetSynthesis;

import java.io.IOException;

public class UmaTest extends hub.top.test.TestCase {
	
  public UmaTest() {
    super("UmaTest");
  }
 
  public static void main(String[] args) {
    setParameters(args);
    junit.textui.TestRunner.run(UmaTest.class);
  }
  
  public void testPetrinetPrefix_lexic () {
    
    lastTest = "Construct prefix of Petri net with lexicographic order";
    
    try {

      ISystemModel sysModel = Uma.readSystemFromFile(testFileRoot+"/net_lexik.lola");
      DNodeSys sys = Uma.getBehavioralSystemModel(sysModel);
      DNodeBP build = Uma.initBuildPrefix(sys, 1);
      
      while (build.step() > 0) {
      }
      
      System.out.println(build.getStatistics());

      //String targetPath_dot = testFileRoot+"/net_lexik.bp.dot";
      //writeFile(targetPath_dot, build.toDot());
      
      assertTrue(lastTest+" "+build.getStatistics()+" == 10,14,3,24",
          build.statistic_eventNum == 10
          && build.statistic_condNum == 14
          && build.statistic_cutOffNum == 3
          && build.statistic_arcNum == 24);
      
    } catch (InvalidModelException e) {
      System.err.println("Invalid model: "+e);
      assertTrue(lastTest, false);
    } catch (IOException e) {
      System.err.println("Couldn't read test file: "+e);
      assertTrue(lastTest, false);
    }
  }
  
  /*
  private void writeFile(String fileName, String contents) throws IOException {
    FileWriter fstream = new FileWriter(fileName);
    BufferedWriter out = new BufferedWriter(fstream);
    out.write(contents);
    out.close();
  }
  */
  
  public void testPetrinet2Bounded () {
    
    lastTest = "Construct prefix of 2-bounded Petri net";
    
    try {
      PetriNet net = PetriNetIO.readNetFromFile(testFileRoot+"/net_2bounded.lola");
      DNodeSys_PetriNet sys = new DNodeSys_PetriNet(net);
      DNodeBP build = new DNodeBP(sys);
      build.configure_buildOnly();
      build.configure_PetriNet();
      build.configure_setBound(2);
      
      while (build.step() > 0) {
      }
      
      build.getStatistics();
      
      assertTrue(lastTest+" "+build.getStatistics()+" == 28,48,20,100",
          build.statistic_eventNum == 28
          && build.statistic_condNum == 48
          && build.statistic_cutOffNum == 20
          && build.statistic_arcNum == 100);
      
    } catch (InvalidModelException e) {
      assertTrue(lastTest, false);
    } catch (IOException e) {
      System.err.println("Couldn't read test file: "+e);
      assertTrue(lastTest, false);
    }
  }

  public void testPetrinet1Bounded_violated () {
    
    lastTest = "Violate 1-bound during construction";
    
    try {
      PetriNet net = PetriNetIO.readNetFromFile(testFileRoot+"/net_2bounded.lola");
      DNodeSys_PetriNet sys = new DNodeSys_PetriNet(net);
      DNodeBP build = new DNodeBP(sys);
      build.configure_buildOnly();
      build.configure_PetriNet();
      build.configure_stopIfUnSafe();
      
      while (build.step() > 0) {
      }
      
      assertFalse(lastTest,  build.isGloballySafe() );
      
    } catch (InvalidModelException e) {
      assertTrue(lastTest, false);
    } catch (IOException e) {
      System.err.println("Couldn't read test file: "+e);
      assertTrue(lastTest, false);
    }

  }
  
  public void testOcletPrefix_lexic() {
    
    lastTest = "Construct prefix of oclet specification";

    try {
      PetriNet net = OcletIO.readNetFromFile(testFileRoot+"/spec1.oclets");
      OcletSpecification os = new OcletSpecification(net);
      
      DNodeSys_OcletSpecification sys = new DNodeSys_OcletSpecification(os);
      DNodeBP build = new DNodeBP(sys);
      build.configure_buildOnly();
      build.configure_Scenarios();
      build.configure_stopIfUnSafe();
      
      while (build.step() > 0) {
      }
      
      build.getStatistics();
      
      assertTrue(lastTest, build.statistic_eventNum == 4
          && build.statistic_condNum == 10
          && build.statistic_cutOffNum == 2
          && build.statistic_arcNum == 16);
      
    } catch (Exception e) {
      assertTrue(lastTest, false);
    }
  }
  
  public void testOcletPrefix_executable1() {
    
    lastTest = "Test for executability of events of an oclet specification 1";

    try {
      PetriNet net = OcletIO.readNetFromFile(testFileRoot+"/spec1.oclets");
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
      
      assertTrue(lastTest, build.statistic_eventNum == 2
          && build.statistic_condNum == 6
          && build.statistic_cutOffNum == 1
          && build.statistic_arcNum == 8
          && build.canExecuteEvent());
      
    } catch (Exception e) {
      assertTrue(lastTest, false);
    }
  }
  
  public void testOcletPrefix_executable2() {
    
    lastTest = "Test for executability of events of an oclet specification 2";

    try {
      PetriNet net = OcletIO.readNetFromFile(testFileRoot+"/spec2.oclets");
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
      
      assertTrue(lastTest, build.statistic_eventNum == 4
          && build.statistic_condNum == 10
          && build.statistic_cutOffNum == 2
          && build.statistic_arcNum == 16);
      assertFalse(lastTest, build.canExecuteEvent());
      
    } catch (Exception e) {
      assertTrue(lastTest, false);
    }
  }
  
  public void testOcletPrefix_executable3() {
    
    lastTest = "Test for executability of events of an oclet specification 3";
    
    try {

      PetriNet net = OcletIO.readNetFromFile(testFileRoot+"/spec1.oclets");
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
      
      assertTrue(lastTest, build.statistic_eventNum == 4
          && build.statistic_condNum == 10
          && build.statistic_cutOffNum == 2
          && build.statistic_arcNum == 16);
      assertFalse(lastTest, build.canExecuteEvent());
  
      
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
      
      assertTrue(lastTest, build.statistic_eventNum == 2
          && build.statistic_condNum == 6
          && build.statistic_cutOffNum == 1
          && build.statistic_arcNum == 8
          && build.canExecuteEvent());
    } catch (Exception e) {
      assertTrue(lastTest, false);
    }
  }

  public void testOcletPrefix_standardExample_EMS() {
    
    lastTest = "Construct prefix of oclet specification (EMS)";

    try {
      PetriNet net = OcletIO.readNetFromFile(testFileRoot+"/EMS.oclets");
      OcletSpecification os = new OcletSpecification(net);
      
      DNodeSys_OcletSpecification sys = new DNodeSys_OcletSpecification(os);
      DNodeBP build = new DNodeBP(sys);
      build.configure_buildOnly();
      build.configure_Scenarios();
      build.configure_stopIfUnSafe();
      
      while (build.step() > 0) {
      }
      
      build.getStatistics();
      
      assertTrue(lastTest, build.statistic_eventNum == 21
          && build.statistic_condNum == 35
          && build.statistic_cutOffNum == 3
          && build.statistic_arcNum == 64);
      
    } catch (Exception e) {
      assertTrue(lastTest, false);
    }
  }
  
  public void testOcletPrefix_standardExample_flood() {
    
    lastTest = "Construct prefix of oclet specification with anti-oclets (flood alert)";

    try {
      PetriNet net = OcletIO.readNetFromFile(testFileRoot+"/flood.oclets");
      OcletSpecification os = new OcletSpecification(net);
      
      DNodeSys_OcletSpecification sys = new DNodeSys_OcletSpecification(os);
      DNodeBP build = new DNodeBP(sys);
      build.configure_buildOnly();
      build.configure_Scenarios();
      build.configure_stopIfUnSafe();
      
      while (build.step() > 0) {
      }
      
      build.getStatistics();
      
      //String targetPath_dot = testFileRoot+"/flood.bp.dot";
      //writeFile(targetPath_dot, build.toDot());
      
      assertTrue(lastTest+" "+ build.getStatistics()+" == 13,19,2,39",
          build.statistic_eventNum == 13
          && build.statistic_condNum == 19
          && build.statistic_cutOffNum == 2
          && build.statistic_arcNum == 39);
      
      int antiNodes = 0;
      for (DNode d : build.getBranchingProcess().getAllNodes()) {
        if (d.isAnti) antiNodes++;
      }
      assertEquals(lastTest, antiNodes, 2);
      
    } catch (Exception e) {
      assertTrue(lastTest, false);
    }
  }
  
  public void testOcletSynthesis_standardExample_EMS() {

    lastTest = "Synthesize Petri net from oclets (EMS)";

    try {
    
      PetriNet net = OcletIO.readNetFromFile(testFileRoot+"/EMS.oclets");
      OcletSpecification os = new OcletSpecification(net);
      
      DNodeSys_OcletSpecification sys = new DNodeSys_OcletSpecification(os);
      DNodeBP build = new DNodeBP(sys);
      build.configure_buildOnly();
      build.configure_Scenarios();
      build.configure_stopIfUnSafe();
      
      while (build.step() > 0) {
      }
      
      PetriNet net2 = hub.top.uma.synthesis.NetSynthesis.foldToNet_labeled(build, true);
      NetSynthesis.Diagnostic_Implements diag =
        NetSynthesis.doesImplement(net2, build);
      
      assertEquals(lastTest, diag.result, NetSynthesis.COMPARE_EQUAL);
      
    } catch (IOException e) {
      System.err.println("Couldn't read test file: "+e);
      assertTrue(lastTest, false);
    } catch (InvalidModelException e) {
      assertTrue(lastTest, false);
    }
  }
  
  public void testOcletSynthesis_standardExample_flood() {

    lastTest = "Synthesize Petri net with anti-oclets (flood alert)";

    try {
    
      PetriNet net = OcletIO.readNetFromFile(testFileRoot+"/flood.oclets");
      OcletSpecification os = new OcletSpecification(net);
      
      DNodeSys_OcletSpecification sys = new DNodeSys_OcletSpecification(os);
      DNodeBP build = new DNodeBP(sys);
      build.configure_buildOnly();
      build.configure_Scenarios();
      build.configure_stopIfUnSafe();
      
      while (build.step() > 0) {
      }
      
      PetriNet net2 = hub.top.uma.synthesis.NetSynthesis.foldToNet_labeled(build);
      NetSynthesis.Diagnostic_Implements diag =
        NetSynthesis.doesImplement(net2, build);
      
      assertEquals(lastTest, diag.result, NetSynthesis.COMPARE_EQUAL);
      
    } catch (IOException e) {
      System.err.println("Couldn't read test file: "+e);
      assertTrue(lastTest, false);
    } catch (InvalidModelException e) {
      assertTrue(lastTest, false);
    }
  }
}
