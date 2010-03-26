/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.lang.flowcharts.impl;

import hub.top.lang.flowcharts.FlowPackage;
import hub.top.lang.flowcharts.ActivityNode;
import hub.top.lang.flowcharts.ResourceNode;
import hub.top.lang.flowcharts.FlowChartPackage;
import hub.top.lang.flowcharts.WorkflowDiagram;

import java.util.Collection;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;

import org.eclipse.emf.ecore.util.EObjectResolvingEList;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Resource Node</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * </p>
 *
 * @generated
 */
public abstract class ResourceNodeImpl extends DiagramNodeImpl implements ResourceNode {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected ResourceNodeImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return FlowPackage.Literals.RESOURCE_NODE;
	}

} //ResourceNodeImpl
