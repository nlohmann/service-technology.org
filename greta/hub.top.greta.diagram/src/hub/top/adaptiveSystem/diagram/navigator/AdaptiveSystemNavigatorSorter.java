package hub.top.adaptiveSystem.diagram.navigator;

import org.eclipse.jface.viewers.ViewerSorter;

/**
 * @generated
 */
public class AdaptiveSystemNavigatorSorter extends ViewerSorter {

	/**
	 * @generated
	 */
	private static final int GROUP_CATEGORY = 5005;

	/**
	 * @generated
	 */
	public int category(Object element) {
		if (element instanceof hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorItem) {
			hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorItem item = (hub.top.adaptiveSystem.diagram.navigator.AdaptiveSystemNavigatorItem) element;
			return hub.top.adaptiveSystem.diagram.part.AdaptiveSystemVisualIDRegistry
					.getVisualID(item.getView());
		}
		return GROUP_CATEGORY;
	}

}
