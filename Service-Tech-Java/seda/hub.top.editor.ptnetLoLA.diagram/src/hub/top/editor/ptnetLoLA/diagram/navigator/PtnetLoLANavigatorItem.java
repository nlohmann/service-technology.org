package hub.top.editor.ptnetLoLA.diagram.navigator;

import org.eclipse.core.runtime.IAdapterFactory;
import org.eclipse.core.runtime.Platform;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.EcoreUtil;
import org.eclipse.gmf.runtime.notation.View;

/**
 * @generated
 */
public class PtnetLoLANavigatorItem extends
		hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLAAbstractNavigatorItem {

	/**
	 * @generated
	 */
	static {
		final Class[] supportedTypes = new Class[] { View.class, EObject.class };
		Platform.getAdapterManager().registerAdapters(new IAdapterFactory() {

			public Object getAdapter(Object adaptableObject, Class adapterType) {
				if (adaptableObject instanceof hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem
						&& (adapterType == View.class || adapterType == EObject.class)) {
					return ((hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem) adaptableObject)
							.getView();
				}
				return null;
			}

			public Class[] getAdapterList() {
				return supportedTypes;
			}
		}, hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem.class);
	}

	/**
	 * @generated
	 */
	private View myView;

	/**
	 * @generated
	 */
	private boolean myLeaf = false;

	/**
	 * @generated
	 */
	public PtnetLoLANavigatorItem(View view, Object parent, boolean isLeaf) {
		super(parent);
		myView = view;
		myLeaf = isLeaf;
	}

	/**
	 * @generated
	 */
	public View getView() {
		return myView;
	}

	/**
	 * @generated
	 */
	public boolean isLeaf() {
		return myLeaf;
	}

	/**
	 * @generated
	 */
	public boolean equals(Object obj) {
		if (obj instanceof hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem) {
			return EcoreUtil
					.getURI(getView())
					.equals(
							EcoreUtil
									.getURI(((hub.top.editor.ptnetLoLA.diagram.navigator.PtnetLoLANavigatorItem) obj)
											.getView()));
		}
		return super.equals(obj);
	}

	/**
	 * @generated
	 */
	public int hashCode() {
		return EcoreUtil.getURI(getView()).hashCode();
	}

}
