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
package hub.top.lang.flowcharts.util;

import hub.top.lang.flowcharts.*;

import org.eclipse.emf.common.notify.Adapter;
import org.eclipse.emf.common.notify.Notifier;

import org.eclipse.emf.common.notify.impl.AdapterFactoryImpl;

import org.eclipse.emf.ecore.EObject;

/**
 * <!-- begin-user-doc -->
 * The <b>Adapter Factory</b> for the model.
 * It provides an adapter <code>createXXX</code> method for each class of the model.
 * <!-- end-user-doc -->
 * @see hub.top.lang.flowcharts.FlowPackage
 * @generated
 */
public class FlowAdapterFactory extends AdapterFactoryImpl {
  /**
	 * The cached model package.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  protected static FlowPackage modelPackage;

  /**
	 * Creates an instance of the adapter factory.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public FlowAdapterFactory() {
		if (modelPackage == null) {
			modelPackage = FlowPackage.eINSTANCE;
		}
	}

  /**
	 * Returns whether this factory is applicable for the type of the object.
	 * <!-- begin-user-doc -->
   * This implementation returns <code>true</code> if the object is either the model's package or is an instance object of the model.
   * <!-- end-user-doc -->
	 * @return whether this factory is applicable for the type of the object.
	 * @generated
	 */
  @Override
  public boolean isFactoryForType(Object object) {
		if (object == modelPackage) {
			return true;
		}
		if (object instanceof EObject) {
			return ((EObject)object).eClass().getEPackage() == modelPackage;
		}
		return false;
	}

  /**
	 * The switch that delegates to the <code>createXXX</code> methods.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  protected FlowSwitch<Adapter> modelSwitch =
    new FlowSwitch<Adapter>() {
			@Override
			public Adapter caseWorkflowDiagram(WorkflowDiagram object) {
				return createWorkflowDiagramAdapter();
			}
			@Override
			public Adapter caseDiagramNode(DiagramNode object) {
				return createDiagramNodeAdapter();
			}
			@Override
			public Adapter caseControlFlowNode(ControlFlowNode object) {
				return createControlFlowNodeAdapter();
			}
			@Override
			public Adapter caseActivityNode(ActivityNode object) {
				return createActivityNodeAdapter();
			}
			@Override
			public Adapter caseResourceNode(ResourceNode object) {
				return createResourceNodeAdapter();
			}
			@Override
			public Adapter caseStartNode(StartNode object) {
				return createStartNodeAdapter();
			}
			@Override
			public Adapter caseEndnode(Endnode object) {
				return createEndnodeAdapter();
			}
			@Override
			public Adapter caseSimpleNode(SimpleNode object) {
				return createSimpleNodeAdapter();
			}
			@Override
			public Adapter caseStructuredNode(StructuredNode object) {
				return createStructuredNodeAdapter();
			}
			@Override
			public Adapter caseSplitNode(SplitNode object) {
				return createSplitNodeAdapter();
			}
			@Override
			public Adapter caseMergeNode(MergeNode object) {
				return createMergeNodeAdapter();
			}
			@Override
			public Adapter caseFlow(Flow object) {
				return createFlowAdapter();
			}
			@Override
			public Adapter caseSimpleActivity(SimpleActivity object) {
				return createSimpleActivityAdapter();
			}
			@Override
			public Adapter caseSubprocess(Subprocess object) {
				return createSubprocessAdapter();
			}
			@Override
			public Adapter caseWorkflow(Workflow object) {
				return createWorkflowAdapter();
			}
			@Override
			public Adapter casePersistentResource(PersistentResource object) {
				return createPersistentResourceAdapter();
			}
			@Override
			public Adapter caseTransientResource(TransientResource object) {
				return createTransientResourceAdapter();
			}
			@Override
			public Adapter caseDocument(Document object) {
				return createDocumentAdapter();
			}
			@Override
			public Adapter caseEvent(Event object) {
				return createEventAdapter();
			}
			@Override
			public Adapter caseDiagramArc(DiagramArc object) {
				return createDiagramArcAdapter();
			}
			@Override
			public Adapter defaultCase(EObject object) {
				return createEObjectAdapter();
			}
		};

  /**
	 * Creates an adapter for the <code>target</code>.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @param target the object to adapt.
	 * @return the adapter for the <code>target</code>.
	 * @generated
	 */
  @Override
  public Adapter createAdapter(Notifier target) {
		return modelSwitch.doSwitch((EObject)target);
	}


