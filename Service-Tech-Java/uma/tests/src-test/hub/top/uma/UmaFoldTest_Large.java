package hub.top.uma;

import hub.top.petrinet.PetriNet;
import hub.top.uma.view.MineSimplify;
import hub.top.uma.view.MineSimplify.Configuration;

import java.io.IOException;

import org.junit.Test;

public class UmaFoldTest_Large extends hub.top.test.TestCase {
  
  public UmaFoldTest_Large() {
    super("UmaFoldTest_Large");
  }
 
  public static void main(String[] args) {
    setParameters(args);
    junit.textui.TestRunner.run(UmaTest.class);
  }

  @Test
  public void testRegression_fold_maximal_conditions() {
    lastTest = "testRegression_fold_maximal_conditions()";
    System.out.println(lastTest);

    try {
      Configuration config = new Configuration();
      config.unfold_refold = true;
      config.remove_implied = Configuration.REMOVE_IMPLIED_PRESERVE_ALL;
      config.abstract_chains = true;
      config.remove_flower_places = false;
      config.filter_threshold = 0;
      
      MineSimplify simplify = new MineSimplify(testFileRoot+"/regression_fold_maximal_a22f0n05.lola", testFileRoot+"/regression_fold_maximal_a22f0n05.log.txt", config);
      simplify.prepareModel();
      simplify.run();
      PetriNet simplifiedNet = simplify.getSimplifiedNet();
      
      assertEquals(lastTest+": number of places", 36, simplifiedNet.getPlaces().size());
      assertEquals(lastTest+": number of transitions", 22, simplifiedNet.getTransitions().size());
      assertEquals(lastTest+": number of arcs", 188, simplifiedNet.getArcs().size());
      
    } catch (InvalidModelException e) {
      System.err.println("Invalid model: "+e);
      assertTrue(lastTest, false);
    } catch (IOException e) {
      System.err.println("Couldn't read test file: "+e);
      assertTrue(lastTest, false);
    }
  }
  
  @Test
  public void testRegression_fold_maximal_conditions_filter05() {
    lastTest = "testRegression_fold_maximal_conditions()";
    System.out.println(lastTest);

    try {
      Configuration config = new Configuration();
      config.unfold_refold = true;
      config.remove_implied = Configuration.REMOVE_IMPLIED_PRESERVE_ALL;
      config.abstract_chains = true;
      config.remove_flower_places = false;
      config.filter_threshold = 0.05;
      
      MineSimplify simplify = new MineSimplify(testFileRoot+"/regression_fold_maximal_a22f0n05.lola", testFileRoot+"/regression_fold_maximal_a22f0n05.log.txt", config);
      simplify.prepareModel();
      simplify.run();
      PetriNet simplifiedNet = simplify.getSimplifiedNet();
      
      assertEquals(lastTest+": number of places", 38, simplifiedNet.getPlaces().size());
      assertEquals(lastTest+": number of transitions", 22, simplifiedNet.getTransitions().size());
      assertEquals(lastTest+": number of arcs", 186, simplifiedNet.getArcs().size());
      
    } catch (InvalidModelException e) {
      System.err.println("Invalid model: "+e);
      assertTrue(lastTest, false);
    } catch (IOException e) {
      System.err.println("Couldn't read test file: "+e);
      assertTrue(lastTest, false);
    }
  }
}
