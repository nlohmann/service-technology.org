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
package hub.top.lang.flowcharts.provider;


import hub.top.lang.flowcharts.FlowFactory;
import hub.top.lang.flowcharts.FlowPackage;
import hub.top.lang.flowcharts.WorkflowDiagram;

import java.util.Collection;
import java.util.List;

import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.common.util.ResourceLocator;

import org.eclipse.emf.ecore.EStructuralFeature;

import org.eclipse.emf.edit.provider.ComposeableAdapterFactory;
import org.eclipse.emf.edit.provider.IEditingDomainItemProvider;
import org.eclipse.emf.edit.provider.IItemLabelProvider;
import org.eclipse.emf.edit.provider.IItemPropertyDescriptor;
import org.eclipse.emf.edit.provider.IItemPropertySource;
import org.eclipse.emf.edit.provider.IStructuredItemContentProvider;
import org.eclipse.emf.edit.provider.ITreeItemContentProvider;
import org.eclipse.emf.edit.provider.ItemPropertyDescriptor;
import org.eclipse.emf.edit.provider.ItemProviderAdapter;
import org.eclipse.emf.edit.provider.ViewerNotification;

/**
 * This is the item provider adapter for a {@link hub.top.lang.flowcharts.WorkflowDiagram} object.
 * <!-- begin-user-doc -->
 * <!-- end-user-doc -->
 * @generated
 */
