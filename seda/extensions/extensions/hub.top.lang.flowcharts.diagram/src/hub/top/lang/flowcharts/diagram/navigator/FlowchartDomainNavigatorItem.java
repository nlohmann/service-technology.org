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

import org.eclipse.core.runtime.IAdapterFactory;
import org.eclipse.core.runtime.Platform;
import org.eclipse.core.runtime.PlatformObject;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.ui.views.properties.IPropertySource;
import org.eclipse.ui.views.properties.IPropertySourceProvider;

/**
 * @generated
 */
public class FlowchartDomainNavigatorItem extends PlatformObject {

  /**
   * @generated
   */
  static {
    final Class[] supportedTypes = new Class[] { EObject.class,
        IPropertySource.class };
    Platform.getAdapterManager().registerAdapters(
        new IAdapterFactory() {

          public Object getAdapter(Object adaptableObject, Class adapterType) {
            if (adaptableObject instanceof hub.top.lang.flowcharts.diagram.navigator.FlowchartDomainNavigatorItem) {
              hub.top.lang.flowcharts.diagram.navigator.FlowchartDomainNavigatorItem domainNavigatorItem = (hub.top.lang.flowcharts.diagram.navigator.FlowchartDomainNavigatorItem) adaptableObject;
              EObject eObject = domainNavigatorItem.getEObject();
              if (adapterType == EObject.class) {
                return eObject;
              }
              if (adapterType == IPropertySource.class) {
                return domainNavigatorItem.getPropertySourceProvider()
                    .getPropertySource(eObject);
              }
            }

            return null;
          }

          public Class[] getAdapterList() {
            return supportedTypes;
          }
        },
        hub.top.lang.flowcharts.diagram.navigator.FlowchartDomainNavigatorItem.class);
  }

  /**
   * @generated
   */
  private Object myParent;

  /**
   * @generated
   */
  private EObject myEObject;

  /**
   * @generated
   */
  private IPropertySourceProvider myPropertySourceProvider;

  /**
   * @generated
   */
  public FlowchartDomainNavigatorItem(EObject eObject, Object parent,
      IPropertySourceProvider propertySourceProvider) {
    myParent = parent;
    myEObject = eObject;
    myPropertySourceProvider = propertySourceProvider;
  }

  /**
   * @generated
   */
  public Object getParent() {
    return myParent;
  }

  /**
   * @generated
   */
  public EObject getEObject() {
    return myEObject;
  }

  /**
   * @generated
   */
  public IPropertySourceProvider getPropertySourceProvider() {
    return myPropertySourceProvider;
  }

  /**
   * @generated
   */
  public boolean equals(Object obj) {
    if (obj instanceof hub.top.lang.flowcharts.diagram.navigator.FlowchartDomainNavigatorItem) {
      return EcoreUtil
          .getURI(getEObject())
          .equals(
              EcoreUtil
                  .getURI(((hub.top.lang.flowcharts.diagram.navigator.FlowchartDomainNavigatorItem) obj)
                      .getEObject()));
    }
    return super.equals(obj);
  }

  /**
   * @generated
   */
  public int hashCode() {
    return EcoreUtil.getURI(getEObject()).hashCode();
  }

}
