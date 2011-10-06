/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.editor.ptnetLoLA;

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
 * @see hub.top.editor.ptnetLoLA.PtnetLoLAFactory
 * @model kind="package"
 * @generated
 */
public interface PtnetLoLAPackage extends EPackage {
	/**
	 * The package name.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String eNAME = "ptnetLoLA";

	/**
	 * The package namespace URI.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String eNS_URI = "http://www.informatik.hu-berlin.de/top/tools/editors/petrinets";

	/**
	 * The package namespace name.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	String eNS_PREFIX = "ptnetLoLA";

	/**
	 * The singleton instance of the package.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	PtnetLoLAPackage eINSTANCE = hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl.init();

	/**
	 * The meta object id for the '{@link hub.top.editor.ptnetLoLA.impl.NodeImpl <em>Node</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.editor.ptnetLoLA.impl.NodeImpl
	 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getNode()
	 * @generated
	 */
	int NODE = 3;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int NODE__NAME = 0;

	/**
	 * The feature id for the '<em><b>Annotation</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int NODE__ANNOTATION = 1;

	/**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int NODE__INCOMING = 2;

	/**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int NODE__OUTGOING = 3;

	/**
	 * The feature id for the '<em><b>Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int NODE__TYPE = 4;

	/**
	 * The number of structural features of the '<em>Node</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int NODE_FEATURE_COUNT = 5;

	/**
	 * The meta object id for the '{@link hub.top.editor.ptnetLoLA.impl.PlaceImpl <em>Place</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.editor.ptnetLoLA.impl.PlaceImpl
	 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getPlace()
	 * @generated
	 */
	int PLACE = 0;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PLACE__NAME = NODE__NAME;

	/**
	 * The feature id for the '<em><b>Annotation</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PLACE__ANNOTATION = NODE__ANNOTATION;

	/**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PLACE__INCOMING = NODE__INCOMING;

	/**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PLACE__OUTGOING = NODE__OUTGOING;

	/**
	 * The feature id for the '<em><b>Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PLACE__TYPE = NODE__TYPE;

	/**
	 * The feature id for the '<em><b>Token</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PLACE__TOKEN = NODE_FEATURE_COUNT + 0;

	/**
	 * The feature id for the '<em><b>Final Marking</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PLACE__FINAL_MARKING = NODE_FEATURE_COUNT + 1;

	/**
	 * The number of structural features of the '<em>Place</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PLACE_FEATURE_COUNT = NODE_FEATURE_COUNT + 2;

	/**
	 * The meta object id for the '{@link hub.top.editor.ptnetLoLA.impl.PtNetImpl <em>Pt Net</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.editor.ptnetLoLA.impl.PtNetImpl
	 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getPtNet()
	 * @generated
	 */
	int PT_NET = 1;

	/**
	 * The feature id for the '<em><b>Places</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PT_NET__PLACES = 0;

	/**
	 * The feature id for the '<em><b>Transitions</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PT_NET__TRANSITIONS = 1;

	/**
	 * The feature id for the '<em><b>Initial Marking</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PT_NET__INITIAL_MARKING = 2;

	/**
	 * The feature id for the '<em><b>Annotation</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PT_NET__ANNOTATION = 3;

	/**
	 * The feature id for the '<em><b>Arcs</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PT_NET__ARCS = 4;

	/**
	 * The feature id for the '<em><b>Final Marking</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PT_NET__FINAL_MARKING = 5;

	/**
	 * The number of structural features of the '<em>Pt Net</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PT_NET_FEATURE_COUNT = 6;

	/**
	 * The meta object id for the '{@link hub.top.editor.ptnetLoLA.impl.TransitionImpl <em>Transition</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.editor.ptnetLoLA.impl.TransitionImpl
	 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getTransition()
	 * @generated
	 */
	int TRANSITION = 2;

	/**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int TRANSITION__NAME = NODE__NAME;

	/**
	 * The feature id for the '<em><b>Annotation</b></em>' containment reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int TRANSITION__ANNOTATION = NODE__ANNOTATION;

	/**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int TRANSITION__INCOMING = NODE__INCOMING;

	/**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int TRANSITION__OUTGOING = NODE__OUTGOING;

	/**
	 * The feature id for the '<em><b>Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int TRANSITION__TYPE = NODE__TYPE;

	/**
	 * The number of structural features of the '<em>Transition</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int TRANSITION_FEATURE_COUNT = NODE_FEATURE_COUNT + 0;

	/**
	 * The meta object id for the '{@link hub.top.editor.ptnetLoLA.impl.MarkingImpl <em>Marking</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.editor.ptnetLoLA.impl.MarkingImpl
	 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getMarking()
	 * @generated
	 */
	int MARKING = 4;

	/**
	 * The feature id for the '<em><b>Places</b></em>' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int MARKING__PLACES = 0;

	/**
	 * The number of structural features of the '<em>Marking</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int MARKING_FEATURE_COUNT = 1;

	/**
	 * The meta object id for the '{@link hub.top.editor.ptnetLoLA.impl.PlaceReferenceImpl <em>Place Reference</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.editor.ptnetLoLA.impl.PlaceReferenceImpl
	 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getPlaceReference()
	 * @generated
	 */
	int PLACE_REFERENCE = 5;

