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

import java.util.ArrayList;
import java.util.Iterator;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.edit.domain.AdapterFactoryEditingDomain;
import org.eclipse.emf.edit.provider.IItemPropertySource;
import org.eclipse.emf.edit.ui.provider.PropertySource;
import org.eclipse.emf.transaction.TransactionalEditingDomain;
import org.eclipse.emf.transaction.util.TransactionUtil;
import org.eclipse.gef.EditPart;
import org.eclipse.gmf.runtime.diagram.ui.properties.sections.AdvancedPropertySection;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.StructuredSelection;
import org.eclipse.ui.IWorkbenchPart;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySourceProvider;

/**
 * @generated
 */
public class FlowchartPropertySection extends AdvancedPropertySection implements
    IPropertySourceProvider {

  /**
   * @generated
   */
  public IPropertySource getPropertySource(Object object) {
    if (object instanceof IPropertySource) {
      return (IPropertySource) object;
    }
    AdapterFactory af = getAdapterFactory(object);
    if (af != null) {
      IItemPropertySource ips = (IItemPropertySource) af.adapt(object,
          IItemPropertySource.class);
      if (ips != null) {
        return new PropertySource(object, ips);
      }
    }
    if (object instanceof IAdaptable) {
      return (IPropertySource) ((IAdaptable) object)
          .getAdapter(IPropertySource.class);
    }
    return null;
  }

  /**
   * @generated
   */
  protected IPropertySourceProvider getPropertySourceProvider() {
    return this;
  }

  /**
   * Modify/unwrap selection.
   * @generated
   */
  protected Object transformSelection(Object selected) {

    if (selected instanceof EditPart) {
      Object model = ((EditPart) selected).getModel();
      return model instanceof View ? ((View) model).getElement() : null;
    }
    if (selected instanceof View) {
      return ((View) selected).getElement();
    }
    if (selected instanceof IAdaptable) {
      View view = (View) ((IAdaptable) selected).getAdapter(View.class);
      if (view != null) {
        return view.getElement();
      }
    }
    return selected;
  }

  /**
   * @generated
   */
  public void setInput(IWorkbenchPart part, ISelection selection) {
    if (selection.isEmpty()
        || false == selection instanceof StructuredSelection) {
      super.setInput(part, selection);
      return;
    }
    final StructuredSelection structuredSelection = ((StructuredSelection) selection);
    ArrayList transformedSelection = new ArrayList(structuredSelection.size());
    for (Iterator it = structuredSelection.iterator(); it.hasNext();) {
      Object r = transformSelection(it.next());
      if (r != null) {
        transformedSelection.add(r);
      }
    }
    super.setInput(part, new StructuredSelection(transformedSelection));
  }

  /**
   * @generated
   */
  protected AdapterFactory getAdapterFactory(Object object) {
    if (getEditingDomain() instanceof AdapterFactoryEditingDomain) {
      return ((AdapterFactoryEditingDomain) getEditingDomain())
          .getAdapterFactory();
    }
    TransactionalEditingDomain editingDomain = TransactionUtil
        .getEditingDomain(object);
    if (editingDomain != null) {
      return ((AdapterFactoryEditingDomain) editingDomain).getAdapterFactory();
    }
    return null;
  }

}
