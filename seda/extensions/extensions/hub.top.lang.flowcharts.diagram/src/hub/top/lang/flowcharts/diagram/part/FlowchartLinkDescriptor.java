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
package hub.top.lang.flowcharts.diagram.part;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.gmf.runtime.emf.core.util.EObjectAdapter;
import org.eclipse.gmf.runtime.emf.type.core.IElementType;

/**
 * @generated
 */
public class FlowchartLinkDescriptor extends FlowchartNodeDescriptor {

  /**
   * @generated
   */
  private EObject mySource;

  /**
   * @generated
   */
  private EObject myDestination;

  /**
   * @generated
   */
  private IAdaptable mySemanticAdapter;

  /**
   * @generated
   */
  private FlowchartLinkDescriptor(EObject source, EObject destination,
      EObject linkElement, int linkVID) {
    super(linkElement, linkVID);
    mySource = source;
    myDestination = destination;
  }

  /**
   * @generated
   */
  public FlowchartLinkDescriptor(EObject source, EObject destination,
      IElementType elementType, int linkVID) {
    this(source, destination, (EObject) null, linkVID);
    final IElementType elementTypeCopy = elementType;
    mySemanticAdapter = new IAdaptable() {
      public Object getAdapter(Class adapter) {
        if (IElementType.class.equals(adapter)) {
          return elementTypeCopy;
        }
        return null;
      }
    };
  }

  /**
   * @generated
   */
  public FlowchartLinkDescriptor(EObject source, EObject destination,
      EObject linkElement, IElementType elementType, int linkVID) {
    this(source, destination, linkElement, linkVID);
    final IElementType elementTypeCopy = elementType;
    mySemanticAdapter = new EObjectAdapter(linkElement) {
      public Object getAdapter(Class adapter) {
        if (IElementType.class.equals(adapter)) {
          return elementTypeCopy;
        }
        return super.getAdapter(adapter);
      }
    };
  }

  /**
   * @generated
   */
  public EObject getSource() {
    return mySource;
  }

  /**
   * @generated
   */
  public EObject getDestination() {
    return myDestination;
  }

  /**
   * @generated
   */
  public IAdaptable getSemanticAdapter() {
    return mySemanticAdapter;
  }

}