	/**
	 * The feature id for the '<em><b>Place</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PLACE_REFERENCE__PLACE = 0;

	/**
	 * The number of structural features of the '<em>Place Reference</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PLACE_REFERENCE_FEATURE_COUNT = 1;

	/**
	 * The meta object id for the '{@link hub.top.editor.ptnetLoLA.impl.RefMarkedPlaceImpl <em>Ref Marked Place</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.editor.ptnetLoLA.impl.RefMarkedPlaceImpl
	 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getRefMarkedPlace()
	 * @generated
	 */
	int REF_MARKED_PLACE = 6;

	/**
	 * The feature id for the '<em><b>Place</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int REF_MARKED_PLACE__PLACE = PLACE_REFERENCE__PLACE;

	/**
	 * The feature id for the '<em><b>Token</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int REF_MARKED_PLACE__TOKEN = PLACE_REFERENCE_FEATURE_COUNT + 0;

	/**
	 * The number of structural features of the '<em>Ref Marked Place</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int REF_MARKED_PLACE_FEATURE_COUNT = PLACE_REFERENCE_FEATURE_COUNT + 1;

	/**
	 * The meta object id for the '{@link hub.top.editor.ptnetLoLA.impl.AnnotationImpl <em>Annotation</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.editor.ptnetLoLA.impl.AnnotationImpl
	 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getAnnotation()
	 * @generated
	 */
	int ANNOTATION = 7;

	/**
	 * The feature id for the '<em><b>Text</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ANNOTATION__TEXT = 0;

	/**
	 * The number of structural features of the '<em>Annotation</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ANNOTATION_FEATURE_COUNT = 1;

	/**
	 * The meta object id for the '{@link hub.top.editor.ptnetLoLA.impl.ArcImpl <em>Arc</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.editor.ptnetLoLA.impl.ArcImpl
	 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getArc()
	 * @generated
	 */
	int ARC = 8;

	/**
	 * The feature id for the '<em><b>Source</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC__SOURCE = 0;

	/**
	 * The feature id for the '<em><b>Target</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC__TARGET = 1;

	/**
	 * The feature id for the '<em><b>Weight</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC__WEIGHT = 2;

	/**
	 * The number of structural features of the '<em>Arc</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC_FEATURE_COUNT = 3;

	/**
	 * The meta object id for the '{@link hub.top.editor.ptnetLoLA.impl.ArcToPlaceImpl <em>Arc To Place</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.editor.ptnetLoLA.impl.ArcToPlaceImpl
	 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getArcToPlace()
	 * @generated
	 */
	int ARC_TO_PLACE = 9;

	/**
	 * The feature id for the '<em><b>Source</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC_TO_PLACE__SOURCE = ARC__SOURCE;

	/**
	 * The feature id for the '<em><b>Target</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC_TO_PLACE__TARGET = ARC__TARGET;

	/**
	 * The feature id for the '<em><b>Weight</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC_TO_PLACE__WEIGHT = ARC__WEIGHT;

	/**
	 * The number of structural features of the '<em>Arc To Place</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC_TO_PLACE_FEATURE_COUNT = ARC_FEATURE_COUNT + 0;

	/**
	 * The meta object id for the '{@link hub.top.editor.ptnetLoLA.impl.ArcToTransitionImpl <em>Arc To Transition</em>}' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.editor.ptnetLoLA.impl.ArcToTransitionImpl
	 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getArcToTransition()
	 * @generated
	 */
	int ARC_TO_TRANSITION = 10;

	/**
	 * The feature id for the '<em><b>Source</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC_TO_TRANSITION__SOURCE = ARC__SOURCE;

	/**
	 * The feature id for the '<em><b>Target</b></em>' reference.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC_TO_TRANSITION__TARGET = ARC__TARGET;

	/**
	 * The feature id for the '<em><b>Weight</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC_TO_TRANSITION__WEIGHT = ARC__WEIGHT;

	/**
	 * The number of structural features of the '<em>Arc To Transition</em>' class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int ARC_TO_TRANSITION_FEATURE_COUNT = ARC_FEATURE_COUNT + 0;


	/**
	 * The meta object id for the '{@link hub.top.editor.ptnetLoLA.impl.TransitionExtImpl <em>Transition Ext</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.editor.ptnetLoLA.impl.TransitionExtImpl
	 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getTransitionExt()
	 * @generated
	 */
  int TRANSITION_EXT = 11;

