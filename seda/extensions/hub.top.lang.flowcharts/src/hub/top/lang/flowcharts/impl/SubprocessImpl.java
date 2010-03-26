/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.lang.flowcharts.impl;

import hub.top.lang.flowcharts.FlowPackage;
import hub.top.lang.flowcharts.Subprocess;
import hub.top.lang.flowcharts.FlowChartPackage;
import hub.top.lang.flowcharts.WorkflowDiagram;

import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Subprocess</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link hub.top.lang.flowcharts.impl.SubprocessImpl#getSubprocess <em>Subprocess</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.SubprocessImpl#getWeight <em>Weight</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class SubprocessImpl extends ActivityNodeImpl implements Subprocess {
	/**
	 * The cached value of the '{@link #getSubprocess() <em>Subprocess</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getSubprocess()
	 * @generated
	 * @ordered
	 */
	protected WorkflowDiagram subprocess;

	/**
	 * The default value of the '{@link #getWeight() <em>Weight</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getWeight()
	 * @generated
	 * @ordered
	 */
	protected static final int WEIGHT_EDEFAULT = 1;
	/**
	 * The cached value of the '{@link #getWeight() <em>Weight</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getWeight()
	 * @generated
	 * @ordered
	 */
	protected int weight = WEIGHT_EDEFAULT;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected SubprocessImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return FlowPackage.Literals.SUBPROCESS;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public WorkflowDiagram getSubprocess() {
		if (subprocess != null && subprocess.eIsProxy()) {
			InternalEObject oldSubprocess = (InternalEObject)subprocess;
			subprocess = (WorkflowDiagram)eResolveProxy(oldSubprocess);
			if (subprocess != oldSubprocess) {
				if (eNotificationRequired())
					eNotify(new ENotificationImpl(this, Notification.RESOLVE, FlowPackage.SUBPROCESS__SUBPROCESS, oldSubprocess, subprocess));
			}
		}
		return subprocess;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public WorkflowDiagram basicGetSubprocess() {
		return subprocess;
	}

	/**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public NotificationChain basicSetSubprocess(WorkflowDiagram newSubprocess, NotificationChain msgs) {
		WorkflowDiagram oldSubprocess = subprocess;
		subprocess = newSubprocess;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, FlowPackage.SUBPROCESS__SUBPROCESS, oldSubprocess, newSubprocess);
			if (msgs == null) msgs = notification; else msgs.add(notification);
		}
		return msgs;
	}

  /**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setSubprocess(WorkflowDiagram newSubprocess) {
		if (newSubprocess != subprocess) {
			NotificationChain msgs = null;
			if (subprocess != null)
				msgs = ((InternalEObject)subprocess).eInverseRemove(this, FlowPackage.WORKFLOW_DIAGRAM__PROCESS_CALLS, WorkflowDiagram.class, msgs);
			if (newSubprocess != null)
				msgs = ((InternalEObject)newSubprocess).eInverseAdd(this, FlowPackage.WORKFLOW_DIAGRAM__PROCESS_CALLS, WorkflowDiagram.class, msgs);
			msgs = basicSetSubprocess(newSubprocess, msgs);
			if (msgs != null) msgs.dispatch();
		}
		else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, FlowPackage.SUBPROCESS__SUBPROCESS, newSubprocess, newSubprocess));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public int getWeight() {
		return weight;
	}

		/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setWeight(int newWeight) {
		int oldWeight = weight;
		weight = newWeight;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, FlowPackage.SUBPROCESS__WEIGHT, oldWeight, weight));
	}

	/**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  @Override
  public NotificationChain eInverseAdd(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
		switch (featureID) {
			case FlowPackage.SUBPROCESS__SUBPROCESS:
				if (subprocess != null)
					msgs = ((InternalEObject)subprocess).eInverseRemove(this, FlowPackage.WORKFLOW_DIAGRAM__PROCESS_CALLS, WorkflowDiagram.class, msgs);
				return basicSetSubprocess((WorkflowDiagram)otherEnd, msgs);
		}
		return super.eInverseAdd(otherEnd, featureID, msgs);
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  @Override
  public NotificationChain eInverseRemove(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
		switch (featureID) {
			case FlowPackage.SUBPROCESS__SUBPROCESS:
				return basicSetSubprocess(null, msgs);
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
			case FlowPackage.SUBPROCESS__SUBPROCESS:
				if (resolve) return getSubprocess();
				return basicGetSubprocess();
			case FlowPackage.SUBPROCESS__WEIGHT:
				return new Integer(getWeight());
		}
		return super.eGet(featureID, resolve, coreType);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public void eSet(int featureID, Object newValue) {
		switch (featureID) {
			case FlowPackage.SUBPROCESS__SUBPROCESS:
				setSubprocess((WorkflowDiagram)newValue);
				return;
			case FlowPackage.SUBPROCESS__WEIGHT:
				setWeight(((Integer)newValue).intValue());
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
			case FlowPackage.SUBPROCESS__SUBPROCESS:
				setSubprocess((WorkflowDiagram)null);
				return;
			case FlowPackage.SUBPROCESS__WEIGHT:
				setWeight(WEIGHT_EDEFAULT);
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
			case FlowPackage.SUBPROCESS__SUBPROCESS:
				return subprocess != null;
			case FlowPackage.SUBPROCESS__WEIGHT:
				return weight != WEIGHT_EDEFAULT;
		}
		return super.eIsSet(featureID);
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public String toString() {
		if (eIsProxy()) return super.toString();

		StringBuffer result = new StringBuffer(super.toString());
		result.append(" (weight: ");
		result.append(weight);
		result.append(')');
		return result.toString();
	}

} //SubprocessImpl
