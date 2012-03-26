/*****************************************************************************\
 * Copyright (c) 2008-2012 Dirk Fahland. EPL1.0/AGPL3.0
 * All rights reserved.
 * 
 * ServiceTechnolog.org - Greta
 *                       (Graphical Runtime Environment for Adaptive Processes) 
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License v1.0, which accompanies this
 * distribution, and is available at http://www.eclipse.org/legal/epl-v10.html
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * 
 * The Original Code is this file as it was released on December 04, 2008.
 * The Initial Developer of the Original Code are
 *    Dirk Fahland
 * 
 * Portions created by the Initial Developer are Copyright (c) 2008, 2009
 * the Initial Developer. All Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Alternatively, the contents of this file may be used under the terms of
 * the GNU Affero General Public License Version 3 or later (the "GPL") in
 * which case the provisions of the AGPL are applicable instead of those above.
 * If you wish to allow use of your version of this file only under the terms
 * of the AGPL and not to allow others to use your version of this file under
 * the terms of the EPL, indicate your decision by deleting the provisions
 * above and replace them with the notice and other provisions required by the 
 * AGPL. If you do not delete the provisions above, a recipient may use your
 * version of this file under the terms of any one of the EPL or the AGPL.
\*****************************************************************************/

package hub.top.greta.cpn;

import java.util.HashMap;
import java.util.Map;

import hub.top.adaptiveSystem.Condition;
import hub.top.uma.DNode;
import hub.top.uma.DNodeEmbeddingVisitor;
import hub.top.uma.DNodeSys_AdaptiveSystem;

public class ColoredConditionVisitor extends DNodeEmbeddingVisitor {
  
  private DNodeSys_AdaptiveSystem system;
  
  public ColoredConditionVisitor(DNodeSys_AdaptiveSystem system) {
    this.system = system;
  }
  
  protected static String[] getTupleParts(String exp) {
    if (exp.length() > 2  && exp.charAt(0)=='(' && exp.charAt(exp.length()-1) == ')') {
          
      String[] arguments = exp.substring(1, exp.length()-1).split(",");
      for (int j=0; j<arguments.length; j++) {
        // strip surrounding white-spaces
        while (Character.isWhitespace(arguments[j].charAt(0)))
          arguments[j] = arguments[j].substring(1);
        while (Character.isWhitespace(arguments[j].charAt(arguments[j].length()-1)))
          arguments[j] = arguments[j].substring(0,arguments[j].length()-1);
      }
      
      return arguments;
    }

    return new String[] { exp };
  }
  
  protected static boolean isVariable(String exp) {
    for (int i=0; i<exp.length(); i++) {
      char c = exp.charAt(i);
      if (   !Character.isJavaIdentifierPart(c)
          && !Character.isLetterOrDigit(c)
          && !Character.isWhitespace(c)) {
        return false;
      }
    }
    return true;
  }

  private Map<String, String> getBinding(DNode systemNode, DNode runNode) {
    
    Map<String, String> binding = new HashMap<String, String>();
    
    
    if (!systemNode.isEvent) {
      
      Condition c_sys = (Condition)system.getOriginalNode(systemNode);
      Condition c_run = (Condition)system.getOriginalNode(runNode);
      
      String variables = AdaptiveSystemToCPN.getToken(c_sys.getName());
      String values = AdaptiveSystemToCPN.getToken(c_run.getName());
      
      String vars[] = getTupleParts(variables);
      String vals[] = getTupleParts(values);
      if (vars.length != vals.length) {
        System.err.println("Error! Trying to match tuples of different length");
        for (String s : vars) System.out.print(s+" "); System.out.println();
        for (String s : vals) System.out.print(s+" "); System.out.println();
      } else {
        for (int i=0; i<vars.length; i++) {
          if (!isVariable(vars[i])) continue;
          binding.put(vars[i], vals[i]);
        }
      }
      
    }
    
    return binding;
  }
  
  private Map<String, String> getBinding(Map<DNode, DNode> embedding) {
    
    Map<String, String> binding = new HashMap<String, String>();
    
    for (Map.Entry<DNode, DNode> e : embedding.entrySet()) {
      binding.putAll(getBinding(e.getKey(), e.getValue()));
    }
    
    return binding;
  }
  
  private boolean isConsistent(Map<String, String> binding1, Map<String, String> binding2) {
    for (Map.Entry<String, String> b : binding2.entrySet()) {
      String var = b.getKey();
      if (binding1.containsKey(var) && !binding1.get(var).equals(binding2.get(var))) return false;
    }
    return true;
  }
  
  @Override
  public boolean canBeExtended(DNode toEmbed, DNode embedIn) {
    if (!super.canBeExtended(toEmbed, embedIn)) return false;
    if (!getEmbedding().containsKey(toEmbed) && toEmbed instanceof Condition) {
      
      // TODO: build evaluation string
      /* let
       *   val x1 = v1;
       *   val x2 = v2;
       * in
       *   constraints over x1, x2, ...
       * end; 
       */
      // TODO: evaluate
      
      Map<String, String> existingBinding = getBinding(getEmbedding());
      Map<String, String> newBinding = getBinding(toEmbed, embedIn);
      if (!isConsistent(existingBinding, newBinding)) return false;
    }
    
    return true;
  }
  
}
