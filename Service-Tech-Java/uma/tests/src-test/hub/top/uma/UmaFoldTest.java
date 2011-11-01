package hub.top.uma;

import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.Place;
import hub.top.petrinet.unfold.DNodeSys_OccurrenceNet;
import hub.top.uma.DNodeSet.DNodeSetElement;
import hub.top.uma.synthesis.NetSynthesis;
import hub.top.uma.view.MineSimplify;
import hub.top.uma.view.MineSimplify.Configuration;
import hub.top.uma.view.ViewGeneration2;

import java.io.IOException;
import java.util.LinkedList;

import org.junit.Test;

import com.google.gwt.dev.util.collect.HashSet;

public class UmaFoldTest extends hub.top.test.TestCase {
  
  public UmaFoldTest() {
    super("UmaFoldTest");
  }
 
  public static void main(String[] args) {
    setParameters(args);
    junit.textui.TestRunner.run(UmaTest.class);
  }
  
  @Test
  public void testMineSimplify_basic() {
    lastTest = "Simplify mined process model";
    System.out.println(lastTest);

    try {
      MineSimplify simplify = new MineSimplify(testFileRoot+"/a12f0n05.lola", testFileRoot+"/a12f0n05.log.txt");
      simplify.prepareModel();
      simplify.run();
      PetriNet simplifiedNet = simplify.getSimplifiedNet();
      
      float compBefore = MineSimplify.complexitySimple(simplify.getOriginalNet());
      float compAfter = MineSimplify.complexitySimple(simplifiedNet);
      
      assertTrue(lastTest+": model is simpler", compBefore > compAfter);
      assertFalse(lastTest+": model is non-empty", simplifiedNet.getTransitions().isEmpty());
      assertFalse(lastTest+": model is non-empty", simplifiedNet.getPlaces().isEmpty());
      assertFalse(lastTest+": model is non-empty", simplifiedNet.getArcs().isEmpty());
      
    } catch (InvalidModelException e) {
      System.err.println("Invalid model: "+e);
      assertTrue(lastTest, false);
    } catch (IOException e) {
      System.err.println("Couldn't read test file: "+e);
      assertTrue(lastTest, false);
    }

  }
  
  @Test
  public void testMineSimplify_mapping() {
    lastTest = "Simplify mined process model, map event names";
    System.out.println(lastTest);
    
    try {
      Configuration config = new Configuration();
      
      // create mapping from "Name+complete" to "Name"
      LinkedList<String[]> log = ViewGeneration2.readTraces(testFileRoot+"/a12f0n05.log.txt");
      for (String[] trace : log) {
        for (String event : trace) {
          config.eventToTransition.put(event, event.substring(0, event.indexOf('+')));
        }
      }
      
      // call miner with this event-to-transition mapping
      MineSimplify simplify = new MineSimplify(testFileRoot+"/a12f0n05_shortened.lola", testFileRoot+"/a12f0n05.log.txt", config);
      simplify.prepareModel();
      simplify.run();
      PetriNet simplifiedNet = simplify.getSimplifiedNet();
      
      float compBefore = MineSimplify.complexitySimple(simplify.getOriginalNet());
      float compAfter = MineSimplify.complexitySimple(simplifiedNet);
      
      assertTrue(lastTest+": model is simpler", compBefore > compAfter);
      assertFalse(lastTest+": model is non-empty", simplifiedNet.getTransitions().isEmpty());
      assertFalse(lastTest+": model is non-empty", simplifiedNet.getPlaces().isEmpty());
      assertFalse(lastTest+": model is non-empty", simplifiedNet.getArcs().isEmpty());
      
    } catch (InvalidModelException e) {
      System.err.println("Invalid model: "+e);
      assertTrue(lastTest, false);
    } catch (IOException e) {
      System.err.println("Couldn't read test file: "+e);
      assertTrue(lastTest, false);
    }

  }
  
