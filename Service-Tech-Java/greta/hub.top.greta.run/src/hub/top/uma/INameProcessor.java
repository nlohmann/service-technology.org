package hub.top.uma;

public interface INameProcessor {
  
  public static final INameProcessor IDENTITY = new INameProcessor() {
    public String process(String name) {
      return name;
    }
  };
  
  public static final INameProcessor HLtoLL = new INameProcessor() {
    
    public String process(String name) {
      int lpar = name.indexOf('(');
      if (lpar == -1) return name;
      
      String realName = name.substring(
          name.indexOf('(')+1,
          name.indexOf(','));
      String parameter = name.substring(
          name.indexOf(',')+1,
          name.indexOf(')'));
      
      return realName;
    }
  };

  public String process(String name);
  
}
