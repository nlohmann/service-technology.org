/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.lang.flowcharts.impl;

import hub.top.lang.flowcharts.DiagramArc;
import hub.top.lang.flowcharts.DiagramNode;
import hub.top.lang.flowcharts.Endnode;
import hub.top.lang.flowcharts.FlowPackage;
import hub.top.lang.flowcharts.ResourceNode;
import hub.top.lang.flowcharts.StartNode;
import hub.top.lang.flowcharts.Subprocess;
import hub.top.lang.flowcharts.FlowChartPackage;
import hub.top.lang.flowcharts.WorkflowDiagram;

import java.util.Collection;

import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;

import org.eclipse.emf.common.util.EList;

import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;

import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.EObjectResolvingEList;
import org.eclipse.emf.ecore.util.EObjectWithInverseResolvingEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Workflow Diagram</b></em>'.
 * <!-- end-user-doc -->
 * <p>
 * The following features are implemented:
 * <ul>
 *   <li>{@link hub.top.lang.flowcharts.impl.WorkflowDiagramImpl#getDiagramNodes <em>Diagram Nodes</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.WorkflowDiagramImpl#getStartNode <em>Start Node</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.WorkflowDiagramImpl#getEndNode <em>End Node</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.WorkflowDiagramImpl#getProcessOutput <em>Process Output</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.WorkflowDiagramImpl#getProcessOutputOpt <em>Process Output Opt</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.WorkflowDiagramImpl#getDiagramArcs <em>Diagram Arcs</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.WorkflowDiagramImpl#getName <em>Name</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.WorkflowDiagramImpl#getProcessInput <em>Process Input</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.WorkflowDiagramImpl#getProcessInputOpt <em>Process Input Opt</em>}</li>
 *   <li>{@link hub.top.lang.flowcharts.impl.WorkflowDiagramImpl#getProcessCalls <em>Process Calls</em>}</li>
 * </ul>
 * </p>
 *
 * @generated
 */
public class WorkflowDiagramImpl extends EObjectImpl implements WorkflowDiagram {
	/**
	 * The cached value of the '{@link #getDiagramNodes() <em>Diagram Nodes</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getDiagramNodes()
	 * @generated
	 * @ordered
	 */
	protected EList<DiagramNode> diagramNodes;

	/**
	 * The cached value of the '{@link #getProcessOutput() <em>Process Output</em>}' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getProcessOutput()
	 * @generated
	 * @ordered
	 */
	protected EList<ResourceNode> processOutput;

	/**
	 * The cached value of the '{@link #getProcessOutputOpt() <em>Process Output Opt</em>}' reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getProcessOutputOpt()
	 * @generated
	 * @ordered
	 */
	protected EList<ResourceNode> processOutputOpt;

	/**
	 * The cached value of the '{@link #getDiagramArcs() <em>Diagram Arcs</em>}' containment reference list.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @see #getDiagramArcs()
	 * @generated
	 * @ordered
	 */
	protected EList<DiagramArc> diagramArcs;

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
	 * The cached value of the '{@link #getProcessInput() <em>Process Input</em>}' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see #getProcessInput()
	 * @generated
	 * @ordered
	 */
  protected EList<ResourceNode> processInput;

  /**
	 * The cached value of the '{@link #getProcessInputOpt() <em>Process Input Opt</em>}' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see #getProcessInputOpt()
	 * @generated
	 * @ordered
	 */
  protected EList<ResourceNode> processInputOpt;

  /**
	 * The cached value of the '{@link #getProcessCalls() <em>Process Calls</em>}' reference list.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @see #getProcessCalls()
	 * @generated
	 * @ordered
	 */
  protected EList<Subprocess> processCalls;

  /**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	protected WorkflowDiagramImpl() {
		super();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	protected EClass eStaticClass() {
		return FlowPackage.Literals.WORKFLOW_DIAGRAM;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<DiagramNode> getDiagramNodes() {
		if (diagramNodes == null) {
			diagramNodes = new EObjectContainmentEList<DiagramNode>(DiagramNode.class, this, FlowPackage.WORKFLOW_DIAGRAM__DIAGRAM_NODES);
		}
		return diagramNodes;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public StartNode getStartNode() {
		StartNode startNode = basicGetStartNode();
		return startNode != null && startNode.eIsProxy() ? (StartNode)eResolveProxy((InternalEObject)startNode) : startNode;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public StartNode basicGetStartNode() {
		// TODO: implement this method to return the 'Start Node' reference
		// -> do not perform proxy resolution
		// Ensure that you remove @generated or mark it @generated NOT
		throw new UnsupportedOperationException();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setStartNode(StartNode newStartNode) {
		// TODO: implement this method to set the 'Start Node' reference
		// Ensure that you remove @generated or mark it @generated NOT
		throw new UnsupportedOperationException();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Endnode getEndNode() {
		Endnode endNode = basicGetEndNode();
		return endNode != null && endNode.eIsProxy() ? (Endnode)eResolveProxy((InternalEObject)endNode) : endNode;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public Endnode basicGetEndNode() {
		// TODO: implement this method to return the 'End Node' reference
		// -> do not perform proxy resolution
		// Ensure that you remove @generated or mark it @generated NOT
		throw new UnsupportedOperationException();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setEndNode(Endnode newEndNode) {
		// TODO: implement this method to set the 'End Node' reference
		// Ensure that you remove @generated or mark it @generated NOT
		throw new UnsupportedOperationException();
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<ResourceNode> getProcessOutput() {
		if (processOutput == null) {
			processOutput = new EObjectResolvingEList<ResourceNode>(ResourceNode.class, this, FlowPackage.WORKFLOW_DIAGRAM__PROCESS_OUTPUT);
		}
		return processOutput;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<ResourceNode> getProcessOutputOpt() {
		if (processOutputOpt == null) {
			processOutputOpt = new EObjectResolvingEList<ResourceNode>(ResourceNode.class, this, FlowPackage.WORKFLOW_DIAGRAM__PROCESS_OUTPUT_OPT);
		}
		return processOutputOpt;
	}

	/**
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public EList<DiagramArc> getDiagramArcs() {
		if (diagramArcs == null) {
			diagramArcs = new EObjectContainmentEList<DiagramArc>(DiagramArc.class, this, FlowPackage.WORKFLOW_DIAGRAM__DIAGRAM_ARCS);
		}
		return diagramArcs;
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
	 * <!-- end-user-doc -->
	 * @generated
	 */
	public void setName(String newName) {
		String oldName = name;
		name = newName;
		if (eNotificationRequired())
			eNotify(new ENotificationImpl(this, Notification.SET, FlowPackage.WORKFLOW_DIAGRAM__NAME, oldName, name));
	}

	/**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public EList<ResourceNode> getProcessInput() {
		if (processInput == null) {
			processInput = new EObjectResolvingEList<ResourceNode>(ResourceNode.class, this, FlowPackage.WORKFLOW_DIAGRAM__PROCESS_INPUT);
		}
		return processInput;
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public EList<ResourceNode> getProcessInputOpt() {
		if (processInputOpt == null) {
			processInputOpt = new EObjectResolvingEList<ResourceNode>(ResourceNode.class, this, FlowPackage.WORKFLOW_DIAGRAM__PROCESS_INPUT_OPT);
		}
		return processInputOpt;
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public EList<Subprocess> getProcessCalls() {
		if (processCalls == null) {
			processCalls = new EObjectWithInverseResolvingEList<Subprocess>(Subprocess.class, this, FlowPackage.WORKFLOW_DIAGRAM__PROCESS_CALLS, FlowPackage.SUBPROCESS__SUBPROCESS);
		}
		return processCalls;
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  @SuppressWarnings("unchecked")
  @Override
  public NotificationChain eInverseAdd(InternalEObject otherEnd, int featureID, NotificationChain msgs) {
		switch (featureID) {
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_CALLS:
				return ((InternalEList<InternalEObject>)(InternalEList<?>)getProcessCalls()).basicAdd(otherEnd, msgs);
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
			case FlowPackage.WORKFLOW_DIAGRAM__DIAGRAM_NODES:
				return ((InternalEList<?>)getDiagramNodes()).basicRemove(otherEnd, msgs);
			case FlowPackage.WORKFLOW_DIAGRAM__DIAGRAM_ARCS:
				return ((InternalEList<?>)getDiagramArcs()).basicRemove(otherEnd, msgs);
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_CALLS:
				return ((InternalEList<?>)getProcessCalls()).basicRemove(otherEnd, msgs);
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
			case FlowPackage.WORKFLOW_DIAGRAM__DIAGRAM_NODES:
				return getDiagramNodes();
			case FlowPackage.WORKFLOW_DIAGRAM__START_NODE:
				if (resolve) return getStartNode();
				return basicGetStartNode();
			case FlowPackage.WORKFLOW_DIAGRAM__END_NODE:
				if (resolve) return getEndNode();
				return basicGetEndNode();
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_OUTPUT:
				return getProcessOutput();
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_OUTPUT_OPT:
				return getProcessOutputOpt();
			case FlowPackage.WORKFLOW_DIAGRAM__DIAGRAM_ARCS:
				return getDiagramArcs();
			case FlowPackage.WORKFLOW_DIAGRAM__NAME:
				return getName();
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_INPUT:
				return getProcessInput();
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_INPUT_OPT:
				return getProcessInputOpt();
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_CALLS:
				return getProcessCalls();
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
			case FlowPackage.WORKFLOW_DIAGRAM__DIAGRAM_NODES:
				getDiagramNodes().clear();
				getDiagramNodes().addAll((Collection<? extends DiagramNode>)newValue);
				return;
			case FlowPackage.WORKFLOW_DIAGRAM__START_NODE:
				setStartNode((StartNode)newValue);
				return;
			case FlowPackage.WORKFLOW_DIAGRAM__END_NODE:
				setEndNode((Endnode)newValue);
				return;
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_OUTPUT:
				getProcessOutput().clear();
				getProcessOutput().addAll((Collection<? extends ResourceNode>)newValue);
				return;
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_OUTPUT_OPT:
				getProcessOutputOpt().clear();
				getProcessOutputOpt().addAll((Collection<? extends ResourceNode>)newValue);
				return;
			case FlowPackage.WORKFLOW_DIAGRAM__DIAGRAM_ARCS:
				getDiagramArcs().clear();
				getDiagramArcs().addAll((Collection<? extends DiagramArc>)newValue);
				return;
			case FlowPackage.WORKFLOW_DIAGRAM__NAME:
				setName((String)newValue);
				return;
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_INPUT:
				getProcessInput().clear();
				getProcessInput().addAll((Collection<? extends ResourceNode>)newValue);
				return;
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_INPUT_OPT:
				getProcessInputOpt().clear();
				getProcessInputOpt().addAll((Collection<? extends ResourceNode>)newValue);
				return;
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_CALLS:
				getProcessCalls().clear();
				getProcessCalls().addAll((Collection<? extends Subprocess>)newValue);
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
			case FlowPackage.WORKFLOW_DIAGRAM__DIAGRAM_NODES:
				getDiagramNodes().clear();
				return;
			case FlowPackage.WORKFLOW_DIAGRAM__START_NODE:
				setStartNode((StartNode)null);
				return;
			case FlowPackage.WORKFLOW_DIAGRAM__END_NODE:
				setEndNode((Endnode)null);
				return;
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_OUTPUT:
				getProcessOutput().clear();
				return;
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_OUTPUT_OPT:
				getProcessOutputOpt().clear();
				return;
			case FlowPackage.WORKFLOW_DIAGRAM__DIAGRAM_ARCS:
				getDiagramArcs().clear();
				return;
			case FlowPackage.WORKFLOW_DIAGRAM__NAME:
				setName(NAME_EDEFAULT);
				return;
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_INPUT:
				getProcessInput().clear();
				return;
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_INPUT_OPT:
				getProcessInputOpt().clear();
				return;
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_CALLS:
				getProcessCalls().clear();
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
			case FlowPackage.WORKFLOW_DIAGRAM__DIAGRAM_NODES:
				return diagramNodes != null && !diagramNodes.isEmpty();
			case FlowPackage.WORKFLOW_DIAGRAM__START_NODE:
				return basicGetStartNode() != null;
			case FlowPackage.WORKFLOW_DIAGRAM__END_NODE:
				return basicGetEndNode() != null;
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_OUTPUT:
				return processOutput != null && !processOutput.isEmpty();
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_OUTPUT_OPT:
				return processOutputOpt != null && !processOutputOpt.isEmpty();
			case FlowPackage.WORKFLOW_DIAGRAM__DIAGRAM_ARCS:
				return diagramArcs != null && !diagramArcs.isEmpty();
			case FlowPackage.WORKFLOW_DIAGRAM__NAME:
				return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT.equals(name);
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_INPUT:
				return processInput != null && !processInput.isEmpty();
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_INPUT_OPT:
				return processInputOpt != null && !processInputOpt.isEmpty();
			case FlowPackage.WORKFLOW_DIAGRAM__PROCESS_CALLS:
				return processCalls != null && !processCalls.isEmpty();
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
		result.append(')');
		return result.toString();
	}

} //WorkflowDiagramImpl
