package hub.top.uma;

import hub.top.petrinet.ISystemModel;
import hub.top.petrinet.PetriNet;
import hub.top.petrinet.PetriNetIO;
import hub.top.petrinet.Place;
import hub.top.petrinet.unfold.DNodeSys_OccurrenceNet;
import hub.top.uma.DNodeSet.DNodeSetElement;
import hub.top.uma.synthesis.NetSynthesis;
import hub.top.uma.synthesis.TransitiveDependencies;
import hub.top.uma.view.MineSimplify;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintStream;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Set;

import com.google.gwt.dev.util.collect.HashSet;

public class UmaFoldTest extends hub.top.test.TestCase {
  
  public UmaFoldTest() {
    super("UmaFoldTest");
  }
 
  public static void main(String[] args) {
    setParameters(args);
    junit.textui.TestRunner.run(UmaTest.class);
  }
  
  public void testMineSimplify_basic() {
    lastTest = "Simplify mined process model";

    try {
      MineSimplify simplify = new MineSimplify(testFileRoot+"/a12f0n05.lola", testFileRoot+"/a12f0n05.log.txt");
      simplify.prepareModel();
      simplify.run();
      PetriNet simplifiedNet = simplify.getSimplifiedNet();
      
      float compBefore = MineSimplify.complexitySimple(simplify.getOriginalNet());
      float compAfter = MineSimplify.complexitySimple(simplifiedNet);
      
      assertTrue(lastTest+": model is simpler", compBefore > compAfter);
      
    } catch (InvalidModelException e) {
      System.err.println("Invalid model: "+e);
      assertTrue(lastTest, false);
    } catch (IOException e) {
      System.err.println("Couldn't read test file: "+e);
      assertTrue(lastTest, false);
    }

  }
  
  public void testUnfoldRefold_bpStruct_acyclic () {
    lastTest = "Unfold and refold model (acyclic)";
    
    try {
      
      PetriNet occnet = PetriNetIO.readNetFromFile(testFileRoot+"/unfold_refold1.lola");
      occnet.turnIntoLabeledNet();
      
      DNodeSys_OccurrenceNet sys = new DNodeSys_OccurrenceNet(occnet, new HashSet<Place>());
      DNodeBP build = Uma.initBuildPrefix_View(sys, 0);

      Uma.out.println("equivalence..");
      build.foldingEquivalence();


      Uma.out.println("join maximal..");
      build.extendFoldingEquivalence_maximal();
      Uma.out.println("fold backwards..");
      while (build.extendFoldingEquivalence_backwards()) {
        Uma.out.println("fold backwards..");
      }
      
      while (build.refineFoldingEquivalence_removeSuperfluous()) {
        Uma.out.println("remove superfluous..");
      }

      hub.top.uma.synthesis.EquivalenceRefineSuccessor splitter = new hub.top.uma.synthesis.EquivalenceRefineSuccessor(build); 

      Uma.out.println("relax..");
      build.relaxFoldingEquivalence(splitter);
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
  
  public void testUnfoldRefold_bpStruct_acyclic2 () {
    lastTest = "Unfold and refold model (acyclic)";
    
    try {
      
      PetriNet occnet = PetriNetIO.readNetFromFile(testFileRoot+"/unfold_refold2.lola");
      occnet.turnIntoLabeledNet();
      
      DNodeSys_OccurrenceNet sys = new DNodeSys_OccurrenceNet(occnet, new HashSet<Place>());
      DNodeBP build = Uma.initBuildPrefix_View(sys, 0);

      Uma.out.println("equivalence..");
      build.foldingEquivalence();

      Uma.out.println("join maximal..");
      build.extendFoldingEquivalence_maximal();
      Uma.out.println("fold backwards..");
      while (build.extendFoldingEquivalence_backwards()) {
        Uma.out.println("fold backwards..");
      }
      
      while (build.refineFoldingEquivalence_removeSuperfluous()) {
        Uma.out.println("remove superfluous..");
      }

      hub.top.uma.synthesis.EquivalenceRefineSuccessor splitter = new hub.top.uma.synthesis.EquivalenceRefineSuccessor(build); 

      Uma.out.println("relax..");
      build.relaxFoldingEquivalence(splitter);
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
