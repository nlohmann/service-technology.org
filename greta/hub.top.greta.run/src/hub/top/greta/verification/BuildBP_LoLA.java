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

import hub.top.editor.eclipse.FileIOHelper;
import hub.top.editor.lola.text.ModelEditor;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.greta.oclets.canonical.DNodeBP;
import hub.top.greta.oclets.canonical.InvalidModelException;

import java.io.PrintStream;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IProgressMonitor;


/**
 * A wrapper class for verifying a Petri net in LoLA format with the Oclet
 * Branching Process algorithm. This wrapper invokes a parser that reads the
 * file, constructs the internal system representation, and invokes the
 * verification algorithm.
 * 
 * To execute, construct a new object with the source file that stores the
 * model. Then call {@link BuildBP_LoLA#run(IProgressMonitor, PrintStream)}
 * for instance from within a {@link org.eclipse.core.runtime.jobs.Job}.
 * 
 * @author Dirk Fahland
 */
public class BuildBP_LoLA {
  
  private DNodeBP bp;
  private IFile   srcFile;
  
  private boolean writeBPtoFile = true;
  private boolean printStatistic = true;
  
  private long analysisTime = 0;
  private long parseTime = 0;
  private long prepareTime = 0;
  
  public BuildBP_LoLA(IFile srcFile) throws InvalidModelException {
    this.srcFile = srcFile;
    
    parseTime = -System.currentTimeMillis();
    String netText = FileIOHelper.readFile(srcFile);
    PtNet ptnet = ModelEditor.getModel(netText);
    parseTime += System.currentTimeMillis();
    
    prepareTime = -System.currentTimeMillis();
    this.bp = new DNodeBP(ptnet);
    prepareTime += System.currentTimeMillis();
  }
  
  /**
   * Set whether the resulting branching process shall be written to a file.
   * The standard output format is GraphViz Dot 
   * @param doWrite
   */
  public void setWriteBPtoFile(boolean doWrite) {
    writeBPtoFile = doWrite;
  }
  
  /**
   * Set whether statistics on the size of the branching process shall be
   * written to the output.
   * @param doPrint
   */
  public void setPrintStatistics(boolean doPrint) {
    printStatistic = doPrint;
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
    out.println("------- constructing branching process II -------");
    out.println("input file: "+srcFile.getFullPath().toString());
    
    int steps = 0;
    int current_steps = 0;
    int num = 0;
    
    boolean interrupted = false;
    long tStart = System.currentTimeMillis();
    while ((current_steps = bp.step()) > 0) {
      steps += current_steps;

      monitor.subTask("explored "+steps+" events");
      
      if (monitor.isCanceled()) {
        interrupted = true;
        break;
      }
      
      out.print(steps+"... ");
      if (num++ > 10) { System.out.print("\n"); num = 0; }
    }
    
    out.println();
    
    monitor.subTask("searching for dead conditions");
    bp.findDeadConditions(true);

    //monitor.subTask("searching for unsafe markings (globally)");
    //bp.isGloballySafe();
    
    // compute time until all analysis is done
    long tEnd = System.currentTimeMillis();
    analysisTime = tEnd-tStart;

    if (interrupted) out.print("\ninterrupted ");
    else out.print("\ndone ");
    out.println("after "+steps+" steps, "+(analysisTime)+"ms");
    
    if (printStatistic)
      out.println(bp.getStatistics());
    
    if (writeBPtoFile) {
      if (srcFile != null) {
        monitor.subTask("writing dot file");
        IOUtil.writeDotFile(bp, srcFile, "_bp");
      }
    }

    /*
    if (interrupted)
      return false;
    else */
      return true;
  }
  
  /**
   * @return time for parsing the source file into the intermediate format
   */
  public long getParseTime() {
    return parseTime;
  }
  
  /**
   * @return analysis time after finishing the verification
   */
  public long getAnalysisTime() {
    return analysisTime;
  }
  
  /**
   * @return time for preparing the model for verification
   */
  public long getPrepareTime() {
    return prepareTime;
  }
  
  /**
   * @return the constructed branching process
   */
  public DNodeBP getBranchingProcess() {
    return bp;
  }
}