  @Test
  public void testUnfoldRefold_bpStruct_acyclic () {
    lastTest = "Unfold and refold model (acyclic)";
    System.out.println(lastTest);
    
    try {
      
      PetriNet occnet = PetriNetIO.readNetFromFile(testFileRoot+"/unfold_refold1.lola");
      occnet.turnIntoLabeledNet();
      
      DNodeSys_OccurrenceNet sys = new DNodeSys_OccurrenceNet(occnet, new HashSet<Place>());
      DNodeRefold build = Uma.initBuildPrefix_View(sys, 0);

      Uma.out.println("equivalence..");
      build.futureEquivalence();


      Uma.out.println("join maximal..");
      build.extendFutureEquivalence_maximal();
      Uma.out.println("fold backwards..");
      while (build.extendFutureEquivalence_backwards()) {
        Uma.out.println("fold backwards..");
      }
      
      while (build.refineFutureEquivalence_removeSuperfluous()) {
        Uma.out.println("remove superfluous..");
      }

      hub.top.uma.synthesis.EquivalenceRefineSuccessor splitter = new hub.top.uma.synthesis.EquivalenceRefineSuccessor(build); 

      Uma.out.println("relax..");
      build.relaxFutureEquivalence(splitter);
      Uma.out.println("determinize..");

      NetSynthesis synth = new NetSynthesis(build);
      DNodeSetElement nonImplied = new DNodeSetElement();
      for (DNode d : build.getBranchingProcess().getAllNodes())
        if (!d.isImplied) nonImplied.add(d);
      PetriNet net = synth.foldToNet_labeled(nonImplied, false);
      
      assertTrue("Size of folded model",
          net.getPlaces().size() == 7
          && net.getTransitions().size() == 6
          && net.getArcs().size() == 15);

    
    } catch (InvalidModelException e) {
      System.err.println("Invalid model: "+e);
      assertTrue(lastTest, false);
    } catch (IOException e) {
      System.err.println("Couldn't read test file: "+e);
      assertTrue(lastTest, false);
    }
  }
  
  @Test
  public void testUnfoldRefold_bpStruct_acyclic2 () {
    lastTest = "Unfold and refold model (acyclic)";
    System.out.println(lastTest);
    
    try {
      
      PetriNet occnet = PetriNetIO.readNetFromFile(testFileRoot+"/unfold_refold2.lola");
      occnet.turnIntoLabeledNet();
      
      DNodeSys_OccurrenceNet sys = new DNodeSys_OccurrenceNet(occnet, new HashSet<Place>());
      DNodeRefold build = Uma.initBuildPrefix_View(sys, 0);

      Uma.out.println("equivalence..");
      build.futureEquivalence();

      Uma.out.println("join maximal..");
      build.extendFutureEquivalence_maximal();
      Uma.out.println("fold backwards..");
      while (build.extendFutureEquivalence_backwards()) {
        Uma.out.println("fold backwards..");
      }
      
      while (build.refineFutureEquivalence_removeSuperfluous()) {
        Uma.out.println("remove superfluous..");
      }

      hub.top.uma.synthesis.EquivalenceRefineSuccessor splitter = new hub.top.uma.synthesis.EquivalenceRefineSuccessor(build); 

      Uma.out.println("relax..");
      build.relaxFutureEquivalence(splitter);
      Uma.out.println("determinize..");

      NetSynthesis synth = new NetSynthesis(build);
      DNodeSetElement nonImplied = new DNodeSetElement();
      for (DNode d : build.getBranchingProcess().getAllNodes())
        if (!d.isImplied) nonImplied.add(d);
      PetriNet net = synth.foldToNet_labeled(nonImplied, false);
      
      assertTrue("Size of folded model",
          net.getPlaces().size() == 8
          && net.getTransitions().size() == 7
          && net.getArcs().size() == 17);
          
      
      PetriNetIO.writeToFile(net, testFileRoot+"/unfold_refold2_result.dot", PetriNetIO.FORMAT_DOT, 0);

    
    } catch (InvalidModelException e) {
      System.err.println("Invalid model: "+e);
      assertTrue(lastTest, false);
    } catch (IOException e) {
      System.err.println("Couldn't read test file: "+e);
      assertTrue(lastTest, false);
    }
  }

}
