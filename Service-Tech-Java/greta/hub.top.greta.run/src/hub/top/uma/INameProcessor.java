package hub.top.uma;

import hub.top.greta.cpn.AdaptiveSystemToCPN;

public interface INameProcessor {
  
  public static final INameProcessor IDENTITY = new INameProcessor() {
    public String process(Object o, String name) {
      return name;
    }
  };
  
  public static final INameProcessor HLtoLL = new INameProcessor() {
    
    public String process(Object o, String name) {
      
      if (o instanceof hub.top.adaptiveSystem.Condition) {
        int lpar = name.indexOf('(');
        if (lpar == -1) return name;
        
        return AdaptiveSystemToCPN.getPlaceName(name);
        
      } else if (o instanceof hub.top.adaptiveSystem.Event) {
        
        return AdaptiveSystemToCPN.getTransitionName(name);
      }
      
      return name;
    }
  };

  public String process(Object o, String name);
  
}
