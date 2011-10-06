/**
 * Humboldt Universität zu Berlin, Theory of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem.impl;

import hub.top.adaptiveSystem.*;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EDataType;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EPackage;

import org.eclipse.emf.ecore.impl.EFactoryImpl;

import org.eclipse.emf.ecore.plugin.EcorePlugin;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model <b>Factory</b>.
 * <!-- end-user-doc -->
 * @generated
 */
public class AdaptiveSystemFactoryImpl extends EFactoryImpl implements AdaptiveSystemFactory {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static final String copyright = "Humboldt Universit\u00e4t zu Berlin, Theory of Programming";

	/**
	 * Creates the default factory implementation.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public static AdaptiveSystemFactory init() {
		try {
			AdaptiveSystemFactory theAdaptiveSystemFactory = (AdaptiveSystemFactory)EPackage.Registry.INSTANCE.getEFactory("adaptiveSystem"); 
			if (theAdaptiveSystemFactory != null) {
				return theAdaptiveSystemFactory;
			}
		}
		catch (Exception exception) {
			EcorePlugin.INSTANCE.log(exception);
		}
		return new AdaptiveSystemFactoryImpl();
	}

	/**
	 * Creates an instance of the factory.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public AdaptiveSystemFactoryImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public EObject create(EClass eClass) {
		switch (eClass.getClassifierID()) {
			case AdaptiveSystemPackage.ADAPTIVE_SYSTEM: return createAdaptiveSystem();
			case AdaptiveSystemPackage.OCLET: return createOclet();
			case AdaptiveSystemPackage.DO_NET: return createDoNet();
			case AdaptiveSystemPackage.PRE_NET: return createPreNet();
			case AdaptiveSystemPackage.ADAPTIVE_PROCESS: return createAdaptiveProcess();
			case AdaptiveSystemPackage.CONDITION: return createCondition();
			case AdaptiveSystemPackage.EVENT: return createEvent();
			case AdaptiveSystemPackage.ARC: return createArc();
			case AdaptiveSystemPackage.ARC_TO_EVENT: return createArcToEvent();
			case AdaptiveSystemPackage.ARC_TO_CONDITION: return createArcToCondition();
			default:
				throw new IllegalArgumentException("The class '" + eClass.getName() + "' is not a valid classifier");
		}
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public Object createFromString(EDataType eDataType, String initialValue) {
		switch (eDataType.getClassifierID()) {
			case AdaptiveSystemPackage.ORIENTATION:
				return createOrientationFromString(eDataType, initialValue);
			case AdaptiveSystemPackage.QUANTOR:
				return createQuantorFromString(eDataType, initialValue);
			case AdaptiveSystemPackage.TEMP:
				return createTempFromString(eDataType, initialValue);
			default:
				throw new IllegalArgumentException("The datatype '" + eDataType.getName() + "' is not a valid classifier");
		}
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public String convertToString(EDataType eDataType, Object instanceValue) {
		switch (eDataType.getClassifierID()) {
			case AdaptiveSystemPackage.ORIENTATION:
				return convertOrientationToString(eDataType, instanceValue);
			case AdaptiveSystemPackage.QUANTOR:
				return convertQuantorToString(eDataType, instanceValue);
			case AdaptiveSystemPackage.TEMP:
				return convertTempToString(eDataType, instanceValue);
			default:
				throw new IllegalArgumentException("The datatype '" + eDataType.getName() + "' is not a valid classifier");
		}
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public AdaptiveSystem createAdaptiveSystem() {
		AdaptiveSystemImpl adaptiveSystem = new AdaptiveSystemImpl();
		return adaptiveSystem;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Oclet createOclet() {
		OcletImpl oclet = new OcletImpl();
		return oclet;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated not
	 */
	public DoNet createDoNet() {
		DoNetImpl doNet = new DoNetImpl();
		doNet.setName("do");			// always set a name
		return doNet;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated not
	 */
	public PreNet createPreNet() {
		PreNetImpl preNet = new PreNetImpl();
		preNet.setName("pre");			// always set a name
		return preNet;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated NOT
	 */
	public AdaptiveProcess createAdaptiveProcess() {
		AdaptiveProcessImpl adaptiveProcess = new AdaptiveProcessImpl();
		adaptiveProcess.setName("ap");	// always set a name
		return adaptiveProcess;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Condition createCondition() {
		ConditionImpl condition = new ConditionImpl();
		condition.setName("");  // set the empty name
		return condition;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Event createEvent() {
		EventImpl event = new EventImpl();
    event.setName("");  // set the empty name
		return event;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Arc createArc() {
		ArcImpl arc = new ArcImpl();
		return arc;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public ArcToEvent createArcToEvent() {
		ArcToEventImpl arcToEvent = new ArcToEventImpl();
		return arcToEvent;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public ArcToCondition createArcToCondition() {
		ArcToConditionImpl arcToCondition = new ArcToConditionImpl();
		return arcToCondition;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Orientation createOrientationFromString(EDataType eDataType, String initialValue) {
		Orientation result = Orientation.get(initialValue);
		if (result == null) throw new IllegalArgumentException("The value '" + initialValue + "' is not a valid enumerator of '" + eDataType.getName() + "'");
		return result;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String convertOrientationToString(EDataType eDataType, Object instanceValue) {
		return instanceValue == null ? null : instanceValue.toString();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Quantor createQuantorFromString(EDataType eDataType, String initialValue) {
		Quantor result = Quantor.get(initialValue);
		if (result == null) throw new IllegalArgumentException("The value '" + initialValue + "' is not a valid enumerator of '" + eDataType.getName() + "'");
		return result;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String convertQuantorToString(EDataType eDataType, Object instanceValue) {
		return instanceValue == null ? null : instanceValue.toString();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Temp createTempFromString(EDataType eDataType, String initialValue) {
		Temp result = Temp.get(initialValue);
		if (result == null) throw new IllegalArgumentException("The value '" + initialValue + "' is not a valid enumerator of '" + eDataType.getName() + "'");
		return result;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public String convertTempToString(EDataType eDataType, Object instanceValue) {
		return instanceValue == null ? null : instanceValue.toString();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public AdaptiveSystemPackage getAdaptiveSystemPackage() {
		return (AdaptiveSystemPackage)getEPackage();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @deprecated
	 * @generated
	 */
	@Deprecated
	public static AdaptiveSystemPackage getPackage() {
		return AdaptiveSystemPackage.eINSTANCE;
	}

} //AdaptiveSystemFactoryImpl