  /**
	 * Creates a new adapter for an object of class '{@link hub.top.lang.flowcharts.WorkflowDiagram <em>Workflow Diagram</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.lang.flowcharts.WorkflowDiagram
	 * @generated
	 */
  public Adapter createWorkflowDiagramAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.lang.flowcharts.DiagramNode <em>Diagram Node</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.lang.flowcharts.DiagramNode
	 * @generated
	 */
  public Adapter createDiagramNodeAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.lang.flowcharts.ControlFlowNode <em>Control Flow Node</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.lang.flowcharts.ControlFlowNode
	 * @generated
	 */
  public Adapter createControlFlowNodeAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.lang.flowcharts.ActivityNode <em>Activity Node</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.lang.flowcharts.ActivityNode
	 * @generated
	 */
  public Adapter createActivityNodeAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.lang.flowcharts.ResourceNode <em>Resource Node</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.lang.flowcharts.ResourceNode
	 * @generated
	 */
  public Adapter createResourceNodeAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.lang.flowcharts.StartNode <em>Start Node</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.lang.flowcharts.StartNode
	 * @generated
	 */
  public Adapter createStartNodeAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.lang.flowcharts.Endnode <em>Endnode</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.lang.flowcharts.Endnode
	 * @generated
	 */
  public Adapter createEndnodeAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.lang.flowcharts.SimpleNode <em>Simple Node</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.lang.flowcharts.SimpleNode
	 * @generated
	 */
  public Adapter createSimpleNodeAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.lang.flowcharts.StructuredNode <em>Structured Node</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.lang.flowcharts.StructuredNode
	 * @generated
	 */
  public Adapter createStructuredNodeAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.lang.flowcharts.SplitNode <em>Split Node</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.lang.flowcharts.SplitNode
	 * @generated
	 */
  public Adapter createSplitNodeAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.lang.flowcharts.MergeNode <em>Merge Node</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.lang.flowcharts.MergeNode
	 * @generated
	 */
  public Adapter createMergeNodeAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.lang.flowcharts.Flow <em>Flow</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.lang.flowcharts.Flow
	 * @generated
	 */
  public Adapter createFlowAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.lang.flowcharts.SimpleActivity <em>Simple Activity</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.lang.flowcharts.SimpleActivity
	 * @generated
	 */
  public Adapter createSimpleActivityAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.lang.flowcharts.Subprocess <em>Subprocess</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.lang.flowcharts.Subprocess
	 * @generated
	 */
  public Adapter createSubprocessAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.lang.flowcharts.Workflow <em>Workflow</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.lang.flowcharts.Workflow
	 * @generated
	 */
  public Adapter createWorkflowAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.lang.flowcharts.PersistentResource <em>Persistent Resource</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.lang.flowcharts.PersistentResource
	 * @generated
	 */
  public Adapter createPersistentResourceAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.lang.flowcharts.TransientResource <em>Transient Resource</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.lang.flowcharts.TransientResource
	 * @generated
	 */
  public Adapter createTransientResourceAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.lang.flowcharts.Document <em>Document</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.lang.flowcharts.Document
	 * @generated
	 */
  public Adapter createDocumentAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.lang.flowcharts.Event <em>Event</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.lang.flowcharts.Event
	 * @generated
	 */
  public Adapter createEventAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for an object of class '{@link hub.top.lang.flowcharts.DiagramArc <em>Diagram Arc</em>}'.
	 * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases;
   * it's useful to ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @see hub.top.lang.flowcharts.DiagramArc
	 * @generated
	 */
  public Adapter createDiagramArcAdapter() {
		return null;
	}

  /**
	 * Creates a new adapter for the default case.
	 * <!-- begin-user-doc -->
   * This default implementation returns null.
   * <!-- end-user-doc -->
	 * @return the new adapter.
	 * @generated
	 */
  public Adapter createEObjectAdapter() {
		return null;
	}

} //FlowAdapterFactory
