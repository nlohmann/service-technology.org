/**
 * <copyright>
 * </copyright>
 *
 * $Id$
 */
package hub.top.editor.ptnetLoLA.provider;


import hub.top.editor.ptnetLoLA.PtNet;
import hub.top.editor.ptnetLoLA.PtnetLoLAFactory;
import hub.top.editor.ptnetLoLA.PtnetLoLAPackage;

import java.util.Collection;
import java.util.List;

import org.eclipse.emf.common.notify.AdapterFactory;
import org.eclipse.emf.common.notify.Notification;

import org.eclipse.emf.common.util.ResourceLocator;

import org.eclipse.emf.ecore.EStructuralFeature;

import org.eclipse.emf.edit.provider.IEditingDomainItemProvider;
import org.eclipse.emf.edit.provider.IItemLabelProvider;
import org.eclipse.emf.edit.provider.IItemPropertyDescriptor;
import org.eclipse.emf.edit.provider.IItemPropertySource;
import org.eclipse.emf.edit.provider.IStructuredItemContentProvider;
import org.eclipse.emf.edit.provider.ITreeItemContentProvider;
import org.eclipse.emf.edit.provider.ItemProviderAdapter;
import org.eclipse.emf.edit.provider.ViewerNotification;

/**
 * This is the item provider adapter for a {@link hub.top.editor.ptnetLoLA.PtNet} object.
 * <!-- begin-user-doc -->
 * <!-- end-user-doc -->
 * @generated
 */
public class PtNetItemProvider
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
	public PtNetItemProvider(AdapterFactory adapterFactory) {
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

		}
		return itemPropertyDescriptors;
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
			childrenFeatures.add(PtnetLoLAPackage.Literals.PT_NET__PLACES);
			childrenFeatures.add(PtnetLoLAPackage.Literals.PT_NET__TRANSITIONS);
			childrenFeatures.add(PtnetLoLAPackage.Literals.PT_NET__INITIAL_MARKING);
			childrenFeatures.add(PtnetLoLAPackage.Literals.PT_NET__ANNOTATION);
			childrenFeatures.add(PtnetLoLAPackage.Literals.PT_NET__ARCS);
			childrenFeatures.add(PtnetLoLAPackage.Literals.PT_NET__FINAL_MARKING);
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
	 * This returns PtNet.gif.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public Object getImage(Object object) {
		return overlayImage(object, getResourceLocator().getImage("full/obj16/PtNet"));
	}

	/**
	 * This returns the label text for the adapted class.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public String getText(Object object) {
		return getString("_UI_PtNet_type");
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

		switch (notification.getFeatureID(PtNet.class)) {
			case PtnetLoLAPackage.PT_NET__PLACES:
			case PtnetLoLAPackage.PT_NET__TRANSITIONS:
			case PtnetLoLAPackage.PT_NET__INITIAL_MARKING:
			case PtnetLoLAPackage.PT_NET__ANNOTATION:
			case PtnetLoLAPackage.PT_NET__ARCS:
			case PtnetLoLAPackage.PT_NET__FINAL_MARKING:
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
				(PtnetLoLAPackage.Literals.PT_NET__PLACES,
				 PtnetLoLAFactory.eINSTANCE.createPlace()));

		newChildDescriptors.add
			(createChildParameter
				(PtnetLoLAPackage.Literals.PT_NET__TRANSITIONS,
				 PtnetLoLAFactory.eINSTANCE.createTransition()));

		newChildDescriptors.add
			(createChildParameter
				(PtnetLoLAPackage.Literals.PT_NET__INITIAL_MARKING,
				 PtnetLoLAFactory.eINSTANCE.createMarking()));

		newChildDescriptors.add
			(createChildParameter
				(PtnetLoLAPackage.Literals.PT_NET__ANNOTATION,
				 PtnetLoLAFactory.eINSTANCE.createAnnotation()));

		newChildDescriptors.add
			(createChildParameter
				(PtnetLoLAPackage.Literals.PT_NET__ARCS,
				 PtnetLoLAFactory.eINSTANCE.createArc()));

		newChildDescriptors.add
			(createChildParameter
				(PtnetLoLAPackage.Literals.PT_NET__ARCS,
				 PtnetLoLAFactory.eINSTANCE.createArcToPlace()));

		newChildDescriptors.add
			(createChildParameter
				(PtnetLoLAPackage.Literals.PT_NET__ARCS,
				 PtnetLoLAFactory.eINSTANCE.createArcToTransition()));

		newChildDescriptors.add
			(createChildParameter
				(PtnetLoLAPackage.Literals.PT_NET__FINAL_MARKING,
				 PtnetLoLAFactory.eINSTANCE.createMarking()));
	}

	/**
	 * This returns the label text for {@link org.eclipse.emf.edit.command.CreateChildCommand}.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public String getCreateChildText(Object owner, Object feature, Object child, Collection<?> selection) {
		Object childFeature = feature;
		Object childObject = child;

		boolean qualify =
			childFeature == PtnetLoLAPackage.Literals.PT_NET__INITIAL_MARKING ||
			childFeature == PtnetLoLAPackage.Literals.PT_NET__FINAL_MARKING;

		if (qualify) {
			return getString
				("_UI_CreateChild_text2",
				 new Object[] { getTypeText(childObject), getFeatureText(childFeature), getTypeText(owner) });
		}
		return super.getCreateChildText(owner, feature, child, selection);
	}

	/**
	 * Return the resource locator for this item provider's resources.
	 * <!-- begin-user-doc -->
	 * <!-- end-user-doc -->
	 * @generated
	 */
	@Override
	public ResourceLocator getResourceLocator() {
		return PtnetLoLAEditPlugin.INSTANCE;
	}

}
