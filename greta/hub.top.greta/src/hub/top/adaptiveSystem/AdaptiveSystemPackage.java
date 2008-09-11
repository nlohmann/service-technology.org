/**
 * Humboldt Universität zu Berlin, Thoery of Programming
 *
 * $Id$
 */
package hub.top.adaptiveSystem;

import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EEnum;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.EReference;

/**
 * <!-- begin-user-doc -->
 * The <b>Package</b> for the model.
 * It contains accessors for the meta objects to represent
 * <ul>
 *   <li>each class,</li>
 *   <li>each feature of each class,</li>
 *   <li>each enum,</li>
 *   <li>and each data type</li>
 * </ul>
 * <!-- end-user-doc -->
 * @see hub.top.adaptiveSystem.AdaptiveSystemFactory
 * @model kind="package"
 * @generated
 */
public interface AdaptiveSystemPackage extends EPackage {
	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String copyright = "Humboldt Universit\u00e4t zu Berlin, Thoery of Programming";

	/**
	 * The package name.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String eNAME = "adaptiveSystem";

	/**
	 * The package namespace URI.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String eNS_URI = "adaptiveSystem";

	/**
	 * The package namespace name.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String eNS_PREFIX = "adaptiveSystem";

	/**
	 * The singleton instance of the package.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	AdaptiveSystemPackage eINSTANCE = hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl.init();

	/**
	 * The meta object id for the '{@link hub.top.adaptiveSystem.impl.AdaptiveSystemImpl <em>Adaptive System</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemImpl
	 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getAdaptiveSystem()
	 * @generated
	 */
	int ADAPTIVE_SYSTEM = 0;

	/**
	 * The feature id for the '<em><b>Oclets</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ADAPTIVE_SYSTEM__OCLETS = 0;

	/**
	 * The feature id for the '<em><b>Adaptive Process</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ADAPTIVE_SYSTEM__ADAPTIVE_PROCESS = 1;

	/**
	 * The feature id for the '<em><b>Set Wellformedness To Oclets</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ADAPTIVE_SYSTEM__SET_WELLFORMEDNESS_TO_OCLETS = 2;

	/**
	 * The number of structural features of the '<em>Adaptive System</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ADAPTIVE_SYSTEM_FEATURE_COUNT = 3;

	/**
	 * The meta object id for the '{@link hub.top.adaptiveSystem.impl.OcletImpl <em>Oclet</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.adaptiveSystem.impl.OcletImpl
	 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getOclet()
	 * @generated
	 */
	int OCLET = 1;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int OCLET__NAME = 0;

	/**
	 * The feature id for the '<em><b>Orientation</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int OCLET__ORIENTATION = 1;

	/**
	 * The feature id for the '<em><b>Quantor</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int OCLET__QUANTOR = 2;

	/**
	 * The feature id for the '<em><b>Pre Net</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int OCLET__PRE_NET = 3;

	/**
	 * The feature id for the '<em><b>Do Net</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int OCLET__DO_NET = 4;

	/**
	 * The feature id for the '<em><b>Well Formed</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int OCLET__WELL_FORMED = 5;

	/**
	 * The number of structural features of the '<em>Oclet</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int OCLET_FEATURE_COUNT = 6;

	/**
	 * The meta object id for the '{@link hub.top.adaptiveSystem.impl.OccurrenceNetImpl <em>Occurrence Net</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.adaptiveSystem.impl.OccurrenceNetImpl
	 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getOccurrenceNet()
	 * @generated
	 */
	int OCCURRENCE_NET = 2;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int OCCURRENCE_NET__NAME = 0;

	/**
	 * The feature id for the '<em><b>Nodes</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int OCCURRENCE_NET__NODES = 1;

	/**
	 * The feature id for the '<em><b>Arcs</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int OCCURRENCE_NET__ARCS = 2;

	/**
	 * The number of structural features of the '<em>Occurrence Net</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int OCCURRENCE_NET_FEATURE_COUNT = 3;

	/**
	 * The meta object id for the '{@link hub.top.adaptiveSystem.impl.DoNetImpl <em>Do Net</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.adaptiveSystem.impl.DoNetImpl
	 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getDoNet()
	 * @generated
	 */
	int DO_NET = 3;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DO_NET__NAME = OCCURRENCE_NET__NAME;

	/**
	 * The feature id for the '<em><b>Nodes</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DO_NET__NODES = OCCURRENCE_NET__NODES;

	/**
	 * The feature id for the '<em><b>Arcs</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DO_NET__ARCS = OCCURRENCE_NET__ARCS;

	/**
	 * The number of structural features of the '<em>Do Net</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int DO_NET_FEATURE_COUNT = OCCURRENCE_NET_FEATURE_COUNT + 0;

	/**
	 * The meta object id for the '{@link hub.top.adaptiveSystem.impl.PreNetImpl <em>Pre Net</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.adaptiveSystem.impl.PreNetImpl
	 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getPreNet()
	 * @generated
	 */
	int PRE_NET = 4;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PRE_NET__NAME = OCCURRENCE_NET__NAME;

