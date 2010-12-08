package hub.top.test;

public class TestCase extends junit.framework.TestCase{
  
  public TestCase (String name) {
    super();
    setName(name);
  }
  
  public static String testFileRoot = System.getProperty("test.testFileRoot", ".");;
  
  public static String lastTest = "";
  public int testNum = 0;
  public int testFail = 0;
  
  private static final String RESULT_FAIL = "[failed]";
  private static final String RESULT_OK = "[ok]";
  
  public void assertTrue2(boolean result) {
    assertEquals2(result, true);
  }
  
  public void assertFalse2(boolean result) {
    assertEquals2(result, false);
  }
  
  public void assertEquals2(Object result, Object expected) {
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
    int fill = 79 - (5 + getName().length());
    String header = "=== "+getName()+" ";
    for (int i=0;i<fill;i++) header += "=";
    
    System.out.println(header);
  }
  
  public void evaluateTests() {
    System.out.println("-------------------------------------------------------------------------------");
    System.out.println(" executed "+testNum+" test cases, failed: "+testFail);
    System.out.println("===============================================================================\n");
  }
  
  public static void setParameters(String args[]) {
    if (args.length > 0) testFileRoot = args[0];
  }
}