  /**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int TRANSITION_EXT__NAME = TRANSITION__NAME;

  /**
	 * The feature id for the '<em><b>Annotation</b></em>' containment reference.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int TRANSITION_EXT__ANNOTATION = TRANSITION__ANNOTATION;

  /**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int TRANSITION_EXT__INCOMING = TRANSITION__INCOMING;

  /**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int TRANSITION_EXT__OUTGOING = TRANSITION__OUTGOING;

  /**
	 * The feature id for the '<em><b>Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int TRANSITION_EXT__TYPE = TRANSITION__TYPE;

  /**
	 * The feature id for the '<em><b>Probability</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int TRANSITION_EXT__PROBABILITY = TRANSITION_FEATURE_COUNT + 0;

  /**
	 * The feature id for the '<em><b>Min Time</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int TRANSITION_EXT__MIN_TIME = TRANSITION_FEATURE_COUNT + 1;

		/**
	 * The feature id for the '<em><b>Cost</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int TRANSITION_EXT__COST = TRANSITION_FEATURE_COUNT + 2;

		/**
	 * The feature id for the '<em><b>Max Time</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int TRANSITION_EXT__MAX_TIME = TRANSITION_FEATURE_COUNT + 3;

		/**
	 * The number of structural features of the '<em>Transition Ext</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int TRANSITION_EXT_FEATURE_COUNT = TRANSITION_FEATURE_COUNT + 4;

  /**
	 * The meta object id for the '{@link hub.top.editor.ptnetLoLA.impl.PlaceExtImpl <em>Place Ext</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.editor.ptnetLoLA.impl.PlaceExtImpl
	 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getPlaceExt()
	 * @generated
	 */
  int PLACE_EXT = 12;

  /**
	 * The feature id for the '<em><b>Name</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int PLACE_EXT__NAME = PLACE__NAME;

  /**
	 * The feature id for the '<em><b>Annotation</b></em>' containment reference.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int PLACE_EXT__ANNOTATION = PLACE__ANNOTATION;

  /**
	 * The feature id for the '<em><b>Incoming</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int PLACE_EXT__INCOMING = PLACE__INCOMING;

  /**
	 * The feature id for the '<em><b>Outgoing</b></em>' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int PLACE_EXT__OUTGOING = PLACE__OUTGOING;

  /**
	 * The feature id for the '<em><b>Type</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int PLACE_EXT__TYPE = PLACE__TYPE;

  /**
	 * The feature id for the '<em><b>Token</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int PLACE_EXT__TOKEN = PLACE__TOKEN;

  /**
	 * The feature id for the '<em><b>Final Marking</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int PLACE_EXT__FINAL_MARKING = PLACE__FINAL_MARKING;

  /**
	 * The feature id for the '<em><b>Probability</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int PLACE_EXT__PROBABILITY = PLACE_FEATURE_COUNT + 0;

  /**
	 * The feature id for the '<em><b>Is Start</b></em>' attribute.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
	int PLACE_EXT__IS_START = PLACE_FEATURE_COUNT + 1;

		/**
	 * The number of structural features of the '<em>Place Ext</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int PLACE_EXT_FEATURE_COUNT = PLACE_FEATURE_COUNT + 2;

  /**
	 * The meta object id for the '{@link hub.top.editor.ptnetLoLA.impl.ArcToPlaceExtImpl <em>Arc To Place Ext</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.editor.ptnetLoLA.impl.ArcToPlaceExtImpl
	 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getArcToPlaceExt()
	 * @generated
	 */
  int ARC_TO_PLACE_EXT = 13;

  /**
	 * The feature id for the '<em><b>Source</b></em>' reference.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ARC_TO_PLACE_EXT__SOURCE = ARC_TO_PLACE__SOURCE;

  /**
	 * The feature id for the '<em><b>Target</b></em>' reference.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ARC_TO_PLACE_EXT__TARGET = ARC_TO_PLACE__TARGET;

  /**
	 * The feature id for the '<em><b>Weight</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ARC_TO_PLACE_EXT__WEIGHT = ARC_TO_PLACE__WEIGHT;

  /**
	 * The feature id for the '<em><b>Probability</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ARC_TO_PLACE_EXT__PROBABILITY = ARC_TO_PLACE_FEATURE_COUNT + 0;

  /**
	 * The number of structural features of the '<em>Arc To Place Ext</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ARC_TO_PLACE_EXT_FEATURE_COUNT = ARC_TO_PLACE_FEATURE_COUNT + 1;

  /**
	 * The meta object id for the '{@link hub.top.editor.ptnetLoLA.impl.ArcToTransitionExtImpl <em>Arc To Transition Ext</em>}' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see hub.top.editor.ptnetLoLA.impl.ArcToTransitionExtImpl
	 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getArcToTransitionExt()
	 * @generated
	 */
  int ARC_TO_TRANSITION_EXT = 14;

  /**
	 * The feature id for the '<em><b>Source</b></em>' reference.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ARC_TO_TRANSITION_EXT__SOURCE = ARC_TO_TRANSITION__SOURCE;

  /**
	 * The feature id for the '<em><b>Target</b></em>' reference.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ARC_TO_TRANSITION_EXT__TARGET = ARC_TO_TRANSITION__TARGET;

  /**
	 * The feature id for the '<em><b>Weight</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ARC_TO_TRANSITION_EXT__WEIGHT = ARC_TO_TRANSITION__WEIGHT;

  /**
	 * The feature id for the '<em><b>Probability</b></em>' attribute.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ARC_TO_TRANSITION_EXT__PROBABILITY = ARC_TO_TRANSITION_FEATURE_COUNT + 0;

  /**
	 * The number of structural features of the '<em>Arc To Transition Ext</em>' class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 * @ordered
	 */
  int ARC_TO_TRANSITION_EXT_FEATURE_COUNT = ARC_TO_TRANSITION_FEATURE_COUNT + 1;

