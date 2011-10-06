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

package hub.top.greta.run.actions;

//import hub.top.adaptiveProcess.diagram.part.AdaptiveProcessDiagramViewer;
import hub.top.adaptiveSystem.AdaptiveProcess;
import hub.top.adaptiveSystem.AdaptiveSystem;
import hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditor;
import hub.top.adaptiveSystem.presentation.AdaptiveSystemEditor;
import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.uma.DNodeBP;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.IPath;
import org.eclipse.core.runtime.Path;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.ResourceSet;
import org.eclipse.emf.ecore.resource.impl.ResourceSetImpl;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.jface.dialogs.MessageDialog;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorPart;

public class ActionHelper extends hub.top.editor.eclipse.ActionHelper {

	/**
	 * Retrieve {@link AdaptiveSystem} object from the current editor,
	 * if this editor is known and stores such an object.
	 * 
	 * @param editor
	 * @return
	 */
	public static AdaptiveSystem getAdaptiveSystem(IEditorPart editor) {
		AdaptiveSystem adaptiveSystem = null;
		if (editor instanceof AdaptiveSystemDiagramEditor) {
			AdaptiveSystemDiagramEditor adaptiveSystemDiagramEditor = (AdaptiveSystemDiagramEditor) editor;
			adaptiveSystem = (AdaptiveSystem) adaptiveSystemDiagramEditor.getDiagram().getElement();
			
		} else if (editor instanceof AdaptiveSystemEditor) {
			AdaptiveSystemEditor adaptiveSystemEditor = (AdaptiveSystemEditor)editor;
			Resource r0 = adaptiveSystemEditor.getEditingDomain().getResourceSet().getResources().get(0);
			if (r0.getContents().get(0) instanceof AdaptiveSystem) {
				adaptiveSystem = (AdaptiveSystem)r0.getContents().get(0);
			}
			
			/*
		// FIXME: remove this dependency from the hub.top.GRETA.diagram.process package
		} else if (editor instanceof AdaptiveProcessDiagramViewer) {
			AdaptiveProcessDiagramViewer apViewer = (AdaptiveProcessDiagramViewer)editor;
			AdaptiveProcess ap = (AdaptiveProcess)apViewer.getDiagram().getElement();
			adaptiveSystem = (AdaptiveSystem)ap.eContainer();
      */

		}
		
		return adaptiveSystem;
	}
	
	 /**
   * Retrieve {@link Diagram} object from the current editor, if this editor 
   * if it represents an {@link AdaptiveSystem} the editor stores such an object.
   * 
   * @param editor
   * @return
   */
  public static Diagram getAdaptiveSystemDiagram(IEditorPart editor) {
    Diagram d = null;
    if (editor instanceof AdaptiveSystemDiagramEditor) {
      AdaptiveSystemDiagramEditor adaptiveSystemDiagramEditor = (AdaptiveSystemDiagramEditor) editor;
      d = adaptiveSystemDiagramEditor.getDiagram();
      
    }
    
    return d;
  }
	
	/**
	 * Retrieve {@link AdaptiveSystem} object from the resource at
	 * the given uri if the resource stores an {@link AdaptiveSystem}.
	 * 
	 * @param uri
	 * @return
	 */
	public static AdaptiveSystem getAdaptiveSystem(URI uri) {
		AdaptiveSystem adaptiveSystem = null;
		
		if (uri != null) {
			ResourceSet rs = new ResourceSetImpl();
			Resource r = rs.getResource(uri, true);
			
			// check the contents of the resource
			if (r.getContents() != null && r.getContents().size() > 0) {
				// first element only
				EObject o = r.getContents().get(0);
				if (o instanceof AdaptiveSystem) {
					// the resource stores an adaptive system object, return it
					adaptiveSystem = (AdaptiveSystem)o;
				} else if (o instanceof Diagram) {
					// the resource stores a diagram object
					if ( ((Diagram)o).getElement() instanceof AdaptiveSystem ) {
						// which refers to an adaptive system object, return that
						adaptiveSystem = (AdaptiveSystem)((Diagram)o).getElement();
					}  else if ( ((Diagram)o).getElement() instanceof AdaptiveProcess ) {
						// which refers to an adaptive process object,
						// which is contained in an adaptive process, return that
						adaptiveSystem = (AdaptiveSystem)((Diagram)o).getElement().eContainer();
					}
				}
			}
		}
		return adaptiveSystem;
	}
	
  /**
   * Retrieve {@link PtNet} object from the resource at
   * the given uri if the resource stores an {@link PtNet}.
   * 
   * @param uri
   * @return
   */
  public static PtNet getPtNet(URI uri) {
    PtNet net = null;
    
    if (uri != null) {
      ResourceSet rs = new ResourceSetImpl();
      Resource r = rs.getResource(uri, true);
      
      // check the contents of the resource
      if (r.getContents() != null && r.getContents().size() > 0) {
        // first element only
        EObject o = r.getContents().get(0);
        if (o instanceof PtNet) {
          // the resource stores an adaptive system object, return it
          net = (PtNet)o;
        } else if (o instanceof Diagram) {
          // the resource stores a diagram object
          if ( ((Diagram)o).getElement() instanceof PtNet ) {
            // which refers to an adaptive system object, return that
            net = (PtNet)((Diagram)o).getElement();
          }  else if ( ((Diagram)o).getElement() instanceof PtNet ) {
            // which refers to an adaptive process object,
            // which is contained in an adaptive process, return that
            net = (PtNet)((Diagram)o).getElement().eContainer();
          }
        }
      }
    }
    return net;
  }
  
	/**
	 * Write given {@link DNodeBP} as dot graphics and write it at the given location.
	 * @param bp
	 * @param inputFile
	 * @param suffix
	 */
  public static void writeDotFile (DNodeBP bp, IFile inputFile, String suffix) {

    String targetPathStr = inputFile.getFullPath().removeFileExtension().toString();
    IPath targetPath = new Path(targetPathStr+suffix+".dot");

    ActionHelper.writeFile (targetPath, bp.toDot());
  }
  
  public static void showMessageDialogue(final int kind, final String title, final String message) {
    Display.getDefault().syncExec(new Runnable() {
      public void run() {         
        MessageDialog.open(kind, null, title, message, SWT.NONE);
      }
    });
  }
}
