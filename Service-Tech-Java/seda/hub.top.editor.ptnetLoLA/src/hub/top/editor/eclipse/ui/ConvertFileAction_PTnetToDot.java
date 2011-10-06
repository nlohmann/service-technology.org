/*****************************************************************************\
 * Copyright (c) 2008-2010. All rights reserved. Dirk Fahland. EPL1.0/AGPL3.0
 * 
 *  ServiceTechnolog.org - PetriNet Editor Framework
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

package hub.top.editor.eclipse.ui;

import java.util.HashMap;

import hub.top.editor.eclipse.FileIOHelper;
import hub.top.editor.ptnetLoLA.PNAPI;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.resource.Activator;

import org.eclipse.core.resources.IFile;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.common.util.WrappedException;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.ResourceSet;
import org.eclipse.emf.transaction.TransactionalEditingDomain;

public class ConvertFileAction_PTnetToDot extends ConvertFileAction {

  /*
   * (non-Javadoc)
   * @see hub.top.editor.eclipse.ui.ConvertFileAction#getModelObject(org.eclipse.emf.transaction.TransactionalEditingDomain, org.eclipse.emf.common.util.URI)
   */
  @Override
  protected Object getModelObject(TransactionalEditingDomain editingDomain,
      URI uri) {
    ResourceSet resourceSet = editingDomain.getResourceSet();
    EObject modelRoot = null;
    try {
      Resource resource = resourceSet.getResource(uri, true);
      modelRoot = (EObject) resource.getContents().get(0);
    } catch (WrappedException ex) {
      Activator.getPluginHelper().logError(
          "Unable to load resource: " + uri, ex);
    }
    return modelRoot;
  }

  /**
   * create a new file wizard that writes the Petri net to DOT
   * 
   * @see ConvertFileAction#createConvertFileWizard(URI, Object, TransactionalEditingDomain)
   */
  @Override
  public ConvertFileWizard createConvertFileWizard(URI srcURI, Object root,
      TransactionalEditingDomain editingDomain) {
    
    ConvertFileWizard wizard = new ConvertFileWizard(srcURI, root, editingDomain, "dot") {
      @Override
      public boolean performFinish() {
        
        if (modelObject instanceof PtNet) {
          PtNet net = (PtNet)modelObject;
          
          HashMap<String, Integer> cluster = new HashMap<String, Integer>();

          int cCUST = 0;
          int cGFZ = 1;
          int cPART = 2;

          int cGFZ_int = 3;
          int cPART_int = 4;
          int cTF = 5;
          
          /*
          cluster.put("eq", cTF);
          cluster.put("car", cTF);
          cluster.put("map", cTF);
          cluster.put("check car rentals", cTF);
          cluster.put("rent car", cTF);
          cluster.put("ask for maps", cTF);
          cluster.put("get maps", cTF);
          cluster.put("transport equipment to storage", cTF);
          cluster.put("begin mission", cTF);
          cluster.put("no car available", cTF);
          cluster.put("!request vehicle", cTF);
          cluster.put("?requested vehicle", cTF);
          cluster.put("contact embassy", cTF);
          cluster.put("?letter", cTF);
          cluster.put("!req missing", cTF);
          cluster.put("?missing docs", cTF);
               */ 
          cluster.put("GFZ", cGFZ);
          cluster.put("?support", cGFZ);
          cluster.put("!letter", cGFZ);
          cluster.put("!missing", cGFZ);
          cluster.put("?req missing", cGFZ);
          /*
          cluster.put("support", cGFZ_int);
          cluster.put("letter", cGFZ_int);
          cluster.put("req missing docs", cGFZ_int);
          cluster.put("missing docs", cGFZ_int);
          */
          cluster.put("partner", cPART);
          cluster.put("?request_", cPART);
          cluster.put("provide jeep", cPART);
          cluster.put("?map req", cPART);
          cluster.put("!maps", cPART);
          /*
          cluster.put("map_", cPART_int);
          cluster.put("map req", cPART_int);
          cluster.put("request vehicle", cPART_int);
          cluster.put("granted vehicle", cPART_int);
          */
          cluster.put("cust", cCUST);
          cluster.put("present equipment", cCUST);
          cluster.put("clear", cCUST);
          cluster.put("demonstration of devices", cCUST);
          cluster.put("documents missing", cCUST);
          cluster.put("clear some", cCUST);
          
          IFile targetFile = pageFileCreation.createNewFile();
          FileIOHelper.setCharset(targetFile);
          FileIOHelper.writeFile(targetFile, PNAPI.toDot(net, cluster));
    
          return true;
        }
        return false;
      }
    };
    wizard.setWindowTitle("Convert Petri net to Graphviz dot");
    return wizard;
  }

}
