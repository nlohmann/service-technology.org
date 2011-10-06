/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.lang.flowcharts.impl;

import hub.top.lang.flowcharts.DiagramArc;
import hub.top.lang.flowcharts.DiagramNode;
import hub.top.lang.flowcharts.FlowPackage;
import hub.top.lang.flowcharts.FlowChartPackage;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Diagram Arc</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link hub.top.lang.flowcharts.impl.DiagramArcImpl#getSrc <em>Src</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.DiagramArcImpl#getTarget <em>Target</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.DiagramArcImpl#getLabel <em>Label</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.DiagramArcImpl#isOptional <em>Optional</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.DiagramArcImpl#getProbability <em>Probability</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.DiagramArcImpl#getMinWeight <em>Min Weight</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.DiagramArcImpl#getMaxWeight <em>Max Weight</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class DiagramArcImpl extends EObjectImpl implements DiagramArc {
	/**
	 * The cached value of the '{@link #getSrc() <em>Src</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getSrc()
	 * @generated
	 * @ordered
	 */
	protected DiagramNode src;

	/**
	 * The cached value of the '{@link #getTarget() <em>Target</em>}' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getTarget()
	 * @generated
	 * @ordered
	 */
	protected DiagramNode target;

	/**
	 * The default value of the '{@link #getLabel() <em>Label</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getLabel()
	 * @generated
	 * @ordered
	 */
	protected static final String LABEL_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getLabel() <em>Label</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getLabel()
	 * @generated
	 * @ordered
	 */
	protected String label = LABEL_EDEFAULT;

	/**
	 * The default value of the '{@link #isOptional() <em>Optional</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isOptional()
	 * @generated
	 * @ordered
	 */
	protected static final boolean OPTIONAL_EDEFAULT = false;

	/**
	 * The cached value of the '{@link #isOptional() <em>Optional</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isOptional()
	 * @generated
	 * @ordered
	 */
	protected boolean optional = OPTIONAL_EDEFAULT;

	/**
	 * The default value of the '{@link #getProbability() <em>Probability</em>}' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see #getProbability()
	 * @generated
	 * @ordered
	 */
  protected static final double PROBABILITY_EDEFAULT = 1.0;

  /**
	 * The cached value of the '{@link #getProbability() <em>Probability</em>}' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see #getProbability()
	 * @generated
	 * @ordered
	 */
  protected double probability = PROBABILITY_EDEFAULT;

		/**
	 * The default value of the '{@link #getMinWeight() <em>Min Weight</em>}' attribute.
	 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
	 * @see #getMinWeight()
	 * @generated
	 * @ordered
	 */
		protected static final int MIN_WEIGHT_EDEFAULT = 1;

		/**
	 * The cached value of the '{@link #getMinWeight() <em>Min Weight</em>}' attribute.
	 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
	 * @see #getMinWeight()
	 * @generated
	 * @ordered
	 */
		protected int minWeight = MIN_WEIGHT_EDEFAULT;

  /**
	 * The default value of the '{@link #getMaxWeight() <em>Max Weight</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getMaxWeight()
	 * @generated
	 * @ordered
	 */
	protected static final int MAX_WEIGHT_EDEFAULT = 1;

		/**
	 * The cached value of the '{@link #getMaxWeight() <em>Max Weight</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getMaxWeight()
	 * @generated
	 * @ordered
	 */
	protected int maxWeight = MAX_WEIGHT_EDEFAULT;

		/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected DiagramArcImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return FlowPackage.Literals.DIAGRAM_ARC;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public DiagramNode getSrc() {
		if (src != null && src.eIsProxy()) {
			InternalEObject oldSrc = (InternalEObject)src;
			src = (DiagramNode)eResolveProxy(oldSrc);
			if (src != oldSrc) {
				if (eNotificationRequired())
					eNotify(new ENotificationImpl(this, Notification.RESOLVE, FlowPackage.DIAGRAM_ARC__SRC, oldSrc, src));
			}
		}
		return src;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public DiagramNode basicGetSrc() {
		return src;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetSrc(DiagramNode newSrc, NotificationChain msgs) {
		DiagramNode oldSrc = src;
		src = newSrc;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, FlowPackage.DIAGRAM_ARC__SRC, oldSrc, newSrc);
			if (msgs == null) msgs = notification; else msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setSrc(DiagramNode newSrc) {
		if (newSrc != src) {
			NotificationChain msgs = null;
			if (src != null)
				msgs = ((InternalEObject)src).eInverseRemove(this, FlowPackage.DIAGRAM_NODE__OUTGOING, DiagramNode.class, msgs);
			if (newSrc != null)
				msgs = ((InternalEObject)newSrc).eInverseAdd(this, FlowPackage.DIAGRAM_NODE__OUTGOING, DiagramNode.class, msgs);
			msgs = basicSetSrc(newSrc, msgs);
			if (msgs != null) msgs.dispatch();
		}
		else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, FlowPackage.DIAGRAM_ARC__SRC, newSrc, newSrc));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public DiagramNode getTarget() {
		if (target != null && target.eIsProxy()) {
			InternalEObject oldTarget = (InternalEObject)target;
			target = (DiagramNode)eResolveProxy(oldTarget);
			if (target != oldTarget) {
				if (eNotificationRequired())
					eNotify(new ENotificationImpl(this, Notification.RESOLVE, FlowPackage.DIAGRAM_ARC__TARGET, oldTarget, target));
			}
		}
		return target;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public DiagramNode basicGetTarget() {
		return target;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public NotificationChain basicSetTarget(DiagramNode newTarget, NotificationChain msgs) {
		DiagramNode oldTarget = target;
		target = newTarget;
		if (eNotificationRequired()) {
			ENotificationImpl notification = new ENotificationImpl(this, Notification.SET, FlowPackage.DIAGRAM_ARC__TARGET, oldTarget, newTarget);
			if (msgs == null) msgs = notification; else msgs.add(notification);
		}
		return msgs;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setTarget(DiagramNode newTarget) {
		if (newTarget != target) {
			NotificationChain msgs = null;
			if (target != null)
				msgs = ((InternalEObject)target).eInverseRemove(this, FlowPackage.DIAGRAM_NODE__INCOMING, DiagramNode.class, msgs);
			if (newTarget != null)
				msgs = ((InternalEObject)newTarget).eInverseAdd(this, FlowPackage.DIAGRAM_NODE__INCOMING, DiagramNode.class, msgs);
			msgs = basicSetTarget(newTarget, msgs);
			if (msgs != null) msgs.dispatch();
		}
		else if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, FlowPackage.DIAGRAM_ARC__TARGET, newTarget, newTarget));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String getLabel() {
		return label;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setLabel(String newLabel) {
		String oldLabel = label;
		label = newLabel;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, FlowPackage.DIAGRAM_ARC__LABEL, oldLabel, label));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public boolean isOptional() {
		return optional;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setOptional(boolean newOptional) {
		boolean oldOptional = optional;
		optional = newOptional;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, FlowPackage.DIAGRAM_ARC__OPTIONAL, oldOptional, optional));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public int getMinWeight() {
		return minWeight;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setMinWeight(int newMinWeight) {
		int oldMinWeight = minWeight;
		minWeight = newMinWeight;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, FlowPackage.DIAGRAM_ARC__MIN_WEIGHT, oldMinWeight, minWeight));
	}

	/**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public double getProbability() {
		return probability;
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public void setProbability(double newProbability) {
		double oldProbability = probability;
		probability = newProbability;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, FlowPackage.DIAGRAM_ARC__PROBABILITY, oldProbability, probability));
	}

  /**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public int getMaxWeight() {
		return maxWeight;
	}

		/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setMaxWeight(int newMaxWeight) {
		int oldMaxWeight = maxWeight;
		maxWeight = newMaxWeight;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, FlowPackage.DIAGRAM_ARC__MAX_WEIGHT, oldMaxWeight, maxWeight));
	}

		/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public NotificationChain eInverseAdd(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
		switch (featureID) {
			case FlowPackage.DIAGRAM_ARC__SRC:
				if (src != null)
					msgs = ((InternalEObject)src).eInverseRemove(this, FlowPackage.DIAGRAM_NODE__OUTGOING, DiagramNode.class, msgs);
				return basicSetSrc((DiagramNode)otherEnd, msgs);
			case FlowPackage.DIAGRAM_ARC__TARGET:
				if (target != null)
					msgs = ((InternalEObject)target).eInverseRemove(this, FlowPackage.DIAGRAM_NODE__INCOMING, DiagramNode.class, msgs);
				return basicSetTarget((DiagramNode)otherEnd, msgs);
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
			case FlowPackage.DIAGRAM_ARC__SRC:
				return basicSetSrc(null, msgs);
			case FlowPackage.DIAGRAM_ARC__TARGET:
				return basicSetTarget(null, msgs);
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
			case FlowPackage.DIAGRAM_ARC__SRC:
				if (resolve) return getSrc();
				return basicGetSrc();
			case FlowPackage.DIAGRAM_ARC__TARGET:
				if (resolve) return getTarget();
				return basicGetTarget();
			case FlowPackage.DIAGRAM_ARC__LABEL:
				return getLabel();
			case FlowPackage.DIAGRAM_ARC__OPTIONAL:
				return isOptional() ? Boolean.TRUE : Boolean.FALSE;
			case FlowPackage.DIAGRAM_ARC__PROBABILITY:
				return new Double(getProbability());
			case FlowPackage.DIAGRAM_ARC__MIN_WEIGHT:
				return new Integer(getMinWeight());
			case FlowPackage.DIAGRAM_ARC__MAX_WEIGHT:
				return new Integer(getMaxWeight());
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
			case FlowPackage.DIAGRAM_ARC__SRC:
				setSrc((DiagramNode)newValue);
				return;
			case FlowPackage.DIAGRAM_ARC__TARGET:
				setTarget((DiagramNode)newValue);
				return;
			case FlowPackage.DIAGRAM_ARC__LABEL:
				setLabel((String)newValue);
				return;
			case FlowPackage.DIAGRAM_ARC__OPTIONAL:
				setOptional(((Boolean)newValue).booleanValue());
				return;
			case FlowPackage.DIAGRAM_ARC__PROBABILITY:
				setProbability(((Double)newValue).doubleValue());
				return;
			case FlowPackage.DIAGRAM_ARC__MIN_WEIGHT:
				setMinWeight(((Integer)newValue).intValue());
				return;
			case FlowPackage.DIAGRAM_ARC__MAX_WEIGHT:
				setMaxWeight(((Integer)newValue).intValue());
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
			case FlowPackage.DIAGRAM_ARC__SRC:
				setSrc((DiagramNode)null);
				return;
			case FlowPackage.DIAGRAM_ARC__TARGET:
				setTarget((DiagramNode)null);
				return;
			case FlowPackage.DIAGRAM_ARC__LABEL:
				setLabel(LABEL_EDEFAULT);
				return;
			case FlowPackage.DIAGRAM_ARC__OPTIONAL:
				setOptional(OPTIONAL_EDEFAULT);
				return;
			case FlowPackage.DIAGRAM_ARC__PROBABILITY:
				setProbability(PROBABILITY_EDEFAULT);
				return;
			case FlowPackage.DIAGRAM_ARC__MIN_WEIGHT:
				setMinWeight(MIN_WEIGHT_EDEFAULT);
				return;
			case FlowPackage.DIAGRAM_ARC__MAX_WEIGHT:
				setMaxWeight(MAX_WEIGHT_EDEFAULT);
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
			case FlowPackage.DIAGRAM_ARC__SRC:
				return src != null;
			case FlowPackage.DIAGRAM_ARC__TARGET:
				return target != null;
			case FlowPackage.DIAGRAM_ARC__LABEL:
				return LABEL_EDEFAULT == null ? label != null : !LABEL_EDEFAULT.equals(label);
			case FlowPackage.DIAGRAM_ARC__OPTIONAL:
				return optional != OPTIONAL_EDEFAULT;
			case FlowPackage.DIAGRAM_ARC__PROBABILITY:
				return probability != PROBABILITY_EDEFAULT;
			case FlowPackage.DIAGRAM_ARC__MIN_WEIGHT:
				return minWeight != MIN_WEIGHT_EDEFAULT;
			case FlowPackage.DIAGRAM_ARC__MAX_WEIGHT:
				return maxWeight != MAX_WEIGHT_EDEFAULT;
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
		result.append(" (label: ");
		result.append(label);
		result.append(", optional: ");
		result.append(optional);
		result.append(", probability: ");
		result.append(probability);
		result.append(", minWeight: ");
		result.append(minWeight);
		result.append(", maxWeight: ");
		result.append(maxWeight);
		result.append(')');
		return result.toString();
	}

} //DiagramArcImpl
