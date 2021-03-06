/*****************************************************************************\
 * Copyright (c) 2008, 2009. All rights reserved. Dirk Fahland. EPL1.0/AGPL3.0
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
 * The Original Code is this file as it was released on June 6, 2009.
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

package hub.top.greta.verification;

import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.greta.run.actions.ActionHelper;
import hub.top.uma.DNode;
import hub.top.uma.DNodeBP;
import hub.top.uma.DNodeBP_Scenario;
import hub.top.uma.DNodeRefold;
import hub.top.uma.DNodeSys_AdaptiveSystem;
import hub.top.uma.DNodeSys_PtNet;
import hub.top.uma.InvalidModelException;
import hub.top.uma.Options;

import java.io.PrintStream;
import java.util.LinkedList;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IProgressMonitor;

public class BuildBP {
  
  // limit construction of complete finite prefix to systems with bound MAX_BOUND
  // if no other parameter is supplied
  private static int MAX_BOUND = 3;

  private       DNodeRefold bp;
  private long  analysisTime = 0;
  private int   steps = 0;
  
  private IFile srcFile = null;
  
  /**
   * @param system
   * @param srcFile
   */
  public BuildBP(DNodeRefold bp, IFile srcFile) {
    this.bp = bp;
    this.srcFile = srcFile;
  }
  
  /**
   * @param system
   */
  public BuildBP(DNodeRefold bp) {
    this(bp, null);
  }
  
  /**
   * @param adaptiveSystem
   */
  public BuildBP(AdaptiveSystem adaptiveSystem) throws InvalidModelException {
    this(BuildBP.init(adaptiveSystem));
  }
  
  /**
   * @param adaptiveSystem
   */
  public BuildBP(PtNet net) throws InvalidModelException {
    this(BuildBP.init(net));
  }
  
  /**
   * @param adaptiveSystem
   * @param srcFile
   */
  public BuildBP(AdaptiveSystem adaptiveSystem, IFile srcFile) throws InvalidModelException {
    this(BuildBP.init(adaptiveSystem), srcFile);
  }
  
  /**
   * @param adaptiveSystem
   * @param srcFile
   */
  public BuildBP(PtNet net, IFile srcFile) throws InvalidModelException  {
    this(BuildBP.init(net), srcFile);
  }
  
  /**
   * Run the verification and generation of result files. Call from within
   * a {@link org.eclipse.core.runtime.jobs.Job} or with
   * a {@link org.eclipse.core.runtime.NullProgressMonitor} 
   *  
   * @param monitor
   * @param out
   * @return
   */
  public boolean run(IProgressMonitor monitor, PrintStream out)
  {
    // console output
    if (out != null) out.println("------- constructing branching process II -------");

    int current_steps = 0;
    int num = 0;
    
    boolean interrupted = false;
    // branching process construction loop
    // repeat step() as long as an event can be fired
    long tStart = System.currentTimeMillis();
    while ((current_steps = BuildBP.step(bp)) > 0) {
      steps += current_steps;

      // graphical output and UI interaction
      if (monitor != null) {
        monitor.subTask("BP: explored "+steps+" events");
        
        if (monitor.isCanceled()) {
          interrupted = true;
          break;
        }
      }
      
      // console output
      if (out != null) {
        //out.print(steps+".. ");
        //if (num++ > 10) { System.out.print("\n"); num = 0; }
      }
    } // end of BP construction loop
    long tEnd = System.currentTimeMillis();
    analysisTime = (tEnd - tStart);
    
    // console output
    if (out != null) { 
      out.println();
      if (interrupted) out.print("\ninterrupted ");
      else out.print("\ndone ");
    }

    /*
    if (interrupted)
      return false;
    else */
      return true;
  }
  
  public int analyze(IProgressMonitor monitor, PrintStream out) {
    
    boolean hasDeadlock = bp.findDeadConditions(true);
    boolean isUnsafe = !bp.isGloballySafe();
    
    int result = DNodeBP.PROP_NONE;
    
    if (hasDeadlock) {
      if (out != null) out.println("has a deadlock.");
      result |= DNodeBP.PROP_DEADCONDITION;
    }
    if (isUnsafe) {
      if (out != null) out.println("is unsafe.");
      result |= DNodeBP.PROP_UNSAFE;
    }
    
    if (bp instanceof DNodeBP_Scenario) {
      DNodeBP_Scenario bps = (DNodeBP_Scenario)bp;
    
      LinkedList<DNode> implied = bps.getImpliedScenarios();
      if (!implied.isEmpty()) {
        if (out != null) out.println("has implied behavior (by incomplete successor events):.");
        for (DNode d : implied) {
          if (out != null) out.println(d); 
        }
      }
      long startTime = System.currentTimeMillis();
      implied = bps.getImpliedScenarios_global();
      if (!implied.isEmpty()) {
        if (out != null) out.println("has implied behavior (global history not found):");
        for (DNode d : implied) {
          if (out != null) out.println(d); 
        }
      }
      long endTime = System.currentTimeMillis();
      if (out != null) out.println("time for checking implied scenarios (global): "+(endTime-startTime)+"ms");
    }
    
    return result;
  }
  
  public boolean minimize(IProgressMonitor monitor, PrintStream out) {
    try {
      bp.minimizeFoldingRelation();
    } catch (NullPointerException e) {
      
    }
    return true;
  }
  
  public boolean writeBPtoFile(IProgressMonitor monitor, PrintStream out,
        String outFileAppendix) {
    if (srcFile != null) {
      monitor.subTask("writing dot file");
      ActionHelper.writeDotFile(bp, srcFile, outFileAppendix);
      /*
      DNodeSet.option_printAnti = false;
      IOUtil.writeDotFile(bp, srcFile, outFileAppendix+"_positive");
      DNodeSet.option_printAnti = true;
      
      try {
        FileIOHelper.writeFile(srcFile.getRawLocation()+outFileAppendix+".csv", bp._debug_executionTimeProfile.toString());
        FileIOHelper.writeFile(srcFile.getRawLocation()+outFileAppendix+".log", bp._debug_log.toString());
      } catch (IOException e) {
        
      }
      */
      return true;
    }
    return false;
  }
  
  public void printStatistics(PrintStream out) {
    out.println("after "+steps+" steps, "+(analysisTime)+"ms");
    out.println(bp.getStatistics(false));

  }
  
  /**
   * @return the constructed branching process
   */
  public DNodeRefold getBranchingProcess() {
    return bp;
  }
  
  public static DNodeRefold init(AdaptiveSystem adaptiveSystem) throws InvalidModelException {
    DNodeSys_AdaptiveSystem system = new DNodeSys_AdaptiveSystem(adaptiveSystem);
    
    Options o = new Options(system);
    o.configure_buildOnly();

    DNodeRefold bp = new DNodeBP_Scenario(system, o);
    //bp.configure_stopIfUnSafe();
    return bp;
  }
  
  public static DNodeRefold initSynthesis(AdaptiveSystem adaptiveSystem) throws InvalidModelException {
    DNodeSys_AdaptiveSystem system = new DNodeSys_AdaptiveSystem(adaptiveSystem);
    
    Options o = new Options(system);
    o.configure_synthesis();
    o.configure_stopIfUnSafe();

    DNodeRefold bp = new DNodeBP_Scenario(system, o);
    return bp;
  }
  
  public static DNodeRefold init(PtNet net) throws InvalidModelException {
    DNodeSys_PtNet system = new DNodeSys_PtNet(net);
    
    Options o = new Options(system);
    o.configure_buildOnly();
    o.configure_stopIfUnSafe();
    
    DNodeRefold bp = new DNodeRefold(system, o);
    return bp;
  }
  
  public static int step(DNodeBP bp) {
    return bp.step();
  }

  public static void finish(DNodeBP bp) {
    
  }
}