	/**
	 * The feature id for the '<em><b>Nodes</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PRE_NET__NODES = OCCURRENCE_NET__NODES;

	/**
	 * The feature id for the '<em><b>Arcs</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PRE_NET__ARCS = OCCURRENCE_NET__ARCS;

	/**
	 * The feature id for the '<em><b>Marked Conditions</b></em>' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PRE_NET__MARKED_CONDITIONS = OCCURRENCE_NET_FEATURE_COUNT + 0;

	/**
	 * The number of structural features of the '<em>Pre Net</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PRE_NET_FEATURE_COUNT = OCCURRENCE_NET_FEATURE_COUNT + 1;

	/**
	 * The meta object id for the '{@link hub.top.adaptiveSystem.impl.AdaptiveProcessImpl <em>Adaptive Process</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.adaptiveSystem.impl.AdaptiveProcessImpl
	 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getAdaptiveProcess()
	 * @generated
	 */
	int ADAPTIVE_PROCESS = 5;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ADAPTIVE_PROCESS__NAME = OCCURRENCE_NET__NAME;

	/**
	 * The feature id for the '<em><b>Nodes</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ADAPTIVE_PROCESS__NODES = OCCURRENCE_NET__NODES;

	/**
	 * The feature id for the '<em><b>Arcs</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ADAPTIVE_PROCESS__ARCS = OCCURRENCE_NET__ARCS;

	/**
	 * The feature id for the '<em><b>Marked Conditions</b></em>' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ADAPTIVE_PROCESS__MARKED_CONDITIONS = OCCURRENCE_NET_FEATURE_COUNT + 0;

	/**
	 * The number of structural features of the '<em>Adaptive Process</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ADAPTIVE_PROCESS_FEATURE_COUNT = OCCURRENCE_NET_FEATURE_COUNT + 1;

	/**
	 * The meta object id for the '{@link hub.top.adaptiveSystem.impl.NodeImpl <em>Node</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.adaptiveSystem.impl.NodeImpl
	 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getNode()
	 * @generated
	 */
	int NODE = 6;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int NODE__NAME = 0;

	/**
	 * The feature id for the '<em><b>Abstract</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int NODE__ABSTRACT = 1;

	/**
	 * The feature id for the '<em><b>Temp</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int NODE__TEMP = 2;

	/**
	 * The feature id for the '<em><b>Disabled By Anti Oclet</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int NODE__DISABLED_BY_ANTI_OCLET = 3;

	/**
	 * The feature id for the '<em><b>Disabled By Conflict</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int NODE__DISABLED_BY_CONFLICT = 4;

	/**
	 * The number of structural features of the '<em>Node</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int NODE_FEATURE_COUNT = 5;

	/**
	 * The meta object id for the '{@link hub.top.adaptiveSystem.impl.ConditionImpl <em>Condition</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.adaptiveSystem.impl.ConditionImpl
	 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getCondition()
	 * @generated
	 */
	int CONDITION = 7;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CONDITION__NAME = NODE__NAME;

	/**
	 * The feature id for the '<em><b>Abstract</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CONDITION__ABSTRACT = NODE__ABSTRACT;

	/**
	 * The feature id for the '<em><b>Temp</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CONDITION__TEMP = NODE__TEMP;

	/**
	 * The feature id for the '<em><b>Disabled By Anti Oclet</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CONDITION__DISABLED_BY_ANTI_OCLET = NODE__DISABLED_BY_ANTI_OCLET;

	/**
	 * The feature id for the '<em><b>Disabled By Conflict</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CONDITION__DISABLED_BY_CONFLICT = NODE__DISABLED_BY_CONFLICT;

	/**
	 * The feature id for the '<em><b>Pre Events</b></em>' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CONDITION__PRE_EVENTS = NODE_FEATURE_COUNT + 0;

	/**
	 * The feature id for the '<em><b>Post Events</b></em>' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CONDITION__POST_EVENTS = NODE_FEATURE_COUNT + 1;

	/**
	 * The feature id for the '<em><b>Token</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CONDITION__TOKEN = NODE_FEATURE_COUNT + 2;

	/**
	 * The feature id for the '<em><b>Marked</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CONDITION__MARKED = NODE_FEATURE_COUNT + 3;

	/**
	 * The feature id for the '<em><b>Maximal</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CONDITION__MAXIMAL = NODE_FEATURE_COUNT + 4;

	/**
	 * The feature id for the '<em><b>Minimal</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CONDITION__MINIMAL = NODE_FEATURE_COUNT + 5;

	/**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CONDITION__INCOMING = NODE_FEATURE_COUNT + 6;

	/**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CONDITION__OUTGOING = NODE_FEATURE_COUNT + 7;

	/**
	 * The number of structural features of the '<em>Condition</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int CONDITION_FEATURE_COUNT = NODE_FEATURE_COUNT + 8;

	/**
	 * The meta object id for the '{@link hub.top.adaptiveSystem.impl.EventImpl <em>Event</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.adaptiveSystem.impl.EventImpl
	 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getEvent()
	 * @generated
	 */
	int EVENT = 8;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int EVENT__NAME = NODE__NAME;

