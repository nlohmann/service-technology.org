package hub.top.adaptiveSystem.diagram.providers;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.edit.ui.provider.AdapterFactoryLabelProvider;
import org.eclipse.gmf.runtime.diagram.ui.editparts.IGraphicalEditPart;
import org.eclipse.gmf.runtime.emf.type.core.ElementTypeRegistry;
import org.eclipse.gmf.runtime.emf.type.core.IElementType;
import org.eclipse.gmf.runtime.emf.ui.services.modelingassistant.ModelingAssistantProvider;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.jface.viewers.ILabelProvider;
import org.eclipse.jface.window.Window;
import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.dialogs.ElementListSelectionDialog;

/**
 * @generated
 */
public class AdaptiveSystemModelingAssistantProvider extends
		ModelingAssistantProvider {

	/**
	 * @generated
	 */
	public List getTypesForPopupBar(IAdaptable host) {
		IGraphicalEditPart editPart = (IGraphicalEditPart) host
				.getAdapter(IGraphicalEditPart.class);
		if (editPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.OcletEditPart) {
			List types = new ArrayList();
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.PreNet_2003);
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.DoNet_2006);
			return types;
		}
		if (editPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessCompartmentEditPart) {
			List types = new ArrayList();
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2001);
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2002);
			return types;
		}
		if (editPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.PreNetCompartmentEditPart) {
			List types = new ArrayList();
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2004);
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2005);
			return types;
		}
		if (editPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.DoNetCompartmentEditPart) {
			List types = new ArrayList();
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2007);
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2008);
			return types;
		}
		if (editPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart) {
			List types = new ArrayList();
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.AdaptiveProcess_1001);
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Oclet_1002);
			return types;
		}
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public List getRelTypesOnSource(IAdaptable source) {
		IGraphicalEditPart sourceEditPart = (IGraphicalEditPart) source
				.getAdapter(IGraphicalEditPart.class);
		if (sourceEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart) {
			List types = new ArrayList();
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002);
			return types;
		}
		if (sourceEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart) {
			List types = new ArrayList();
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001);
			return types;
		}
		if (sourceEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart) {
			List types = new ArrayList();
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002);
			return types;
		}
		if (sourceEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart) {
			List types = new ArrayList();
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001);
			return types;
		}
		if (sourceEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart) {
			List types = new ArrayList();
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002);
			return types;
		}
		if (sourceEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart) {
			List types = new ArrayList();
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001);
			return types;
		}
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public List getRelTypesOnTarget(IAdaptable target) {
		IGraphicalEditPart targetEditPart = (IGraphicalEditPart) target
				.getAdapter(IGraphicalEditPart.class);
		if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart) {
			List types = new ArrayList();
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001);
			return types;
		}
		if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart) {
			List types = new ArrayList();
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002);
			return types;
		}
		if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart) {
			List types = new ArrayList();
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001);
			return types;
		}
		if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart) {
			List types = new ArrayList();
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002);
			return types;
		}
		if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart) {
			List types = new ArrayList();
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001);
			return types;
		}
		if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart) {
			List types = new ArrayList();
			types
					.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002);
			return types;
		}
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public List getRelTypesOnSourceAndTarget(IAdaptable source,
			IAdaptable target) {
		IGraphicalEditPart sourceEditPart = (IGraphicalEditPart) source
				.getAdapter(IGraphicalEditPart.class);
		IGraphicalEditPart targetEditPart = (IGraphicalEditPart) target
				.getAdapter(IGraphicalEditPart.class);
		if (sourceEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart) {
			List types = new ArrayList();
			if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002);
			}
			if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002);
			}
			if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002);
			}
			return types;
		}
		if (sourceEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart) {
			List types = new ArrayList();
			if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001);
			}
			if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001);
			}
			if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001);
			}
			return types;
		}
		if (sourceEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart) {
			List types = new ArrayList();
			if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002);
			}
			if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002);
			}
			if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002);
			}
			return types;
		}
		if (sourceEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart) {
			List types = new ArrayList();
			if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001);
			}
			if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001);
			}
			if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001);
			}
			return types;
		}
		if (sourceEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart) {
			List types = new ArrayList();
			if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002);
			}
			if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002);
			}
			if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002);
			}
			return types;
		}
		if (sourceEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart) {
			List types = new ArrayList();
			if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001);
			}
			if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001);
			}
			if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001);
			}
			return types;
		}
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public List getTypesForSource(IAdaptable target,
			IElementType relationshipType) {
		IGraphicalEditPart targetEditPart = (IGraphicalEditPart) target
				.getAdapter(IGraphicalEditPart.class);
		if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart) {
			List types = new ArrayList();
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2002);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2005);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2008);
			}
			return types;
		}
		if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart) {
			List types = new ArrayList();
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2001);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2004);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2007);
			}
			return types;
		}
		if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart) {
			List types = new ArrayList();
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2002);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2005);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2008);
			}
			return types;
		}
		if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart) {
			List types = new ArrayList();
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2001);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2004);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2007);
			}
			return types;
		}
		if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart) {
			List types = new ArrayList();
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2002);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2005);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2008);
			}
			return types;
		}
		if (targetEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart) {
			List types = new ArrayList();
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2001);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2004);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2007);
			}
			return types;
		}
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public List getTypesForTarget(IAdaptable source,
			IElementType relationshipType) {
		IGraphicalEditPart sourceEditPart = (IGraphicalEditPart) source
				.getAdapter(IGraphicalEditPart.class);
		if (sourceEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart) {
			List types = new ArrayList();
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2002);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2005);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2008);
			}
			return types;
		}
		if (sourceEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart) {
			List types = new ArrayList();
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2001);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2004);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2007);
			}
			return types;
		}
		if (sourceEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart) {
			List types = new ArrayList();
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2002);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2005);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2008);
			}
			return types;
		}
		if (sourceEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart) {
			List types = new ArrayList();
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2001);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2004);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2007);
			}
			return types;
		}
		if (sourceEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart) {
			List types = new ArrayList();
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2002);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2005);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2008);
			}
			return types;
		}
		if (sourceEditPart instanceof hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart) {
			List types = new ArrayList();
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2001);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2004);
			}
			if (relationshipType == hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001) {
				types
						.add(hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2007);
			}
			return types;
		}
		return Collections.EMPTY_LIST;
	}

	/**
	 * @generated
	 */
	public EObject selectExistingElementForSource(IAdaptable target,
			IElementType relationshipType) {
		return selectExistingElement(target, getTypesForSource(target,
				relationshipType));
	}

	/**
	 * @generated
	 */
	public EObject selectExistingElementForTarget(IAdaptable source,
			IElementType relationshipType) {
		return selectExistingElement(source, getTypesForTarget(source,
				relationshipType));
	}

	/**
	 * @generated
	 */
	protected EObject selectExistingElement(IAdaptable host, Collection types) {
		if (types.isEmpty()) {
			return null;
		}
		IGraphicalEditPart editPart = (IGraphicalEditPart) host
				.getAdapter(IGraphicalEditPart.class);
		if (editPart == null) {
			return null;
		}
		Diagram diagram = (Diagram) editPart.getRoot().getContents().getModel();
		Collection elements = new HashSet();
		for (Iterator it = diagram.getElement().eAllContents(); it.hasNext();) {
			EObject element = (EObject) it.next();
			if (isApplicableElement(element, types)) {
				elements.add(element);
			}
		}
		if (elements.isEmpty()) {
			return null;
		}
		return selectElement((EObject[]) elements.toArray(new EObject[elements
				.size()]));
	}

	/**
	 * @generated
	 */
	protected boolean isApplicableElement(EObject element, Collection types) {
		IElementType type = ElementTypeRegistry.getInstance().getElementType(
				element);
		return types.contains(type);
	}

	/**
	 * @generated
	 */
	protected EObject selectElement(EObject[] elements) {
		Shell shell = Display.getCurrent().getActiveShell();
		ILabelProvider labelProvider = new AdapterFactoryLabelProvider(
				hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
						.getInstance().getItemProvidersAdapterFactory());
		ElementListSelectionDialog dialog = new ElementListSelectionDialog(
				shell, labelProvider);
		dialog
				.setMessage(hub.top.adaptiveSystem.diagram.part.Messages.AdaptiveSystemModelingAssistantProviderMessage);
		dialog
				.setTitle(hub.top.adaptiveSystem.diagram.part.Messages.AdaptiveSystemModelingAssistantProviderTitle);
		dialog.setMultipleSelection(false);
		dialog.setElements(elements);
		EObject selected = null;
		if (dialog.open() == Window.OK) {
			selected = (EObject) dialog.getFirstResult();
		}
		return selected;
	}
}
