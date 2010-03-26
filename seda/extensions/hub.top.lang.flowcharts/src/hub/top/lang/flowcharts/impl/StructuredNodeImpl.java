/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.lang.flowcharts.impl;

import hub.top.lang.flowcharts.DiagramNode;
import hub.top.lang.flowcharts.FlowPackage;
import hub.top.lang.flowcharts.StructuredNode;
import hub.top.lang.flowcharts.FlowChartPackage;

import java.util.Collection;

import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Structured Node</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link hub.top.lang.flowcharts.impl.StructuredNodeImpl#getChildNodes <em>Child Nodes</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public abstract class StructuredNodeImpl extends ControlFlowNodeImpl implements StructuredNode {
	/**
	 * The cached value of the '{@link #getChildNodes() <em>Child Nodes</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getChildNodes()
	 * @generated
	 * @ordered
	 */
	protected EList<DiagramNode> childNodes;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected StructuredNodeImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return FlowPackage.Literals.STRUCTURED_NODE;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<DiagramNode> getChildNodes() {
		if (childNodes == null) {
			childNodes = new EObjectContainmentEList<DiagramNode>(DiagramNode.class, this, FlowPackage.STRUCTURED_NODE__CHILD_NODES);
		}
		return childNodes;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
		switch (featureID) {
			case FlowPackage.STRUCTURED_NODE__CHILD_NODES:
				return ((InternalEList<?>)getChildNodes()).basicRemove(otherEnd, msgs);
		}
		return super.eInverseRemove(otherEnd, featureID, msgs);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public Object eGet(int featureID, boolean resolve, boolean coreType) {
		switch (featureID) {
			case FlowPackage.STRUCTURED_NODE__CHILD_NODES:
				return getChildNodes();
		}
		return super.eGet(featureID, resolve, coreType);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@SuppressWarnings("unchecked")
	@Override
	public void eSet(int featureID, Object newValue) {
		switch (featureID) {
			case FlowPackage.STRUCTURED_NODE__CHILD_NODES:
				getChildNodes().clear();
				getChildNodes().addAll((Collection<? extends DiagramNode>)newValue);
				return;
		}
		super.eSet(featureID, newValue);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public void eUnset(int featureID) {
		switch (featureID) {
			case FlowPackage.STRUCTURED_NODE__CHILD_NODES:
				getChildNodes().clear();
				return;
		}
		super.eUnset(featureID);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public boolean eIsSet(int featureID) {
		switch (featureID) {
			case FlowPackage.STRUCTURED_NODE__CHILD_NODES:
				return childNodes != null && !childNodes.isEmpty();
		}
		return super.eIsSet(featureID);
	}

} //StructuredNodeImpl