	/**
	 * The feature id for the '<em><b>Abstract</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int EVENT__ABSTRACT = NODE__ABSTRACT;

	/**
	 * The feature id for the '<em><b>Temp</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int EVENT__TEMP = NODE__TEMP;

	/**
	 * The feature id for the '<em><b>Disabled By Anti Oclet</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int EVENT__DISABLED_BY_ANTI_OCLET = NODE__DISABLED_BY_ANTI_OCLET;

	/**
	 * The feature id for the '<em><b>Disabled By Conflict</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int EVENT__DISABLED_BY_CONFLICT = NODE__DISABLED_BY_CONFLICT;

	/**
	 * The feature id for the '<em><b>Pre Conditions</b></em>' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int EVENT__PRE_CONDITIONS = NODE_FEATURE_COUNT + 0;

	/**
	 * The feature id for the '<em><b>Post Conditions</b></em>' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int EVENT__POST_CONDITIONS = NODE_FEATURE_COUNT + 1;

	/**
	 * The feature id for the '<em><b>Saturated</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int EVENT__SATURATED = NODE_FEATURE_COUNT + 2;

	/**
	 * The feature id for the '<em><b>Enabled</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int EVENT__ENABLED = NODE_FEATURE_COUNT + 3;

	/**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int EVENT__INCOMING = NODE_FEATURE_COUNT + 4;

	/**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int EVENT__OUTGOING = NODE_FEATURE_COUNT + 5;

	/**
	 * The number of structural features of the '<em>Event</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int EVENT_FEATURE_COUNT = NODE_FEATURE_COUNT + 6;

	/**
	 * The meta object id for the '{@link hub.top.adaptiveSystem.impl.ArcImpl <em>Arc</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.adaptiveSystem.impl.ArcImpl
	 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getArc()
	 * @generated
	 */
	int ARC = 9;

	/**
	 * The feature id for the '<em><b>Weight</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC__WEIGHT = 0;

	/**
	 * The number of structural features of the '<em>Arc</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC_FEATURE_COUNT = 1;

	/**
	 * The meta object id for the '{@link hub.top.adaptiveSystem.impl.ArcToEventImpl <em>Arc To Event</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.adaptiveSystem.impl.ArcToEventImpl
	 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getArcToEvent()
	 * @generated
	 */
	int ARC_TO_EVENT = 10;

	/**
	 * The feature id for the '<em><b>Weight</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC_TO_EVENT__WEIGHT = ARC__WEIGHT;

	/**
	 * The feature id for the '<em><b>Source</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC_TO_EVENT__SOURCE = ARC_FEATURE_COUNT + 0;

	/**
	 * The feature id for the '<em><b>Destination</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC_TO_EVENT__DESTINATION = ARC_FEATURE_COUNT + 1;

	/**
	 * The number of structural features of the '<em>Arc To Event</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC_TO_EVENT_FEATURE_COUNT = ARC_FEATURE_COUNT + 2;

	/**
	 * The meta object id for the '{@link hub.top.adaptiveSystem.impl.ArcToConditionImpl <em>Arc To Condition</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.adaptiveSystem.impl.ArcToConditionImpl
	 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getArcToCondition()
	 * @generated
	 */
	int ARC_TO_CONDITION = 11;

	/**
	 * The feature id for the '<em><b>Weight</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC_TO_CONDITION__WEIGHT = ARC__WEIGHT;

	/**
	 * The feature id for the '<em><b>Source</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC_TO_CONDITION__SOURCE = ARC_FEATURE_COUNT + 0;

	/**
	 * The feature id for the '<em><b>Destination</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC_TO_CONDITION__DESTINATION = ARC_FEATURE_COUNT + 1;

	/**
	 * The number of structural features of the '<em>Arc To Condition</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC_TO_CONDITION_FEATURE_COUNT = ARC_FEATURE_COUNT + 2;

	/**
	 * The meta object id for the '{@link hub.top.adaptiveSystem.Orientation <em>Orientation</em>}' enum.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.adaptiveSystem.Orientation
	 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getOrientation()
	 * @generated
	 */
	int ORIENTATION = 12;

	/**
	 * The meta object id for the '{@link hub.top.adaptiveSystem.Quantor <em>Quantor</em>}' enum.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.adaptiveSystem.Quantor
	 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getQuantor()
	 * @generated
	 */
	int QUANTOR = 13;

	/**
	 * The meta object id for the '{@link hub.top.adaptiveSystem.Temp <em>Temp</em>}' enum.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.adaptiveSystem.Temp
	 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getTemp()
	 * @generated
	 */
	int TEMP = 14;


	/**
	 * Returns the meta object for class '{@link hub.top.adaptiveSystem.AdaptiveSystem <em>Adaptive System</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Adaptive System</em>'.
	 * @see hub.top.adaptiveSystem.AdaptiveSystem
	 * @generated
	 */
	EClass getAdaptiveSystem();

	/**
	 * Returns the meta object for the containment reference list '{@link hub.top.adaptiveSystem.AdaptiveSystem#getOclets <em>Oclets</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Oclets</em>'.
	 * @see hub.top.adaptiveSystem.AdaptiveSystem#getOclets()
	 * @see #getAdaptiveSystem()
	 * @generated
	 */
	EReference getAdaptiveSystem_Oclets();

	/**
	 * Returns the meta object for the containment reference '{@link hub.top.adaptiveSystem.AdaptiveSystem#getAdaptiveProcess <em>Adaptive Process</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Adaptive Process</em>'.
	 * @see hub.top.adaptiveSystem.AdaptiveSystem#getAdaptiveProcess()
	 * @see #getAdaptiveSystem()
	 * @generated
	 */
	EReference getAdaptiveSystem_AdaptiveProcess();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.adaptiveSystem.AdaptiveSystem#isSetWellformednessToOclets <em>Set Wellformedness To Oclets</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Set Wellformedness To Oclets</em>'.
	 * @see hub.top.adaptiveSystem.AdaptiveSystem#isSetWellformednessToOclets()
	 * @see #getAdaptiveSystem()
	 * @generated
	 */
	EAttribute getAdaptiveSystem_SetWellformednessToOclets();