  /**
	 * The meta object id for the '{@link hub.top.editor.ptnetLoLA.NodeType <em>Node Type</em>}' enum.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see hub.top.editor.ptnetLoLA.NodeType
	 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getNodeType()
	 * @generated
	 */
	int NODE_TYPE = 15;


	/**
	 * Returns the meta object for class '{@link hub.top.editor.ptnetLoLA.Place <em>Place</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Place</em>'.
	 * @see hub.top.editor.ptnetLoLA.Place
	 * @generated
	 */
	EClass getPlace();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.editor.ptnetLoLA.Place#getToken <em>Token</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Token</em>'.
	 * @see hub.top.editor.ptnetLoLA.Place#getToken()
	 * @see #getPlace()
	 * @generated
	 */
	EAttribute getPlace_Token();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.editor.ptnetLoLA.Place#getFinalMarking <em>Final Marking</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Final Marking</em>'.
	 * @see hub.top.editor.ptnetLoLA.Place#getFinalMarking()
	 * @see #getPlace()
	 * @generated
	 */
	EAttribute getPlace_FinalMarking();

	/**
	 * Returns the meta object for class '{@link hub.top.editor.ptnetLoLA.PtNet <em>Pt Net</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Pt Net</em>'.
	 * @see hub.top.editor.ptnetLoLA.PtNet
	 * @generated
	 */
	EClass getPtNet();

	/**
	 * Returns the meta object for the containment reference list '{@link hub.top.editor.ptnetLoLA.PtNet#getPlaces <em>Places</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Places</em>'.
	 * @see hub.top.editor.ptnetLoLA.PtNet#getPlaces()
	 * @see #getPtNet()
	 * @generated
	 */
	EReference getPtNet_Places();

	/**
	 * Returns the meta object for the containment reference list '{@link hub.top.editor.ptnetLoLA.PtNet#getTransitions <em>Transitions</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Transitions</em>'.
	 * @see hub.top.editor.ptnetLoLA.PtNet#getTransitions()
	 * @see #getPtNet()
	 * @generated
	 */
	EReference getPtNet_Transitions();

	/**
	 * Returns the meta object for the containment reference '{@link hub.top.editor.ptnetLoLA.PtNet#getInitialMarking <em>Initial Marking</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Initial Marking</em>'.
	 * @see hub.top.editor.ptnetLoLA.PtNet#getInitialMarking()
	 * @see #getPtNet()
	 * @generated
	 */
	EReference getPtNet_InitialMarking();

	/**
	 * Returns the meta object for the containment reference '{@link hub.top.editor.ptnetLoLA.PtNet#getAnnotation <em>Annotation</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Annotation</em>'.
	 * @see hub.top.editor.ptnetLoLA.PtNet#getAnnotation()
	 * @see #getPtNet()
	 * @generated
	 */
	EReference getPtNet_Annotation();

	/**
	 * Returns the meta object for the containment reference list '{@link hub.top.editor.ptnetLoLA.PtNet#getArcs <em>Arcs</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Arcs</em>'.
	 * @see hub.top.editor.ptnetLoLA.PtNet#getArcs()
	 * @see #getPtNet()
	 * @generated
	 */
	EReference getPtNet_Arcs();

	/**
	 * Returns the meta object for the containment reference '{@link hub.top.editor.ptnetLoLA.PtNet#getFinalMarking <em>Final Marking</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Final Marking</em>'.
	 * @see hub.top.editor.ptnetLoLA.PtNet#getFinalMarking()
	 * @see #getPtNet()
	 * @generated
	 */
	EReference getPtNet_FinalMarking();

	/**
	 * Returns the meta object for class '{@link hub.top.editor.ptnetLoLA.Transition <em>Transition</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Transition</em>'.
	 * @see hub.top.editor.ptnetLoLA.Transition
	 * @generated
	 */
	EClass getTransition();

	/**
	 * Returns the meta object for class '{@link hub.top.editor.ptnetLoLA.Node <em>Node</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Node</em>'.
	 * @see hub.top.editor.ptnetLoLA.Node
	 * @generated
	 */
	EClass getNode();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.editor.ptnetLoLA.Node#getName <em>Name</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Name</em>'.
	 * @see hub.top.editor.ptnetLoLA.Node#getName()
	 * @see #getNode()
	 * @generated
	 */
	EAttribute getNode_Name();

	/**
	 * Returns the meta object for the containment reference '{@link hub.top.editor.ptnetLoLA.Node#getAnnotation <em>Annotation</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference '<em>Annotation</em>'.
	 * @see hub.top.editor.ptnetLoLA.Node#getAnnotation()
	 * @see #getNode()
	 * @generated
	 */
	EReference getNode_Annotation();

