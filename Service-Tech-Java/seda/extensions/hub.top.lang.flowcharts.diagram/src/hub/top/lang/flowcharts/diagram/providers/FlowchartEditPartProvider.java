/*
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - FlowChart Editors
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
 * The Original Code is this file as it was released on July 30, 2008.
 * The Initial Developer of the Original Code is
 *  		Dirk Fahland
 *  
 * Portions created by the Initial Developer are Copyright (c) 2008
 * the Initial Developer. All Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the EPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the EPL, the GPL or the LGPL.
 */
package hub.top.lang.flowcharts.diagram.providers;

import hub.top.lang.flowcharts.diagram.edit.parts.FlowchartEditPartFactory;
import hub.top.lang.flowcharts.diagram.edit.parts.WorkflowDiagramEditPart;
import hub.top.lang.flowcharts.diagram.part.FlowchartVisualIDRegistry;

import java.lang.ref.WeakReference;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;
import org.eclipse.gmf.runtime.common.core.service.IOperation;
import org.eclipse.gmf.runtime.diagram.ui.editparts.IGraphicalEditPart;
import org.eclipse.gmf.runtime.diagram.ui.services.editpart.AbstractEditPartProvider;
import org.eclipse.gmf.runtime.diagram.ui.services.editpart.CreateGraphicEditPartOperation;
import org.eclipse.gmf.runtime.diagram.ui.services.editpart.IEditPartOperation;
import org.eclipse.gmf.runtime.notation.View;

/**
 * @generated
 */
public class FlowchartEditPartProvider extends AbstractEditPartProvider {

  /**
   * @generated
   */
  private EditPartFactory factory;

  /**
   * @generated
   */
  private boolean allowCaching;

  /**
   * @generated
   */
  private WeakReference cachedPart;

  /**
   * @generated
   */
  private WeakReference cachedView;

  /**
   * @generated
   */
  public FlowchartEditPartProvider() {
    setFactory(new FlowchartEditPartFactory());
    setAllowCaching(true);
  }

  /**
   * @generated
   */
  public final EditPartFactory getFactory() {
    return factory;
  }

  /**
   * @generated
   */
  protected void setFactory(EditPartFactory factory) {
    this.factory = factory;
  }

  /**
   * @generated
   */
  public final boolean isAllowCaching() {
    return allowCaching;
  }

  /**
   * @generated
   */
  protected synchronized void setAllowCaching(boolean allowCaching) {
    this.allowCaching = allowCaching;
    if (!allowCaching) {
      cachedPart = null;
      cachedView = null;
    }
  }

  /**
   * @generated
   */
  protected IGraphicalEditPart createEditPart(View view) {
    EditPart part = factory.createEditPart(null, view);
    if (part instanceof IGraphicalEditPart) {
      return (IGraphicalEditPart) part;
    }
    return null;
  }

  /**
   * @generated
   */
  protected IGraphicalEditPart getCachedPart(View view) {
    if (cachedView != null && cachedView.get() == view) {
      return (IGraphicalEditPart) cachedPart.get();
    }
    return null;
  }

  /**
   * @generated
   */
  public synchronized IGraphicalEditPart createGraphicEditPart(View view) {
    if (isAllowCaching()) {
      IGraphicalEditPart part = getCachedPart(view);
      cachedPart = null;
      cachedView = null;
      if (part != null) {
        return part;
      }
    }
    return createEditPart(view);
  }

  /**
   * @generated
   */
  public synchronized boolean provides(IOperation operation) {
    if (operation instanceof CreateGraphicEditPartOperation) {
      View view = ((IEditPartOperation) operation).getView();
      if (!WorkflowDiagramEditPart.MODEL_ID.equals(FlowchartVisualIDRegistry
          .getModelID(view))) {
        return false;
      }
      if (isAllowCaching() && getCachedPart(view) != null) {
        return true;
      }
      IGraphicalEditPart part = createEditPart(view);
      if (part != null) {
        if (isAllowCaching()) {
          cachedPart = new WeakReference(part);
          cachedView = new WeakReference(view);
        }
        return true;
      }
    }
    return false;
  }
}