	/**
	 * Returns the meta object for class '{@link hub.top.adaptiveSystem.Oclet <em>Oclet</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Oclet</em>'.
	 * @see hub.top.adaptiveSystem.Oclet
	 * @generated
	 */
	EClass getOclet();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.adaptiveSystem.Oclet#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see hub.top.adaptiveSystem.Oclet#getName()
	 * @see #getOclet()
	 * @generated
	 */
	EAttribute getOclet_Name();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.adaptiveSystem.Oclet#getOrientation <em>Orientation</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Orientation</em>'.
	 * @see hub.top.adaptiveSystem.Oclet#getOrientation()
	 * @see #getOclet()
	 * @generated
	 */
	EAttribute getOclet_Orientation();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.adaptiveSystem.Oclet#getQuantor <em>Quantor</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Quantor</em>'.
	 * @see hub.top.adaptiveSystem.Oclet#getQuantor()
	 * @see #getOclet()
	 * @generated
	 */
	EAttribute getOclet_Quantor();

	/**
	 * Returns the meta object for the containment reference '{@link hub.top.adaptiveSystem.Oclet#getPreNet <em>Pre Net</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Pre Net</em>'.
	 * @see hub.top.adaptiveSystem.Oclet#getPreNet()
	 * @see #getOclet()
	 * @generated
	 */
	EReference getOclet_PreNet();

	/**
	 * Returns the meta object for the containment reference '{@link hub.top.adaptiveSystem.Oclet#getDoNet <em>Do Net</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Do Net</em>'.
	 * @see hub.top.adaptiveSystem.Oclet#getDoNet()
	 * @see #getOclet()
	 * @generated
	 */
	EReference getOclet_DoNet();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.adaptiveSystem.Oclet#isWellFormed <em>Well Formed</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Well Formed</em>'.
	 * @see hub.top.adaptiveSystem.Oclet#isWellFormed()
	 * @see #getOclet()
	 * @generated
	 */
	EAttribute getOclet_WellFormed();

	/**
	 * Returns the meta object for class '{@link hub.top.adaptiveSystem.OccurrenceNet <em>Occurrence Net</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Occurrence Net</em>'.
	 * @see hub.top.adaptiveSystem.OccurrenceNet
	 * @generated
	 */
	EClass getOccurrenceNet();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.adaptiveSystem.OccurrenceNet#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see hub.top.adaptiveSystem.OccurrenceNet#getName()
	 * @see #getOccurrenceNet()
	 * @generated
	 */
	EAttribute getOccurrenceNet_Name();

	/**
	 * Returns the meta object for the containment reference list '{@link hub.top.adaptiveSystem.OccurrenceNet#getNodes <em>Nodes</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Nodes</em>'.
	 * @see hub.top.adaptiveSystem.OccurrenceNet#getNodes()
	 * @see #getOccurrenceNet()
	 * @generated
	 */
	EReference getOccurrenceNet_Nodes();

	/**
	 * Returns the meta object for the containment reference list '{@link hub.top.adaptiveSystem.OccurrenceNet#getArcs <em>Arcs</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Arcs</em>'.
	 * @see hub.top.adaptiveSystem.OccurrenceNet#getArcs()
	 * @see #getOccurrenceNet()
	 * @generated
	 */
	EReference getOccurrenceNet_Arcs();

	/**
	 * Returns the meta object for class '{@link hub.top.adaptiveSystem.DoNet <em>Do Net</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Do Net</em>'.
	 * @see hub.top.adaptiveSystem.DoNet
	 * @generated
	 */
	EClass getDoNet();

	/**
	 * Returns the meta object for class '{@link hub.top.adaptiveSystem.PreNet <em>Pre Net</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Pre Net</em>'.
	 * @see hub.top.adaptiveSystem.PreNet
	 * @generated
	 */
	EClass getPreNet();

	/**
	 * Returns the meta object for the reference list '{@link hub.top.adaptiveSystem.PreNet#getMarkedConditions <em>Marked Conditions</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Marked Conditions</em>'.
	 * @see hub.top.adaptiveSystem.PreNet#getMarkedConditions()
	 * @see #getPreNet()
	 * @generated
	 */
	EReference getPreNet_MarkedConditions();

	/**
	 * Returns the meta object for class '{@link hub.top.adaptiveSystem.AdaptiveProcess <em>Adaptive Process</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Adaptive Process</em>'.
	 * @see hub.top.adaptiveSystem.AdaptiveProcess
	 * @generated
	 */
	EClass getAdaptiveProcess();

	/**
	 * Returns the meta object for the reference list '{@link hub.top.adaptiveSystem.AdaptiveProcess#getMarkedConditions <em>Marked Conditions</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Marked Conditions</em>'.
	 * @see hub.top.adaptiveSystem.AdaptiveProcess#getMarkedConditions()
	 * @see #getAdaptiveProcess()
	 * @generated
	 */
	EReference getAdaptiveProcess_MarkedConditions();