	/**
	 * Returns the meta object for the reference list '{@link hub.top.editor.ptnetLoLA.Node#getIncoming <em>Incoming</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Incoming</em>'.
	 * @see hub.top.editor.ptnetLoLA.Node#getIncoming()
	 * @see #getNode()
	 * @generated
	 */
	EReference getNode_Incoming();

	/**
	 * Returns the meta object for the reference list '{@link hub.top.editor.ptnetLoLA.Node#getOutgoing <em>Outgoing</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference list '<em>Outgoing</em>'.
	 * @see hub.top.editor.ptnetLoLA.Node#getOutgoing()
	 * @see #getNode()
	 * @generated
	 */
	EReference getNode_Outgoing();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.editor.ptnetLoLA.Node#getType <em>Type</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Type</em>'.
	 * @see hub.top.editor.ptnetLoLA.Node#getType()
	 * @see #getNode()
	 * @generated
	 */
	EAttribute getNode_Type();

	/**
	 * Returns the meta object for class '{@link hub.top.editor.ptnetLoLA.Marking <em>Marking</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Marking</em>'.
	 * @see hub.top.editor.ptnetLoLA.Marking
	 * @generated
	 */
	EClass getMarking();

	/**
	 * Returns the meta object for the containment reference list '{@link hub.top.editor.ptnetLoLA.Marking#getPlaces <em>Places</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the containment reference list '<em>Places</em>'.
	 * @see hub.top.editor.ptnetLoLA.Marking#getPlaces()
	 * @see #getMarking()
	 * @generated
	 */
	EReference getMarking_Places();

	/**
	 * Returns the meta object for class '{@link hub.top.editor.ptnetLoLA.PlaceReference <em>Place Reference</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Place Reference</em>'.
	 * @see hub.top.editor.ptnetLoLA.PlaceReference
	 * @generated
	 */
	EClass getPlaceReference();

	/**
	 * Returns the meta object for the reference '{@link hub.top.editor.ptnetLoLA.PlaceReference#getPlace <em>Place</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference '<em>Place</em>'.
	 * @see hub.top.editor.ptnetLoLA.PlaceReference#getPlace()
	 * @see #getPlaceReference()
	 * @generated
	 */
	EReference getPlaceReference_Place();

	/**
	 * Returns the meta object for class '{@link hub.top.editor.ptnetLoLA.RefMarkedPlace <em>Ref Marked Place</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Ref Marked Place</em>'.
	 * @see hub.top.editor.ptnetLoLA.RefMarkedPlace
	 * @generated
	 */
	EClass getRefMarkedPlace();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.editor.ptnetLoLA.RefMarkedPlace#getToken <em>Token</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Token</em>'.
	 * @see hub.top.editor.ptnetLoLA.RefMarkedPlace#getToken()
	 * @see #getRefMarkedPlace()
	 * @generated
	 */
	EAttribute getRefMarkedPlace_Token();

	/**
	 * Returns the meta object for class '{@link hub.top.editor.ptnetLoLA.Annotation <em>Annotation</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Annotation</em>'.
	 * @see hub.top.editor.ptnetLoLA.Annotation
	 * @generated
	 */
	EClass getAnnotation();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.editor.ptnetLoLA.Annotation#getText <em>Text</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Text</em>'.
	 * @see hub.top.editor.ptnetLoLA.Annotation#getText()
	 * @see #getAnnotation()
	 * @generated
	 */
	EAttribute getAnnotation_Text();

	/**
	 * Returns the meta object for class '{@link hub.top.editor.ptnetLoLA.Arc <em>Arc</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Arc</em>'.
	 * @see hub.top.editor.ptnetLoLA.Arc
	 * @generated
	 */
	EClass getArc();

	/**
	 * Returns the meta object for the reference '{@link hub.top.editor.ptnetLoLA.Arc#getSource <em>Source</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference '<em>Source</em>'.
	 * @see hub.top.editor.ptnetLoLA.Arc#getSource()
	 * @see #getArc()
	 * @generated
	 */
	EReference getArc_Source();

	/**
	 * Returns the meta object for the reference '{@link hub.top.editor.ptnetLoLA.Arc#getTarget <em>Target</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the reference '<em>Target</em>'.
	 * @see hub.top.editor.ptnetLoLA.Arc#getTarget()
	 * @see #getArc()
	 * @generated
	 */
	EReference getArc_Target();

	/**
	 * Returns the meta object for the attribute '{@link hub.top.editor.ptnetLoLA.Arc#getWeight <em>Weight</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Weight</em>'.
	 * @see hub.top.editor.ptnetLoLA.Arc#getWeight()
	 * @see #getArc()
	 * @generated
	 */
	EAttribute getArc_Weight();

	/**
	 * Returns the meta object for class '{@link hub.top.editor.ptnetLoLA.ArcToPlace <em>Arc To Place</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Arc To Place</em>'.
	 * @see hub.top.editor.ptnetLoLA.ArcToPlace
	 * @generated
	 */
	EClass getArcToPlace();

	/**
	 * Returns the meta object for class '{@link hub.top.editor.ptnetLoLA.ArcToTransition <em>Arc To Transition</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Arc To Transition</em>'.
	 * @see hub.top.editor.ptnetLoLA.ArcToTransition
	 * @generated
	 */
	EClass getArcToTransition();

