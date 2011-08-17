package org.st.sam.mine.test;


import java.io.File;

import org.junit.Test;
import org.st.sam.mine.MineBranchingLSC;

import junit.framework.Assert;
import junit.framework.TestSuite;

public class BranchingMinerTest extends TestSuite {
  
  public static String defaultTestDir = "./tests/testfiles";


	@Test
	public void mine_test_1 () throws Exception {
		
		String testFileRoot = System.getProperty("test.testFileRoot", defaultTestDir);
		String sourceFile = testFileRoot+"/crossftp_succinct_traceset.xes";
		
		MineBranchingLSC miner = new MineBranchingLSC();
		miner.mineLSCs(sourceFile, 10, 1.0);
		
		File f1 = new File(sourceFile+".dot");
		Assert.assertTrue("Deleted tree representation of the log", f1.delete());
    File f2 = new File(sourceFile+"_cov.dot");
    Assert.assertTrue("Deleted coverage tree of the mined scenarios", f2.delete());
	}
}