	/**
	 * Returns the meta object for class '{@link hub.top.adaptiveSystem.Node <em>Node</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Node</em>'.
	 * @see hub.top.adaptiveSystem.Node
	 * @generated
	 */
	EClass getNode();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.adaptiveSystem.Node#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see hub.top.adaptiveSystem.Node#getName()
	 * @see #getNode()
	 * @generated
	 */
	EAttribute getNode_Name();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.adaptiveSystem.Node#isAbstract <em>Abstract</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Abstract</em>'.
	 * @see hub.top.adaptiveSystem.Node#isAbstract()
	 * @see #getNode()
	 * @generated
	 */
	EAttribute getNode_Abstract();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.adaptiveSystem.Node#getTemp <em>Temp</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Temp</em>'.
	 * @see hub.top.adaptiveSystem.Node#getTemp()
	 * @see #getNode()
	 * @generated
	 */
	EAttribute getNode_Temp();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.adaptiveSystem.Node#isDisabledByAntiOclet <em>Disabled By Anti Oclet</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Disabled By Anti Oclet</em>'.
	 * @see hub.top.adaptiveSystem.Node#isDisabledByAntiOclet()
	 * @see #getNode()
	 * @generated
	 */
	EAttribute getNode_DisabledByAntiOclet();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.adaptiveSystem.Node#isDisabledByConflict <em>Disabled By Conflict</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Disabled By Conflict</em>'.
	 * @see hub.top.adaptiveSystem.Node#isDisabledByConflict()
	 * @see #getNode()
	 * @generated
	 */
	EAttribute getNode_DisabledByConflict();

	/**
	 * Returns the meta object for class '{@link hub.top.adaptiveSystem.Condition <em>Condition</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Condition</em>'.
	 * @see hub.top.adaptiveSystem.Condition
	 * @generated
	 */
	EClass getCondition();

	/**
	 * Returns the meta object for the reference list '{@link hub.top.adaptiveSystem.Condition#getPreEvents <em>Pre Events</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Pre Events</em>'.
	 * @see hub.top.adaptiveSystem.Condition#getPreEvents()
	 * @see #getCondition()
	 * @generated
	 */
	EReference getCondition_PreEvents();

	/**
	 * Returns the meta object for the reference list '{@link hub.top.adaptiveSystem.Condition#getPostEvents <em>Post Events</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Post Events</em>'.
	 * @see hub.top.adaptiveSystem.Condition#getPostEvents()
	 * @see #getCondition()
	 * @generated
	 */
	EReference getCondition_PostEvents();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.adaptiveSystem.Condition#getToken <em>Token</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Token</em>'.
	 * @see hub.top.adaptiveSystem.Condition#getToken()
	 * @see #getCondition()
	 * @generated
	 */
	EAttribute getCondition_Token();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.adaptiveSystem.Condition#isMarked <em>Marked</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Marked</em>'.
	 * @see hub.top.adaptiveSystem.Condition#isMarked()
	 * @see #getCondition()
	 * @generated
	 */
	EAttribute getCondition_Marked();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.adaptiveSystem.Condition#isMaximal <em>Maximal</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Maximal</em>'.
	 * @see hub.top.adaptiveSystem.Condition#isMaximal()
	 * @see #getCondition()
	 * @generated
	 */
	EAttribute getCondition_Maximal();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.adaptiveSystem.Condition#isMinimal <em>Minimal</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Minimal</em>'.
	 * @see hub.top.adaptiveSystem.Condition#isMinimal()
	 * @see #getCondition()
	 * @generated
	 */
	EAttribute getCondition_Minimal();

	/**
	 * Returns the meta object for the reference list '{@link hub.top.adaptiveSystem.Condition#getIncoming <em>Incoming</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Incoming</em>'.
	 * @see hub.top.adaptiveSystem.Condition#getIncoming()
	 * @see #getCondition()
	 * @generated
	 */
	EReference getCondition_Incoming();

	/**
	 * Returns the meta object for the reference list '{@link hub.top.adaptiveSystem.Condition#getOutgoing <em>Outgoing</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Outgoing</em>'.
	 * @see hub.top.adaptiveSystem.Condition#getOutgoing()
	 * @see #getCondition()
	 * @generated
	 */
	EReference getCondition_Outgoing();

	/**
	 * Returns the meta object for class '{@link hub.top.adaptiveSystem.Event <em>Event</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Event</em>'.
	 * @see hub.top.adaptiveSystem.Event
	 * @generated
	 */
	EClass getEvent();

	/**
	 * Returns the meta object for the reference list '{@link hub.top.adaptiveSystem.Event#getPreConditions <em>Pre Conditions</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Pre Conditions</em>'.
	 * @see hub.top.adaptiveSystem.Event#getPreConditions()
	 * @see #getEvent()
	 * @generated
	 */
	EReference getEvent_PreConditions();

	/**
	 * Returns the meta object for the reference list '{@link hub.top.adaptiveSystem.Event#getPostConditions <em>Post Conditions</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Post Conditions</em>'.
	 * @see hub.top.adaptiveSystem.Event#getPostConditions()
	 * @see #getEvent()
	 * @generated
	 */
	EReference getEvent_PostConditions();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.adaptiveSystem.Event#isSaturated <em>Saturated</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Saturated</em>'.
	 * @see hub.top.adaptiveSystem.Event#isSaturated()
	 * @see #getEvent()
	 * @generated
	 */
	EAttribute getEvent_Saturated();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.adaptiveSystem.Event#isEnabled <em>Enabled</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Enabled</em>'.
	 * @see hub.top.adaptiveSystem.Event#isEnabled()
	 * @see #getEvent()
	 * @generated
	 */
	EAttribute getEvent_Enabled();

