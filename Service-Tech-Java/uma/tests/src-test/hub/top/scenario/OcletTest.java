package hub.top.scenario;

import hub.top.petrinet.PetriNet;
import hub.top.uma.InvalidModelException;

import java.io.IOException;

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
    assertEquals(result, true);
  }
  
  public void assertFalse(boolean result) {
    assertEquals(result, false);
  }
  
  public void assertEquals(Object result, Object expected) {
    String testMessage = lastTest;
    String resultMessage = result+", expected: "+expected;
    boolean resultMatch = result.equals(expected);
    String resultString = (resultMatch ? RESULT_OK : RESULT_FAIL);
    
    int fill = 77 - (testMessage.length() + resultMessage.length() + resultString.length());
    String fillString1 = "";
    String fillString2 = "";
    if (fill >= 1) {
      fillString1 = ": ";
      for (int i=0; i < fill; i++) fillString2 += " ";
    } else {
      fillString1 += ":\n  ";
      fill = 77 - (resultMessage.length() + resultString.length());
      for (int i=0; i < fill; i++) fillString2 += " ";
    }
    System.out.println(testMessage+fillString1+resultMessage+fillString2+resultString);
    
    if (!resultMatch) testFail++;
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

    try {
      PetriNet net = OcletIO.readNetFromFile("./testfiles/oclet_syntax_1.oclets");
      OcletSpecification os = new OcletSpecification(net);
      Oclet o = os.getOclets().getFirst();
    
      //assertTrue(o.isCausalNet() == true && o.historyIsPrefix() == false);
      int error = 0;
      try {
        o.isValidOclet();
      } catch (InvalidModelException e) {
        error = e.getReason();
      }
      
      assertEquals(error, InvalidModelException.OCLET_HISTORY_NOT_PREFIX );
    
    } catch (IOException e) {
      System.err.println("Couldn't read test file: "+e);
      assertTrue(false);
    }
  }
  
  public void testOcletCycle_fail () {
    
    lastTest = "Detect cycle in oclet";
    
    try {
      PetriNet net = OcletIO.readNetFromFile("./testfiles/oclet_syntax_2.oclets");
      OcletSpecification os = new OcletSpecification(net);
      Oclet o = os.getOclets().getFirst();
    
      int error = 0;
      try {
        o.isValidOclet();
      } catch (InvalidModelException e) {
        error = e.getReason();
      }
      
      assertEquals(error, InvalidModelException.OCLET_NO_CAUSALNET );
    
    } catch (IOException e) {
      System.err.println("Couldn't read test file: "+e);
      assertTrue(false);
    }
      
  }
  
  public void testOcletAll_yes () {
    
    lastTest = "Check valid oclet";
    
    try {
      PetriNet net = OcletIO.readNetFromFile("./testfiles/oclet_syntax_3.oclets");
      OcletSpecification os = new OcletSpecification(net);
      Oclet o = os.getOclets().getFirst();
    
      int error = 0;
      try {
        o.isValidOclet();
      } catch (InvalidModelException e) {
        error = e.getReason();
      }
      
      assertEquals(error, 0 );
    
    } catch (IOException e) {
      System.err.println("Couldn't read test file: "+e);
      assertTrue(false);
    }
  }  
}
