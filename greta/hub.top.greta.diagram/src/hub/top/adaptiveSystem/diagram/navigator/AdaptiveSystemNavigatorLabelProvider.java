package hub.top.adaptiveSystem.diagram.navigator;

import org.eclipse.core.runtime.IAdaptable;
import org.eclipse.gmf.runtime.common.ui.services.parser.IParser;
import org.eclipse.gmf.runtime.common.ui.services.parser.ParserOptions;
import org.eclipse.gmf.runtime.common.ui.services.parser.ParserService;
import org.eclipse.gmf.runtime.emf.type.core.IElementType;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.jface.resource.ImageDescriptor;
import org.eclipse.jface.resource.ImageRegistry;
import org.eclipse.jface.viewers.ITreePathLabelProvider;
import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.jface.viewers.TreePath;
import org.eclipse.jface.viewers.ViewerLabel;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.IMemento;
import org.eclipse.ui.navigator.ICommonContentExtensionSite;
import org.eclipse.ui.navigator.ICommonLabelProvider;

/**
 * @generated
 */
public class AdaptiveSystemNavigatorLabelProvider extends LabelProvider
		implements ICommonLabelProvider, ITreePathLabelProvider {

	/**
	 * @generated
	 */
	static {
		hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
				.getInstance()
				.getImageRegistry()
				.put(
						"Navigator?UnknownElement", ImageDescriptor.getMissingImageDescriptor()); //$NON-NLS-1$
		hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
				.getInstance()
				.getImageRegistry()
				.put(
						"Navigator?ImageNotFound", ImageDescriptor.getMissingImageDescriptor()); //$NON-NLS-1$
	}

	/**
	 * @generated
	 */
	public void updateLabel(ViewerLabel label, TreePath elementPath) {
		Object element = elementPath.getLastSegment();
		if (element instanceof hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorItem
				&& !isOwnView(((hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorItem) element)
						.getView())) {
			return;
		}
		label.setText(getText(element));
		label.setImage(getImage(element));
	}

	/**
	 * @generated
	 */
	public Image getImage(Object element) {
		if (element instanceof hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup) {
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup group = (hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup) element;
			return hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
					.getInstance().getBundledImage(group.getIcon());
		}

		if (element instanceof hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorItem) {
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorItem navigatorItem = (hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorItem) element;
			if (!isOwnView(navigatorItem.getView())) {
				return super.getImage(element);
			}
			return getImage(navigatorItem.getView());
		}

		return super.getImage(element);
	}

	/**
	 * @generated
	 */
	public Image getImage(View view) {
		switch (hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getVisualID(view)) {
		case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.VISUAL_ID:
			return getImage(
					"Navigator?Diagram?adaptiveSystem?AdaptiveSystem", hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.AdaptiveSystem_79); //$NON-NLS-1$
		case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessEditPart.VISUAL_ID:
			return getImage(
					"Navigator?TopLevelNode?adaptiveSystem?AdaptiveProcess", hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.AdaptiveProcess_1001); //$NON-NLS-1$
		case hub.top.adaptiveSystem.diagram.edit.parts.OcletEditPart.VISUAL_ID:
			return getImage(
					"Navigator?TopLevelNode?adaptiveSystem?Oclet", hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Oclet_1002); //$NON-NLS-1$
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart.VISUAL_ID:
			return getImage(
					"Navigator?Node?adaptiveSystem?Condition", hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2001); //$NON-NLS-1$
		case hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart.VISUAL_ID:
			return getImage(
					"Navigator?Node?adaptiveSystem?Event", hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2002); //$NON-NLS-1$
		case hub.top.adaptiveSystem.diagram.edit.parts.PreNetEditPart.VISUAL_ID:
			return getImage(
					"Navigator?Node?adaptiveSystem?PreNet", hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.PreNet_2003); //$NON-NLS-1$
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart.VISUAL_ID:
			return getImage(
					"Navigator?Node?adaptiveSystem?Condition", hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2004); //$NON-NLS-1$
		case hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart.VISUAL_ID:
			return getImage(
					"Navigator?Node?adaptiveSystem?Event", hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2005); //$NON-NLS-1$
		case hub.top.adaptiveSystem.diagram.edit.parts.DoNetEditPart.VISUAL_ID:
			return getImage(
					"Navigator?Node?adaptiveSystem?DoNet", hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.DoNet_2006); //$NON-NLS-1$
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart.VISUAL_ID:
			return getImage(
					"Navigator?Node?adaptiveSystem?Condition", hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2007); //$NON-NLS-1$
		case hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart.VISUAL_ID:
			return getImage(
					"Navigator?Node?adaptiveSystem?Event", hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2008); //$NON-NLS-1$
		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID:
			return getImage(
					"Navigator?Link?adaptiveSystem?ArcToCondition", hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001); //$NON-NLS-1$
		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID:
			return getImage(
					"Navigator?Link?adaptiveSystem?ArcToEvent", hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002); //$NON-NLS-1$
		}
		return getImage("Navigator?UnknownElement", null); //$NON-NLS-1$
	}

	/**
	 * @generated
	 */
	private Image getImage(String key, IElementType elementType) {
		ImageRegistry imageRegistry = hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
				.getInstance().getImageRegistry();
		Image image = imageRegistry.get(key);
		if (image == null
				&& elementType != null
				&& hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes
						.isKnownElementType(elementType)) {
			image = hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes
					.getImage(elementType);
			imageRegistry.put(key, image);
		}

		if (image == null) {
			image = imageRegistry.get("Navigator?ImageNotFound"); //$NON-NLS-1$
			imageRegistry.put(key, image);
		}
		return image;
	}

	/**
	 * @generated
	 */
	public String getText(Object element) {
		if (element instanceof hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup) {
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup group = (hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorGroup) element;
			return group.getGroupName();
		}

		if (element instanceof hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorItem) {
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorItem navigatorItem = (hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorItem) element;
			if (!isOwnView(navigatorItem.getView())) {
				return null;
			}
			return getText(navigatorItem.getView());
		}

		return super.getText(element);
	}

	/**
	 * @generated
	 */
	public String getText(View view) {
		if (view.getElement() != null && view.getElement().eIsProxy()) {
			return getUnresolvedDomainElementProxyText(view);
		}
		switch (hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
				.getVisualID(view)) {
		case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.VISUAL_ID:
			return getAdaptiveSystem_79Text(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveProcessEditPart.VISUAL_ID:
			return getAdaptiveProcess_1001Text(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.OcletEditPart.VISUAL_ID:
			return getOclet_1002Text(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPEditPart.VISUAL_ID:
			return getCondition_2001Text(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.EventAPEditPart.VISUAL_ID:
			return getEvent_2002Text(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.PreNetEditPart.VISUAL_ID:
			return getPreNet_2003Text(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetEditPart.VISUAL_ID:
			return getCondition_2004Text(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetEditPart.VISUAL_ID:
			return getEvent_2005Text(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.DoNetEditPart.VISUAL_ID:
			return getDoNet_2006Text(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetEditPart.VISUAL_ID:
			return getCondition_2007Text(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetEditPart.VISUAL_ID:
			return getEvent_2008Text(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionEditPart.VISUAL_ID:
			return getArcToCondition_3001Text(view);
		case hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventEditPart.VISUAL_ID:
			return getArcToEvent_3002Text(view);
		}
		return getUnknownElementText(view);
	}

	/**
	 * @generated
	 */
	private String getAdaptiveSystem_79Text(View view) {
		hub.top.adaptiveSystem.AdaptiveSystem domainModelElement = (hub.top.adaptiveSystem.AdaptiveSystem) view
				.getElement();
		if (domainModelElement != null) {
			return String.valueOf(domainModelElement
					.isSetWellformednessToOclets());
		} else {
			hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
					.getInstance().logError(
							"No domain element for view with visualID = " + 79); //$NON-NLS-1$
			return ""; //$NON-NLS-1$
		}
	}

	/**
	 * @generated
	 */
	private String getAdaptiveProcess_1001Text(View view) {
		hub.top.adaptiveSystem.AdaptiveProcess domainModelElement = (hub.top.adaptiveSystem.AdaptiveProcess) view
				.getElement();
		if (domainModelElement != null) {
			return domainModelElement.getName();
		} else {
			hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
					.getInstance()
					.logError(
							"No domain element for view with visualID = " + 1001); //$NON-NLS-1$
			return ""; //$NON-NLS-1$
		}
	}

	/**
	 * @generated
	 */
	private String getOclet_1002Text(View view) {
		IAdaptable hintAdapter = new hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemParserProvider.HintAdapter(
				hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Oclet_1002,
				(view.getElement() != null ? view.getElement() : view),
				hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
						.getType(hub.top.adaptiveSystem.diagram.edit.parts.OcletNameEditPart.VISUAL_ID));
		IParser parser = ParserService.getInstance().getParser(hintAdapter);

		if (parser != null) {
			return parser.getPrintString(hintAdapter, ParserOptions.NONE
					.intValue());
		} else {
			hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
					.getInstance().logError(
							"Parser was not found for label " + 4016); //$NON-NLS-1$
			return ""; //$NON-NLS-1$
		}

	}

	/**
	 * @generated
	 */
	private String getCondition_2001Text(View view) {
		IAdaptable hintAdapter = new hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemParserProvider.HintAdapter(
				hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2001,
				(view.getElement() != null ? view.getElement() : view),
				hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
						.getType(hub.top.adaptiveSystem.diagram.edit.parts.ConditionAPNameEditPart.VISUAL_ID));
		IParser parser = ParserService.getInstance().getParser(hintAdapter);

		if (parser != null) {
			return parser.getPrintString(hintAdapter, ParserOptions.NONE
					.intValue());
		} else {
			hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
					.getInstance().logError(
							"Parser was not found for label " + 4001); //$NON-NLS-1$
			return ""; //$NON-NLS-1$
		}

	}

	/**
	 * @generated
	 */
	private String getEvent_2002Text(View view) {
		IAdaptable hintAdapter = new hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemParserProvider.HintAdapter(
				hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2002,
				(view.getElement() != null ? view.getElement() : view),
				hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
						.getType(hub.top.adaptiveSystem.diagram.edit.parts.EventAPNameEditPart.VISUAL_ID));
		IParser parser = ParserService.getInstance().getParser(hintAdapter);

		if (parser != null) {
			return parser.getPrintString(hintAdapter, ParserOptions.NONE
					.intValue());
		} else {
			hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
					.getInstance().logError(
							"Parser was not found for label " + 4004); //$NON-NLS-1$
			return ""; //$NON-NLS-1$
		}

	}

	/**
	 * @generated
	 */
	private String getPreNet_2003Text(View view) {
		hub.top.adaptiveSystem.PreNet domainModelElement = (hub.top.adaptiveSystem.PreNet) view
				.getElement();
		if (domainModelElement != null) {
			return domainModelElement.getName();
		} else {
			hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
					.getInstance()
					.logError(
							"No domain element for view with visualID = " + 2003); //$NON-NLS-1$
			return ""; //$NON-NLS-1$
		}
	}

	/**
	 * @generated
	 */
	private String getCondition_2004Text(View view) {
		IAdaptable hintAdapter = new hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemParserProvider.HintAdapter(
				hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2004,
				(view.getElement() != null ? view.getElement() : view),
				hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
						.getType(hub.top.adaptiveSystem.diagram.edit.parts.ConditionPreNetNameEditPart.VISUAL_ID));
		IParser parser = ParserService.getInstance().getParser(hintAdapter);

		if (parser != null) {
			return parser.getPrintString(hintAdapter, ParserOptions.NONE
					.intValue());
		} else {
			hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
					.getInstance().logError(
							"Parser was not found for label " + 4006); //$NON-NLS-1$
			return ""; //$NON-NLS-1$
		}

	}

	/**
	 * @generated
	 */
	private String getEvent_2005Text(View view) {
		IAdaptable hintAdapter = new hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemParserProvider.HintAdapter(
				hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2005,
				(view.getElement() != null ? view.getElement() : view),
				hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
						.getType(hub.top.adaptiveSystem.diagram.edit.parts.EventPreNetNameEditPart.VISUAL_ID));
		IParser parser = ParserService.getInstance().getParser(hintAdapter);

		if (parser != null) {
			return parser.getPrintString(hintAdapter, ParserOptions.NONE
					.intValue());
		} else {
			hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
					.getInstance().logError(
							"Parser was not found for label " + 4009); //$NON-NLS-1$
			return ""; //$NON-NLS-1$
		}

	}

	/**
	 * @generated
	 */
	private String getDoNet_2006Text(View view) {
		hub.top.adaptiveSystem.DoNet domainModelElement = (hub.top.adaptiveSystem.DoNet) view
				.getElement();
		if (domainModelElement != null) {
			return domainModelElement.getName();
		} else {
			hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
					.getInstance()
					.logError(
							"No domain element for view with visualID = " + 2006); //$NON-NLS-1$
			return ""; //$NON-NLS-1$
		}
	}

	/**
	 * @generated
	 */
	private String getCondition_2007Text(View view) {
		IAdaptable hintAdapter = new hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemParserProvider.HintAdapter(
				hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Condition_2007,
				(view.getElement() != null ? view.getElement() : view),
				hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
						.getType(hub.top.adaptiveSystem.diagram.edit.parts.ConditionDoNetNameEditPart.VISUAL_ID));
		IParser parser = ParserService.getInstance().getParser(hintAdapter);

		if (parser != null) {
			return parser.getPrintString(hintAdapter, ParserOptions.NONE
					.intValue());
		} else {
			hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
					.getInstance().logError(
							"Parser was not found for label " + 4011); //$NON-NLS-1$
			return ""; //$NON-NLS-1$
		}

	}

	/**
	 * @generated
	 */
	private String getEvent_2008Text(View view) {
		IAdaptable hintAdapter = new hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemParserProvider.HintAdapter(
				hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.Event_2008,
				(view.getElement() != null ? view.getElement() : view),
				hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
						.getType(hub.top.adaptiveSystem.diagram.edit.parts.EventDoNetNameEditPart.VISUAL_ID));
		IParser parser = ParserService.getInstance().getParser(hintAdapter);

		if (parser != null) {
			return parser.getPrintString(hintAdapter, ParserOptions.NONE
					.intValue());
		} else {
			hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
					.getInstance().logError(
							"Parser was not found for label " + 4014); //$NON-NLS-1$
			return ""; //$NON-NLS-1$
		}

	}

	/**
	 * @generated
	 */
	private String getArcToCondition_3001Text(View view) {
		IAdaptable hintAdapter = new hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemParserProvider.HintAdapter(
				hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToCondition_3001,
				(view.getElement() != null ? view.getElement() : view),
				hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
						.getType(hub.top.adaptiveSystem.diagram.edit.parts.ArcToConditionWeightEditPart.VISUAL_ID));
		IParser parser = ParserService.getInstance().getParser(hintAdapter);

		if (parser != null) {
			return parser.getPrintString(hintAdapter, ParserOptions.NONE
					.intValue());
		} else {
			hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
					.getInstance().logError(
							"Parser was not found for label " + 6001); //$NON-NLS-1$
			return ""; //$NON-NLS-1$
		}

	}

	/**
	 * @generated
	 */
	private String getArcToEvent_3002Text(View view) {
		IAdaptable hintAdapter = new hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemParserProvider.HintAdapter(
				hub.top.adaptiveSystem.diagram.providers.AdaptiveSystemElementTypes.ArcToEvent_3002,
				(view.getElement() != null ? view.getElement() : view),
				hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
						.getType(hub.top.adaptiveSystem.diagram.edit.parts.ArcToEventWeightEditPart.VISUAL_ID));
		IParser parser = ParserService.getInstance().getParser(hintAdapter);

		if (parser != null) {
			return parser.getPrintString(hintAdapter, ParserOptions.NONE
					.intValue());
		} else {
			hub.top.adaptiveSystem.diagram.part.AdaptiveSystemDiagramEditorPlugin
					.getInstance().logError(
							"Parser was not found for label " + 6002); //$NON-NLS-1$
			return ""; //$NON-NLS-1$
		}

	}

	/**
	 * @generated
	 */
	private String getUnknownElementText(View view) {
		return "<UnknownElement Visual_ID = " + view.getType() + ">"; //$NON-NLS-1$ //$NON-NLS-2$
	}

	/**
	 * @generated
	 */
	private String getUnresolvedDomainElementProxyText(View view) {
		return "<Unresolved domain element Visual_ID = " + view.getType() + ">"; //$NON-NLS-1$ //$NON-NLS-2$
	}

	/**
	 * @generated
	 */
	public void init(ICommonContentExtensionSite aConfig) {
	}

	/**
	 * @generated
	 */
	public void restoreState(IMemento aMemento) {
	}

	/**
	 * @generated
	 */
	public void saveState(IMemento aMemento) {
	}

	/**
	 * @generated
	 */
	public String getDescription(Object anElement) {
		return null;
	}

	/**
	 * @generated
	 */
	private boolean isOwnView(View view) {
		return hub.top.adaptiveSystem.diagram.edit.parts.AdaptiveSystemEditPart.MODEL_ID
				.equals(hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
						.getModelID(view));
	}

}