	/**
	 * Returns the meta object for the reference list '{@link hub.top.adaptiveSystem.Event#getIncoming <em>Incoming</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Incoming</em>'.
	 * @see hub.top.adaptiveSystem.Event#getIncoming()
	 * @see #getEvent()
	 * @generated
	 */
	EReference getEvent_Incoming();

	/**
	 * Returns the meta object for the reference list '{@link hub.top.adaptiveSystem.Event#getOutgoing <em>Outgoing</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Outgoing</em>'.
	 * @see hub.top.adaptiveSystem.Event#getOutgoing()
	 * @see #getEvent()
	 * @generated
	 */
	EReference getEvent_Outgoing();

	/**
	 * Returns the meta object for class '{@link hub.top.adaptiveSystem.Arc <em>Arc</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Arc</em>'.
	 * @see hub.top.adaptiveSystem.Arc
	 * @generated
	 */
	EClass getArc();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.adaptiveSystem.Arc#getWeight <em>Weight</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Weight</em>'.
	 * @see hub.top.adaptiveSystem.Arc#getWeight()
	 * @see #getArc()
	 * @generated
	 */
	EAttribute getArc_Weight();

	/**
	 * Returns the meta object for class '{@link hub.top.adaptiveSystem.ArcToEvent <em>Arc To Event</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Arc To Event</em>'.
	 * @see hub.top.adaptiveSystem.ArcToEvent
	 * @generated
	 */
	EClass getArcToEvent();

	/**
	 * Returns the meta object for the reference '{@link hub.top.adaptiveSystem.ArcToEvent#getSource <em>Source</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference '<em>Source</em>'.
	 * @see hub.top.adaptiveSystem.ArcToEvent#getSource()
	 * @see #getArcToEvent()
	 * @generated
	 */
	EReference getArcToEvent_Source();

	/**
	 * Returns the meta object for the reference '{@link hub.top.adaptiveSystem.ArcToEvent#getDestination <em>Destination</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference '<em>Destination</em>'.
	 * @see hub.top.adaptiveSystem.ArcToEvent#getDestination()
	 * @see #getArcToEvent()
	 * @generated
	 */
	EReference getArcToEvent_Destination();

	/**
	 * Returns the meta object for class '{@link hub.top.adaptiveSystem.ArcToCondition <em>Arc To Condition</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Arc To Condition</em>'.
	 * @see hub.top.adaptiveSystem.ArcToCondition
	 * @generated
	 */
	EClass getArcToCondition();

	/**
	 * Returns the meta object for the reference '{@link hub.top.adaptiveSystem.ArcToCondition#getSource <em>Source</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference '<em>Source</em>'.
	 * @see hub.top.adaptiveSystem.ArcToCondition#getSource()
	 * @see #getArcToCondition()
	 * @generated
	 */
	EReference getArcToCondition_Source();

	/**
	 * Returns the meta object for the reference '{@link hub.top.adaptiveSystem.ArcToCondition#getDestination <em>Destination</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference '<em>Destination</em>'.
	 * @see hub.top.adaptiveSystem.ArcToCondition#getDestination()
	 * @see #getArcToCondition()
	 * @generated
	 */
	EReference getArcToCondition_Destination();

	/**
	 * Returns the meta object for enum '{@link hub.top.adaptiveSystem.Orientation <em>Orientation</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for enum '<em>Orientation</em>'.
	 * @see hub.top.adaptiveSystem.Orientation
	 * @generated
	 */
	EEnum getOrientation();

	/**
	 * Returns the meta object for enum '{@link hub.top.adaptiveSystem.Quantor <em>Quantor</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for enum '<em>Quantor</em>'.
	 * @see hub.top.adaptiveSystem.Quantor
	 * @generated
	 */
	EEnum getQuantor();

	/**
	 * Returns the meta object for enum '{@link hub.top.adaptiveSystem.Temp <em>Temp</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for enum '<em>Temp</em>'.
	 * @see hub.top.adaptiveSystem.Temp
	 * @generated
	 */
	EEnum getTemp();

	/**
	 * Returns the factory that creates the instances of the model.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the factory that creates the instances of the model.
	 * @generated
	 */
	AdaptiveSystemFactory getAdaptiveSystemFactory();

	/**
	 * <!-- begin-user-doc -->
	 * Defines literals for the meta objects that represent
	 * <ul>
	 *   <li>each class,</li>
	 *   <li>each feature of each class,</li>
	 *   <li>each enum,</li>
	 *   <li>and each data type</li>
	 * </ul>
	 * <!-- end-user-doc -->
	 * @generated
	 */
	interface Literals {
		/**
		 * The meta object literal for the '{@link hub.top.adaptiveSystem.impl.AdaptiveSystemImpl <em>Adaptive System</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemImpl
		 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getAdaptiveSystem()
		 * @generated
		 */
		EClass ADAPTIVE_SYSTEM = eINSTANCE.getAdaptiveSystem();

