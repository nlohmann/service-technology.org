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

public class SScenarioTest extends TestSuite {
  
	@Test
	public void test_scenario_subsumption() {
	  
	  short[] pre1 = new short[] { 15 };
	  short[] main1 = new short[] { 16, 17, 18, 19, 20, 30, 31, 32, 35, 36, 42, 43, 44, 57, 58, 59, 60, 61, 62, 13, 14 };
	  
	  short[] pre2 = new short[] { 19 };
    short[] main2 = new short[] {                 20, 30, 31, 32, 35, 36, 42, 43, 44, 57, 58, 59, 60, 61, 62, 13, 14 };
    
    SScenario s1 = new SScenario(pre1, main1, 255);
    SScenario s2 = new SScenario(pre2, main2, 255);
	  
    Assert.assertEquals(true, s1.implies(s2));
	}
	
	@Test
	public void test_scenario_implies1() {

	  SScenario s1 = new SScenario(new short[] {     184, 185, 186,           115}, new short[] {197, 189, 190, 191, 5}, 255);
	  SScenario s2 = new SScenario(new short[] {190, 184, 185, 186, 187, 188, 115}, new short[] {197, 189 }, 255);

	  Assert.assertEquals(true, s1.implies(s2));
	}

	@Test
	public void test_scenario_implies2() {

	  SScenario s1 = new SScenario(new short[] {75, 211}, new short[] {212, 23}, 255);
	  SScenario s2 = new SScenario(new short[] {75, 211}, new short[] {23}, 255);

	  Assert.assertEquals(true, s1.implies(s2));
	}

  @Test
  public void test_scenario_implies3() {

    SScenario s1 = new SScenario(new short[] {5, 184, 185, 188}, new short[] {189, 190, 191}, 255);
    SScenario s2 = new SScenario(new short[] {5, 184, 185, 188, 197}, new short[] {190}, 255);

    Assert.assertEquals(true, s1.implies(s2));
  }


  @Test
  public void test_scenario_implies4() {

    SScenario s1 = new SScenario(new short[] {5,      184, 187, 188, 197}, new short[] {189, 190,               191}, 255);
    SScenario s2 = new SScenario(new short[] {5, 192, 184, 187, 188, 197,                    190}, new short[] {191}, 255);

    Assert.assertEquals(true, s1.implies(s2));
  }


  @Test
  public void test_scenario_implies5() {

    SScenario s1 = new SScenario(new short[] {5, 184, 187, 188},      new short[] {189, 190,               191}, 255);
    SScenario s2 = new SScenario(new short[] {5, 184, 187, 188, 197}, new short[] {189}, 255);

    Assert.assertEquals(true, s1.implies(s2));
  }


  @Test
  public void test_scenario_implies6() {
    SScenario s1 = new SScenario(new short[] {5,  184, 185, 186, 187, 115}, new short[] {197, 189, 190, 191}, 255);
    SScenario s2 = new SScenario(new short[] {34, 184, 185, 186, 187, 115}, new short[] {197, 189, 190, 191}, 255);
    
    Assert.assertEquals(false, s1.implies(s2));
  }
  
  @Test
  public void test_scenario_implies7() {
    SScenario s1 = new SScenario(new short[] {184}, new short[] {185, 186,               187, 192}, 255);
    SScenario s2 = new SScenario(                        new short[] {186}, new short[] {187, 192}, 255);
    
    Assert.assertEquals(true, s1.implies(s2));
  }

}
