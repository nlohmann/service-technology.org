/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.editor.ptnetLoLA;

import org.eclipse.emf.common.util.EList;


/**
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Transition</b></em>'.
 * <!-- end-user-doc -->
 *
 *
 * @see hub.top.editor.ptnetLoLA.PtnetLoLAPackage#getTransition()
 * @model
 * @generated
 */
public interface Transition extends Node {
  
  /**
   * @return a list of all direct pre-places of this transition
   */
  EList<Place> getPreSet();

  /**
   * @return a list of all direct post-places of this transition
   */
  EList<Place> getPostSet();
  
} // Transition
