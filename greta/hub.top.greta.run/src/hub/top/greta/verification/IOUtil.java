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

import hub.top.greta.oclets.canonical.DNodeBP;
import hub.top.greta.run.actions.ActionHelper;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;

public class IOUtil {

  /**
   * Write a dot file that graphically represents the given branching process
   * into a file. The method will take the inputFile, strip its extension, and
   * append a suffix and the standard extension ".dot". For instance the
   * branching process of "<code>/a/input.model</code>" is written to
   * "<code>/a/input_bp.dot</code>" 
   * 
   * @param bp
   * @param inputFile
   * @param suffix
   */
  public static void writeDotFile (DNodeBP bp, IFile inputFile, String suffix) {

    String targetPathStr = inputFile.getFullPath().removeFileExtension().toString();
    IPath targetPath = new Path(targetPathStr+suffix+".dot");

    ActionHelper.writeFile (targetPath, bp.toDot());
  }
}
