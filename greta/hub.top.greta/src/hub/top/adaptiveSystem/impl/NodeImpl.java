/**
 * Humboldt Universität zu Berlin, Thoery of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem.impl;

import hub.top.adaptiveSystem.AdaptiveProcess;
import hub.top.adaptiveSystem.AdaptiveSystemPackage;
import hub.top.adaptiveSystem.Condition;
import hub.top.adaptiveSystem.DoNet;
import hub.top.adaptiveSystem.Event;
import hub.top.adaptiveSystem.Node;
import hub.top.adaptiveSystem.PreNet;
import hub.top.adaptiveSystem.Temp;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Node</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link hub.top.adaptiveSystem.impl.NodeImpl#getName <em>Name</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.NodeImpl#isAbstract <em>Abstract</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.NodeImpl#getTemp <em>Temp</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.NodeImpl#isDisabledByAntiOclet <em>Disabled By Anti Oclet</em>}</li>
 *   <li>{@link hub.top.adaptiveSystem.impl.NodeImpl#isDisabledByConflict <em>Disabled By Conflict</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public abstract class NodeImpl extends EObjectImpl implements Node {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "Humboldt Universit\u00e4t zu Berlin, Thoery of Programming";

	/**
	 * The default value of the '{@link #getName() <em>Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getName()
	 * @generated
	 * @ordered
	 */
	protected static final String NAME_EDEFAULT = null;

	/**
	 * The cached value of the '{@link #getName() <em>Name</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getName()
	 * @generated
	 * @ordered
	 */
	protected String name = NAME_EDEFAULT;

	/**
	 * The default value of the '{@link #isAbstract() <em>Abstract</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isAbstract()
	 * @generated
	 * @ordered
	 */
	protected static final boolean ABSTRACT_EDEFAULT = false;

	/**
	 * The cached value of the '{@link #isAbstract() <em>Abstract</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isAbstract()
	 * @generated
	 * @ordered
	 */
	protected boolean abstract_ = ABSTRACT_EDEFAULT;

	/**
	 * This is true if the Abstract attribute has been set.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	protected boolean abstractESet;

	/**
	 * The default value of the '{@link #getTemp() <em>Temp</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getTemp()
	 * @generated
	 * @ordered
	 */
	protected static final Temp TEMP_EDEFAULT = Temp.COLD;

	/**
	 * The cached value of the '{@link #getTemp() <em>Temp</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getTemp()
	 * @generated
	 * @ordered
	 */
	protected Temp temp = TEMP_EDEFAULT;

	/**
	 * The default value of the '{@link #isDisabledByAntiOclet() <em>Disabled By Anti Oclet</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isDisabledByAntiOclet()
	 * @generated
	 * @ordered
	 */
	protected static final boolean DISABLED_BY_ANTI_OCLET_EDEFAULT = false;

	/**
	 * The cached value of the '{@link #isDisabledByAntiOclet() <em>Disabled By Anti Oclet</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isDisabledByAntiOclet()
	 * @generated
	 * @ordered
	 */
	protected boolean disabledByAntiOclet = DISABLED_BY_ANTI_OCLET_EDEFAULT;

	/**
	 * This is true if the Disabled By Anti Oclet attribute has been set.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	protected boolean disabledByAntiOcletESet;

	/**
	 * The default value of the '{@link #isDisabledByConflict() <em>Disabled By Conflict</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isDisabledByConflict()
	 * @generated
	 * @ordered
	 */
	protected static final boolean DISABLED_BY_CONFLICT_EDEFAULT = false;

	/**
	 * The cached value of the '{@link #isDisabledByConflict() <em>Disabled By Conflict</em>}' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #isDisabledByConflict()
	 * @generated
	 * @ordered
	 */
	protected boolean disabledByConflict = DISABLED_BY_CONFLICT_EDEFAULT;

	/**
	 * This is true if the Disabled By Conflict attribute has been set.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	protected boolean disabledByConflictESet;

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected NodeImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return AdaptiveSystemPackage.Literals.NODE;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String getName() {
		return name;
	}

	/**
	 * <!-- begin-user-doc -->
	 * if Node is abstract it has no name;
	 * <!-- end-user-doc -->
	 * @generated NOT
	 */
	public void setName(String newName) {
		String oldName = name;
		//START: Manja Wolf
		if(isAbstract()) newName = "";
		//END: Manja Wolf
		name = newName;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.NODE__NAME, oldName, name));
	}

	/**
	 * <!-- begin-user-doc -->
	 * abstract isn't settable in AdaptiveProcess
	 * <!-- end-user-doc -->
	 * @generated NOT
	 */
	public boolean isAbstract() {
		//START: Manja Wolf
		if(eContainer instanceof AdaptiveProcess) unsetAbstract();
		//END: Manja Wolf
		return abstract_;
	}

	/**
	 * <!-- begin-user-doc -->
	 * if isAbstract() true then setTemp(Temp.WITHOUT).
	 * <!-- end-user-doc -->
	 * @generated NOT
	 */
	public void setAbstract(boolean newAbstract) {
		boolean oldAbstract = abstract_;
		abstract_ = newAbstract;
		boolean oldAbstractESet = abstractESet;
		abstractESet = true;
		//START: Manja Wolf
		if(!(eContainer instanceof AdaptiveProcess) && abstract_) {
			setName("");
			setTemp(Temp.WITHOUT);
		}
		//END: Manja Wolf
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.NODE__ABSTRACT, oldAbstract, abstract_, !oldAbstractESet));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void unsetAbstract() {
		boolean oldAbstract = abstract_;
		boolean oldAbstractESet = abstractESet;
		abstract_ = ABSTRACT_EDEFAULT;
		abstractESet = false;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.UNSET, AdaptiveSystemPackage.NODE__ABSTRACT, oldAbstract, ABSTRACT_EDEFAULT, oldAbstractESet));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public boolean isSetAbstract() {
		return abstractESet;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Temp getTemp() {
		return temp;
	}

	/**
	 * <!-- begin-user-doc -->
	 * if node is in preNet: temp.without
	 * nodes in AdaptiveProcess are not passiv, e.g. they have a temperature
	 * <!-- end-user-doc -->
	 * @generated NOT
	 */
	public void setTemp(Temp newTemp) {
		Temp oldTemp = temp;
		//START: Manja Wolf
		if(this.eContainer instanceof PreNet) {
			newTemp = Temp.WITHOUT;
		} 		
		if(eContainer instanceof AdaptiveProcess && newTemp.equals(Temp.WITHOUT)) {
			newTemp = Temp.COLD;
		}
		//END Manja Wolf
		temp = newTemp == null ? TEMP_EDEFAULT : newTemp;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.NODE__TEMP, oldTemp, temp));
	}

	/**
	 * <!-- begin-user-doc -->
	 * value is only settable in Adaptive Process
	 * <!-- end-user-doc -->
	 * @generated NOT
	 */
	public boolean isDisabledByAntiOclet() {
		if(!(this.eContainer instanceof AdaptiveProcess)) unsetDisabledByAntiOclet();
		return disabledByAntiOclet;
	}

	/**
	 * <!-- begin-user-doc -->
	 * Sets the attribute of the node and the whole postSet to the value of parameter
	 * <!-- end-user-doc -->
	 * @generated NOT
	 */
	public void setDisabledByAntiOclet(boolean newDisabledByAntiOclet) {
		//START: Manja Wolf
		if(this instanceof Condition) {
			if(!((Condition) this).getPostEvents().isEmpty()) {
				for(Event event : ((Condition) this).getPostEvents()) {
					event.setDisabledByAntiOclet(newDisabledByAntiOclet);
				}
			}
		}
		if(this instanceof Event) {
			if(!((Event) this).getPostConditions().isEmpty()) {
				for(Condition condition : ((Event) this).getPostConditions()) {
					condition.setDisabledByAntiOclet(newDisabledByAntiOclet);
				}
			}
		}
		//END: Manja Wolf
		boolean oldDisabledByAntiOclet = disabledByAntiOclet;
		disabledByAntiOclet = newDisabledByAntiOclet;
		boolean oldDisabledByAntiOcletESet = disabledByAntiOcletESet;
		disabledByAntiOcletESet = true;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.NODE__DISABLED_BY_ANTI_OCLET, oldDisabledByAntiOclet, disabledByAntiOclet, !oldDisabledByAntiOcletESet));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void unsetDisabledByAntiOclet() {
		boolean oldDisabledByAntiOclet = disabledByAntiOclet;
		boolean oldDisabledByAntiOcletESet = disabledByAntiOcletESet;
		disabledByAntiOclet = DISABLED_BY_ANTI_OCLET_EDEFAULT;
		disabledByAntiOcletESet = false;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.UNSET, AdaptiveSystemPackage.NODE__DISABLED_BY_ANTI_OCLET, oldDisabledByAntiOclet, DISABLED_BY_ANTI_OCLET_EDEFAULT, oldDisabledByAntiOcletESet));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public boolean isSetDisabledByAntiOclet() {
		return disabledByAntiOcletESet;
	}

	/**
	 * <!-- begin-user-doc -->
	 * Value only settable in Adaptive Process
	 * <!-- end-user-doc -->
	 * @generated NOT
	 */
	public boolean isDisabledByConflict() {
		//START: Manja Wolf
		if(!(eContainer instanceof AdaptiveProcess)) unsetDisabledByConflict();
		//END: Manja Wolf
		return disabledByConflict;
	}

	/**
	 * <!-- begin-user-doc -->
	 * Sets the attribute of the node and the whole postSet to the value of parameter
	 * <!-- end-user-doc -->
	 * @generated NOT
	 */
	public void setDisabledByConflict(boolean newDisabledByConflict) {
		//START: Manja Wolf
		if(this instanceof Condition) {
			if(!((Condition) this).getPostEvents().isEmpty()) {
				for(Event event : ((Condition) this).getPostEvents()) {
					event.setDisabledByAntiOclet(newDisabledByConflict);
				}
			}
		}
		if(this instanceof Event) {
			if(!((Event) this).getPostConditions().isEmpty()) {
				for(Condition condition : ((Event) this).getPostConditions()) {
					condition.setDisabledByAntiOclet(newDisabledByConflict);
				}
			}
		}
		//END: Manja Wolf
		boolean oldDisabledByConflict = disabledByConflict;
		disabledByConflict = newDisabledByConflict;
		boolean oldDisabledByConflictESet = disabledByConflictESet;
		disabledByConflictESet = true;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, AdaptiveSystemPackage.NODE__DISABLED_BY_CONFLICT, oldDisabledByConflict, disabledByConflict, !oldDisabledByConflictESet));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void unsetDisabledByConflict() {
		boolean oldDisabledByConflict = disabledByConflict;
		boolean oldDisabledByConflictESet = disabledByConflictESet;
		disabledByConflict = DISABLED_BY_CONFLICT_EDEFAULT;
		disabledByConflictESet = false;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.UNSET, AdaptiveSystemPackage.NODE__DISABLED_BY_CONFLICT, oldDisabledByConflict, DISABLED_BY_CONFLICT_EDEFAULT, oldDisabledByConflictESet));
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public boolean isSetDisabledByConflict() {
		return disabledByConflictESet;
	}

	/**
	 * @author Manja Wolf
	 */
	public EObject eOldContainer;
	
	/**
	 * @author Manja Wolf
	 */
	public EObject eOldContainer() {
		return eOldContainer;
	}
	
	/**
	 * <!-- begin-user-doc -->
	 * if node moves to preNet: temp.without
	 * if node moves to doNet or AdaptiveProcess: 
	 * 		(eContainer != eOldContainer) initial value of temp = temp.cold
	 * 
	 * <!-- end-user-doc -->
	 */
	@Override
	  protected void eBasicSetContainer(InternalEObject newContainer, int newContainerFeatureID)
	  {
	    // caches the old eContainer
	    eOldContainer = this.eContainer();
	   
	    super.eBasicSetContainer(newContainer, newContainerFeatureID);
	  //sets the Temperature of a node to the initial value of the special net (preNet, doNet, mainProcess)

	    if(newContainer instanceof PreNet) setTemp(Temp.WITHOUT);
	    if(eOldContainer != null && eOldContainer != newContainer && !(eOldContainer instanceof PreNet && newContainer instanceof DoNet)) {
	    	//TODO: proof whether the arcs between nodes are in the same eContainer and if not delete arcs from Node
	    	if(!(newContainer instanceof PreNet)) setTemp(TEMP_EDEFAULT);
	    }
	  }
		
	
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public Object eGet(int featureID, boolean resolve, boolean coreType) {
		switch (featureID) {
			case AdaptiveSystemPackage.NODE__NAME:
				return getName();
			case AdaptiveSystemPackage.NODE__ABSTRACT:
				return isAbstract() ? Boolean.TRUE : Boolean.FALSE;
			case AdaptiveSystemPackage.NODE__TEMP:
				return getTemp();
			case AdaptiveSystemPackage.NODE__DISABLED_BY_ANTI_OCLET:
				return isDisabledByAntiOclet() ? Boolean.TRUE : Boolean.FALSE;
			case AdaptiveSystemPackage.NODE__DISABLED_BY_CONFLICT:
				return isDisabledByConflict() ? Boolean.TRUE : Boolean.FALSE;
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
			case AdaptiveSystemPackage.NODE__NAME:
				setName((String)newValue);
				return;
			case AdaptiveSystemPackage.NODE__ABSTRACT:
				setAbstract(((Boolean)newValue).booleanValue());
				return;
			case AdaptiveSystemPackage.NODE__TEMP:
				setTemp((Temp)newValue);
				return;
			case AdaptiveSystemPackage.NODE__DISABLED_BY_ANTI_OCLET:
				setDisabledByAntiOclet(((Boolean)newValue).booleanValue());
				return;
			case AdaptiveSystemPackage.NODE__DISABLED_BY_CONFLICT:
				setDisabledByConflict(((Boolean)newValue).booleanValue());
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
			case AdaptiveSystemPackage.NODE__NAME:
				setName(NAME_EDEFAULT);
				return;
			case AdaptiveSystemPackage.NODE__ABSTRACT:
				unsetAbstract();
				return;
			case AdaptiveSystemPackage.NODE__TEMP:
				setTemp(TEMP_EDEFAULT);
				return;
			case AdaptiveSystemPackage.NODE__DISABLED_BY_ANTI_OCLET:
				unsetDisabledByAntiOclet();
				return;
			case AdaptiveSystemPackage.NODE__DISABLED_BY_CONFLICT:
				unsetDisabledByConflict();
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
			case AdaptiveSystemPackage.NODE__NAME:
				return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT.equals(name);
			case AdaptiveSystemPackage.NODE__ABSTRACT:
				return isSetAbstract();
			case AdaptiveSystemPackage.NODE__TEMP:
				return temp != TEMP_EDEFAULT;
			case AdaptiveSystemPackage.NODE__DISABLED_BY_ANTI_OCLET:
				return isSetDisabledByAntiOclet();
			case AdaptiveSystemPackage.NODE__DISABLED_BY_CONFLICT:
				return isSetDisabledByConflict();
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
		result.append(" (name: ");
		result.append(name);
		result.append(", abstract: ");
		if (abstractESet) result.append(abstract_); else result.append("<unset>");
		result.append(", temp: ");
		result.append(temp);
		result.append(", disabledByAntiOclet: ");
		if (disabledByAntiOcletESet) result.append(disabledByAntiOclet); else result.append("<unset>");
		result.append(", disabledByConflict: ");
		if (disabledByConflictESet) result.append(disabledByConflict); else result.append("<unset>");
		result.append(')');
		return result.toString();
	}

} //NodeImpl
