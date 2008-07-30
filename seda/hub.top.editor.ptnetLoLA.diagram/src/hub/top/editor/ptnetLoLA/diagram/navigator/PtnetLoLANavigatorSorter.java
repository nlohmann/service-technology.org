package hub.top.editor.ptnetLoLA.diagram.navigator;

import org.eclipse.jface.viewers.ViewerSorter;

/**
 * @generated
 */
public class PtnetLoLANavigatorSorter extends ViewerSorter {

	/**
	 * @generated
	 */
	private static final int GROUP_CATEGORY = 3004;

	/**
	 * @generated
	 */
	public int category(Object element) {
		if (element instanceof hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem) {
			hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem item = (hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem) element;
			return hub.top.editor.ptnetLoLA.diagram.part.PtnetLoLAVisualIDRegistry
					.getVisualID(item.getView());
		}
		return GROUP_CATEGORY;
	}

}