public class WorkflowDiagramItemProvider
  extends ItemProviderAdapter
  implements
    IEditingDomainItemProvider,
    IStructuredItemContentProvider,
    ITreeItemContentProvider,
    IItemLabelProvider,
    IItemPropertySource {
  /**
	 * This constructs an instance from a factory and a notifier.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  public WorkflowDiagramItemProvider(AdapterFactory adapterFactory) {
		super(adapterFactory);
	}

  /**
	 * This returns the property descriptors for the adapted class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  @Override
  public List<IItemPropertyDescriptor> getPropertyDescriptors(Object object) {
		if (itemPropertyDescriptors == null) {
			super.getPropertyDescriptors(object);

			addStartNodePropertyDescriptor(object);
			addEndNodePropertyDescriptor(object);
			addProcessOutputPropertyDescriptor(object);
			addProcessOutputOptPropertyDescriptor(object);
			addNamePropertyDescriptor(object);
			addProcessInputPropertyDescriptor(object);
			addProcessInputOptPropertyDescriptor(object);
			addProcessCallsPropertyDescriptor(object);
		}
		return itemPropertyDescriptors;
	}

  /**
	 * This adds a property descriptor for the Start Node feature.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  protected void addStartNodePropertyDescriptor(Object object) {
		itemPropertyDescriptors.add
			(createItemPropertyDescriptor
				(((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
				 getResourceLocator(),
				 getString("_UI_WorkflowDiagram_startNode_feature"),
				 getString("_UI_PropertyDescriptor_description", "_UI_WorkflowDiagram_startNode_feature", "_UI_WorkflowDiagram_type"),
				 FlowPackage.Literals.WORKFLOW_DIAGRAM__START_NODE,
				 true,
				 false,
				 true,
				 null,
				 null,
				 null));
	}

  /**
	 * This adds a property descriptor for the End Node feature.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  protected void addEndNodePropertyDescriptor(Object object) {
		itemPropertyDescriptors.add
			(createItemPropertyDescriptor
				(((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
				 getResourceLocator(),
				 getString("_UI_WorkflowDiagram_endNode_feature"),
				 getString("_UI_PropertyDescriptor_description", "_UI_WorkflowDiagram_endNode_feature", "_UI_WorkflowDiagram_type"),
				 FlowPackage.Literals.WORKFLOW_DIAGRAM__END_NODE,
				 true,
				 false,
				 true,
				 null,
				 null,
				 null));
	}

  /**
	 * This adds a property descriptor for the Process Output feature.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  protected void addProcessOutputPropertyDescriptor(Object object) {
		itemPropertyDescriptors.add
			(createItemPropertyDescriptor
				(((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
				 getResourceLocator(),
				 getString("_UI_WorkflowDiagram_processOutput_feature"),
				 getString("_UI_PropertyDescriptor_description", "_UI_WorkflowDiagram_processOutput_feature", "_UI_WorkflowDiagram_type"),
				 FlowPackage.Literals.WORKFLOW_DIAGRAM__PROCESS_OUTPUT,
				 true,
				 false,
				 true,
				 null,
				 null,
				 null));
	}

  /**
	 * This adds a property descriptor for the Process Output Opt feature.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  protected void addProcessOutputOptPropertyDescriptor(Object object) {
		itemPropertyDescriptors.add
			(createItemPropertyDescriptor
				(((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
				 getResourceLocator(),
				 getString("_UI_WorkflowDiagram_processOutputOpt_feature"),
				 getString("_UI_PropertyDescriptor_description", "_UI_WorkflowDiagram_processOutputOpt_feature", "_UI_WorkflowDiagram_type"),
				 FlowPackage.Literals.WORKFLOW_DIAGRAM__PROCESS_OUTPUT_OPT,
				 true,
				 false,
				 true,
				 null,
				 null,
				 null));
	}

  /**
	 * This adds a property descriptor for the Name feature.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  protected void addNamePropertyDescriptor(Object object) {
		itemPropertyDescriptors.add
			(createItemPropertyDescriptor
				(((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
				 getResourceLocator(),
				 getString("_UI_WorkflowDiagram_name_feature"),
				 getString("_UI_PropertyDescriptor_description", "_UI_WorkflowDiagram_name_feature", "_UI_WorkflowDiagram_type"),
				 FlowPackage.Literals.WORKFLOW_DIAGRAM__NAME,
				 true,
				 false,
				 false,
				 ItemPropertyDescriptor.GENERIC_VALUE_IMAGE,
				 null,
				 null));
	}

  /**
	 * This adds a property descriptor for the Process Input feature.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  protected void addProcessInputPropertyDescriptor(Object object) {
		itemPropertyDescriptors.add
			(createItemPropertyDescriptor
				(((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
				 getResourceLocator(),
				 getString("_UI_WorkflowDiagram_processInput_feature"),
				 getString("_UI_PropertyDescriptor_description", "_UI_WorkflowDiagram_processInput_feature", "_UI_WorkflowDiagram_type"),
				 FlowPackage.Literals.WORKFLOW_DIAGRAM__PROCESS_INPUT,
				 true,
				 false,
				 true,
				 null,
				 null,
				 null));
	}

  /**
	 * This adds a property descriptor for the Process Input Opt feature.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  protected void addProcessInputOptPropertyDescriptor(Object object) {
		itemPropertyDescriptors.add
			(createItemPropertyDescriptor
				(((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
				 getResourceLocator(),
				 getString("_UI_WorkflowDiagram_processInputOpt_feature"),
				 getString("_UI_PropertyDescriptor_description", "_UI_WorkflowDiagram_processInputOpt_feature", "_UI_WorkflowDiagram_type"),
				 FlowPackage.Literals.WORKFLOW_DIAGRAM__PROCESS_INPUT_OPT,
				 true,
				 false,
				 true,
				 null,
				 null,
				 null));
	}

  /**
	 * This adds a property descriptor for the Process Calls feature.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  protected void addProcessCallsPropertyDescriptor(Object object) {
		itemPropertyDescriptors.add
			(createItemPropertyDescriptor
				(((ComposeableAdapterFactory)adapterFactory).getRootAdapterFactory(),
				 getResourceLocator(),
				 getString("_UI_WorkflowDiagram_processCalls_feature"),
				 getString("_UI_PropertyDescriptor_description", "_UI_WorkflowDiagram_processCalls_feature", "_UI_WorkflowDiagram_type"),
				 FlowPackage.Literals.WORKFLOW_DIAGRAM__PROCESS_CALLS,
				 true,
				 false,
				 true,
				 null,
				 null,
				 null));
	}

  /**
	 * This specifies how to implement {@link #getChildren} and is used to deduce an appropriate feature for an
	 * {@link org.eclipse.emf.edit.command.AddCommand}, {@link org.eclipse.emf.edit.command.RemoveCommand} or
	 * {@link org.eclipse.emf.edit.command.MoveCommand} in {@link #createCommand}.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  @Override
  public Collection<? extends EStructuralFeature> getChildrenFeatures(Object object) {
		if (childrenFeatures == null) {
			super.getChildrenFeatures(object);
			childrenFeatures.add(FlowPackage.Literals.WORKFLOW_DIAGRAM__DIAGRAM_NODES);
			childrenFeatures.add(FlowPackage.Literals.WORKFLOW_DIAGRAM__DIAGRAM_ARCS);
		}
		return childrenFeatures;
	}

  /**
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  @Override
  protected EStructuralFeature getChildFeature(Object object, Object child) {
		// Check the type of the specified child object and return the proper feature to use for
		// adding (see {@link AddCommand}) it as a child.

		return super.getChildFeature(object, child);
	}

  /**
	 * This returns WorkflowDiagram.gif.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  @Override
  public Object getImage(Object object) {
		return overlayImage(object, getResourceLocator().getImage("full/obj16/WorkflowDiagram"));
	}

  /**
	 * This returns the label text for the adapted class.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  @Override
  public String getText(Object object) {
		String label = ((WorkflowDiagram)object).getName();
		return label == null || label.length() == 0 ?
			getString("_UI_WorkflowDiagram_type") :
			getString("_UI_WorkflowDiagram_type") + " " + label;
	}

  /**
	 * This handles model notifications by calling {@link #updateChildren} to update any cached
	 * children and by creating a viewer notification, which it passes to {@link #fireNotifyChanged}.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  @Override
  public void notifyChanged(Notification notification) {
		updateChildren(notification);

		switch (notification.getFeatureID(WorkflowDiagram.class)) {
			case FlowPackage.WORKFLOW_DIAGRAM__NAME:
				fireNotifyChanged(new ViewerNotification(notification, notification.getNotifier(), false, true));
				return;
			case FlowPackage.WORKFLOW_DIAGRAM__DIAGRAM_NODES:
			case FlowPackage.WORKFLOW_DIAGRAM__DIAGRAM_ARCS:
				fireNotifyChanged(new ViewerNotification(notification, notification.getNotifier(), true, false));
				return;
		}
		super.notifyChanged(notification);
	}

  /**
	 * This adds {@link org.eclipse.emf.edit.command.CommandParameter}s describing the children
	 * that can be created under this object.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  @Override
  protected void collectNewChildDescriptors(Collection<Object> newChildDescriptors, Object object) {
		super.collectNewChildDescriptors(newChildDescriptors, object);

		newChildDescriptors.add
			(createChildParameter
				(FlowPackage.Literals.WORKFLOW_DIAGRAM__DIAGRAM_NODES,
				 FlowFactory.eINSTANCE.createStartNode()));

		newChildDescriptors.add
			(createChildParameter
				(FlowPackage.Literals.WORKFLOW_DIAGRAM__DIAGRAM_NODES,
				 FlowFactory.eINSTANCE.createEndnode()));

		newChildDescriptors.add
			(createChildParameter
				(FlowPackage.Literals.WORKFLOW_DIAGRAM__DIAGRAM_NODES,
				 FlowFactory.eINSTANCE.createSplitNode()));

		newChildDescriptors.add
			(createChildParameter
				(FlowPackage.Literals.WORKFLOW_DIAGRAM__DIAGRAM_NODES,
				 FlowFactory.eINSTANCE.createMergeNode()));

		newChildDescriptors.add
			(createChildParameter
				(FlowPackage.Literals.WORKFLOW_DIAGRAM__DIAGRAM_NODES,
				 FlowFactory.eINSTANCE.createFlow()));

		newChildDescriptors.add
			(createChildParameter
				(FlowPackage.Literals.WORKFLOW_DIAGRAM__DIAGRAM_NODES,
				 FlowFactory.eINSTANCE.createSimpleActivity()));

		newChildDescriptors.add
			(createChildParameter
				(FlowPackage.Literals.WORKFLOW_DIAGRAM__DIAGRAM_NODES,
				 FlowFactory.eINSTANCE.createSubprocess()));

		newChildDescriptors.add
			(createChildParameter
				(FlowPackage.Literals.WORKFLOW_DIAGRAM__DIAGRAM_NODES,
				 FlowFactory.eINSTANCE.createPersistentResource()));

		newChildDescriptors.add
			(createChildParameter
				(FlowPackage.Literals.WORKFLOW_DIAGRAM__DIAGRAM_NODES,
				 FlowFactory.eINSTANCE.createTransientResource()));

		newChildDescriptors.add
			(createChildParameter
				(FlowPackage.Literals.WORKFLOW_DIAGRAM__DIAGRAM_NODES,
				 FlowFactory.eINSTANCE.createDocument()));

		newChildDescriptors.add
			(createChildParameter
				(FlowPackage.Literals.WORKFLOW_DIAGRAM__DIAGRAM_NODES,
				 FlowFactory.eINSTANCE.createEvent()));

		newChildDescriptors.add
			(createChildParameter
				(FlowPackage.Literals.WORKFLOW_DIAGRAM__DIAGRAM_ARCS,
				 FlowFactory.eINSTANCE.createDiagramArc()));
	}

  /**
	 * Return the resource locator for this item provider's resources.
	 * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
	 * @generated
	 */
  @Override
  public ResourceLocator getResourceLocator() {
		return FlowchartEditPlugin.INSTANCE;
	}

}
