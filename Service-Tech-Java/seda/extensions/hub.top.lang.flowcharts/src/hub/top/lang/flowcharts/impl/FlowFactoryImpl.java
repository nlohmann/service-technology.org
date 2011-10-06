/**
 * Copyright (c) 2008 Dirk Fahland. All rights reserved. EPL1.0/GPL3.0/LGPL3.0
 * 
 * ServiceTechnolog.org - FlowChart Editors
 * 
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License v1.0, which accompanies this
 * distribution, and is available at http://www.eclipse.org/legal/epl-v10.html
 * 
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 * 
 * The Original Code is this file as it was released on July 30, 2008.
 * The Initial Developer of the Original Code is
 *  		Dirk Fahland
 *  
 * Portions created by the Initial Developer are Copyright (c) 2008
 * the Initial Developer. All Rights Reserved.
 * 
 * Contributor(s):
 * 
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 3 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 3 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the EPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the EPL, the GPL or the LGPL.
 *
 * $Id$
 */
package hub.top.lang.flowcharts.impl;

import hub.top.lang.flowcharts.*;

import org.eclipse.emf.ecore.EClass;
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
public class FlowFactoryImpl extends EFactoryImpl implements FlowFactory {
  /**
	 * Creates the default factory implementation.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public static FlowFactory init() {
		try {
			FlowFactory theFlowFactory = (FlowFactory)EPackage.Registry.INSTANCE.getEFactory("http://hu-berlin.de/flowchart"); 
			if (theFlowFactory != null) {
				return theFlowFactory;
			}
		}
		catch (Exception exception) {
			EcorePlugin.INSTANCE.log(exception);
		}
		return new FlowFactoryImpl();
	}

  /**
	 * Creates an instance of the factory.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public FlowFactoryImpl() {
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
			case FlowPackage.WORKFLOW_DIAGRAM: return createWorkflowDiagram();
			case FlowPackage.START_NODE: return createStartNode();
			case FlowPackage.ENDNODE: return createEndnode();
			case FlowPackage.SPLIT_NODE: return createSplitNode();
			case FlowPackage.MERGE_NODE: return createMergeNode();
			case FlowPackage.FLOW: return createFlow();
			case FlowPackage.SIMPLE_ACTIVITY: return createSimpleActivity();
			case FlowPackage.SUBPROCESS: return createSubprocess();
			case FlowPackage.WORKFLOW: return createWorkflow();
			case FlowPackage.PERSISTENT_RESOURCE: return createPersistentResource();
			case FlowPackage.TRANSIENT_RESOURCE: return createTransientResource();
			case FlowPackage.DOCUMENT: return createDocument();
			case FlowPackage.EVENT: return createEvent();
			case FlowPackage.DIAGRAM_ARC: return createDiagramArc();
			default:
				throw new IllegalArgumentException("The class '" + eClass.getName() + "' is not a valid classifier");
		}
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public WorkflowDiagram createWorkflowDiagram() {
		WorkflowDiagramImpl workflowDiagram = new WorkflowDiagramImpl();
		return workflowDiagram;
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public StartNode createStartNode() {
		StartNodeImpl startNode = new StartNodeImpl();
		return startNode;
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public Endnode createEndnode() {
		EndnodeImpl endnode = new EndnodeImpl();
		return endnode;
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public SplitNode createSplitNode() {
		SplitNodeImpl splitNode = new SplitNodeImpl();
		return splitNode;
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public MergeNode createMergeNode() {
		MergeNodeImpl mergeNode = new MergeNodeImpl();
		return mergeNode;
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public Flow createFlow() {
		FlowImpl flow = new FlowImpl();
		return flow;
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public SimpleActivity createSimpleActivity() {
		SimpleActivityImpl simpleActivity = new SimpleActivityImpl();
		return simpleActivity;
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public Subprocess createSubprocess() {
		SubprocessImpl subprocess = new SubprocessImpl();
		return subprocess;
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public Workflow createWorkflow() {
		WorkflowImpl workflow = new WorkflowImpl();
		return workflow;
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public PersistentResource createPersistentResource() {
		PersistentResourceImpl persistentResource = new PersistentResourceImpl();
		return persistentResource;
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public TransientResource createTransientResource() {
		TransientResourceImpl transientResource = new TransientResourceImpl();
		return transientResource;
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public Document createDocument() {
		DocumentImpl document = new DocumentImpl();
		return document;
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public Event createEvent() {
		EventImpl event = new EventImpl();
		return event;
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public DiagramArc createDiagramArc() {
		DiagramArcImpl diagramArc = new DiagramArcImpl();
		return diagramArc;
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public FlowPackage getFlowPackage() {
		return (FlowPackage)getEPackage();
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @deprecated
	 * @generated
	 */
  @Deprecated
  public static FlowPackage getPackage() {
		return FlowPackage.eINSTANCE;
	}

} //FlowFactoryImpl
