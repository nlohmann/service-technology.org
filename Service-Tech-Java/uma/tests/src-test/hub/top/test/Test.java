package hub.top.test;

public class Test {
  
  public Test (String name) {
    super();
    testName = name;
  }
  
  public String testFileRoot = "";
  public String testName;
  
  public String lastTest = "";
  public int testNum = 0;
  public int testFail = 0;
  
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
    
    testNum++;
    if (!resultMatch) testFail++;
  }
  
  public void printHeader() {
    int fill = 79 - (5 + testName.length());
    String header = "=== "+testName+" ";
    for (int i=0;i<fill;i++) header += "=";
    
    System.out.println(header);
  }
  
  public void evaluateTests() {
    System.out.println("-------------------------------------------------------------------------------");
    System.out.println(" executed "+testNum+" test cases, failed: "+testFail);
    System.out.println("===============================================================================\n");
  }
  
  public void setParameters(String args[]) {
    if (args.length > 0) testFileRoot = args[0];
  }
  
  public void executeTests() {
    printHeader();
    run();
    evaluateTests();
  }
  
  public void run() {
    
  }

}
