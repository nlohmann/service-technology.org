package hub.top.uma;

import hub.top.petrinet.ISystemModel;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.Place;
import hub.top.petrinet.Transition;
import hub.top.petrinet.unfold.DNodeSys_OccurrenceNet;
import hub.top.uma.DNodeSet.DNodeSetElement;
import hub.top.uma.synthesis.NetSynthesis;
import hub.top.uma.synthesis.TransitiveDependencies;
import hub.top.uma.view.MineSimplify;
import hub.top.uma.view.ViewGeneration2;
import hub.top.uma.view.MineSimplify.Configuration;

import java.awt.List;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintStream;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Set;

import org.junit.Test;

import com.google.gwt.dev.util.collect.HashSet;

public class UmaImpliedTest extends hub.top.test.TestCase {
  
  public UmaImpliedTest() {
    super("UmaImpliedTest");
  }
 
  public static void main(String[] args) {
    setParameters(args);
    junit.textui.TestRunner.run(UmaTest.class);
  }
  
  @Test
  public void testMineImplied_preserve() {
    lastTest = "Test Implied, preserve behavior";
    System.out.println(lastTest);

    try {
      Configuration config = new Configuration();
      config.abstract_chains = false;
      config.unfold_refold = false;
      config.remove_flower_places = false;
      config.remove_implied = Configuration.REMOVE_IMPLIED_PRESERVE_ALL;
      
      MineSimplify simplify = new MineSimplify(testFileRoot+"/pn_ex_01_alpha.lola", testFileRoot+"/pn_ex_01_alpha_aligned.log.txt", config);
      simplify.prepareModel();
      simplify.run();

      boolean p1=false,p2=false,p3=false;
      for (Place p : simplify.result.removedImpliedPlaces) {
        if (p.getName().contains("__P2+ate_abort___-->___P3+complete__P3+pi_abort__")) p1 = true;
        if (p.getName().contains("__P3+complete___-->___P4+schedule__P2+pi_abort__")) p2 = true;
        if (p.getName().contains("__P2+suspend___-->___P2+resume__")) p3 = true;
      }
      
      assertEquals(lastTest+": removed implied places", 3, simplify.result.removedImpliedPlaces.size());
      assertTrue(lastTest+": removed place p1", p1);
      assertTrue(lastTest+": removed place p2", p2);
      assertTrue(lastTest+": removed place p3", p3);
      
    } catch (InvalidModelException e) {
      System.err.println("Invalid model: "+e);
      assertTrue(lastTest, false);
    } catch (IOException e) {
      System.err.println("Couldn't read test file: "+e);
      assertTrue(lastTest, false);
    }
  }

  @Test
  public void testMineImplied_all() {
    lastTest = "Test Implied, preserve connectedness";
    System.out.println(lastTest);

    try {
      Configuration config = new Configuration();
      config.abstract_chains = false;
      config.unfold_refold = false;
      config.remove_flower_places = false;
      config.remove_implied = Configuration.REMOVE_IMPLIED_PRESERVE_CONNECTED;
      
      MineSimplify simplify = new MineSimplify(testFileRoot+"/pn_ex_01_alpha.lola", testFileRoot+"/pn_ex_01_alpha_aligned.log.txt", config);
      simplify.prepareModel();
      simplify.run();

      boolean p1=false,p2=false;
      for (Place p : simplify.result.removedImpliedPlaces) {
        if (p.getName().contains("__P1+complete__P3+ate_abort___-->___P2+schedule__")) p1 = true;
        if (p.getName().contains("__P3+schedule___-->___P3+withdraw__P2+resume__")) p2 = true;
      }
      
      assertEquals(lastTest+": removed implied places", 2, simplify.result.removedImpliedPlaces.size());
      assertTrue(lastTest+": removed place p1", p1);
      assertTrue(lastTest+": removed place p2", p2);
      
    } catch (InvalidModelException e) {
      System.err.println("Invalid model: "+e);
      assertTrue(lastTest, false);
    } catch (IOException e) {
      System.err.println("Couldn't read test file: "+e);
      assertTrue(lastTest, false);
    }
  }

}
