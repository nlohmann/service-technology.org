package hub.top.scenario;

import java.util.LinkedList;

public class OcletTest {
  
  public String lastTest = "";
  public int testNum = 0;
  public int testFail = 0;
  
  public OcletTest(String name) {
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
    testOcletPrefix_fail();
    testOcletCycle_fail();
    testOcletAll_yes();
  }

  public static void main(String[] args) {
    OcletTest t = new OcletTest("Oclets");
    t.run();
  }
  
  public void testOcletPrefix_fail () {
    
    lastTest = "Detect wrong prefix in oclet";
    
    LinkedList<String> b_h = new LinkedList<String>();
    b_h.add("p1a"); b_h.add("p1b"); b_h.add("p1c");
    b_h.add("p2a"); b_h.add("p2b"); b_h.add("p2c");
    
    LinkedList<String> b_c = new LinkedList<String>();
    b_c.add("p3a"); b_c.add("p3b"); b_c.add("p3c");
    
    LinkedList<String[]> arcs = new LinkedList<String[]>();
    
    LinkedList<String> e_h = new LinkedList<String>();
    e_h.add("e1a");
    // CONSUME
      arcs.add( new String[] { "p1a", "e1a"} );
      arcs.add( new String[] { "p1b", "e1a"} );
    // PRODUCE
      arcs.add( new String[] { "e1a", "p2a"} );
      
    e_h.add("e1b");
    // CONSUME
      arcs.add( new String[] { "p1c", "e1b"} );
    // PRODUCE
      arcs.add( new String[] { "e1b", "p2b"} );
      arcs.add( new String[] { "e1b", "p2c"} );
      
    LinkedList<String> e_c = new LinkedList<String>();
      
    e_c.add("e2a");
    // CONSUME
      arcs.add( new String[] { "p2a", "e2a"} );
      arcs.add( new String[] { "p2b", "e2a"} );
    // PRODUCE
      arcs.add( new String[] { "e2a", "p3a"} );
      
    e_c.add("e2b");
    // CONSUME
      arcs.add( new String[] { "p2c", "e2b"} );
    // PRODUCE
      arcs.add( new String[] { "e2b", "p3b"} );
      arcs.add( new String[] { "e2b", "p3c"} );
      
    e_c.add("e3");
    // CONSUME
      arcs.add( new String[] { "p3c", "e3"} );
    // PRODUCE
      arcs.add( new String[] { "e3", "p1b"} );
      
    Oclet o = new Oclet(false, b_h, e_h, b_c, e_c, arcs);
    
    assertTrue(o.isCausalNet() == true && o.historyIsPrefix() == false);
  }
  
  public void testOcletCycle_fail () {
    
    lastTest = "Detect cycle in oclet";
    
    LinkedList<String> b_h = new LinkedList<String>();
    b_h.add("p1a"); b_h.add("p1b"); b_h.add("p1c");
    b_h.add("p2a"); b_h.add("p2b"); b_h.add("p2c");
    
    LinkedList<String> b_c = new LinkedList<String>();
    b_c.add("p3a"); b_c.add("p3b"); b_c.add("p3c");
    
    LinkedList<String[]> arcs = new LinkedList<String[]>();
    
    LinkedList<String> e_h = new LinkedList<String>();
    e_h.add("e1a");
    // CONSUME
      arcs.add( new String[] { "p1a", "e1a"} );
      arcs.add( new String[] { "p1b", "e1a"} );
    // PRODUCE
      arcs.add( new String[] { "e1a", "p2a"} );
      
    e_h.add("e1b");
    // CONSUME
      arcs.add( new String[] { "p1c", "e1b"} );
    // PRODUCE
      arcs.add( new String[] { "e1b", "p2b"} );
      arcs.add( new String[] { "e1b", "p2c"} );
      
    LinkedList<String> e_c = new LinkedList<String>();
      
    e_c.add("e2a");
    // CONSUME
      arcs.add( new String[] { "p2a", "e2a"} );
      arcs.add( new String[] { "p2b", "e2a"} );
    // PRODUCE
      arcs.add( new String[] { "e2a", "p3a"} );
      
    e_c.add("e2b");
    // CONSUME
      arcs.add( new String[] { "p2c", "e2b"} );
    // PRODUCE
      arcs.add( new String[] { "e2b", "p3b"} );
      arcs.add( new String[] { "e2b", "p3c"} );
      
    e_c.add("e3");
    // CONSUME
      arcs.add( new String[] { "p3a", "e3"} );
    // PRODUCE
      arcs.add( new String[] { "e3", "p1b"} );
      
    Oclet o = new Oclet(false, b_h, e_h, b_c, e_c, arcs);
    
    assertFalse( o.isCausalNet() );
  }
  
  public void testOcletAll_yes () {
    
    lastTest = "Check valid oclet";
    
    LinkedList<String> b_h = new LinkedList<String>();
    b_h.add("p1a"); b_h.add("p1b"); b_h.add("p1c");
    b_h.add("p2a"); b_h.add("p2b"); b_h.add("p2c");
    
    LinkedList<String> b_c = new LinkedList<String>();
    b_c.add("p3a"); b_c.add("p3b"); b_c.add("p3c"); b_c.add("p4");
    
    LinkedList<String[]> arcs = new LinkedList<String[]>();
    
    LinkedList<String> e_h = new LinkedList<String>();
    e_h.add("e1a");
    // CONSUME
      arcs.add( new String[] { "p1a", "e1a"} );
      arcs.add( new String[] { "p1b", "e1a"} );
    // PRODUCE
      arcs.add( new String[] { "e1a", "p2a"} );
      
    e_h.add("e1b");
    // CONSUME
      arcs.add( new String[] { "p1c", "e1b"} );
    // PRODUCE
      arcs.add( new String[] { "e1b", "p2b"} );
      arcs.add( new String[] { "e1b", "p2c"} );
      
    LinkedList<String> e_c = new LinkedList<String>();
      
    e_c.add("e2a");
    // CONSUME
      arcs.add( new String[] { "p2a", "e2a"} );
      arcs.add( new String[] { "p2b", "e2a"} );
    // PRODUCE
      arcs.add( new String[] { "e2a", "p3a"} );
      
    e_c.add("e2b");
    // CONSUME
      arcs.add( new String[] { "p2c", "e2b"} );
    // PRODUCE
      arcs.add( new String[] { "e2b", "p3b"} );
      arcs.add( new String[] { "e2b", "p3c"} );
      
    e_c.add("e3");
    // CONSUME
      arcs.add( new String[] { "p3c", "e3"} );
    // PRODUCE
      arcs.add( new String[] { "e3", "p4"} );
      
    Oclet o = new Oclet(false, b_h, e_h, b_c, e_c, arcs);
    
    assertTrue( o.isValidOclet() );
  }  
}