	/**
	 * Returns the meta object for class '{@link hub.top.editor.ptnetLoLA.TransitionExt <em>Transition Ext</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Transition Ext</em>'.
	 * @see hub.top.editor.ptnetLoLA.TransitionExt
	 * @generated
	 */
  EClass getTransitionExt();

  /**
	 * Returns the meta object for the attribute '{@link hub.top.editor.ptnetLoLA.TransitionExt#getProbability <em>Probability</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Probability</em>'.
	 * @see hub.top.editor.ptnetLoLA.TransitionExt#getProbability()
	 * @see #getTransitionExt()
	 * @generated
	 */
  EAttribute getTransitionExt_Probability();

  /**
	 * Returns the meta object for the attribute '{@link hub.top.editor.ptnetLoLA.TransitionExt#getMinTime <em>Min Time</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Min Time</em>'.
	 * @see hub.top.editor.ptnetLoLA.TransitionExt#getMinTime()
	 * @see #getTransitionExt()
	 * @generated
	 */
	EAttribute getTransitionExt_MinTime();

		/**
	 * Returns the meta object for the attribute '{@link hub.top.editor.ptnetLoLA.TransitionExt#getCost <em>Cost</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Cost</em>'.
	 * @see hub.top.editor.ptnetLoLA.TransitionExt#getCost()
	 * @see #getTransitionExt()
	 * @generated
	 */
	EAttribute getTransitionExt_Cost();

		/**
	 * Returns the meta object for the attribute '{@link hub.top.editor.ptnetLoLA.TransitionExt#getMaxTime <em>Max Time</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Max Time</em>'.
	 * @see hub.top.editor.ptnetLoLA.TransitionExt#getMaxTime()
	 * @see #getTransitionExt()
	 * @generated
	 */
	EAttribute getTransitionExt_MaxTime();

		/**
	 * Returns the meta object for class '{@link hub.top.editor.ptnetLoLA.PlaceExt <em>Place Ext</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Place Ext</em>'.
	 * @see hub.top.editor.ptnetLoLA.PlaceExt
	 * @generated
	 */
  EClass getPlaceExt();

  /**
	 * Returns the meta object for the attribute '{@link hub.top.editor.ptnetLoLA.PlaceExt#getProbability <em>Probability</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Probability</em>'.
	 * @see hub.top.editor.ptnetLoLA.PlaceExt#getProbability()
	 * @see #getPlaceExt()
	 * @generated
	 */
  EAttribute getPlaceExt_Probability();

  /**
	 * Returns the meta object for the attribute '{@link hub.top.editor.ptnetLoLA.PlaceExt#isIsStart <em>Is Start</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Is Start</em>'.
	 * @see hub.top.editor.ptnetLoLA.PlaceExt#isIsStart()
	 * @see #getPlaceExt()
	 * @generated
	 */
	EAttribute getPlaceExt_IsStart();

		/**
	 * Returns the meta object for class '{@link hub.top.editor.ptnetLoLA.ArcToPlaceExt <em>Arc To Place Ext</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Arc To Place Ext</em>'.
	 * @see hub.top.editor.ptnetLoLA.ArcToPlaceExt
	 * @generated
	 */
  EClass getArcToPlaceExt();

  /**
	 * Returns the meta object for the attribute '{@link hub.top.editor.ptnetLoLA.ArcToPlaceExt#getProbability <em>Probability</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Probability</em>'.
	 * @see hub.top.editor.ptnetLoLA.ArcToPlaceExt#getProbability()
	 * @see #getArcToPlaceExt()
	 * @generated
	 */
  EAttribute getArcToPlaceExt_Probability();

  /**
	 * Returns the meta object for class '{@link hub.top.editor.ptnetLoLA.ArcToTransitionExt <em>Arc To Transition Ext</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for class '<em>Arc To Transition Ext</em>'.
	 * @see hub.top.editor.ptnetLoLA.ArcToTransitionExt
	 * @generated
	 */
  EClass getArcToTransitionExt();

  /**
	 * Returns the meta object for the attribute '{@link hub.top.editor.ptnetLoLA.ArcToTransitionExt#getProbability <em>Probability</em>}'.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @return the meta object for the attribute '<em>Probability</em>'.
	 * @see hub.top.editor.ptnetLoLA.ArcToTransitionExt#getProbability()
	 * @see #getArcToTransitionExt()
	 * @generated
	 */
  EAttribute getArcToTransitionExt_Probability();

  /**
	 * Returns the meta object for enum '{@link hub.top.editor.ptnetLoLA.NodeType <em>Node Type</em>}'.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the meta object for enum '<em>Node Type</em>'.
	 * @see hub.top.editor.ptnetLoLA.NodeType
	 * @generated
	 */
	EEnum getNodeType();

