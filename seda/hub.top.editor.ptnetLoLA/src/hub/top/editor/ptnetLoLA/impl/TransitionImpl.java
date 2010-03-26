/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.editor.ptnetLoLA.impl;

import hub.top.editor.ptnetLoLA.Arc;
import hub.top.editor.ptnetLoLA.Node;
import hub.top.editor.ptnetLoLA.Place;
import hub.top.editor.ptnetLoLA.PtnetLoLAPackage;
import hub.top.editor.ptnetLoLA.Transition;

import org.eclipse.emf.common.util.BasicEList;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;


/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Transition</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * </p>
 *
 * @generated
 */
public class TransitionImpl extends NodeImpl implements Transition {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected TransitionImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return PtnetLoLAPackage.Literals.TRANSITION;
	}
	
	 /**
   * @return the list of pre-transitions of this place
   * 
   * @see hub.top.editor.ptnetLoLA.Node#getPreSet()
   */
  public EList<Place> getPreSet() {
    EList<Place> preNodes = new BasicEList<Place>();
    for (Arc a : getIncoming())
      preNodes.add((Place)a.getSource());
    return preNodes;
  }
  
  /**
   * @return the list of post-transitions of this place
   * 
   * @see hub.top.editor.ptnetLoLA.Node#getPostSet()
   */
  public EList<Place> getPostSet() {
    EList<Place> postNodes = new BasicEList<Place>();
    for (Arc a : getOutgoing())
      postNodes.add((Place)a.getTarget());
    return postNodes;
  }

} //TransitionImpl
