package hub.top.greta.cpn;

import java.util.Map;

import hub.top.uma.DNode;
import hub.top.uma.DNodeSys_AdaptiveSystem;

public class ColoredConditionVisitor {
  
  private DNodeSys_AdaptiveSystem system;
  
  public ColoredConditionVisitor(DNodeSys_AdaptiveSystem system) {
    this.system = system;
  }
  
  public boolean satisfiesColoring(Map<DNode, DNode> embedding) {
    
    // TODO: build evaluation string
    /* let
     *   val x1 = v1;
     *   val x2 = v2;
     * in
     *   constraints over x1, x2, ...
     * end; 
     */
    // TODO: evaluate
    for (Map.Entry<DNode, DNode> m : embedding.entrySet()) {
      System.out.println(m.getKey()+" --> "+system.getOriginalNode(m.getValue()));
    }
    
    return true;
  }

}