	/**
	 * Returns the factory that creates the instances of the model.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @return the factory that creates the instances of the model.
	 * @generated
	 */
	PtnetLoLAFactory getPtnetLoLAFactory();
	
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
		 * The meta object literal for the '{@link hub.top.editor.ptnetLoLA.impl.PlaceImpl <em>Place</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.editor.ptnetLoLA.impl.PlaceImpl
		 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getPlace()
		 * @generated
		 */
		EClass PLACE = eINSTANCE.getPlace();

		/**
		 * The meta object literal for the '<em><b>Token</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute PLACE__TOKEN = eINSTANCE.getPlace_Token();

		/**
		 * The meta object literal for the '<em><b>Final Marking</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute PLACE__FINAL_MARKING = eINSTANCE.getPlace_FinalMarking();

		/**
		 * The meta object literal for the '{@link hub.top.editor.ptnetLoLA.impl.PtNetImpl <em>Pt Net</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.editor.ptnetLoLA.impl.PtNetImpl
		 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getPtNet()
		 * @generated
		 */
		EClass PT_NET = eINSTANCE.getPtNet();

		/**
		 * The meta object literal for the '<em><b>Places</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference PT_NET__PLACES = eINSTANCE.getPtNet_Places();

		/**
		 * The meta object literal for the '<em><b>Transitions</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference PT_NET__TRANSITIONS = eINSTANCE.getPtNet_Transitions();

		/**
		 * The meta object literal for the '<em><b>Initial Marking</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference PT_NET__INITIAL_MARKING = eINSTANCE.getPtNet_InitialMarking();

		/**
		 * The meta object literal for the '<em><b>Annotation</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference PT_NET__ANNOTATION = eINSTANCE.getPtNet_Annotation();

		/**
		 * The meta object literal for the '<em><b>Arcs</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference PT_NET__ARCS = eINSTANCE.getPtNet_Arcs();

		/**
		 * The meta object literal for the '<em><b>Final Marking</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference PT_NET__FINAL_MARKING = eINSTANCE.getPtNet_FinalMarking();

		/**
		 * The meta object literal for the '{@link hub.top.editor.ptnetLoLA.impl.TransitionImpl <em>Transition</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.editor.ptnetLoLA.impl.TransitionImpl
		 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getTransition()
		 * @generated
		 */
		EClass TRANSITION = eINSTANCE.getTransition();

		/**
		 * The meta object literal for the '{@link hub.top.editor.ptnetLoLA.impl.NodeImpl <em>Node</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.editor.ptnetLoLA.impl.NodeImpl
		 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getNode()
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
		 * The meta object literal for the '<em><b>Annotation</b></em>' containment reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference NODE__ANNOTATION = eINSTANCE.getNode_Annotation();

		/**
		 * The meta object literal for the '<em><b>Incoming</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference NODE__INCOMING = eINSTANCE.getNode_Incoming();

		/**
		 * The meta object literal for the '<em><b>Outgoing</b></em>' reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference NODE__OUTGOING = eINSTANCE.getNode_Outgoing();

		/**
		 * The meta object literal for the '<em><b>Type</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute NODE__TYPE = eINSTANCE.getNode_Type();

		/**
		 * The meta object literal for the '{@link hub.top.editor.ptnetLoLA.impl.MarkingImpl <em>Marking</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.editor.ptnetLoLA.impl.MarkingImpl
		 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getMarking()
		 * @generated
		 */
		EClass MARKING = eINSTANCE.getMarking();

		/**
		 * The meta object literal for the '<em><b>Places</b></em>' containment reference list feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference MARKING__PLACES = eINSTANCE.getMarking_Places();

		/**
		 * The meta object literal for the '{@link hub.top.editor.ptnetLoLA.impl.PlaceReferenceImpl <em>Place Reference</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.editor.ptnetLoLA.impl.PlaceReferenceImpl
		 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getPlaceReference()
		 * @generated
		 */
		EClass PLACE_REFERENCE = eINSTANCE.getPlaceReference();

		/**
		 * The meta object literal for the '<em><b>Place</b></em>' reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference PLACE_REFERENCE__PLACE = eINSTANCE.getPlaceReference_Place();

		/**
		 * The meta object literal for the '{@link hub.top.editor.ptnetLoLA.impl.RefMarkedPlaceImpl <em>Ref Marked Place</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.editor.ptnetLoLA.impl.RefMarkedPlaceImpl
		 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getRefMarkedPlace()
		 * @generated
		 */
		EClass REF_MARKED_PLACE = eINSTANCE.getRefMarkedPlace();

		/**
		 * The meta object literal for the '<em><b>Token</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute REF_MARKED_PLACE__TOKEN = eINSTANCE.getRefMarkedPlace_Token();

		/**
		 * The meta object literal for the '{@link hub.top.editor.ptnetLoLA.impl.AnnotationImpl <em>Annotation</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.editor.ptnetLoLA.impl.AnnotationImpl
		 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getAnnotation()
		 * @generated
		 */
		EClass ANNOTATION = eINSTANCE.getAnnotation();

