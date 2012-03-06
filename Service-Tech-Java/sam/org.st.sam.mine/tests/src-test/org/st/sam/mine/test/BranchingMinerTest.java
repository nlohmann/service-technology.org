package org.st.sam.mine.test;


import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Scanner;

import org.junit.Test;
import org.st.sam.log.SScenario;
import org.st.sam.mine.MineBranchingLSC;
import org.st.sam.mine.MineLSC;

import junit.framework.Assert;
import junit.framework.TestSuite;

public class BranchingMinerTest extends TestSuite {
  
  public static String defaultTestDir = "./tests/testfiles";
  
  public static String readFile(File f) throws FileNotFoundException {
    StringBuilder text = new StringBuilder();
    String NL = System.getProperty("line.separator");
    Scanner scanner = new Scanner(new FileInputStream(f));
    try {
      while (scanner.hasNextLine()){
        text.append(scanner.nextLine() + NL);
      }
    }
    finally{
      scanner.close();
    }
    return text.toString();
  }
  
  public static void checkOutputGeneral(String sourceFile, int expectedScenarios) throws IOException {
    File f1 = new File(sourceFile+".dot");
    Assert.assertTrue("Deleted tree representation of the log", f1.delete());
    File f2 = new File(sourceFile+"_cov.dot");
    Assert.assertTrue("Deleted coverage tree of the mined scenarios", f2.delete());
    File f3 = new File(sourceFile+"_all_lscs.txt");
    File f3_expected = new File(sourceFile+"_all_lscs.expected.txt");
    
    String scenarios = readFile(f3);
    String scenarios_expected = readFile(f3_expected);
    
    Assert.assertEquals("Discovered scenarios are correct.", scenarios_expected, scenarios);
    Assert.assertTrue("Deleted found LSCs", f3.delete());


    // expect 11 scenarios
    for (int i=1; i<=expectedScenarios; i++) {
      File f = new File(sourceFile+"_cov_"+i+".dot");
      Assert.assertTrue("Deleted coverage tree of scenario #"+i, f.delete());
    }
    File f = new File(sourceFile+"_cov_"+(expectedScenarios+1)+".dot");
    Assert.assertFalse("No unexpected scenario #"+(expectedScenarios+1), f.exists());
  }

	@Test
	public void mine_test_crossftp_succinct () throws Exception {
		
		String testFileRoot = System.getProperty("test.testFileRoot", defaultTestDir);
		String sourceFile = testFileRoot+"/crossftp_succinct_traceset.xes.gz";
		
		MineLSC miner = new MineBranchingLSC();
		miner.mineLSCs_writeResults(sourceFile, 40, 1.0);
		
		checkOutputGeneral(sourceFile, 1);
	}
	
	@Test
	public void mine_test_crossftp_invariants () throws Exception {
	    
    String testFileRoot = System.getProperty("test.testFileRoot", defaultTestDir);
    String sourceFile = testFileRoot+"/crossftp_invariants.xes.gz";
    
    MineLSC miner = new MineBranchingLSC();
    miner.mineLSCs_writeResults(sourceFile, 80, 1.0);
    
    checkOutputGeneral(sourceFile, 109);
  }
	
	@Test
	public void test_scenario_subsumption() {
	  
	  short[] pre1 = new short[] { 15 };
	  short[] main1 = new short[] { 16, 17, 18, 19, 20, 30, 31, 32, 35, 36, 42, 43, 44, 57, 58, 59, 60, 61, 62, 13, 14 };
	  
	  short[] pre2 = new short[] { 19 };
    short[] main2 = new short[] {                 20, 30, 31, 32, 35, 36, 42, 43, 44, 57, 58, 59, 60, 61, 62, 13, 14 };
    
    SScenario s1 = new SScenario(pre1, main1);
    SScenario s2 = new SScenario(pre2, main2);
	  
    Assert.assertEquals(true, s1.implies(s2));
	}
	
	@Test
  public void mine_test_jeti_invariants () throws Exception {
      
    String testFileRoot = System.getProperty("test.testFileRoot", defaultTestDir);
    String sourceFile = testFileRoot+"/jeti_invariants.xes.gz";
    
    MineLSC miner = new MineBranchingLSC();
    miner.mineLSCs_writeResults(sourceFile, 10, 1.0);
    
    checkOutputGeneral(sourceFile, 95);
  }
}