		/**
		 * The meta object literal for the '<em><b>Oclets</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference ADAPTIVE_SYSTEM__OCLETS = eINSTANCE.getAdaptiveSystem_Oclets();

		/**
		 * The meta object literal for the '<em><b>Adaptive Process</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference ADAPTIVE_SYSTEM__ADAPTIVE_PROCESS = eINSTANCE.getAdaptiveSystem_AdaptiveProcess();

		/**
		 * The meta object literal for the '<em><b>Set Wellformedness To Oclets</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute ADAPTIVE_SYSTEM__SET_WELLFORMEDNESS_TO_OCLETS = eINSTANCE.getAdaptiveSystem_SetWellformednessToOclets();

		/**
		 * The meta object literal for the '{@link hub.top.adaptiveSystem.impl.OcletImpl <em>Oclet</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.adaptiveSystem.impl.OcletImpl
		 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getOclet()
		 * @generated
		 */
		EClass OCLET = eINSTANCE.getOclet();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute OCLET__NAME = eINSTANCE.getOclet_Name();

		/**
		 * The meta object literal for the '<em><b>Orientation</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute OCLET__ORIENTATION = eINSTANCE.getOclet_Orientation();

		/**
		 * The meta object literal for the '<em><b>Quantor</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute OCLET__QUANTOR = eINSTANCE.getOclet_Quantor();

		/**
		 * The meta object literal for the '<em><b>Pre Net</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference OCLET__PRE_NET = eINSTANCE.getOclet_PreNet();

		/**
		 * The meta object literal for the '<em><b>Do Net</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference OCLET__DO_NET = eINSTANCE.getOclet_DoNet();

		/**
		 * The meta object literal for the '<em><b>Well Formed</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute OCLET__WELL_FORMED = eINSTANCE.getOclet_WellFormed();

		/**
		 * The meta object literal for the '{@link hub.top.adaptiveSystem.impl.OccurrenceNetImpl <em>Occurrence Net</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.adaptiveSystem.impl.OccurrenceNetImpl
		 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getOccurrenceNet()
		 * @generated
		 */
		EClass OCCURRENCE_NET = eINSTANCE.getOccurrenceNet();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute OCCURRENCE_NET__NAME = eINSTANCE.getOccurrenceNet_Name();

		/**
		 * The meta object literal for the '<em><b>Nodes</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference OCCURRENCE_NET__NODES = eINSTANCE.getOccurrenceNet_Nodes();

		/**
		 * The meta object literal for the '<em><b>Arcs</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference OCCURRENCE_NET__ARCS = eINSTANCE.getOccurrenceNet_Arcs();

		/**
		 * The meta object literal for the '{@link hub.top.adaptiveSystem.impl.DoNetImpl <em>Do Net</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.adaptiveSystem.impl.DoNetImpl
		 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getDoNet()
		 * @generated
		 */
		EClass DO_NET = eINSTANCE.getDoNet();

		/**
		 * The meta object literal for the '{@link hub.top.adaptiveSystem.impl.PreNetImpl <em>Pre Net</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.adaptiveSystem.impl.PreNetImpl
		 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getPreNet()
		 * @generated
		 */
		EClass PRE_NET = eINSTANCE.getPreNet();

		/**
		 * The meta object literal for the '<em><b>Marked Conditions</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference PRE_NET__MARKED_CONDITIONS = eINSTANCE.getPreNet_MarkedConditions();

		/**
		 * The meta object literal for the '{@link hub.top.adaptiveSystem.impl.AdaptiveProcessImpl <em>Adaptive Process</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.adaptiveSystem.impl.AdaptiveProcessImpl
		 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getAdaptiveProcess()
		 * @generated
		 */
		EClass ADAPTIVE_PROCESS = eINSTANCE.getAdaptiveProcess();

		/**
		 * The meta object literal for the '<em><b>Marked Conditions</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference ADAPTIVE_PROCESS__MARKED_CONDITIONS = eINSTANCE.getAdaptiveProcess_MarkedConditions();

		/**
		 * The meta object literal for the '{@link hub.top.adaptiveSystem.impl.NodeImpl <em>Node</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.adaptiveSystem.impl.NodeImpl
		 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getNode()
		 * @generated
		 */
		EClass NODE = eINSTANCE.getNode();

		/**
		 * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute NODE__NAME = eINSTANCE.getNode_Name();

		/**
		 * The meta object literal for the '<em><b>Abstract</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute NODE__ABSTRACT = eINSTANCE.getNode_Abstract();

		/**
		 * The meta object literal for the '<em><b>Temp</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute NODE__TEMP = eINSTANCE.getNode_Temp();

		/**
		 * The meta object literal for the '<em><b>Disabled By Anti Oclet</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute NODE__DISABLED_BY_ANTI_OCLET = eINSTANCE.getNode_DisabledByAntiOclet();

		/**
		 * The meta object literal for the '<em><b>Disabled By Conflict</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute NODE__DISABLED_BY_CONFLICT = eINSTANCE.getNode_DisabledByConflict();

		/**
		 * The meta object literal for the '{@link hub.top.adaptiveSystem.impl.ConditionImpl <em>Condition</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.adaptiveSystem.impl.ConditionImpl
		 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getCondition()
		 * @generated
		 */
		EClass CONDITION = eINSTANCE.getCondition();

