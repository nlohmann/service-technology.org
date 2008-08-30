package hub.top.editor.ptnetLoLA.diagram.navigator;

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
public class PtnetLoLANavigatorLabelProvider extends LabelProvider implements
		ICommonLabelProvider, ITreePathLabelProvider {

	/**
	 * @generated
	 */
	static {
		hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
				.getInstance()
				.getImageRegistry()
				.put(
						"Navigator?UnknownElement", ImageDescriptor.getMissingImageDescriptor()); //$NON-NLS-1$
		hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
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
		if (element instanceof hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem
				&& !isOwnView(((hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem) element)
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
		if (element instanceof hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorGroup) {
			hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorGroup group = (hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorGroup) element;
			return hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
					.getInstance().getBundledImage(group.getIcon());
		}

		if (element instanceof hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem) {
			hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem navigatorItem = (hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem) element;
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
		switch (hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
				.getVisualID(view)) {
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.VISUAL_ID:
			return getImage(
					"Navigator?Diagram?http://www.informatik.hu-berlin.de/top/tools/editors/petrinets?PtNet", hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.PtNet_1000); //$NON-NLS-1$
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionEditPart.VISUAL_ID:
			return getImage(
					"Navigator?TopLevelNode?http://www.informatik.hu-berlin.de/top/tools/editors/petrinets?Transition", hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.Transition_2001); //$NON-NLS-1$
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceEditPart.VISUAL_ID:
			return getImage(
					"Navigator?TopLevelNode?http://www.informatik.hu-berlin.de/top/tools/editors/petrinets?Place", hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.Place_2002); //$NON-NLS-1$
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID:
			return getImage(
					"Navigator?Link?http://www.informatik.hu-berlin.de/top/tools/editors/petrinets?ArcToPlace", hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToPlace_4001); //$NON-NLS-1$
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID:
			return getImage(
					"Navigator?Link?http://www.informatik.hu-berlin.de/top/tools/editors/petrinets?ArcToTransition", hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToTransition_4002); //$NON-NLS-1$
		}
		return getImage("Navigator?UnknownElement", null); //$NON-NLS-1$
	}

	/**
	 * @generated
	 */
	private Image getImage(String key, IElementType elementType) {
		ImageRegistry imageRegistry = hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
				.getInstance().getImageRegistry();
		Image image = imageRegistry.get(key);
		if (image == null
				&& elementType != null
				&& hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes
						.isKnownElementType(elementType)) {
			image = hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes
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
		if (element instanceof hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorGroup) {
			hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorGroup group = (hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorGroup) element;
			return group.getGroupName();
		}

		if (element instanceof hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem) {
			hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem navigatorItem = (hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem) element;
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
		switch (hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
				.getVisualID(view)) {
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.VISUAL_ID:
			return getPtNet_1000Text(view);
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionEditPart.VISUAL_ID:
			return getTransition_2001Text(view);
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceEditPart.VISUAL_ID:
			return getPlace_2002Text(view);
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceEditPart.VISUAL_ID:
			return getArcToPlace_4001Text(view);
		case hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionEditPart.VISUAL_ID:
			return getArcToTransition_4002Text(view);
		}
		return getUnknownElementText(view);
	}

	/**
	 * @generated
	 */
	private String getPtNet_1000Text(View view) {
		return ""; //$NON-NLS-1$
	}

	/**
	 * @generated
	 */
	private String getTransition_2001Text(View view) {
		IAdaptable hintAdapter = new hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAParserProvider.HintAdapter(
				hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.Transition_2001,
				(view.getElement() != null ? view.getElement() : view),
				hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
						.getType(hub.top.editor.ptnetLoLA.diagram.edit.parts.TransitionNameEditPart.VISUAL_ID));
		IParser parser = ParserService.getInstance().getParser(hintAdapter);

		if (parser != null) {
			return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
		} else {
			hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
					.getInstance().logError("Parser was not found for label " + 5001); //$NON-NLS-1$
			return ""; //$NON-NLS-1$
		}

	}

	/**
	 * @generated
	 */
	private String getPlace_2002Text(View view) {
		IAdaptable hintAdapter = new hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAParserProvider.HintAdapter(
				hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.Place_2002,
				(view.getElement() != null ? view.getElement() : view),
				hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
						.getType(hub.top.editor.ptnetLoLA.diagram.edit.parts.PlaceNameEditPart.VISUAL_ID));
		IParser parser = ParserService.getInstance().getParser(hintAdapter);

		if (parser != null) {
			return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
		} else {
			hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
					.getInstance().logError("Parser was not found for label " + 5002); //$NON-NLS-1$
			return ""; //$NON-NLS-1$
		}

	}

	/**
	 * @generated
	 */
	private String getArcToPlace_4001Text(View view) {
		IAdaptable hintAdapter = new hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAParserProvider.HintAdapter(
				hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToPlace_4001,
				(view.getElement() != null ? view.getElement() : view),
				hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
						.getType(hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToPlaceWeightEditPart.VISUAL_ID));
		IParser parser = ParserService.getInstance().getParser(hintAdapter);

		if (parser != null) {
			return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
		} else {
			hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
					.getInstance().logError("Parser was not found for label " + 6001); //$NON-NLS-1$
			return ""; //$NON-NLS-1$
		}

	}

	/**
	 * @generated
	 */
	private String getArcToTransition_4002Text(View view) {
		IAdaptable hintAdapter = new hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAParserProvider.HintAdapter(
				hub.top.editor.ptnetLoLA.diagram.providers.PtnetLoLAElementTypes.ArcToTransition_4002,
				(view.getElement() != null ? view.getElement() : view),
				hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
						.getType(hub.top.editor.ptnetLoLA.diagram.edit.parts.ArcToTransitionWeightEditPart.VISUAL_ID));
		IParser parser = ParserService.getInstance().getParser(hintAdapter);

		if (parser != null) {
			return parser.getPrintString(hintAdapter, ParserOptions.NONE.intValue());
		} else {
			hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLADiagramEditorPlugin
					.getInstance().logError("Parser was not found for label " + 6002); //$NON-NLS-1$
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
		return hub.top.editor.ptnetLoLA.diagram.edit.parts.PtNetEditPart.MODEL_ID
				.equals(hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
						.getModelID(view));
	}

}
