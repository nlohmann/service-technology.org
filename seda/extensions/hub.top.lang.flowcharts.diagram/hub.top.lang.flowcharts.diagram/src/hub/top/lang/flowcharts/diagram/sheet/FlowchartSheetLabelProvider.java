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
package hub.top.lang.flowcharts.diagram.sheet;

import hub.top.lang.flowcharts.diagram.navigator.FlowchartNavigatorGroup;
import hub.top.lang.flowcharts.diagram.part.FlowchartDiagramEditorPlugin;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryLabelProvider;
import org.eclipse.gef.EditPart;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.jface.viewers.DecoratingLabelProvider;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.swt.graphics.Image;

/**
 * @generated
 */
public class FlowchartSheetLabelProvider extends DecoratingLabelProvider {

  /**
   * @generated
   */
  public FlowchartSheetLabelProvider() {
    super(new AdapterFactoryLabelProvider(FlowchartDiagramEditorPlugin
        .getInstance().getItemProvidersAdapterFactory()), null);
  }

  /**
   * @generated
   */
  public String getText(Object element) {
    Object selected = unwrap(element);
    if (selected instanceof FlowchartNavigatorGroup) {
      return ((FlowchartNavigatorGroup) selected).getGroupName();
    }
    return super.getText(selected);
  }

  /**
   * @generated
   */
  public Image getImage(Object element) {
    return super.getImage(unwrap(element));
  }

  /**
   * @generated
   */
  private Object unwrap(Object element) {
    if (element instanceof IStructuredSelection) {
      return unwrap(((IStructuredSelection) element).getFirstElement());
    }
    if (element instanceof EditPart) {
      return unwrapEditPart((EditPart) element);
    }
    if (element instanceof IAdaptable) {
      View view = (View) ((IAdaptable) element).getAdapter(View.class);
      if (view != null) {
        return unwrapView(view);
      }
    }
    return element;
  }

  /**
   * @generated
   */
  private Object unwrapEditPart(EditPart p) {
    if (p.getModel() instanceof View) {
      return unwrapView((View) p.getModel());
    }
    return p.getModel();
  }

  /**
   * @generated
   */
  private Object unwrapView(View view) {
    return view.getElement() == null ? view : view.getElement();
  }

}