		/**
		 * The meta object literal for the '<em><b>Text</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute ANNOTATION__TEXT = eINSTANCE.getAnnotation_Text();

		/**
		 * The meta object literal for the '{@link hub.top.editor.ptnetLoLA.impl.ArcImpl <em>Arc</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.editor.ptnetLoLA.impl.ArcImpl
		 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getArc()
		 * @generated
		 */
		EClass ARC = eINSTANCE.getArc();

		/**
		 * The meta object literal for the '<em><b>Source</b></em>' reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference ARC__SOURCE = eINSTANCE.getArc_Source();

		/**
		 * The meta object literal for the '<em><b>Target</b></em>' reference feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EReference ARC__TARGET = eINSTANCE.getArc_Target();

		/**
		 * The meta object literal for the '<em><b>Weight</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute ARC__WEIGHT = eINSTANCE.getArc_Weight();

		/**
		 * The meta object literal for the '{@link hub.top.editor.ptnetLoLA.impl.ArcToPlaceImpl <em>Arc To Place</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.editor.ptnetLoLA.impl.ArcToPlaceImpl
		 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getArcToPlace()
		 * @generated
		 */
		EClass ARC_TO_PLACE = eINSTANCE.getArcToPlace();

		/**
		 * The meta object literal for the '{@link hub.top.editor.ptnetLoLA.impl.ArcToTransitionImpl <em>Arc To Transition</em>}' class.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.editor.ptnetLoLA.impl.ArcToTransitionImpl
		 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getArcToTransition()
		 * @generated
		 */
		EClass ARC_TO_TRANSITION = eINSTANCE.getArcToTransition();

		/**
		 * The meta object literal for the '{@link hub.top.editor.ptnetLoLA.impl.TransitionExtImpl <em>Transition Ext</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.editor.ptnetLoLA.impl.TransitionExtImpl
		 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getTransitionExt()
		 * @generated
		 */
    EClass TRANSITION_EXT = eINSTANCE.getTransitionExt();

    /**
		 * The meta object literal for the '<em><b>Probability</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EAttribute TRANSITION_EXT__PROBABILITY = eINSTANCE.getTransitionExt_Probability();

    /**
		 * The meta object literal for the '<em><b>Min Time</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute TRANSITION_EXT__MIN_TIME = eINSTANCE.getTransitionExt_MinTime();

				/**
		 * The meta object literal for the '<em><b>Cost</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute TRANSITION_EXT__COST = eINSTANCE.getTransitionExt_Cost();

				/**
		 * The meta object literal for the '<em><b>Max Time</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute TRANSITION_EXT__MAX_TIME = eINSTANCE.getTransitionExt_MaxTime();

				/**
		 * The meta object literal for the '{@link hub.top.editor.ptnetLoLA.impl.PlaceExtImpl <em>Place Ext</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.editor.ptnetLoLA.impl.PlaceExtImpl
		 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getPlaceExt()
		 * @generated
		 */
    EClass PLACE_EXT = eINSTANCE.getPlaceExt();

    /**
		 * The meta object literal for the '<em><b>Probability</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EAttribute PLACE_EXT__PROBABILITY = eINSTANCE.getPlaceExt_Probability();

    /**
		 * The meta object literal for the '<em><b>Is Start</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @generated
		 */
		EAttribute PLACE_EXT__IS_START = eINSTANCE.getPlaceExt_IsStart();

				/**
		 * The meta object literal for the '{@link hub.top.editor.ptnetLoLA.impl.ArcToPlaceExtImpl <em>Arc To Place Ext</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.editor.ptnetLoLA.impl.ArcToPlaceExtImpl
		 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getArcToPlaceExt()
		 * @generated
		 */
    EClass ARC_TO_PLACE_EXT = eINSTANCE.getArcToPlaceExt();

    /**
		 * The meta object literal for the '<em><b>Probability</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EAttribute ARC_TO_PLACE_EXT__PROBABILITY = eINSTANCE.getArcToPlaceExt_Probability();

    /**
		 * The meta object literal for the '{@link hub.top.editor.ptnetLoLA.impl.ArcToTransitionExtImpl <em>Arc To Transition Ext</em>}' class.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @see hub.top.editor.ptnetLoLA.impl.ArcToTransitionExtImpl
		 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getArcToTransitionExt()
		 * @generated
		 */
    EClass ARC_TO_TRANSITION_EXT = eINSTANCE.getArcToTransitionExt();

    /**
		 * The meta object literal for the '<em><b>Probability</b></em>' attribute feature.
		 * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
		 * @generated
		 */
    EAttribute ARC_TO_TRANSITION_EXT__PROBABILITY = eINSTANCE.getArcToTransitionExt_Probability();

    /**
		 * The meta object literal for the '{@link hub.top.editor.ptnetLoLA.NodeType <em>Node Type</em>}' enum.
		 * <!-- begin-user-doc -->
		 * <!-- end-user-doc -->
		 * @see hub.top.editor.ptnetLoLA.NodeType
		 * @see hub.top.editor.ptnetLoLA.impl.PtnetLoLAPackageImpl#getNodeType()
		 * @generated
		 */
		EEnum NODE_TYPE = eINSTANCE.getNodeType();

	}

} //PtnetLoLAPackage
