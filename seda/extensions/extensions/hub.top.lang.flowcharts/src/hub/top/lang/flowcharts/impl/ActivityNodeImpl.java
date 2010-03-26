/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.lang.flowcharts.impl;

import hub.top.lang.flowcharts.ActivityNode;
import hub.top.lang.flowcharts.FlowPackage;
import hub.top.lang.flowcharts.DiagramArc;
import hub.top.lang.flowcharts.ResourceNode;
import hub.top.lang.flowcharts.FlowChartPackage;

import java.util.Collection;

import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.util.EObjectEList;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Activity Node</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link hub.top.lang.flowcharts.impl.ActivityNodeImpl#getActivityOutput <em>Activity Output</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.ActivityNodeImpl#getActivityOutputOpt <em>Activity Output Opt</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.ActivityNodeImpl#isOptional <em>Optional</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.ActivityNodeImpl#getActivityInputOpt <em>Activity Input Opt</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.ActivityNodeImpl#getActivityInput <em>Activity Input</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.ActivityNodeImpl#getMinTime <em>Min Time</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.ActivityNodeImpl#getCost <em>Cost</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.ActivityNodeImpl#getProbability <em>Probability</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.ActivityNodeImpl#getMaxTime <em>Max Time</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public abstract class ActivityNodeImpl extends DiagramNodeImpl implements ActivityNode {
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
	 * The default value of the '{@link #getMinTime() <em>Min Time</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getMinTime()
	 * @generated
	 * @ordered
	 */
	protected static final int MIN_TIME_EDEFAULT = 0;

	/**
	 * The cached value of the '{@link #getMinTime() <em>Min Time</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getMinTime()
	 * @generated
	 * @ordered
	 */
	protected int minTime = MIN_TIME_EDEFAULT;

	/**
	 * The default value of the '{@link #getCost() <em>Cost</em>}' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see #getCost()
	 * @generated
	 * @ordered
	 */
  protected static final int COST_EDEFAULT = 0;

  /**
	 * The cached value of the '{@link #getCost() <em>Cost</em>}' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see #getCost()
	 * @generated
	 * @ordered
	 */
  protected int cost = COST_EDEFAULT;

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
	 * The default value of the '{@link #getMaxTime() <em>Max Time</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getMaxTime()
	 * @generated
	 * @ordered
	 */
	protected static final int MAX_TIME_EDEFAULT = 0;

		/**
	 * The cached value of the '{@link #getMaxTime() <em>Max Time</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getMaxTime()
	 * @generated
	 * @ordered
	 */
	protected int maxTime = MAX_TIME_EDEFAULT;

		/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected ActivityNodeImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return FlowPackage.Literals.ACTIVITY_NODE;
	}

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated NOT
   */
	public EList<ResourceNode> getActivityOutput() {
    // The list is expected to implement org.eclipse.emf.ecore.util.InternalEList and org.eclipse.emf.ecore.EStructuralFeature.Setting
    // so it's likely that an appropriate subclass of org.eclipse.emf.ecore.util.EcoreEList should be used.
	  EObjectEList<ResourceNode> resultSet = new EObjectEList<ResourceNode>(ResourceNode.class, this, FlowChartPackage.ACTIVITY_NODE__ACTIVITY_OUTPUT);
    for (DiagramArc arc : getOutgoing()) {
      if (arc.getTarget() instanceof ResourceNode) {
        if (!arc.isOptional())
          resultSet.add((ResourceNode)arc.getTarget());
      }
    }
	  return resultSet;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated NOT
   */
	public EList<ResourceNode> getActivityOutputOpt() {
    // The list is expected to implement org.eclipse.emf.ecore.util.InternalEList and org.eclipse.emf.ecore.EStructuralFeature.Setting
    // so it's likely that an appropriate subclass of org.eclipse.emf.ecore.util.EcoreEList should be used.
    EObjectEList<ResourceNode> resultSet = new EObjectEList<ResourceNode>(ResourceNode.class, this, FlowChartPackage.ACTIVITY_NODE__ACTIVITY_OUTPUT_OPT);
    for (DiagramArc arc : getOutgoing()) {
      if (arc.getTarget() instanceof ResourceNode) {
        if (arc.isOptional())
          resultSet.add((ResourceNode)arc.getTarget());
      }
    }
    return resultSet;
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
			eNotify(new ENotificationImpl(this, Notification.SET, FlowPackage.ACTIVITY_NODE__OPTIONAL, oldOptional, optional));
	}

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated NOT
   */
	public EList<ResourceNode> getActivityInputOpt() {
    // The list is expected to implement org.eclipse.emf.ecore.util.InternalEList and org.eclipse.emf.ecore.EStructuralFeature.Setting
    // so it's likely that an appropriate subclass of org.eclipse.emf.ecore.util.EcoreEList should be used.
    EObjectEList<ResourceNode> resultSet = new EObjectEList<ResourceNode>(ResourceNode.class, this, FlowChartPackage.ACTIVITY_NODE__ACTIVITY_INPUT);
    for (DiagramArc arc : getIncoming()) {
      if (arc.getSrc() instanceof ResourceNode) {
        if (arc.isOptional())
          resultSet.add((ResourceNode)arc.getSrc());
      }
    }
    return resultSet;
  }

	/**
   * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
   * @generated NOT
   */
	public EList<ResourceNode> getActivityInput() {
    // The list is expected to implement org.eclipse.emf.ecore.util.InternalEList and org.eclipse.emf.ecore.EStructuralFeature.Setting
    // so it's likely that an appropriate subclass of org.eclipse.emf.ecore.util.EcoreEList should be used.
    EObjectEList<ResourceNode> resultSet = new EObjectEList<ResourceNode>(ResourceNode.class, this, FlowChartPackage.ACTIVITY_NODE__ACTIVITY_INPUT_OPT);
    for (DiagramArc arc : getIncoming()) {
      if (arc.getSrc() instanceof ResourceNode) {
        if (!arc.isOptional())
          resultSet.add((ResourceNode)arc.getSrc());
      }
    }
    return resultSet;
  }

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public int getMinTime() {
		return minTime;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setMinTime(int newMinTime) {
		int oldMinTime = minTime;
		minTime = newMinTime;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, FlowPackage.ACTIVITY_NODE__MIN_TIME, oldMinTime, minTime));
	}

	/**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public int getCost() {
		return cost;
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public void setCost(int newCost) {
		int oldCost = cost;
		cost = newCost;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, FlowPackage.ACTIVITY_NODE__COST, oldCost, cost));
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
			eNotify(new ENotificationImpl(this, Notification.SET, FlowPackage.ACTIVITY_NODE__PROBABILITY, oldProbability, probability));
	}

  /**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public int getMaxTime() {
		return maxTime;
	}

		/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setMaxTime(int newMaxTime) {
		int oldMaxTime = maxTime;
		maxTime = newMaxTime;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, FlowPackage.ACTIVITY_NODE__MAX_TIME, oldMaxTime, maxTime));
	}

		/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public Object eGet(int featureID, boolean resolve, boolean coreType) {
		switch (featureID) {
			case FlowPackage.ACTIVITY_NODE__ACTIVITY_OUTPUT:
				return getActivityOutput();
			case FlowPackage.ACTIVITY_NODE__ACTIVITY_OUTPUT_OPT:
				return getActivityOutputOpt();
			case FlowPackage.ACTIVITY_NODE__OPTIONAL:
				return isOptional() ? Boolean.TRUE : Boolean.FALSE;
			case FlowPackage.ACTIVITY_NODE__ACTIVITY_INPUT_OPT:
				return getActivityInputOpt();
			case FlowPackage.ACTIVITY_NODE__ACTIVITY_INPUT:
				return getActivityInput();
			case FlowPackage.ACTIVITY_NODE__MIN_TIME:
				return new Integer(getMinTime());
			case FlowPackage.ACTIVITY_NODE__COST:
				return new Integer(getCost());
			case FlowPackage.ACTIVITY_NODE__PROBABILITY:
				return new Double(getProbability());
			case FlowPackage.ACTIVITY_NODE__MAX_TIME:
				return new Integer(getMaxTime());
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
			case FlowPackage.ACTIVITY_NODE__ACTIVITY_OUTPUT:
				getActivityOutput().clear();
				getActivityOutput().addAll((Collection<? extends ResourceNode>)newValue);
				return;
			case FlowPackage.ACTIVITY_NODE__ACTIVITY_OUTPUT_OPT:
				getActivityOutputOpt().clear();
				getActivityOutputOpt().addAll((Collection<? extends ResourceNode>)newValue);
				return;
			case FlowPackage.ACTIVITY_NODE__OPTIONAL:
				setOptional(((Boolean)newValue).booleanValue());
				return;
			case FlowPackage.ACTIVITY_NODE__ACTIVITY_INPUT_OPT:
				getActivityInputOpt().clear();
				getActivityInputOpt().addAll((Collection<? extends ResourceNode>)newValue);
				return;
			case FlowPackage.ACTIVITY_NODE__ACTIVITY_INPUT:
				getActivityInput().clear();
				getActivityInput().addAll((Collection<? extends ResourceNode>)newValue);
				return;
			case FlowPackage.ACTIVITY_NODE__MIN_TIME:
				setMinTime(((Integer)newValue).intValue());
				return;
			case FlowPackage.ACTIVITY_NODE__COST:
				setCost(((Integer)newValue).intValue());
				return;
			case FlowPackage.ACTIVITY_NODE__PROBABILITY:
				setProbability(((Double)newValue).doubleValue());
				return;
			case FlowPackage.ACTIVITY_NODE__MAX_TIME:
				setMaxTime(((Integer)newValue).intValue());
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
			case FlowPackage.ACTIVITY_NODE__ACTIVITY_OUTPUT:
				getActivityOutput().clear();
				return;
			case FlowPackage.ACTIVITY_NODE__ACTIVITY_OUTPUT_OPT:
				getActivityOutputOpt().clear();
				return;
			case FlowPackage.ACTIVITY_NODE__OPTIONAL:
				setOptional(OPTIONAL_EDEFAULT);
				return;
			case FlowPackage.ACTIVITY_NODE__ACTIVITY_INPUT_OPT:
				getActivityInputOpt().clear();
				return;
			case FlowPackage.ACTIVITY_NODE__ACTIVITY_INPUT:
				getActivityInput().clear();
				return;
			case FlowPackage.ACTIVITY_NODE__MIN_TIME:
				setMinTime(MIN_TIME_EDEFAULT);
				return;
			case FlowPackage.ACTIVITY_NODE__COST:
				setCost(COST_EDEFAULT);
				return;
			case FlowPackage.ACTIVITY_NODE__PROBABILITY:
				setProbability(PROBABILITY_EDEFAULT);
				return;
			case FlowPackage.ACTIVITY_NODE__MAX_TIME:
				setMaxTime(MAX_TIME_EDEFAULT);
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
			case FlowPackage.ACTIVITY_NODE__ACTIVITY_OUTPUT:
				return !getActivityOutput().isEmpty();
			case FlowPackage.ACTIVITY_NODE__ACTIVITY_OUTPUT_OPT:
				return !getActivityOutputOpt().isEmpty();
			case FlowPackage.ACTIVITY_NODE__OPTIONAL:
				return optional != OPTIONAL_EDEFAULT;
			case FlowPackage.ACTIVITY_NODE__ACTIVITY_INPUT_OPT:
				return !getActivityInputOpt().isEmpty();
			case FlowPackage.ACTIVITY_NODE__ACTIVITY_INPUT:
				return !getActivityInput().isEmpty();
			case FlowPackage.ACTIVITY_NODE__MIN_TIME:
				return minTime != MIN_TIME_EDEFAULT;
			case FlowPackage.ACTIVITY_NODE__COST:
				return cost != COST_EDEFAULT;
			case FlowPackage.ACTIVITY_NODE__PROBABILITY:
				return probability != PROBABILITY_EDEFAULT;
			case FlowPackage.ACTIVITY_NODE__MAX_TIME:
				return maxTime != MAX_TIME_EDEFAULT;
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
		result.append(" (optional: ");
		result.append(optional);
		result.append(", minTime: ");
		result.append(minTime);
		result.append(", cost: ");
		result.append(cost);
		result.append(", probability: ");
		result.append(probability);
		result.append(", maxTime: ");
		result.append(maxTime);
		result.append(')');
		return result.toString();
	}

} //ActivityNodeImpl
