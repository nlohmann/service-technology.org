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
package hub.top.lang.flowcharts.diagram.navigator;

import hub.top.lang.flowcharts.diagram.part.FlowchartDiagramEditorPlugin;

import org.eclipse.emf.edit.ui.provider.AdapterFactoryLabelProvider;
import org.eclipse.jface.viewers.ILabelProviderListener;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.navigator.ICommonContentExtensionSite;
import org.eclipse.ui.navigator.ICommonLabelProvider;

/**
 * @generated
 */
public class FlowchartDomainNavigatorLabelProvider implements
    ICommonLabelProvider {

  /**
   * @generated
   */
  private AdapterFactoryLabelProvider myAdapterFactoryLabelProvider = new AdapterFactoryLabelProvider(
      FlowchartDiagramEditorPlugin.getInstance()
          .getItemProvidersAdapterFactory());

  /**
   * @generated
   */
  public void init(ICommonContentExtensionSite aConfig) {
  }

  /**
   * @generated
   */
  public Image getImage(Object element) {
    if (element instanceof FlowchartDomainNavigatorItem) {
      return myAdapterFactoryLabelProvider
          .getImage(((FlowchartDomainNavigatorItem) element).getEObject());
    }
    return null;
  }

  /**
   * @generated
   */
  public String getText(Object element) {
    if (element instanceof FlowchartDomainNavigatorItem) {
      return myAdapterFactoryLabelProvider
          .getText(((FlowchartDomainNavigatorItem) element).getEObject());
    }
    return null;
  }

  /**
   * @generated
   */
  public void addListener(ILabelProviderListener listener) {
    myAdapterFactoryLabelProvider.addListener(listener);
  }

  /**
   * @generated
   */
  public void dispose() {
    myAdapterFactoryLabelProvider.dispose();
  }

  /**
   * @generated
   */
  public boolean isLabelProperty(Object element, String property) {
    return myAdapterFactoryLabelProvider.isLabelProperty(element, property);
  }

  /**
   * @generated
   */
  public void removeListener(ILabelProviderListener listener) {
    myAdapterFactoryLabelProvider.removeListener(listener);
  }

  /**
   * @generated
   */
  public void restoreState(IMemento aMemento) {
  }

  /**
   * @generated
   */
  public void saveState(IMemento aMemento) {
  }

  /**
   * @generated
   */
  public String getDescription(Object anElement) {
    return null;
  }

}