		/**
		 * The meta object literal for the '<em><b>Pre Events</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference CONDITION__PRE_EVENTS = eINSTANCE.getCondition_PreEvents();

		/**
		 * The meta object literal for the '<em><b>Post Events</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference CONDITION__POST_EVENTS = eINSTANCE.getCondition_PostEvents();

		/**
		 * The meta object literal for the '<em><b>Token</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute CONDITION__TOKEN = eINSTANCE.getCondition_Token();

		/**
		 * The meta object literal for the '<em><b>Marked</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute CONDITION__MARKED = eINSTANCE.getCondition_Marked();

		/**
		 * The meta object literal for the '<em><b>Maximal</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute CONDITION__MAXIMAL = eINSTANCE.getCondition_Maximal();

		/**
		 * The meta object literal for the '<em><b>Minimal</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute CONDITION__MINIMAL = eINSTANCE.getCondition_Minimal();

		/**
		 * The meta object literal for the '<em><b>Incoming</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference CONDITION__INCOMING = eINSTANCE.getCondition_Incoming();

		/**
		 * The meta object literal for the '<em><b>Outgoing</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference CONDITION__OUTGOING = eINSTANCE.getCondition_Outgoing();

		/**
		 * The meta object literal for the '{@link hub.top.adaptiveSystem.impl.EventImpl <em>Event</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.adaptiveSystem.impl.EventImpl
		 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getEvent()
		 * @generated
		 */
		EClass EVENT = eINSTANCE.getEvent();

		/**
		 * The meta object literal for the '<em><b>Pre Conditions</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference EVENT__PRE_CONDITIONS = eINSTANCE.getEvent_PreConditions();

		/**
		 * The meta object literal for the '<em><b>Post Conditions</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference EVENT__POST_CONDITIONS = eINSTANCE.getEvent_PostConditions();

		/**
		 * The meta object literal for the '<em><b>Saturated</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute EVENT__SATURATED = eINSTANCE.getEvent_Saturated();

		/**
		 * The meta object literal for the '<em><b>Enabled</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute EVENT__ENABLED = eINSTANCE.getEvent_Enabled();

		/**
		 * The meta object literal for the '<em><b>Incoming</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference EVENT__INCOMING = eINSTANCE.getEvent_Incoming();

		/**
		 * The meta object literal for the '<em><b>Outgoing</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference EVENT__OUTGOING = eINSTANCE.getEvent_Outgoing();

		/**
		 * The meta object literal for the '{@link hub.top.adaptiveSystem.impl.ArcImpl <em>Arc</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.adaptiveSystem.impl.ArcImpl
		 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getArc()
		 * @generated
		 */
		EClass ARC = eINSTANCE.getArc();

		/**
		 * The meta object literal for the '<em><b>Weight</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute ARC__WEIGHT = eINSTANCE.getArc_Weight();

		/**
		 * The meta object literal for the '{@link hub.top.adaptiveSystem.impl.ArcToEventImpl <em>Arc To Event</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.adaptiveSystem.impl.ArcToEventImpl
		 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getArcToEvent()
		 * @generated
		 */
		EClass ARC_TO_EVENT = eINSTANCE.getArcToEvent();

		/**
		 * The meta object literal for the '<em><b>Source</b></em>' reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference ARC_TO_EVENT__SOURCE = eINSTANCE.getArcToEvent_Source();

		/**
		 * The meta object literal for the '<em><b>Destination</b></em>' reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference ARC_TO_EVENT__DESTINATION = eINSTANCE.getArcToEvent_Destination();

		/**
		 * The meta object literal for the '{@link hub.top.adaptiveSystem.impl.ArcToConditionImpl <em>Arc To Condition</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.adaptiveSystem.impl.ArcToConditionImpl
		 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getArcToCondition()
		 * @generated
		 */
		EClass ARC_TO_CONDITION = eINSTANCE.getArcToCondition();

		/**
		 * The meta object literal for the '<em><b>Source</b></em>' reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference ARC_TO_CONDITION__SOURCE = eINSTANCE.getArcToCondition_Source();

		/**
		 * The meta object literal for the '<em><b>Destination</b></em>' reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference ARC_TO_CONDITION__DESTINATION = eINSTANCE.getArcToCondition_Destination();

		/**
		 * The meta object literal for the '{@link hub.top.adaptiveSystem.Orientation <em>Orientation</em>}' enum.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.adaptiveSystem.Orientation
		 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getOrientation()
		 * @generated
		 */
		EEnum ORIENTATION = eINSTANCE.getOrientation();

		/**
		 * The meta object literal for the '{@link hub.top.adaptiveSystem.Quantor <em>Quantor</em>}' enum.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.adaptiveSystem.Quantor
		 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getQuantor()
		 * @generated
		 */
		EEnum QUANTOR = eINSTANCE.getQuantor();

		/**
		 * The meta object literal for the '{@link hub.top.adaptiveSystem.Temp <em>Temp</em>}' enum.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.adaptiveSystem.Temp
		 * @see hub.top.adaptiveSystem.impl.AdaptiveSystemPackageImpl#getTemp()
		 * @generated
		 */
		EEnum TEMP = eINSTANCE.getTemp();

	}

} //AdaptiveSystemPackage
