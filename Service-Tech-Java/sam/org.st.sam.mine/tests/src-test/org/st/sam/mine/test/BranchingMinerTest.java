package org.st.sam.mine.test;


import java.io.File;

import org.junit.Test;
import org.st.sam.mine.MineBranchingLSC;

import junit.framework.Assert;
import junit.framework.TestSuite;

public class BranchingMinerTest extends TestSuite {
  
  public static String defaultTestDir = "./tests/testfiles";

  public void checkOutputGeneral(String sourceFile, int expectedScenarios) {
    File f1 = new File(sourceFile+".dot");
    Assert.assertTrue("Deleted tree representation of the log", f1.delete());
    File f2 = new File(sourceFile+"_cov.dot");
    Assert.assertTrue("Deleted coverage tree of the mined scenarios", f2.delete());

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
		
		MineBranchingLSC miner = new MineBranchingLSC();
		miner.mineLSCs_writeResults(sourceFile, 10, 1.0);
		
		checkOutputGeneral(sourceFile, 11);
	}
	
	@Test(expected=junit.framework.AssertionFailedError.class)
	public void mine_test_crossftp_invariants () throws Exception {
	    
    String testFileRoot = System.getProperty("test.testFileRoot", defaultTestDir);
    String sourceFile = testFileRoot+"/crossftp_invariants.xes.gz";
    
    MineBranchingLSC miner = new MineBranchingLSC();
    miner.mineLSCs_writeResults(sourceFile, 40, 1.0);
    
    checkOutputGeneral(sourceFile, 165);
  }
	
	@Test
  public void mine_test_jeti_invariants () throws Exception {
      
    String testFileRoot = System.getProperty("test.testFileRoot", defaultTestDir);
    String sourceFile = testFileRoot+"/jeti_invariants.xes.gz";
    
    MineBranchingLSC miner = new MineBranchingLSC();
    miner.mineLSCs_writeResults(sourceFile, 10, 1.0);
    
    checkOutputGeneral(sourceFile, 18);
  }
}
